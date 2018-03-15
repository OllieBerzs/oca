/* ollieberzs 2018
** scope.hpp
** scope to hold named stuff
*/

#pragma once

#include <map>
#include <string>

#include "common.hpp"

OCA_BEGIN

class Scope
{
public:
    std::map<std::string, ValuePtr> names;
    Scope* parent;

    Scope(Scope* parent);

    void clean();
    void set(const std::string& name, ValuePtr value);
    ValuePtr get(const std::string& name);

    void print();
};

OCA_END
