/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include "common.hpp"
#include "scope.hpp"
#include "value.hpp"
#include "parse.hpp"
#include "eval.hpp"

#define NIL oca::Nil::in(nullptr)

OCA_BEGIN

class ValueCast
{
    ValuePtr val;
    std::string name;
    Evaluator* evaler;

public:
    ValueCast(ValuePtr val, const std::string& name, Evaluator* e);

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
    State* state;
};

class State
{
    Scope scope = Scope(nullptr);
    Scope global = Scope(nullptr);

    Parser parser;
    Evaluator evaler;

public:
    State();
    ~State();
    ValuePtr script(const std::string& path);
    ValuePtr eval(const std::string& source, const std::string& path = "");

    void load(const std::string& lib);

    ValueCast operator[](const std::string& name);

    std::shared_ptr<Integer> cast(int val);
    std::shared_ptr<Real> cast(float val);
    std::shared_ptr<Bool> cast(bool val);
    std::shared_ptr<String> cast(std::string val);

    friend class Evaluator;
};


OCA_END
