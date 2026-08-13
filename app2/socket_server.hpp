#pragma once

#include <atomic>
#include <string>

/**
 * @brief Класс для управления серверным сокетом
 *
 * Создаёт TCP-сервер
 */
class SocketServer
{
public:
    SocketServer();
    ~SocketServer();

    /**
     * @brief Запуск сервера на указанном порту
     * @param port Порт для прослушивания
     * @return true если сервер запущен успешно
     */
    bool start(int port);

    /**
     * @brief Остановка сервера
     */
    void stop();

    /**
     * @brief Принятие нового клиента (блокирующий)
     * @return Дескриптор клиентского сокета или -1 при ошибке
     */
    int acceptClient();

    /**
     * @brief Проверка, запущен ли сервер
     */
    bool isRunning() const { return running; }

private:
    int server;
    std::atomic<bool> running;
};