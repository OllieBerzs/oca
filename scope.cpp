/* ollieberzs 2018
** scope.cpp
** scope to hold named stuff
*/

#include <iostream>
#include "oca.hpp"

OCA_BEGIN

Scope::Scope(Scope* parent) : parent(parent) {}

// ----------------------------

void Scope::set(const std::string& name, ValuePtr value, bool pub) {
    ValuePtr val = nullptr;
    bool valPub;
    uint index;
    for (index = 0; index < vars.size(); ++index) {
        auto var = vars.at(index);
        if (std::get<1>(var) == name) {
            val = std::get<2>(var);
            valPub = std::get<0>(var);
            break;
        }
    }

    if (val)
        vars[index] = std::make_tuple(valPub, name, value);
    else
        vars.emplace_back(pub, name, value);
}

void Scope::add(const Scope& scope) {
    for (auto var : scope.vars) {
        set(std::get<1>(var), std::get<2>(var), true);
    }
}

ValuePtr Scope::get(const std::string& name, bool super) {
    ValuePtr val = Nil::in(this);
    for (auto var : vars) {
        if (std::get<1>(var) == name) {
            val = std::get<2>(var);
            if (!super && !std::get<0>(var))
                throw Error(NOT_PUBLIC);
            break;
        }
    }
    return val;
}

std::string Scope::find(ValuePtr value) {
    for (auto var : vars) {
        if (value.get() == std::get<2>(var).get())
            return std::get<1>(var);
    }
    return "";
}

// -----------------------------

void Scope::print() {
    std::string out = "{";
    for (auto var : vars) {
        if (std::get<0>(var))
            out += "[p]";
        out += std::get<1>(var) + " ";
    }
    if (out.size() > 1)
        out.pop_back();
    out += "}";
    std::cout << out << "\n";
}

OCA_END
