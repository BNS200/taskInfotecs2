#pragma once

#include <string>
#include <optional>
#include <tuple>

/**
 * @brief Структура для хранения распарсенного сообщения
 */
struct ParsedMessage
{
    std::string data; // Строка данных
    int sum;          // Сумма цифр
};

/**
 * @brief Класс для парсинга сообщений от Программы №1
 */
class MessageParser
{
public:
    /**
     * @brief Парсинг сообщения
     * @param message Строка для парсинга
     * @return ParsedMessage если парсинг успешен, иначе std::nullopt
     */
    static std::optional<ParsedMessage> parse(const std::string &message);

    /**
     * @brief Валидация формата сообщения
     * @param message Строка для проверки
     * @return true если формат корректен
     */
    static bool validate(const std::string &message);
};