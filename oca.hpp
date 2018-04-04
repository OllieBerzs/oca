/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include "common.hpp"
#include "lex.hpp"
#include "scope.hpp"
#include "value.hpp"
#include "parse.hpp"
#include "eval.hpp"
#include "error.hpp"

#define NIL oca::Nil::in(nullptr)

OCA_BEGIN

class ValueCast
{
    ValuePtr val;
    std::string name;
    State* state;

public:
    ValueCast(ValuePtr val, const std::string& name, State* state);

    // api casting
    void operator=(int v);
    void operator=(float v);
    void operator=(const std::string& v);
    void operator=(bool v);

    ValueCast operator[](const std::string& name);
};

struct Arg
{
    ValuePtr caller;
    ValuePtr value;
    ValuePtr yield;
    State* state;

    ValuePtr operator[](uint i);
};

class State
{
    Scope scope = Scope(nullptr);
    Scope global = Scope(nullptr);

    Lexer lexer;
    Parser parser;
    Evaluator evaler;
    Errors err;

public:
    State();
    ValuePtr script(const std::string& path, bool asTuple = false);
    ValuePtr eval(const std::string& source, const std::string& path = "", bool asTuple = false);

    void load(const std::string& lib);

    void bind(const std::string& name, const std::string& params, CPPFunc func);
    ValueCast operator[](const std::string& name);

    std::shared_ptr<Integer> cast(int val);
    std::shared_ptr<Real> cast(float val);
    std::shared_ptr<Bool> cast(bool val);
    std::shared_ptr<String> cast(const std::string& val);
    std::shared_ptr<Tuple> cast(const std::vector<int>& val);

    friend class Lexer;
    friend class Parser;
    friend class Evaluator;
    friend class Value;
    friend class Errors;
    friend class Integer;
    friend class Real;
    friend class String;
    friend class Bool;
    friend class Tuple;
    friend class Block;
    friend class Func;
};


OCA_END
