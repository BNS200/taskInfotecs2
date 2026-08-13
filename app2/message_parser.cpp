/**
 * @file message_parser.cpp
 * @brief Реализация класса MessageParser
 */

#include "message_parser.hpp"
#include <iostream>
#include <stdexcept>

/**
 * @brief Парсинг сообщения
 * @param message Строка
 * @return ParsedMessage или std::nullopt при ошибке
 * Ошибки:
 * - Пустое сообщение
 * - Отсутствует разделитель
 * - Пустое значение суммы
 * - Сумма не является числом
 * - Сумма выходит за пределы int
 */
std::optional<ParsedMessage> MessageParser::parse(const std::string &message)
{
    // Проверка на пустое сообщение
    if (message.empty())
    {
        std::cerr << "[MessageParser] Empty message" << std::endl;
        return std::nullopt;
    }

    // Поиск разделителя '|'
    size_t pos = message.find('|');
    if (pos == std::string::npos)
    {
        std::cerr << "[MessageParser] Invalid format: no separator '|' found" << std::endl;
        return std::nullopt;
    }

    // Извлечение частей сообщения
    std::string data = message.substr(0, pos);
    std::string sumStr = message.substr(pos + 1);

    // Проверка, что сумма не пустая
    if (sumStr.empty())
    {
        std::cerr << "[MessageParser] Empty sum value" << std::endl;
        return std::nullopt;
    }

    // Парсинг суммы с обработкой ошибок
    try
    {
        int sum = std::stoi(sumStr);

        ParsedMessage result;
        result.data = data;
        result.sum = sum;

        return result;
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "[MessageParser] Invalid sum value: " << sumStr << std::endl;
        return std::nullopt;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "[MessageParser] Sum value out of range: " << sumStr << std::endl;
        return std::nullopt;
    }
}

/**
 * @brief Валидация формата сообщения
 * @param message Строка для проверки
 * @return true если формат корректен, иначе false
 */
bool MessageParser::validate(const std::string &message)
{
    // Проверка на пустое сообщение
    if (message.empty())
    {
        return false;
    }

    // Проверка наличия разделителя
    size_t pos = message.find('|');
    if (pos == std::string::npos)
    {
        return false;
    }

    // Проверка, что данные до разделителя не пустые
    std::string data = message.substr(0, pos);
    if (data.empty())
    {
        return false;
    }

    // Проверка, что сумма не пустая
    std::string sumStr = message.substr(pos + 1);
    if (sumStr.empty())
    {
        return false;
    }

    // Проверка, что сумма это корректное число
    try
    {
        std::stoi(sumStr);
        return true;
    }
    catch (...)
    {
        return false;
    }
}