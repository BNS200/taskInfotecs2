#include "data_processor.hpp"
#include <iostream>
#include <chrono>
#include <thread>

/**
 * @brief Конструктор DataProcessor
 * @param loader Указатель на загрузчик библиотеки
 * @param buffer Ссылка на буфер
 * @param socket_val Ссылка на сокет для отправки данных в Программу №2
 * @param running Ссылка на флаг работы программы
 */
DataProcessor::DataProcessor(LibraryLoader *loader,
                             Buffer<std::string, 32> &buffer,
                             Socket &socket_val,
                             std::atomic<bool> &running)
    : loader(loader), buffer(buffer), m_socket(socket_val), running(running)
{
}

/**
 * @brief Основной цикл обработки данных
 * Работает в отдельном потоке (Consumer). Последовательно:
 * 1. Извлекает данные из буфера
 * 2. Выводит обработанные данные на экран
 * 3. Вычисляет сумму цифр через функцию sumDigits() из библиотеки
 * 4. Отправляет результат в Программу №2 через сокет
 */
void DataProcessor::run()
{
    while (running)
    {
        // Блокирующее извлечение данных из буфера
        std::string data = buffer.pop();

        if (!running)
            break;

        std::cout << "\n[DataProcessor] Processed data: " << data << std::endl;

        if (!loader || !loader->isLoaded())
        {
            std::cout << "[DataProcessor] Library not loaded" << std::endl;
            continue;
        }
        // Вызов функции sumDigits из динамической библиотеки
        SumDigitsFunc sumDigits = loader->getSumDigits();
        size_t sum = sumDigits(data.c_str());
        std::cout << "[DataProcessor] Sum of digits: " << sum << std::endl;

        // Отправка данных в Программу №2
        bool sent = sendToProgram2(data, sum);
        if (!sent)
        {
            std::cout << "[DataProcessor] Data not sent, will try again later" << std::endl;
        }
    }
}

/**
 * @brief Остановка обработки данных
 */
void DataProcessor::stop()
{
    running = false;
    buffer.interrupt();
}

/**
 * @brief Отправка данных в Программу №2
 *
 * @param data Обработанные данные
 * @param sum Сумма цифр
 * @return true если отправка успешна, иначе false
 *
 * @details Алгоритм работы:
 * 1. Формирует сообщение
 * 2. Проверяет состояние сокета
 * 3. При необходимости выполняет переподключение
 * 4. Отправляет данные через сокет
 *
 * @note При неудачной отправке сокет отключается для последующего переподключения
 */
bool DataProcessor::sendToProgram2(const std::string &data, size_t sum)
{
    std::string message = data + "|" + std::to_string(sum);

    // Проверяем, запущен ли сокет
    if (!m_socket.isRunning())
    {
        std::cout << "[DataProcessor] Socket is not running" << std::endl;
        return false;
    }

    // Проверяем подключение
    if (!m_socket.isConnected())
    {
        std::cout << "[DataProcessor] Not connected to app2, attempting reconnect" << std::endl;

        // Пытаемся подключиться
        bool connected = m_socket.ensureConnection("127.0.0.1", 8080, 1, RETRY_DELAY_MS);

        if (!connected)
        {
            std::cout << "[DataProcessor] Failed to connect to app2" << std::endl;
            return false;
        }
    }

    // Отправляем данные
    bool sent = m_socket.send(message);
    if (sent)
    {
        std::cout << "[DataProcessor] Data sent to app2" << std::endl;
    }
    else
    {
        std::cout << "[DataProcessor] Failed to send data" << std::endl;
        m_socket.disconnect();
    }

    return sent;
}