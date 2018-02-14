/* ollieberzs 2018
** value.cpp
** value for all oca types
*/ 

#include <iostream>
#include "value.hpp"
#include "parse.hpp"

OCA_BEGIN

std::string& Value::tos()
{
    return val;
}

int Value::toi()
{
    if (type == "int") return std::stoi(val);
    else return 0;
}

float Value::tof()
{
    if (type == "float") return std::stof(val);
    else return 0.0f;
}

bool Value::tob()
{
    if (type == "bool") return val == "true";
    else return false;
}

// ----------------------------

void Value::print(bool debug)
{
    if (debug)
    {
        std::cout << "<" << type << ">" << val;
        if (!table) return;
        std::cout << "(";
        for (const auto& pair : *table)
        {
            std::cout << pair.first << ":";
            pair.second->print(debug);
            std::cout << " ";
        }
        std::cout << ")";
    }
    else
    {
        if (type == "tup")
        {
            std::cout << "(";
            for (const auto& pair : *table)
            {
                std::cout << pair.first << ":";
                pair.second->print(debug);
                std::cout << " ";
            }
            std::cout << ")";
        }
    }
}

OCA_END