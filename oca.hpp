/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include "common.hpp"
#include "scope.hpp"
#include "value.hpp"

#define NIL oca::Nil::in(nullptr)

OCA_BEGIN

class ValueCast
{
    ValuePtr val;
    std::string name;

public:
    ValueCast(ValuePtr val, const std::string& name);

    // api casting
    void operator=(int v);
    void operator=(float v);
    void operator=(const std::string& v);
    void operator=(bool v);
    void operator=(CPPFunc v);

    ValueCast operator[](const std::string& name);
};

struct Arg
{
    ValuePtr caller;
    ValuePtr value;
    ValuePtr block;
};

class State
{
    Scope scope = Scope(nullptr);
    Scope global = Scope(nullptr);

public:
    State();
    ValuePtr script(const std::string& path);
    ValuePtr eval(const std::string& source, const std::string& path = "");

    void load(const std::string& lib);

    ValueCast operator[](const std::string& name);
    
    friend class Evaluator;
};

std::shared_ptr<Integer> cast(int val);
std::shared_ptr<Real> cast(float val);
std::shared_ptr<Bool> cast(bool val);
std::shared_ptr<String> cast(std::string val);

OCA_END
