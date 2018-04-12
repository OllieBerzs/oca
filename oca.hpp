/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include <chrono>
#include <any>
#include "common.hpp"
#include "lex.hpp"
#include "scope.hpp"
#include "value.hpp"
#include "parse.hpp"
#include "eval.hpp"
#include "error.hpp"

#define NIL oca::Nil::in(nullptr)

OCA_BEGIN

struct Arg
{
    ValuePtr caller;
    ValuePtr value;
    ValuePtr yield;

    ValuePtr operator[](uint i);
};

class State
{
    Scope scope;
    Scope global;

    Lexer lexer;
    Parser parser;
    Evaluator evaler;
    Errors err;

    std::chrono::milliseconds lextime;
    std::chrono::milliseconds parsetime;
    std::chrono::milliseconds evaltime;

public:
    State();
    ~State();
    State(const State&) = delete;
    State& operator=(const State&) = delete;

public:
    ValuePtr script(const std::string& path, bool asTuple = false);
    ValuePtr eval(const std::string& source, const std::string& path = "", bool asTuple = false);
    ValuePtr cast(std::any val);

    void load(const std::string& lib);
    void bind(const std::string& name, const std::string& params, CPPFunc func);

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
    friend class Scope;
};


OCA_END
