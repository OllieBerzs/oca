/* ollieberzs 2018
** scope.cpp
** scope to hold named stuff
*/

#include "scope.hpp"

OCA_BEGIN

Scope::Scope(ScopePtr parent) : parent(parent) {}

// ----------------------------

void Scope::clean()
{
    names.clear();
}

void Scope::set(const std::string& name, ValuePtr value)
{
    names.emplace(name, value);
}

ValuePtr Scope::get(const std::string& name)
{
    auto val = names.find(name);
    if (val == names.end())
    {
        if (parent) return parent->get(name);
        else return nullptr;
    }
    return val->second;
}

OCA_END
