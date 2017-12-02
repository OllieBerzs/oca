#pragma once

#include <fstream>
#include <string>
#include "errors.hpp"

std::string file(const std::string& path)
{
    std::ifstream file(path);
    Error::message = "Could not open file " + path;
    if (!file.is_open()) Error::panic();

    std::string result((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();
    return result;
}
