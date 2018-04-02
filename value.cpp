/* ollieberzs 2018
** value.cpp
** all oca types
*/

#include <iostream>
#include <sstream>
#include "value.hpp"
#include "parse.hpp"
#include "oca.hpp"
#include "error.hpp"
#include "eval.hpp"

OCA_BEGIN

bool Value::isNil()
{
    return false;
}

void Value::bind(const std::string& name, const std::string& args, CPPFunc func)
{
    scope.set(name, std::make_shared<Func>(func, args, &scope, evaler));
}

ValueCast Value::operator[](const std::string& name)
{
    ValuePtr var = scope.get(name);
    if (var->isNil())
    {
        scope.set(name, Nil::in(&scope));
        return ValueCast(scope.get(name), name, evaler);
    }
    return ValueCast(var, name, evaler);
}

int Value::toi()
{
    if (TYPE_EQ(*this, Integer))
    {
        return static_cast<Integer&>(*this).val;
    }
    else return 0;
}

float Value::tor()
{
    if (TYPE_EQ(*this, Real))
    {
        return static_cast<Real&>(*this).val;
    }
    else return 0.0f;
}

bool Value::tob()
{
    if (TYPE_EQ(*this, Bool))
    {
        return static_cast<Bool&>(*this).val;
    }
    else return false;
}

bool Value::isi()
{
    if (TYPE_EQ(*this, Integer)) return true;
    else return false;
}

bool Value::isr()
{
    if (TYPE_EQ(*this, Real)) return true;
    else return false;
}

bool Value::isb()
{
    if (TYPE_EQ(*this, Bool)) return true;
    else return false;
}

bool Value::iss()
{
    if (TYPE_EQ(*this, String)) return true;
    else return false;
}

bool Value::ist()
{
    if (TYPE_EQ(*this, Tuple)) return true;
    else return false;
}

// ---------------------------------

Integer::Integer(ExprPtr expr, Scope* parent, Evaluator* e) : Integer(std::stoi(expr->val), parent, e) {}

Integer::Integer(int val, Scope* parent, Evaluator* e) : val(val)
{
    evaler = e;
    scope.parent = parent;

    // functions
    bind("__add", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left + arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left + arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__sub", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left - arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left - arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__mul", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left * arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left * arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__eq", "i", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return arg.state->cast(left == right);
    });

    bind("times", "", [](Arg arg) -> Ret
    {
        int times = arg.caller->toi();
        Block& yield = static_cast<Block&>(*arg.yield);
        for (int i = 0; i < times; ++i)
        {
            yield(Nil::in(arg.caller->scope.parent), arg.state->cast(i), Nil::in(nullptr));
        }
        return Nil::in(nullptr);
    });
}

std::string Integer::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<int>";
    result += std::to_string(val);
    return result;
}

// ----------------------------------

Real::Real(ExprPtr expr, Scope* parent, Evaluator* e) : Real(std::stof(expr->val), parent, e) {}

Real::Real(float val, Scope* parent, Evaluator* e) : val(val)
{
    evaler = e;
    scope.parent = parent;
}

std::string Real::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<real>";
    std::stringstream ss;
    ss << val;
    result += ss.str();
    return result;
}

// ---------------------------------

String::String(ExprPtr expr, Scope* parent, Evaluator* e) : String(expr->val, parent, e) {}

String::String(const std::string& val, Scope* parent, Evaluator* e) : val(val)
{
    evaler = e;
    scope.parent = parent;

    // functions
    bind("__add", "a", [](Arg arg) -> Ret
    {
        std::string left = arg.caller->tos(false);
        std::string right = arg.value->tos(false);
        return arg.state->cast(left + right);
    });
}

std::string String::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<str>";
    result += val;
    return result;
}

// ---------------------------------

Bool::Bool(ExprPtr expr, Scope* parent, Evaluator* e) : Bool((expr->val == "true"), parent, e) {}

Bool::Bool(bool val, Scope* parent, Evaluator* e) : val(val)
{
    evaler = e;
    scope.parent = parent;
}

std::string Bool::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<bool>";
    result += (val) ? "true" : "false";
    return result;
}

// ---------------------------------

Block::Block(ExprPtr expr, Scope* parent, Evaluator* e)
{
    evaler = e;
    scope.parent = parent;
    val = expr;
    // add functions
}

