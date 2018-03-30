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

int Value::toInt()
{
    if (TYPE_EQ(*this, Integer))
    {
        return static_cast<Integer&>(*this).val;
    }
    else return 0;
}

float Value::toFloat()
{
    if (TYPE_EQ(*this, Real))
    {
        return static_cast<Real&>(*this).val;
    }
    else return 0.0f;
}

bool Value::toBool()
{
    if (TYPE_EQ(*this, Bool))
    {
        return static_cast<Bool&>(*this).val;
    }
    else return false;
}

// ---------------------------------

Integer::Integer(ExprPtr expr, Scope* parent, Evaluator* e) : Integer(std::stoi(expr->val), parent, e) {}

Integer::Integer(int val, Scope* parent, Evaluator* e) : val(val)
{
    evaler = e;
    scope.parent = parent;

    // functions
    (*this)["__add"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return arg.state->cast(left + right);
    };

    (*this)["__sub"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return arg.state->cast(left - right);
    };

    (*this)["__mul"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return arg.state->cast(left * right);
    };

    (*this)["__eq"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return arg.state->cast(left == right);
    };

    (*this)["times"] = [](Arg arg) -> Ret
    {
        int counter = arg.caller->toInt();
        Block& yield = static_cast<Block&>(*arg.value);
        for (int i = 0; i < counter; ++i)
        {
            yield(Nil::in(arg.caller->scope.parent), arg.state->cast(i), Nil::in(nullptr));
        }
        return Nil::in(nullptr);
    };
}

std::string Integer::toStr(bool debug)
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

std::string Real::toStr(bool debug)
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
    (*this)["__add"] = [](Arg arg) -> Ret
    {
        std::string left = arg.caller->toStr(false);
        std::string right = arg.value->toStr(false);
        return arg.state->cast(left + right);
    };
}

std::string String::toStr(bool debug)
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

std::string Bool::toStr(bool debug)
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

std::string Block::toStr(bool debug)
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

    // set parameters
    if (params.size() != 0 && arg->isNil()) Errors::instance().panic(NO_ARGUMENT, val);
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

std::string Tuple::toStr(bool debug)
{
    std::string result = "";
    if (debug) result += "<tup>";
    result += "(";
    for (auto& i : scope.names)
    {
        if (debug) result += "[" + i.first + "]";
        result += i.second->toStr(debug);
        result += ", ";
    }
    result.pop_back();
    result.pop_back();
    result += ")";
    return result;
}

// ---------------------------------

Func::Func(CPPFunc func, Scope* parent)
{
    scope.parent = parent;
    val = func;
    // add functions
}

std::string Func::toStr(bool debug)
{
    std::string result = "";
    if (debug) result += "<cppfunc>";
    const void* address = static_cast<const void*>(&val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

// ---------------------------------

std::shared_ptr<Nil> Nil::in(Scope* parent)
{
    auto n = std::make_shared<Nil>();
    n->scope.parent = parent;
    return n;
}

std::string Nil::toStr(bool debug)
{
    return "nil";
}

bool Nil::isNil()
{
    return true;
}

OCA_END
