#pragma once

#include <string>
#include <vector>
#include <functional>
#include "expression.hpp"

namespace oca::internal
{
    struct Value;
    typedef std::function<Value*(std::vector<Value*>)> NativeMethod;
    struct Value
    {
        std::string type;
        Expression* expr;
        unsigned int refCount;
        NativeMethod native;

        Value(const std::string& type, Expression* expr) 
            : type(type), expr(expr), refCount(0) {}
        ~Value()
        {
            if (expr) delete expr;
        }
    };

} // namespace oca::internal