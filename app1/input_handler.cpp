#include "input_handler.hpp"
#include <iostream>
#include <cctype>
#include <cstring>
#include <limits>

/**
 * @brief Конструктор InputHandler
 * @param loader Указатель на загрузчик библиотеки
 * @param buffer Ссылка на буфер
 * @param running Ссылка на флаг работы
 */
InputHandler::InputHandler(LibraryLoader *loader,
                           Buffer<std::string, 32> &buffer,
                           std::atomic<bool> &running)
    : loader(loader), buffer(buffer), running(running) {}

/**
 * @brief Основной цикл ввода и обработки данных
 *
 * Работает в отдельном потоке. Последовательно:
 * 1. Проверяет состояние std::cin (обработка EOF и ошибок)
 * 2. Запрашивает ввод строки у пользователя
 * 3. Выполняет валидацию:
 *    - строка не пустая
 *    - только цифры
 *    - длина ≤ 64 символа
 * 4. Вызывает processDigits() из динамической библиотеки
 * 5. Помещает обработанную строку в буфер
 */
void InputHandler::run()
{
    std::string input;

    while (running)
    {
        if (std::cin.eof())
        {
            std::cout << "\n[InputHandler] EOF detected" << std::endl;
            break;
        }
        // Проверка на ошибку потока ввода
        if (std::cin.fail())
        {
            std::cout << "[InputHandler] cin in fail state" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cout << "\nEnter digits: " << std::flush;
        std::getline(std::cin, input);

        if (std::cin.eof())
        {
            std::cout << "\n[InputHandler] EOF detected" << std::endl;
            break;
        }

        if (!running)
            break;

        if (input.empty())
        {
            continue;
        }
        // Валидация: строка должна состоять только из цифр
        bool onlyDigits = true;
        for (char c : input)
        {
            if (!std::isdigit(static_cast<unsigned char>(c)))
            {
                onlyDigits = false;
                break;
            }
        }

        if (!onlyDigits)
        {
            std::cout << "Error: Only digits are allowed" << std::endl;
            continue;
        }
        // Валидация: длина не должна превышать 64 символа
        if (input.size() > 64)
        {
            std::cout << "Error: String exceeds 64 characters" << std::endl;
            continue;
        }

        if (!loader || !loader->isLoaded())
        {
            std::cout << "Error: Library not loaded" << std::endl;
            continue;
        }
        // Вызов функции processDigits из библиотеки
        char output[128] = {0};
        ProcessDigitsFunc processDigits = loader->getProcessDigits();
        processDigits(input.c_str(), output, sizeof(output));

        // Обработанные данные помещаем в буфер
        std::string processed(output);
        buffer.push(std::move(processed));

        std::cout << "[InputHandler] Data sent to buffer: " << output << std::endl;
    }
}

/**
 * @brief Остановка обработки ввода
 */
void InputHandler::stop()
{
    running = false;
}
