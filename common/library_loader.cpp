#include "library_loader.hpp"
#include <iostream>
#include <dlfcn.h>

/**
 * @brief Конструктор LibraryLoader
 */
LibraryLoader::LibraryLoader()
    : handle(nullptr), loaded(false), processDigits(nullptr), sumDigits(nullptr), analyzeSum(nullptr) {}

/**
 * @brief Деструктор LibraryLoader
 */
LibraryLoader::~LibraryLoader()
{
    if (handle)
    {
        dlclose(handle);
        handle = nullptr;
    }
}

/**
 * @brief Загрузка библиотеки
 * @param path Путь к библиотеке
 * @return true если загрузка успешна, иначе false
 *
 * @details Последовательность действий:
 * 1. Закрытие старой библиотеки
 * 2. Загрузка библиотеки
 * 3. Получение processDigits
 * 4. Получение sumDigits
 * 5. Получение analyzeSum
 * 6. Проверка всех указателей
 */
bool LibraryLoader::load(const std::string &path)
{
    // Закрытие старой библиотеки
    if (handle)
    {
        dlclose(handle);
        handle = nullptr;
        loaded = false;
    }

    // Загрузка библиотеки
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
    {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return false;
    }

    // Очистка ошибок
    dlerror();

    // Загрузка функции processDigits
    processDigits = (ProcessDigitsFunc)dlsym(handle, "processDigits");
    const char *error = dlerror();
    if (error)
    {
        std::cerr << "Failed to load processDigits: " << error << std::endl;
        dlclose(handle);
        handle = nullptr;
        return false;
    }

    // Загрузка функции sumDigits
    sumDigits = (SumDigitsFunc)dlsym(handle, "sumDigits");
    error = dlerror();
    if (error)
    {
        std::cerr << "Failed to load sumDigits: " << error << std::endl;
        dlclose(handle);
        handle = nullptr;
        return false;
    }

    // Загрузка функции analyzeSum
    analyzeSum = (AnalyzeSumFunc)dlsym(handle, "analyzeSum");
    error = dlerror();
    if (error)
    {
        std::cerr << "Failed to load analyzeSum: " << error << std::endl;
        dlclose(handle);
        handle = nullptr;
        return false;
    }

    loaded = true;
    std::cout << "Library loaded successfully" << std::endl;
    return true;
}