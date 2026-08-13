#include "socket_client.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <cstring>
#include <errno.h>

/**
 * @brief Конструктор Socket
 */
Socket::Socket()
    : sock(-1), running(true) {}

/**
 * @brief Деструктор Socket
 */
Socket::~Socket()
{
    disconnect();
}

/**
 * @brief Подключение к серверу
 *
 * @param host Адрес сервера
 * @param port Порт сервера
 * @return true если подключение успешно, иначе false
 *
 * @details Алгоритм работы:
 * 1. Блокировка мьютекса для потокобезопасности
 * 2. Проверка состояния running
 * 3. Закрытие старого сокета
 * 4. Создание нового сокета
 * 5. Настройка адреса
 * 6. Установка соединения
 */
bool Socket::connect(const std::string &host, int port)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (!running)
    {
        return false;
    }

    // Закрытие старого сокета
    if (sock != -1)
    {
        close(sock);
        sock = -1;
    }

    // Создание нового сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "[Socket] Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Настройка адреса
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
    {
        std::cerr << "[Socket] Invalid address: " << host << std::endl;
        close(sock);
        sock = -1;
        return false;
    }

    // Установка соединения
    if (::connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "[Socket] Connection failed: " << strerror(errno) << std::endl;
        close(sock);
        sock = -1;
        return false;
    }

    return true;
}

/**
 * @brief Отключение от сервера
 * Потокобезопасно закрывает сокет.
 * Безопасно вызывать, даже если соединение не установлено.
 */
void Socket::disconnect()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (sock != -1)
    {
        close(sock);
        sock = -1;
    }
}

/**
 * @brief Отправка данных серверу
 * @param data Данные для отправки
 * @return true если данные отправлены полностью, иначе false
 */
bool Socket::send(const std::string &data)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (sock == -1 || !running)
    {
        return false;
    }

    ssize_t sent = ::send(sock, data.c_str(), data.size(), MSG_NOSIGNAL);
    if (sent < 0)
    {
        if (errno == EPIPE || errno == ECONNRESET)
        {
            close(sock);
            sock = -1;
        }
        return false;
    }

    return sent == static_cast<ssize_t>(data.size());
}

/**
 * @brief Проверка состояния подключения
 * @return true если сокет подключён и запущен, иначе false
 */
bool Socket::isConnected() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return sock != -1 && running;
}

/**
 * @brief Проверка и восстановление подключения
 *
 * @param host Адрес сервера
 * @param port Порт сервера
 * @param maxAttempts Максимальное количество попыток
 * @param retryDelayMs Задержка между попытками
 * @return true если подключение установлено, иначе false
 *
 * @details При успешном установлении соединения возвращает true.
 * Если все попытки неудачны, сокет закрывается и возвращается false.
 */
bool Socket::ensureConnection(const std::string &host, int port,
                              int maxAttempts, int retryDelayMs)
{
    if (isConnected())
    {
        return true;
    }

    if (!running)
    {
        return false;
    }

    for (int attempt = 0; attempt < maxAttempts && running; ++attempt)
    {
        if (connect(host, port))
        {
            return true;
        }

        if (running && attempt < maxAttempts - 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
        }
    }

    if (sock != -1)
    {
        close(sock);
        sock = -1;
    }

    return false;
}