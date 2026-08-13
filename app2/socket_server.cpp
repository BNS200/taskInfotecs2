#include "socket_server.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * @brief Конструктор SocketServer
 */
SocketServer::SocketServer()
    : server(-1), running(false) {}

/**
 * @brief Деструктор SocketServer
 */
SocketServer::~SocketServer()
{
    stop();
}

/**
 * @brief Запуск сервера на указанном порту
 * @param port Порт для прослушивания
 * @return true если сервер запущен успешно, иначе false
 *
 * @details Последовательность действий:
 * 1. Проверка состояния
 * 2. Создание TCP-сокета
 * 3. Установка SO_REUSEADDR для быстрого перезапуска
 * 4. Привязка к адресу INADDR_ANY
 * 5. Начало прослушивания с очередью 1
 */
bool SocketServer::start(int port)
{
    if (running)
    {
        return true;
    }

    // Создание серверного сокета
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
    {
        std::cerr << "[SocketServer] Failed to create socket" << std::endl;
        return false;
    }

    // Настройка переиспользования адреса
    int opt = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "[SocketServer] Failed to set SO_REUSEADDR" << std::endl;
        close(server);
        server = -1;
        return false;
    }

    // Настройка адреса
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Привязка к порту
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "[SocketServer] Failed to bind to port " << port << std::endl;
        close(server);
        server = -1;
        return false;
    }

    // Начало прослушивания
    if (listen(server, 1) < 0)
    {
        std::cerr << "[SocketServer] Failed to listen" << std::endl;
        close(server);
        server = -1;
        return false;
    }

    running = true;
    std::cout << "[SocketServer] Server started on port " << port << std::endl;
    return true;
}

/**
 * @brief Остановка сервера
 */
void SocketServer::stop()
{
    running = false;
    if (server != -1)
    {
        close(server);
        server = -1;
    }
}

/**
 * @brief Принятие нового клиента
 * @return Дескриптор клиентского сокета или -1 при ошибке
 */
int SocketServer::acceptClient()
{
    if (!running || server == -1)
    {
        return -1;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Блокирующее ожидание клиента
    int clientFd = accept(server, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientFd < 0)
    {
        return -1;
    }

    // Вывод информации о клиенте
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "[SocketServer] Client connected from " << clientIP
              << ":" << ntohs(clientAddr.sin_port) << std::endl;

    return clientFd;
}