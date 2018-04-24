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
protected:
    State* state = nullptr;

public:
    Scope scope = Scope(nullptr, nullptr);

    virtual ~Value() = default;
    virtual std::string tos(bool debug) = 0;
    virtual bool isNil();

    ValuePtr cast(std::any val);

    int toi();
    float tor();
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
    int val;
    Integer(int val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class Real : public Value {
public:
    float val;
    Real(float val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class String : public Value {
public:
    std::string val;
    String(const std::string& val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class Bool : public Value {
public:
    bool val;
    Bool(bool val, Scope* parent, State* state);
    std::string tos(bool debug);
};

class Block : public Value {
public:
    ExprPtr val;
    std::vector<std::string> params;
    Block(ExprPtr expr, Scope* parent, State* state);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
    std::string tos(bool debug);
};

class Tuple : public Value {
public:
    uint count = 0;
    Tuple(Scope* parent, State* state);
    static std::shared_ptr<Tuple> from(Scope& scope, State* state);
    void add(const std::string& name, std::any val);
    std::string tos(bool debug);
};

class Func : public Value {
    CPPFunc val;

public:
    std::string params;
    Func(CPPFunc func, const std::string& params, Scope* parent, State* state);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
    std::string tos(bool debug);
};

class Nil : public Value {
public:
    static std::shared_ptr<Nil> in(Scope* parent);
    std::string tos(bool debug);
    bool isNil();
};

OCA_END
