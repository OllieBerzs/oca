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

    bool checkArgs(const std::string& pattern, std::vector<internal::Value*> args)
    {
        if (pattern.size() > args.size() || args.size() > pattern.size())
        {
            std::cout << "method expected " << pattern.size() << " arguments " << args.size() << " were given\n";
            exit(0);
        }
        for (unsigned int i = 0; i < pattern.size(); i++)
        {
            if (pattern[i] == 'i' && args[i]->type != "integer") return false;
            else if (pattern[i] == 'f' && args[i]->type != "float") return false;
            else if (pattern[i] == 'b' && args[i]->type != "boolean") return false;
            else if (pattern[i] == 's' && args[i]->type != "string") return false;
        }
        return true;
    }

    int toi(internal::Value* val)
    {
        if (val->type == "integer") return std::stoi(val->expr->value);
        else 
        {
            std::cout << "value is not an integer";
            exit(0);
        }
        return 0;
    }

    float tof(internal::Value* val)
    {
        if (val->type == "float") return std::stof(val->expr->value);
        else 
        {
            std::cout << "value is not a float";
            exit(0);
        }
        return 0.0f;
    }

    bool tob(internal::Value* val)
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

    std::string tos(internal::Value* val)
    {
        if (val->type == "string") return val->expr->value;
        else 
        {
            std::cout << "value is not a string";
            exit(0);
        }
        return "";
    }

    internal::Value* tov(int i)
    {
        internal::Expression* e = new internal::Expression("integer", std::to_string(i));
        return new internal::Value("integer", e);
    }
}
