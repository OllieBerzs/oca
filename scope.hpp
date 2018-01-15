#pragma once

#include <map>
#include <string>
#include "value.hpp"

namespace oca::internal
{

    class Scope
    {
        std::map<std::string, Value*> names;
        Scope* parent;

    public:
        Scope() : parent(nullptr) {}
        Scope(Scope* parent) : parent(parent) {}

        void clean()
        {
            for (auto n : names)
            {
                if (n.second->refCount == 0) delete n.second;
            }
            names.clear();
        }

        ~Scope()
        {
            clean();
        }

        void set(const std::string& name, Value* value)
        {
            names.emplace(name, value);
        }

        Value* get(const std::string& name)
        {
            auto val = names.find(name);
            if (val == names.end())
            {
                if (parent) return parent->get(name);
                else return nullptr;
            }
            return val->second;
        }

        Value* operator[](const std::string& name)
        {
            Value* val = get(name);
            if (!val) names.emplace(name, val);
            return val;
        }
    };

} // namespace oca::internal