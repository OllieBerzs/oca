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
};

class Integer : public Value {
public:
    int val;
    Integer(int val, Scope* parent);
    std::string tos(bool debug);
};

class Real : public Value {
public:
    float val;
    Real(float val, Scope* parent);
    std::string tos(bool debug);
};

class String : public Value {
public:
    std::string val;
    String(const std::string& val, Scope* parent);
    std::string tos(bool debug);
};

class Bool : public Value {
public:
    bool val;
    Bool(bool val, Scope* parent);
    std::string tos(bool debug);
};

class Tuple : public Value {
public:
    uint count = 0;
    explicit Tuple(Scope* parent);
    static std::shared_ptr<Tuple> from(Scope& scope);
    void add(const std::string& name, std::any val);
    std::string tos(bool debug);
};

class Block : public Value {
    Evaluator* evaler;

public:
    ExprPtr val;
    std::vector<std::string> params;
    Block(ExprPtr expr, Scope* parent, Evaluator* evaler);
    ValuePtr operator()(ValuePtr caller, ValuePtr arg, ValuePtr block);
    std::string tos(bool debug);
};

class Func : public Value {
    CPPFunc val;

public:
    std::string params;
    Func(CPPFunc func, const std::string& params, Scope* parent);
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
