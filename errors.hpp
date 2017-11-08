#pragma once

#include <sstream>
#include <string>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

inline void colorize(int color)
{
#if defined(_WIN32) || defined(_WIN64)
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#endif
}

struct Error
{
  std::stringstream stream;

  ~Error()
  {
    colorize(0x04);
    #if defined(__linux)
    std::cout << "\033[31m";
    #endif
    std::cout << stream.str() << '\n';
    #if defined(__linux)
    std::cout << "\033[0m";
    #endif
    // Reset color
    colorize(0x07);
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
