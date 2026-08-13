#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Обработка строки: сортировка по убыванию и замена чётных цифр на "KB"
     *
     * @param digits Входная строка
     * @param output Буфер для результата
     * @param size Размер выходного буфера
     */
    void processDigits(const char *digits, char *output, size_t size);

    /**
     * @brief Вычисление суммы всех цифр в строке
     *
     * @param symbols Входная строка
     */
    size_t sumDigits(const char *symbols);

    /**
     * @brief Анализ суммы цифр в строке
     *
     * @param symbols Входная строка
     */
    bool analyzeSum(const char *symbols);

#ifdef __cplusplus
}
#endif