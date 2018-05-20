/* ollieberzs 2018
** utils.hpp
** file for utility functions
*/

#pragma once

#if _WIN32 || _WIN64
#include <windows.h>
#endif

#include <any>
#include <iostream>
#include "common.hpp"
#include "value.hpp"

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

inline ValuePtr cast(std::any val) {
    if (val.type() == typeid(int)) {
        return std::make_shared<Integer>(std::any_cast<int>(val), nullptr);
    } else if (val.type() == typeid(float)) {
        return std::make_shared<Real>(std::any_cast<float>(val), nullptr);
    } else if (val.type() == typeid(bool)) {
        return std::make_shared<Bool>(std::any_cast<bool>(val), nullptr);
    } else if (val.type() == typeid(std::string)) {
        return std::make_shared<String>(std::any_cast<std::string>(val), nullptr);
    } else if (val.type() == typeid(std::vector<int>)) {
        auto vec = std::any_cast<std::vector<int>>(val);
        auto table = std::make_shared<Table>(nullptr);
        for (uint i = 0; i < vec.size(); ++i) {
            ++static_cast<Table&>(*table).count;
            ++static_cast<Table&>(*table).size;
            table->scope.set(
                std::to_string(i + ARRAY_BEGIN_INDEX),
                std::make_shared<Integer>(vec[i], &table->scope), true);
        }
        return table;
    } else {
        std::cout << "Error: Bad cast.\n";
        return Nil::in(nullptr);
    }
}

OCA_END
