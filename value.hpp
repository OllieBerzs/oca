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
public:
    std::map<std::string, ValuePtr> table;

    virtual ~Value() = default;
    virtual std::string toStr(bool debug) = 0;
};

class Integer : public Value
{
public:
    int val;

    Integer(ExprPtr expr);
    std::string toStr(bool debug);
};

class Real : public Value
{
public:
    float val;

    Real(ExprPtr expr);
    std::string toStr(bool debug);
};

class String : public Value
{
public:
    std::string val;

    String(ExprPtr expr);
    std::string toStr(bool debug);
};

class Bool : public Value
{
public:
    bool val;

    Bool(ExprPtr expr);
    std::string toStr(bool debug);
};

class Block : public Value
{
public:
    ExprPtr val;

    Block(ExprPtr expr);
    std::string toStr(bool debug);
};

class Tuple : public Value
{
public:
    Tuple();
    std::string toStr(bool debug);
};

class Func : public Value
{
public:
    CPPFunc val;

    Func(CPPFunc func);
    std::string toStr(bool debug);
};

OCA_END
