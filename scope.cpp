/* ollieberzs 2018
** scope.cpp
** scope to hold named stuff
*/

#include <iostream>
#include "oca.hpp"

OCA_BEGIN

Scope::Scope(Scope* parent, State* state) : parent(parent), state(state) {}

// ----------------------------

void Scope::clean()
{
    vars.clear();
}

void Scope::set(const std::string& name, ValuePtr value, bool pub)
{
    ValuePtr val = nullptr;
    uint index;
    for (index = 0; index < vars.size(); ++index)
    {
        auto var = vars.at(index);
        if (var.first.second == name)
        {
            val = var.second;
            break;
        }
    }

    if (val) vars[index] = std::make_pair(std::make_pair(pub, name), value);
    else vars.emplace_back(std::make_pair(pub, name), value);
}

ValuePtr Scope::get(const std::string& name, bool super)
{
    ValuePtr val = nullptr;
    for (auto var : vars)
    {
        if (var.first.second == name)
        {
            val = var.second;
            if (!super && !var.first.first) state->err.panic(NOT_PUBLIC);
            break;
        }
    }

    if (val) return val;
    else return Nil::in(this);
}

// -----------------------------

void Scope::print()
{
    std::string out = "{";
    for (auto var : vars)
    {
        if (var.first.first) out += "[p]";
        out += var.first.second + " ";
    }
    if (out.size() > 1) out.pop_back();
    out += "}";
    std::cout << out << "\n";
}

OCA_END
