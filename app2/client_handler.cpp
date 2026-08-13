#include "client_handler.hpp"
#include "message_parser.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * @brief Конструктор ClientHandler
 * @param client Дескриптор клиентского сокета
 * @param loader Указатель на загрузчик библиотеки
 * @param running Ссылка на флаг работы программы
 */
ClientHandler::ClientHandler(int client, LibraryLoader *loader, std::atomic<bool> &running)
    : m_client(client), loader(loader), running(running)
{
    // Получение адреса клиента для логирования
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    if (getpeername(client, (struct sockaddr *)&addr, &addrLen) == 0)
    {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
        clientAddress = std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
    }
}

/**
 * @brief Деструктор ClientHandler
 */
ClientHandler::~ClientHandler()
{
    disconnect();
}

/**
 * @brief Основной цикл обработки клиента
 *
 * Работает в отдельном потоке. Последовательно:
 * 1. Читает данные из сокета
 * 2. Обрабатывает сообщение через processMessage()
 * 3. При разрыве соединения или ошибке завершает работу
 */
void ClientHandler::process()
{
    if (m_client == -1)
    {
        std::cerr << "[ClientHandler] Invalid socket" << std::endl;
        return;
    }

    char buffer[256];

    while (running && m_client != -1)
    {
        // Очистка буфера
        memset(buffer, 0, sizeof(buffer));

        // Чтение данных от клиента
        int bytes = recv(m_client, buffer, sizeof(buffer) - 1, 0);

        // Проверка на закрытие соединения (graceful shutdown)
        if (bytes == 0)
        {
            std::cout << "[ClientHandler] Client " << clientAddress
                      << " disconnected" << std::endl;
            break;
        }

        // Проверка на ошибку чтения
        if (bytes < 0)
        {
            std::cerr << "[ClientHandler] Recv error: " << strerror(errno) << std::endl;
            break;
        }

        // Обработка полученного сообщения
        std::string message(buffer);
        processMessage(message);
    }

    disconnect();
}

/**
 * @brief Обработка одного сообщения от клиента
 * @param message Полученное сообщение
 */
void ClientHandler::processMessage(const std::string &message)
{
    // Парсинг сообщения
    auto parsed = MessageParser::parse(message);
    if (!parsed.has_value())
    {
        std::cout << "[ClientHandler] Invalid message from " << clientAddress
                  << ": " << message << std::endl;
        return;
    }

    // Проверка, что библиотека загружена
    if (!loader || !loader->isLoaded())
    {
        std::cerr << "[ClientHandler] Library not loaded" << std::endl;
        return;
    }

    // Вызов функции 3 из библиотеки
    AnalyzeSumFunc analyzeSum = loader->getAnalyzeSum();
    bool result = analyzeSum(parsed->data.c_str());

    // Вывод результата
    printResult(parsed->data, parsed->sum, result);
}

/**
 * @brief Вывод результата анализа
 * @param data Данные из сообщения
 * @param sum Сумма из сообщения
 * @param result Результат функции analyzeSum()
 */
void ClientHandler::printResult(const std::string &data, int sum, bool result)
{
    std::cout << "[ClientHandler] Received from " << clientAddress << ":" << std::endl;
    std::cout << "  Data: " << data << std::endl;
    std::cout << "  Sum: " << sum << std::endl;
    std::cout << "  Analysis: ";

    if (result)
    {
        std::cout << "TRUE" << std::endl;
        std::cout << "  Condition: sum > 2 AND sum % 32 == 0" << std::endl;
        std::cout << "  " << sum << " > 2 AND " << sum << " % 32 == 0" << std::endl;
        std::cout << "  Result: Data is VALID" << std::endl;
    }
    else
    {
        std::cout << "FALSE" << std::endl;
        std::cout << "  Condition: sum > 2 AND sum % 32 == 0" << std::endl;
        std::cout << "  " << sum << " > 2 AND " << sum << " % 32 == 0" << std::endl;
        std::cout << "  Result: Data is INVALID" << std::endl;
    }
}

/**
 * @brief Закрытие соединения с клиентом
 */
void ClientHandler::disconnect()
{
    if (m_client != -1)
    {
        close(m_client);
        m_client = -1;
        std::cout << "[ClientHandler] Client " << clientAddress << " disconnected" << std::endl;
    }
}