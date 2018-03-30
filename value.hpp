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
    Evaluator* evaler = nullptr;

public:
    Scope scope = Scope(nullptr);

    virtual ~Value() = default;
    virtual std::string toStr(bool debug) = 0;
    virtual bool isNil();

    int toInt();
    float toFloat();
    bool toBool();

    ValueCast operator[](const std::string& name);
};

class Integer : public Value
{
public:
    int val;

    Integer(ExprPtr expr, Scope* parent, Evaluator* e);
    Integer(int val, Scope* parent, Evaluator* e);
    std::string toStr(bool debug);
};

class Real : public Value
{
public:
    float val;

    Real(ExprPtr expr, Scope* parent, Evaluator* e);
    Real(float val, Scope* parent, Evaluator* e);
    std::string toStr(bool debug);
};

class String : public Value
{
public:
    std::string val;

    String(ExprPtr expr, Scope* parent, Evaluator* e);
    String(const std::string& val, Scope* parent, Evaluator* e);
    std::string toStr(bool debug);
};

class Bool : public Value
{
public:
    bool val;

    Bool(ExprPtr expr, Scope* parent, Evaluator* e);
    Bool(bool val, Scope* parent, Evaluator* e);
    std::string toStr(bool debug);
};

class Block : public Value
{
public:
    ExprPtr val;

    Block(ExprPtr expr, Scope* parent, Evaluator* e);
    std::string toStr(bool debug);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
};

class Tuple : public Value
{
public:
    Tuple(Scope* parent);
    std::string toStr(bool debug);
};

class Func : public Value
{
public:
    CPPFunc val;

    Func(CPPFunc func, Scope* parent);
    std::string toStr(bool debug);
};

class Nil : public Value
{
public:
    static std::shared_ptr<Nil> in(Scope* parent);
    std::string toStr(bool debug);
    bool isNil();
};

OCA_END
