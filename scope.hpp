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
    std::map<std::string, ValuePtr> names;
    ScopePtr parent;

public:
    Scope(ScopePtr parent);

    void clean();
    void set(const std::string& name, ValuePtr value);
    ValuePtr get(const std::string& name);
};

OCA_END