std::string Block::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<block>";
    const void* address = static_cast<const void*>(&*val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

ValuePtr Block::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block)
{
    Scope temp(&scope);
    std::vector<std::string> params;
    ValuePtr result = Nil::in(&scope);

    // get parameters
    std::string word = "";
    for (auto& c : val->val)
    {
        if (c == ' ')
        {
            params.push_back(word);
            word = "";
        }
        else word += c;
    }
    if (word != "") params.push_back(word);

    // set super and yield in scope
    if (!block->isNil()) temp.set("yield", block);
    temp.set("super", caller);

    // get argument count
    uint argc = 0;
    if (arg->ist()) argc = static_cast<Tuple&>(*arg).count;
    else if (!arg->isNil()) argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0) Errors::instance().panic(NO_ARGUMENT, val);
    if (argc < params.size())
    {
        Errors::instance().panic(SMALL_TUPLE, val,
            "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");
    }

    // set parameters
    if (params.size() == 1) temp.set(params[0], arg);
    else
    {
        uint counter = ARRAY_BEGIN_INDEX;
        for (auto& param : params)
        {
            ValuePtr item = arg->scope.get(param);
            if (item->isNil())
            {
                item = arg->scope.get(std::to_string(counter));
                ++counter;
            }
            if (item->isNil()) Errors::instance().panic(CANNOT_SPLIT, val);

            temp.set(param, item);
        }
    }

    // evaluate the block's value
    ExprPtr expr = val;
    while (expr)
    {
        if (expr->left->type == Expression::RETURN) return evaler->eval(expr->left->right, temp);
        if (expr->left->type == Expression::BREAK) return result;
        result = evaler->eval(expr->left, temp);
        expr = expr->right;
    }
    return result;
}

// ---------------------------------

Tuple::Tuple(Scope* parent)
{
    scope.parent = parent;
}

std::string Tuple::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<tup>";
    result += "(";
    for (auto& i : scope.names)
    {
        if (debug) result += "[" + i.first + "]";
        result += i.second->tos(debug);
        result += ", ";
    }
    result.pop_back();
    result.pop_back();
    result += ")";
    return result;
}

// ---------------------------------

Func::Func(CPPFunc func, const std::string& params, Scope* parent, Evaluator* e)
    : val(func), params(params)
{
    evaler = e;
    scope.parent = parent;
}

std::string Func::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<cppfunc>";
    const void* address = static_cast<const void*>(&val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

ValuePtr Func::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block)
{
    // get argument count
    uint argc = 0;
    if (arg->ist()) argc = static_cast<Tuple&>(*arg).count;
    else if (!arg->isNil()) argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0) Errors::instance().panic(NO_ARGUMENT, evaler->current);
    if (argc < params.size())
    {
        Errors::instance().panic(SMALL_TUPLE, evaler->current,
            "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");
    }
    // if argc >= args.size ok!

    // check argument types
    for (uint i = 0; i < params.size(); ++i)
    {
        ValuePtr v = arg;
        if (argc > 1) v = arg->scope.get(std::to_string(i));
        switch (params[i])
        {
        case 'i':
            if (!v->isi()) Errors::instance().panic(TYPE_MISMATCH, evaler->current,
                v->tos(true) + " wanted int.");
            break;
        case 'r':
            if (!v->isr()) Errors::instance().panic(TYPE_MISMATCH, evaler->current,
                v->tos(true) + " wanted real.");
            break;
        case 'n':
            if (!v->isi() && !v->isr()) Errors::instance().panic(TYPE_MISMATCH, evaler->current,
                v->tos(true) + " wanted int/real.");
            break;
        case 'b':
            if (!v->isb()) Errors::instance().panic(TYPE_MISMATCH, evaler->current,
                    v->tos(true) + " wanted bool.");
            break;
        case 's':
            if (!v->iss()) Errors::instance().panic(TYPE_MISMATCH, evaler->current,
                    v->tos(true) + " wanted str.");
            break;
        case 't':
            if (!v->ist()) Errors::instance().panic(TYPE_MISMATCH, evaler->current,
                    v->tos(true) + " wanted tuple.");
            break;
        }
    }

    return val({caller, arg, block, evaler->state});
}

// ---------------------------------

std::shared_ptr<Nil> Nil::in(Scope* parent)
{
    auto n = std::make_shared<Nil>();
    n->scope.parent = parent;
    return n;
}

std::string Nil::tos(bool debug)
{
    return "nil";
}

bool Nil::isNil()
{
    return true;
}

OCA_END
