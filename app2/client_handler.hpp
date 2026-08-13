#pragma once

#include <atomic>
#include <string>
#include <memory>
#include "library_loader.hpp"

/**
 * @brief Класс для обработки подключённого клиента
 */
class ClientHandler
{
public:
    /**
     * @param client
     * @param loader Загруженная библиотека
     * @param running Ссылка на флаг работы
     */
    ClientHandler(int client, LibraryLoader *loader, std::atomic<bool> &running);

    ~ClientHandler();

    /**
     * @brief Обработка клиента
     */
    void process();

    /**
     * @brief Закрытие соединения
     */
    void disconnect();

    /**
     * @brief Проверка, подключён ли клиент
     */
    bool isConnected() const { return m_client != -1; }

private:
    /**
     * @brief Обработка одного сообщения
     * @param message Полученное сообщение
     */
    void processMessage(const std::string &message);

    /**
     * @brief Вывод результата анализа
     * @param data Данные из сообщения
     * @param sum Сумма из сообщения
     * @param result Результат функции analyzeSum
     */
    void printResult(const std::string &data, int sum, bool result);

    int m_client;
    LibraryLoader *loader;
    std::atomic<bool> &running;
    std::string clientAddress;
};