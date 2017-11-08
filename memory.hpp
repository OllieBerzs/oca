#pragma once

#include <string>

class Memory
{
    static std::string _mem;

public:
    static void add(char c);
    static void rem(char c);
    static std::string get();
};
