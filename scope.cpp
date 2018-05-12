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
        if (var.name == name) {
            val = var.value;
            valPub = var.publicity;
            break;
        }
    }

    value->scope.parent = this;

    if (val)
        vars[index] = {valPub, name, value};
    else
        vars.push_back({pub, name, value});
}

ValuePtr Scope::get(const std::string& name, bool super) {
    ValuePtr val = Nil::in(this);
    for (auto var : vars) {
        if (var.name == name) {
            val = var.value;
            if (!super && !var.publicity)
                throw Error(NOT_PUBLIC);
            break;
        }
    }
    return val;
}

void Scope::add(const Scope& scope) {
    for (auto var : scope.vars) {
        set(var.name, var.value, true);
    }
}

// -----------------------------

void Scope::print() {
    std::string out = "{";
    for (auto& var : vars) {
        if (var.publicity)
            out += "[p]";
        out += var.name + " ";
    }
    if (out.size() > 1)
        out.pop_back();
    out += "}";
    std::cout << out << "\n";
}

OCA_END
