/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include "common.hpp"
#include "scope.hpp"
#include "object.hpp"

OCA_BEGIN

class State
{
    friend struct Evaluator;
    Scope scope = Scope(nullptr);

public:
    ObjectPtr script(const std::string& path);
    ObjectPtr eval(const std::string& source, const std::string& path = "");

    void load(const std::string& lib);
    void set(const std::string& name, NativeMethod nat);
};

OCA_END
