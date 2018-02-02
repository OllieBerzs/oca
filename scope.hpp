/* ollieberzs 2018
** scope.hpp
** scope to hold named stuff
*/ 

#pragma once

#include <map>
#include <string>
#include <memory>

#include "common.hpp"

OCA_BEGIN

struct Scope;
struct Value;

typedef std::shared_ptr<Scope> ScopePtr;
typedef std::shared_ptr<Value> ValuePtr;

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