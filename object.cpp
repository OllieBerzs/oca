/* ollieberzs 2018
** object.cpp
** object for all oca types
*/

#include <iostream>
#include <sstream>
#include "object.hpp"
#include "parse.hpp"

OCA_BEGIN

Object::Object(const std::string& v)
{
    type = "str";
    val = v;
}

Object::Object(int v)
{
    type = "int";
    val = std::to_string(v);
}

Object::Object(float v)
{
    type = "float";
    val = std::to_string(v);
}

Object::Object(bool v)
{
    type = "bool";
    val = v ? "true" : "false";
}

Object::Object(NativeMethod v)
{
    type = "native";
    nat = v;
}

// -----------------------------

std::string Object::tos(bool debug)
{
    std::string result = "";
    if (type == "tup") // is tuple
    {
        if (debug) result += "<tup>";
        result += "(";
        for (auto& pair : table)
        {
            if (debug) result += "[" + pair.first + "]";
            result += pair.second->tos(debug) + " ";
        }
        if (table.size() > 0) result.pop_back();
        result += ")";
    }
    else
    {
        if (debug) result += "<" + type + ">";
        if (type == "native")
        {
            const void* address = static_cast<const void*>(&nat);
            std::stringstream ss;
            ss << address;
            result += ss.str();
        }
        else if (type == "block")
        {
            const void* address = static_cast<const void*>(block.get());
            std::stringstream ss;
            ss << address;
            result += ss.str();
        }
        else result += val;
    }
    return result;
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

OCA_END
