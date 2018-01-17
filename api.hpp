#pragma once

#include "scope.hpp"

namespace oca
{
    typedef internal::Scope State;
    #define NIL new internal::Value("nil", nullptr)
    #define DLLEXPORT __declspec(dllexport) void

    void def(State& state, const std::string& name, internal::NativeMethod native)
    {
        internal::Value* val = new internal::Value("native", nullptr);
        val->refCount++;
        val->native = native;
        state.set(name, val);
    }

    int integer(internal::Value* val)
    {
        if (val->type == "integer") return std::stoi(val->expr->value);
        else 
        {
            std::cout << "value is not an integer";
            exit(0);
        }
        return 0;
    }
    float floating(internal::Value* val)
    {
        if (val->type == "float") return std::stof(val->expr->value);
        else 
        {
            std::cout << "value is not a float";
            exit(0);
        }
        return 0.0f;
    }
    bool boolean(internal::Value* val)
    {
        if (val->type == "boolean") 
        {
            if (val->expr->value == "true") return true;
            else return false;
        }
        else 
        {
            std::cout << "value is not a boolean";
            exit(0);
        }
        return false;
    }
    std::string string(internal::Value* val)
    {
        if (val->type == "string") return val->expr->value;
        else 
        {
            std::cout << "value is not a string";
            exit(0);
        }
        return "";
    }
}
