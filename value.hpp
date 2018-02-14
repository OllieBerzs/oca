/* ollieberzs 2018
** value.hpp
** value for all oca types
*/ 

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

#include "common.hpp"
#include "scope.hpp"

OCA_BEGIN

struct Value;
struct Expression;

typedef std::shared_ptr<Value> ValuePtr;
typedef std::shared_ptr<Expression> ExprPtr;
typedef std::function<ValuePtr(std::vector<ValuePtr>)> NativeMethod;

struct Value
{
    std::string type;
    std::string val;

    std::shared_ptr<std::map<std::string, ValuePtr>> table;

    NativeMethod native;
    ExprPtr block;

    std::string& tos();
    int toi();
    float tof();
    bool tob();

    void print(bool debug);
};

OCA_END