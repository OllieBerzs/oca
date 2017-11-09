#pragma once

#include <sstream>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

struct Error
{
    std::stringstream stream;

    ~Error()
    {
        // Set message color to red
        #if defined(_WIN32) || defined(_WIN64)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x04);
        #elif defined(__linux)
        std::cout << "\033[31m";
        #endif

        std::cout << stream.str() << '\n';

        // Reset color
        #if defined(_WIN32) || defined(_WIN64)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
        #elif defined(__linux)
        std::cout << "\033[0m";
        #endif

        exit(0);
    }
};

template <typename T>
Error& operator<<(Error& e, T&& t)
{
    e.stream << std::forward<T>(t);
    return e;
}
template <typename T>
Error& operator<<(Error&& e, T&& t)
{
    return e << std::forward<T>(t);
}

#define ERR Error()
