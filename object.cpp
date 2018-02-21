/* ollieberzs 2018
** object.cpp
** object for all oca types
*/ 

#include <iostream>
#include "object.hpp"
#include "parse.hpp"

OCA_BEGIN

Object::Object(const std::string& v)
{
    Value str;
    str.type = "str";
    str.val = v;
    table.emplace("self", str);
}

Object::Object(int v)
{
    Value i;
    i.type = "int";
    i.val = v;
    table.emplace("self", i);
}

Object::Object(float v)
{
    Value f;
    f.type = "float";
    f.val = v;
    table.emplace("self",f);
}

Object::Object(bool v)
{
    Value b;
    b.type = "bool";
    b.val = v;
    table.emplace("self", b);
}

Object::Object(NativeMethod v)
{
    Value n;
    n.type = "native";
    n.nat = v;
    table.emplace("self", n);
}

// -----------------------------

std::string Object::tos()
{
    return "";
}

int Object::toi()
{
    return 0;
}

float Object::tof()
{
    return 0.0f;
}

bool Object::tob()
{
    return false;
}

// ----------------------------

void Object::print(bool debug)
{
    if (table.find("self") == table.end()) // is tuple
    {
        std::cout << "(";
        for (auto pair : table)
        {
            if (debug) std::cout << "<" << pair.second.type 
                << ":" << pair.first << ">";
            if (pair.second.type == "native") std::cout << &pair.second.nat;
            else if (pair.second.type == "block") std::cout << pair.second.block;
            else std::cout << pair.second.val;
            std::cout << " ";
        }
        std::cout << ")";
    }
    else
    {
        Value self = (*table.find("self")).second;
        if (debug) std::cout << "<" <<  self.type << ">";
        if (self.type == "native") std::cout << &self.nat;
        else if (self.type == "block") std::cout << self.block;
        else std::cout << self.val;
    }
}

OCA_END