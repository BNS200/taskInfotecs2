#include "lib.h"

#include <ranges>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <cctype>
#include <string>
#include <string_view>

/**
 * @brief Обработка строки: сортировка по убыванию и замена чётных цифр
 *
 * @param digits Входная строка
 * @param output Буфер для результата
 * @param size Размер выходного буфера
 *
 * @details Использует конвейер для обработки:
 */
void processDigits(const char *digits, char *output, size_t size)
{
    // Проверка валидности входных данных
    if (!digits || !output || size == 0)
        return;

    // Лямбда для сортировки входной строки по убыванию
    const auto sorted_source = [=]()
    {
        std::string source(digits);
        std::ranges::sort(source, std::ranges::greater{});
        return source;
    };

    // Лямбда для замены чётных цифр на "KB"
    const auto replace_even_digits = [](char c) -> std::string
    {
        // Проверка, что символ является цифрой и чётной
        if (std::isdigit(static_cast<unsigned char>(c)) && (c - '0') % 2 == 0)
        {
            return std::string("KB"); // Чётная цифра → "KB"
        }
        return std::string(1, c); // Нечётная или не цифра → оставляем
    };

    // Конвейер обработки
    auto pipeline = sorted_source() | std::views::transform(replace_even_digits) | std::views::join | std::views::take(size - 1);

    // Копирование результата в выходной буфер
    auto result = std::ranges::copy(pipeline, output);
    *result.out = '\0';
}

/**
 * @brief Вычисление суммы всех цифр в строке
 *
 * @param symbols Входная строка
 *
 * @details Использует конвейер для обработки:
 */
size_t sumDigits(const char *symbols)
{
    // Проверка валидности входных данных
    if (!symbols)
        return 0;

    std::string_view input(symbols);
    if (input.empty())
        return 0;

    // Фильтр: оставляет только символы-цифры
    const auto only_digits = std::views::filter([](char c)
                                                { return std::isdigit(static_cast<unsigned char>(c)); });

    // Преобразование: char → числовое значение
    const auto to_numeric_value = std::views::transform([](char c) -> int
                                                        { return c - '0'; });

    // Конвейер обработки
    auto digits = input | only_digits | to_numeric_value;

    // Суммирование всех значений
    return std::accumulate(digits.begin(), digits.end(), 0);
}

/**
 * @brief Анализ суммы цифр в строке
 *
 * @param symbols Входная строка
 */
bool analyzeSum(const char *symbols)
{
    // Проверка валидности входных данных
    if (!symbols)
        return false;

    std::string_view input(symbols);

    // Длина строки > 2 символа
    if (input.length() <= 2)
        return false;

    // Вычисление суммы цифр
    size_t sum = sumDigits(input.data());

    // Сумма > 2 и сумма кратна 32
    return (sum > 2) && (sum % 32 == 0);
}