/* ollieberzs 2018
** oca.hpp
** oca api
*/ 

#pragma once

#include "common.hpp"
#include "scope.hpp"

OCA_BEGIN

class State
{
    Scope scope = Scope(nullptr);

public:
    ObjectPtr script(const std::string& path);
    ObjectPtr eval(const std::string& source, const std::string& path = "");

    void load(const std::string& lib);
    void def(const std::string& name, NativeMethod nat);
};

OCA_END
