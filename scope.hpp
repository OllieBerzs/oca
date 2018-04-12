/* ollieberzs 2018
** scope.hpp
** scope to hold named stuff
*/

#pragma once

#include "common.hpp"

OCA_BEGIN

class Scope
{
public:
    std::vector<std::tuple<bool, std::string, ValuePtr>> vars;
    Scope* parent;
    State* state;

    Scope(Scope* parent, State* state);

    void set(const std::string& name, ValuePtr value, bool pub);
    void add(const Scope& scope);
    ValuePtr get(const std::string& name, bool super);
    std::string find(ValuePtr value);

    void print();
};

OCA_END
