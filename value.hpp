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
    ExprPtr val;
    NativeMethod native;
    Scope table;

    Value();

    static ValuePtr makeStr(ExprPtr v);
    static ValuePtr makeInt(ExprPtr v);
    static ValuePtr makeFloat(ExprPtr v);
    static ValuePtr makeBool(ExprPtr v);
    static ValuePtr makeBlock(ExprPtr v);
    static ValuePtr makeMeth(NativeMethod v);

    std::string tos();
    int toi();
    float tof();
    bool tob();
};

OCA_END