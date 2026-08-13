#pragma once

#include <string>
#include <mutex>
#include <atomic>

/**
 * @brief Класс для работы с TCP-сокетом
 *
 * Обеспечивает подключение к серверу (Программа 2), отправку данных,
 * переподключение при потере связи и управление состоянием соединения.
 *
 * @note Класс является некопируемым и неперемещаемым
 * @note Потокобезопасен благодаря использованию std::mutex
 */
class Socket
{
public:
    /**
     * @brief Конструктор Socket
     * Инициализирует сокет в неактивное состояние (sock = -1, running = true)
     */
    Socket();

    /**
     * @brief Деструктор Socket
     */
    ~Socket();

    /**
     * @brief Запрет копирования
     */
    Socket(const Socket &) = delete;

    /**
     * @brief Запрет присваивания копированием
     */
    Socket &operator=(const Socket &) = delete;

    /**
     * @brief Запрет перемещения
     */
    Socket(Socket &&) = delete;

    /**
     * @brief Запрет присваивания перемещением
     */
    Socket &operator=(Socket &&) = delete;

    /**
     * @brief Подключение к серверу
     *
     * Создаёт TCP-сокет и устанавливает соединение с указанным хостом и портом.
     *
     * @param host Адрес сервера
     * @param port Порт сервера
     * @return true если подключение успешно, иначе false
     *
     * @note При повторном вызове закрывает предыдущее соединение
     */
    bool connect(const std::string &host, int port);

    /**
     * @brief Отключение от сервера
     *
     * Закрывает активное сокет-соединение и сбрасывает состояние сокета.
     * Безопасно вызывать, даже если соединение не установлено.
     */
    void disconnect();

    /**
     * @brief Отправка данных серверу
     *
     * @param data Данные для отправки
     * @return true если данные отправлены полностью, иначе false
     *
     * @note В случае разрыва соединения автоматически закрывает сокет
     */
    bool send(const std::string &data);

    /**
     * @brief Проверка состояния подключения
     *
     * @return true если сокет подключён и программа запущена, иначе false
     */
    bool isConnected() const;

    /**
     * @brief Проверка и восстановление подключения
     *
     * Проверяет текущее состояние подключения. Если соединение отсутствует,
     * выполняет повторные попытки подключения с указанными параметрами.
     *
     * @param host Адрес сервера
     * @param port Порт сервера
     * @param maxAttempts Максимальное количество попыток подключения
     * @param retryDelayMs Задержка между попытками в миллисекундах
     * @return true если подключение установлено, иначе false
     */
    bool ensureConnection(const std::string &host, int port,
                          int maxAttempts = 3, int retryDelayMs = 1000);

    /**
     * @brief Проверка, запущен ли сокет
     *
     * @return true если сокет активен, иначе false
     */
    bool isRunning() const { return running; }

    /**
     * @brief Остановка сокета
     *
     * Устанавливает флаг running = false. После этого все операции
     * с сокетом будут возвращать ошибку.
     */
    void stop() { running = false; }

private:
    int sock;                  ///< Дескриптор сокета
    mutable std::mutex mutex;  ///< Мьютекс для потокобезопасности
    std::atomic<bool> running; ///< Флаг работы сокета
};