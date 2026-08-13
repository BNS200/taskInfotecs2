#pragma once

#include <atomic>
#include <string>
#include "spsc_buffer.hpp"
#include "library_loader.hpp"
#include "socket_client.hpp"

/**
 * @brief Класс для обработки данных из буфера
 *
 * Поток 2: Извлекает данные из буфера, обрабатывает через функцию 2 (sumDigits),
 * отправляет результат в Программу №2 через сокет.
 *
 * @note Реализует паттерн Consumer в архитектуре Producer-Consumer
 */
class DataProcessor
{
public:
    /**
     * @brief Конструктор DataProcessor
     * @param loader Указатель на загрузчик библиотеки
     * @param buffer Ссылка на буфер
     * @param socket_val Ссылка на сокет для отправки данных в Программу №2
     * @param running Ссылка на флаг работы программы
     */
    DataProcessor(LibraryLoader *loader,
                  Buffer<std::string, 32> &buffer,
                  Socket &socket_val,
                  std::atomic<bool> &running);

    /**
     * @brief Запуск основного цикла обработки данных
     *
     * Блокирующий метод. Извлекает данные из буфера, обрабатывает их
     * и отправляет в Программу №2 через сокет.
     *
     * @note Цикл завершается при установке running = false
     */
    void run();

    /**
     * @brief Остановка обработки данных
     *
     * Устанавливает флаг running = false и прерывает ожидание в буфере
     */
    void stop();

private:
    /**
     * @brief Отправка данных в Программу №2
     *
     * Формирует сообщение и отправляет через сокет.
     * При отсутствии подключения выполняет переподключение.
     *
     * @param data Обработанные данные
     * @param sum Сумма цифр из данных
     * @return true если данные отправлены успешно, иначе false
     */
    bool sendToProgram2(const std::string &data, size_t sum);

    LibraryLoader *loader;           ///< Загрузчик библиотеки
    Buffer<std::string, 32> &buffer; ///< Буфер
    Socket &m_socket;                ///< Сокет для связи с Программой №2
    std::atomic<bool> &running;      ///< Флаг работы программы

    static constexpr int MAX_SEND_ATTEMPTS = 3; ///< Максимальное количество попыток отправки
    static constexpr int RETRY_DELAY_MS = 500;  ///< Задержка между попытками (мс)
};