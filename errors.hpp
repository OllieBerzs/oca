#pragma once

#include <string>

struct Error
{
    static std::string script;
    static std::string message;
    static unsigned int line;
    static unsigned int column;

    static std::string getLine();
    static void panic();
};
