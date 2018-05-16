/* ollieberzs 2018
** scope.hpp
** scope to hold named stuff
*/

#pragma once

#include "common.hpp"

OCA_BEGIN

struct Variable {
    bool publicity;
    std::string name;
    ValuePtr value;
};

class Scope {
public:
    std::vector<Variable> vars;
    Scope* parent;

    explicit Scope(Scope* parent);

    void set(const std::string& name, ValuePtr value, bool pub);
    bool remove(const std::string& name);
    ValuePtr get(const std::string& name, bool super);
    void add(const Scope& scope);

    void print();
};

OCA_END
