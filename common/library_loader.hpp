#pragma once

#include <string>

// Типы функций из библиотеки
typedef void (*ProcessDigitsFunc)(const char *, char *, size_t);
typedef size_t (*SumDigitsFunc)(const char *);
typedef bool (*AnalyzeSumFunc)(const char *);

/**
 * @brief Класс для загрузки динамической библиотеки
 *
 * Загружает lib.so и получает указатели на функции:
 * - processDigits
 * - sumDigits
 * - analyzeSum
 */
class LibraryLoader
{
public:
    LibraryLoader();
    ~LibraryLoader();

    /**
     * @brief Загрузка библиотеки
     * @param path Путь к библиотеке
     * @return true если загрузка успешна
     */
    bool load(const std::string &path);

    /**
     * @brief Проверка, загружена ли библиотека
     */
    bool isLoaded() const { return loaded; }

    //  Геттеры
    ProcessDigitsFunc getProcessDigits() const { return processDigits; }
    SumDigitsFunc getSumDigits() const { return sumDigits; }
    AnalyzeSumFunc getAnalyzeSum() const { return analyzeSum; }

private:
    void *handle;
    bool loaded;

    // Указатели на функции
    ProcessDigitsFunc processDigits;
    SumDigitsFunc sumDigits;
    AnalyzeSumFunc analyzeSum;
};