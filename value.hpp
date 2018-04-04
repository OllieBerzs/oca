/* ollieberzs 2018
** value.hpp
** all oca types
*/

#pragma once

#include <string>
#include <map>

#include "common.hpp"
#include "scope.hpp"

OCA_BEGIN

class Value
{
protected:
    State* state = nullptr;

public:
    Scope scope = Scope(nullptr);

    virtual ~Value() = default;
    virtual std::string tos(bool debug) = 0;
    virtual bool isNil();

    int toi();
    float tor();
    bool tob();

    bool isi();
    bool isr();
    bool isb();
    bool iss();
    bool ist();

    void bind(const std::string& name, const std::string& args, CPPFunc func);
    ValueCast operator[](const std::string& name);
};

class Integer : public Value
{
public:
    int val;

    Integer(ExprPtr expr, Scope* parent, State* state);
    Integer(int val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class Real : public Value
{
public:
    float val;

    Real(ExprPtr expr, Scope* parent, State* state);
    Real(float val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class String : public Value
{
public:
    std::string val;

    String(ExprPtr expr, Scope* parent, State* state);
    String(const std::string& val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class Bool : public Value
{
public:
    bool val;

    Bool(ExprPtr expr, Scope* parent, State* state);
    Bool(bool val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class Block : public Value
{
public:
    ExprPtr val;

    Block(ExprPtr expr, Scope* parent, State* state);
    std::string tos(bool debug);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
};

class Tuple : public Value
{
public:
    uint count = 0;
    Tuple(Scope* parent);
    std::string tos(bool debug);
};

class Func : public Value
{
    CPPFunc val;

public:
    std::string params;

    Func(CPPFunc func, const std::string& params, Scope* parent, State* state);
    std::string tos(bool debug);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
};

class Nil : public Value
{
public:
    static std::shared_ptr<Nil> in(Scope* parent);
    std::string tos(bool debug);
    bool isNil();
};

OCA_END
