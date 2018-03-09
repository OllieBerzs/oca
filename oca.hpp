/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include "common.hpp"
#include "scope.hpp"
#include "value.hpp"

OCA_BEGIN

class State
{
    friend class Evaluator;
    Scope scope = Scope(nullptr);

public:
    ValuePtr script(const std::string& path);
    ValuePtr eval(const std::string& source, const std::string& path = "");

    void load(const std::string& lib);
    void set(const std::string& name, CPPFunc func);
};

OCA_END
