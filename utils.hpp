/* ollieberzs 2018
** utils.hpp
** file for utility functions
*/

#pragma once

#if _WIN32 || _WIN64
#include <windows.h>
#endif

#include "common.hpp"

#define ESC "\033["

OCA_BEGIN

inline void enableANSI() {
    #if _WIN32 || _WIN64
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
    #endif
}

OCA_END
