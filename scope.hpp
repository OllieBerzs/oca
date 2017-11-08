#pragma once

#include <map>
#include "errors.hpp"
#include "object.hpp"

class Scope
{
    std::map<std::string, Method*> names;
    Scope* parent;

public:
    Scope(Scope* parent) : parent(parent) {}

    void clean()
    {
        for (auto elem : names) delete elem.second;
        names.clear();
    }

    ~Scope()
    {
        clean();
    }

    void set(const std::string& name, Method* value)
    {
        names.emplace(name, value);
    }

    Method* get(const std::string& name)
    {
        auto met = names.find(name);
        if (met == names.end())
        {
            if (parent) return parent->get(name);
            else return nullptr;
        }
        else
        {
            return met->second;
        }
        return met->second;
    }

    Method* operator[](const std::string& name)
    {
        Method* met = get(name);
        if (met)
        {
            return met;
        }
        else
        {
            names.emplace(name, met);
            return met;
        }
    }
};
