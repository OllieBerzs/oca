/* ollieberzs 2018
** object.hpp
** object for all oca types
*/ 

#pragma once

#include <string>
#include <map>

#include "common.hpp"
#include "scope.hpp"

OCA_BEGIN

struct Value
{
    std::string type;
    std::string val;
    NativeMethod nat;
    ExprPtr block;
};

struct Object
{
    std::map<std::string, Value> table;

    Object() = default;
    Object(const std::string& v);
    Object(int v);
    Object(float v);
    Object(bool v);
    Object(NativeMethod v);

    std::string tos();
    int toi();
    float tof();
    bool tob();

    void print(bool debug);
};

OCA_END