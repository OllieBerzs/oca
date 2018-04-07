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
    std::vector<std::pair<std::pair<bool, std::string>, ValuePtr>> vars;
    Scope* parent;
    State* state;

    Scope(Scope* parent, State* state);

    void clean();
    void set(const std::string& name, ValuePtr value, bool pub = true);
    ValuePtr get(const std::string& name, bool super = false);

    void print();
};

OCA_END
