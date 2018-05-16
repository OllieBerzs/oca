/* ollieberzs 2018
** oca.hpp
** oca api
*/

#pragma once

#include <chrono>
#include "common.hpp"
#include "lex.hpp"
#include "scope.hpp"
#include "value.hpp"
#include "parse.hpp"
#include "eval.hpp"
#include "error.hpp"

#define NIL oca::Nil::in(nullptr)

OCA_BEGIN

struct Arg {
    ValuePtr caller;
    ValuePtr value;
    ValuePtr yield;

    ValuePtr operator[](uint i);
};

class State {
    Scope global;
    Scope scope;

    Lexer lexer;
    Parser parser;
    Evaluator evaler;
    ErrorHandler eh;

    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::chrono::milliseconds lextime;
    std::chrono::milliseconds parsetime;
    std::chrono::milliseconds evaltime;

public:
    State();
    ~State();
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    ValuePtr runFile(const std::string& path, bool asTuple = false);
    ValuePtr runString(const std::string& source, bool asTuple = false);
    void runREPL();

    void load(const std::string& lib);
    void bind(const std::string& name, const std::string& params, CPPFunc func);

private:
    std::vector<Token> lex(const std::string& source);
    std::vector<ExprPtr> parse(const std::vector<Token>& tokens);
    ValuePtr evaluate(const std::vector<ExprPtr>& ast, bool asTuple);

    friend class ErrorHandler;
    friend class Evaluator;
    friend class Value;
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
