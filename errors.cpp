#include "errors.hpp"

#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

std::string Error::script = "";
unsigned int Error::lineNum = 1;

std::string Error::getLine()
{
    std::string ret = "";
    unsigned int line = 1;
    for (char c : script)
    {
        if (c == '\n')
        {
            if (line == lineNum)
            {
                break;
            }
            line++;
            ret = "";
        }
        else
        {
            ret += c;
        }
    }
    return ret;
}


Error::~Error()
{
    // Set message color to red
    #if defined(_WIN32) || defined(_WIN64)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x04);
    #elif defined(__linux)
    std::cout << "\033[31m";
    #endif

    std::cout << "<line " << lineNum << "> " << getLine() << "\n-- " << stream.str() << "\n";

    // Reset color
    #if defined(_WIN32) || defined(_WIN64)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
    #elif defined(__linux)
    std::cout << "\033[0m";
    #endif

    exit(0);
}
