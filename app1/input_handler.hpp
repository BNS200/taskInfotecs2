#pragma once

#include <atomic>
#include <string>
#include "spsc_buffer.hpp"
#include "library_loader.hpp"

/**
 * @brief Класс для обработки ввода пользователя
 *
 * Поток 1: Принимает строку от пользователя, проверяет,
 * обрабатывает через функцию 1 и помещает в буфер
 */
class InputHandler
{
public:
    /**
     * @param loader Загруженная библиотека
     * @param buffer Ссылка на буфер
     * @param running Ссылка на флаг работы
     */
    InputHandler(LibraryLoader *loader,
                 Buffer<std::string, 32> &buffer,
                 std::atomic<bool> &running);

    /**
     * @brief Запуск обработки ввода
     */
    void run();

    /**
     * @brief Остановка обработки
     */
    void stop();

private:
    /**
     * @brief Проверка, что строка состоит только из цифр
     */
    bool isOnlyDigits(const std::string &str) const;

    LibraryLoader *loader;
    Buffer<std::string, 32> &buffer;
    std::atomic<bool> &running;
};