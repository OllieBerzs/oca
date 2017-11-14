#pragma once

#include <sstream>
#include <iostream>

struct Error
{
    static std::string script;
    static unsigned int lineNum;

    std::stringstream stream;

    std::string getLine();

    ~Error();
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
