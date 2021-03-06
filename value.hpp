/* ollieberzs 2018
** value.hpp
** all oca types
*/

#pragma once

#include <string>
#include <map>
#include <any>
#include "common.hpp"
#include "scope.hpp"

OCA_BEGIN

class Value {
public:
    Scope scope = Scope(nullptr);

    virtual ~Value() = default;
    virtual ValuePtr copy() = 0;
    virtual bool isNil();

    virtual std::string tos() = 0;
    virtual std::string typestr() = 0;

    oca_int toi();
    oca_real tor();
    bool tob();

    bool isi();
    bool isr();
    bool isb();
    bool iss();
    bool ist();

    void bind(const std::string& name, const std::string& args, CPPFunc func);
};

class Integer : public Value {
public:
    oca_int val;
    Integer(oca_int val, Scope* parent);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
};

class Real : public Value {
public:
    oca_real val;
    Real(oca_real val, Scope* parent);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
};

class String : public Value {
public:
    std::string val;
    String(const std::string& val, Scope* parent);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
};

class Bool : public Value {
public:
    bool val;
    Bool(bool val, Scope* parent);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
};

class Table : public Value {
public:
    oca_int count = 0;
    oca_int size = 0;
    explicit Table(Scope* parent);
    static std::shared_ptr<Table> from(Scope& scope);
    ValuePtr copy();
    void add(const std::string& name, ValuePtr value);
    bool remove(const std::string& name);
    std::string tos();
    std::string typestr();
};

class Block : public Value {
    Evaluator* evaler;

public:
    ExprPtr val;
    std::vector<std::string> params;
    Block(ExprPtr expr, Scope* parent, Evaluator* evaler);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
};

class Func : public Value {
    CPPFunc val;

public:
    std::string params;
    Func(CPPFunc func, const std::string& params, Scope* parent);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
};

class Nil : public Value {
public:
    static std::shared_ptr<Nil> in(Scope* parent);
    ValuePtr copy();
    std::string tos();
    std::string typestr();
    bool isNil();
};

OCA_END
