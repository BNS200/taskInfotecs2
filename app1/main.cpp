#include <iostream>
#include <thread>
#include <signal.h>
#include "spsc_buffer.hpp"
#include "library_loader.hpp"
#include "socket_client.hpp"
#include "input_handler.hpp"
#include "data_processor.hpp"

/**
 * @brief Буфер для обмена данными между потоками
 *
 * SPSC (Single Producer, Single Consumer) буфер ёмкостью 32 элемента.
 * Использует семафоры для эффективной блокировки потоков.
 */
Buffer<std::string, 32> buffer;

/**
 * @brief Загрузчик динамической библиотеки
 */
LibraryLoader loader;

/**
 * @brief TCP-сокет для связи с Программой 2
 */
Socket m_socket;

/**
 * @brief Флаг работы программы
 */
std::atomic<bool> running(true);

/**
 * @brief Умный указатель на обработчик ввода
 */
std::unique_ptr<InputHandler> inputHandler;

/**
 * @brief Умный указатель на обработчик данных
 */
std::unique_ptr<DataProcessor> dataProcessor;

/**
 * @brief Обработчик сигналов завершения
 *
 * Вызывается при получении SIGINT (Ctrl+C) или SIGTERM.
 * Выполняет корректное завершение программы:
 * 1. Устанавливает флаг running = false
 * 2. Останавливает обработчики (InputHandler и DataProcessor)
 * 3. Прерывает буфер для разблокировки ожидающих потоков
 */
void signalHandler(int signum)
{
    std::cout << "\n[Main] Received signal " << signum << ", shutting down" << std::endl;
    running = false;

    // Остановка обработчиков
    if (inputHandler)
    {
        inputHandler->stop();
    }
    if (dataProcessor)
    {
        dataProcessor->stop();
    }

    // Прерывание буфера для разблокировки потоков
    buffer.interrupt();
}

/**
 * @brief Настройка обработчиков сигналов
 */
void setupSignalHandlers()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

/**
 * @brief Главная функция программы
 *
 * @details Последовательность выполнения:
 * 1. Настройка обработчиков сигналов
 * 2. Загрузка динамической библиотеки
 * 3. Создание объектов InputHandler и DataProcessor
 * 4. Запуск потоков ввода и обработки
 * 5. Ожидание завершения потоков
 * 6. Завершение программы
 */
int main()
{
    // Настройка обработчиков сигналов
    setupSignalHandlers();

    // Загрузка динамической библиотеки
    if (!loader.load("./lib.so"))
    {
        std::cerr << "[Main] Failed to load library" << std::endl;
        return 1;
    }

    // Создание объектов-обработчиков
    inputHandler = std::make_unique<InputHandler>(&loader, buffer, running);
    dataProcessor = std::make_unique<DataProcessor>(&loader, buffer, m_socket, running);

    // Запуск потоков
    std::thread inputThread(&InputHandler::run, inputHandler.get());
    std::thread processorThread(&DataProcessor::run, dataProcessor.get());

    std::cout << "[Main] Threads started." << std::endl;

    // Ожидание завершения потоков
    if (inputThread.joinable())
    {
        inputThread.join();
    }
    if (processorThread.joinable())
    {
        processorThread.join();
    }

    std::cout << "[Main] Shutdown complete." << std::endl;
    return 0;
}