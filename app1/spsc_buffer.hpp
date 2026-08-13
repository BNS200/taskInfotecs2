#pragma once

#include <atomic>
#include <vector>
#include <semaphore>
#include <new>
#include <chrono>

/**
 * @brief Класс SPSC кольцевого буфера
 *
 * Single Producer, Single Consumer — только один поток может записывать
 * и только один поток может читать одновременно.
 *
 * @tparam T Тип элементов в буфере
 * @tparam Capacity Размер буфера
 *
 * @details Особенности реализации:
 * - Lock-free синхронизация через атомарные операции
 * - Блокировка через семафоры
 * - Защита от False Sharing
 * - Поддержка перемещения для эффективности
 */
template <typename T, size_t Capacity>
class Buffer
{
    /**
     * @brief Проверка, что Capacity является степенью двойки
     *
     * Необходимо для эффективного вычисления индекса через побитовое И
     * вместо операции взятия остатка
     */
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

public:
    /**
     * @brief Конструктор буфера
     *
     * Инициализирует буфер в состояние:
     * - head = 0 (индекс чтения)
     * - tail = 0 (индекс записи)
     * - free_slots = Capacity - 1 (свободные слоты)
     * - occupied_slots = 0 (занятые слоты)
     * - interrupted = false (не прерван)
     */
    Buffer()
        : head(0),
          tail(0),
          free_slots(Capacity - 1),
          occupied_slots(0),
          interrupted(false)
    {
        buffer.resize(Capacity);
    }

    /**
     * @brief Деструктор
     */
    ~Buffer() = default;

    /**
     * @brief Запрет копирования
     */
    Buffer(const Buffer &) = delete;

    /**
     * @brief Запрет присваивания копированием
     */
    Buffer &operator=(const Buffer &) = delete;

    /**
     * @brief Запись элемента в буфер (Producer)
     *
     * Блокирует поток, если буфер заполнен.
     *
     * @param item Элемент для записи
     *
     * @details Алгоритм:
     * 1. Ожидание свободного слота (free_slots.acquire)
     * 2. Проверка на прерывание
     * 3. Запись элемента по индексу tail
     * 4. Обновление tail
     * 5. Сигнал о появлении данных (occupied_slots.release)
     */
    void push(T &&item)
    {
        free_slots.acquire();

        if (interrupted)
        {
            return;
        }

        const size_t current_tail = tail.load(std::memory_order_relaxed);
        buffer[current_tail] = std::move(item);
        tail.store((current_tail + 1) % Capacity, std::memory_order_release);

        occupied_slots.release();
    }

    /**
     * @brief Чтение элемента из буфера (Consumer)
     *
     * Блокирует поток, если буфер пуст.
     *
     * @return T Прочитанный элемент
     *
     * @details Алгоритм:
     * 1. Ожидание данных (occupied_slots.acquire)
     * 2. Проверка на прерывание
     * 3. Чтение элемента по индексу head
     * 4. Обновление head
     * 5. Сигнал о свободном месте (free_slots.release)
     * 6. Возврат элемента (перемещённого из буфера)
     */
    T pop()
    {
        occupied_slots.acquire();

        if (interrupted)
        {
            return T{};
        }

        const size_t current_head = head.load(std::memory_order_relaxed);
        T item = std::move(buffer[current_head]);
        head.store((current_head + 1) % Capacity, std::memory_order_release);

        free_slots.release();

        return item;
    }

    /**
     * @brief Проверка, пуст ли буфер
     *
     * @return true если буфер пуст, иначе false
     */
    bool empty() const
    {
        if (occupied_slots.try_acquire())
        {
            occupied_slots.release();
            return false;
        }
        return true;
    }

    /**
     * @brief Проверка, заполнен ли буфер
     *
     * @return true если буфер заполнен, иначе false
     */
    bool full() const
    {
        if (free_slots.try_acquire())
        {
            free_slots.release();
            return false;
        }
        return true;
    }

    /**
     * @brief Прерывание ожидания потоков
     *
     * Разблокирует потоки, заблокированные на push() или pop()
     * Используется для корректного завершения программы
     */
    void interrupt()
    {
        interrupted = true;
        occupied_slots.release();
        free_slots.release();
    }

private:
    std::vector<T> buffer;         ///< Хранилище элементов
    std::atomic<bool> interrupted; ///< Флаг прерывания буфера

    /**
     * @brief Размер кэш-линии процессора
     *
     * Используется для выравнивания переменных и предотвращения False Sharing
     * std::hardware_destructive_interference_size доступен в C++20
     * Для старых компиляторов используется значение по умолчанию 64 байта
     */
#ifdef __cpp_lib_hardware_interference_size
    static constexpr size_t cache_line_size = std::hardware_destructive_interference_size;
#else
    static constexpr size_t cache_line_size = 64;
#endif

    /**
     * @brief Индекс чтения
     * Указывает на позицию, откуда будет прочитан следующий элемент.
     */
    alignas(cache_line_size) std::atomic<size_t> head;

    /**
     * @brief Индекс записи (Producer)
     *
     * Указывает на позицию, куда будет записан следующий элемент.
     */
    alignas(cache_line_size) std::atomic<size_t> tail;

    /**
     * @brief Семафор для свободных слотов
     */
    alignas(cache_line_size) std::counting_semaphore<Capacity> free_slots;

    /**
     * @brief Семафор для занятых слотов
     */
    alignas(cache_line_size) std::counting_semaphore<Capacity> occupied_slots;
};