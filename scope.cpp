/* ollieberzs 2018
** scope.cpp
** scope to hold named stuff
*/

#include <iostream>
#include "scope.hpp"
#include "value.hpp"

OCA_BEGIN

Scope::Scope(Scope* parent) : parent(parent) {}

// ----------------------------

void Scope::clean()
{
    names.clear();
}

void Scope::set(const std::string& name, ValuePtr value)
{
    if (names.find(name) == names.end())
        names.emplace(name, value);
    else names[name] = value;
}

ValuePtr Scope::get(const std::string& name)
{
    auto val = names.find(name);
    if (val == names.end()) return Nil::in(this);
    return val->second;
}

// -----------------------------

void Scope::print()
{
    std::string out = "{";
    for (auto& name : names) out += name.first + " ";
    if (out.size() > 1) out.pop_back();
    out += "}";
    std::cout << out << "\n";
}

OCA_END
