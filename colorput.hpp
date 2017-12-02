#pragma once

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
constexpr int COLORS[3] = { 0x07, 0x04, 0x03 };
#else
constexpr int COLORS[3] = { 0, 31, 36 };
#endif

// Define colors
#define RESET 0
#define RED   1
#define CYAN  2

void colorize(int color)
{
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLORS[color]);
#else
    std::cout << "\003[" << COLORS[color] << "m";
#endif
}
