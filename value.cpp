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
        return ValueCast(scope.get(name), name);
    }
    return ValueCast(var, name);
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

Integer::Integer(ExprPtr expr, Scope* parent) : Integer(std::stoi(expr->val), parent) {}

Integer::Integer(int val, Scope* parent) : val(val)
{
    scope.parent = parent;

    // functions
    (*this)["__add"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return cast(left + right);
    };

    (*this)["__sub"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return cast(left - right);
    };

    (*this)["__mul"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return cast(left * right);
    };

    (*this)["__eq"] = [](Arg arg) -> Ret
    {
        int left = arg.caller->toInt();
        int right = arg.value->toInt();
        return cast(left == right);
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

Real::Real(ExprPtr expr, Scope* parent) : Real(std::stof(expr->val), parent) {}

Real::Real(float val, Scope* parent) : val(val)
{
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

String::String(ExprPtr expr, Scope* parent) : String(expr->val, parent) {}

String::String(const std::string& val, Scope* parent) : val(val)
{
    scope.parent = parent;

    // functions
    (*this)["__add"] = [](Arg arg) -> Ret
    {
        std::string left = arg.caller->toStr(false);
        std::string right = arg.value->toStr(false);
        return cast(left + right);
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

Bool::Bool(ExprPtr expr, Scope* parent) : Bool((expr->val == "true"), parent) {}

Bool::Bool(bool val, Scope* parent) : val(val)
{
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

Block::Block(ExprPtr expr, Scope* parent)
{
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

ValuePtr Block::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block, Evaluator* e)
{
    // get parameters
    std::vector<std::string> params;
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

    // create temp scope
    Scope temp(&scope);
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
            ValuePtr item = Nil::in(&scope);
            if ((item = arg->scope.get(param))->isNil())
            {
                item = arg->scope.get(std::to_string(counter++));
            }
            if (item->isNil()) Errors::instance().panic(CANNOT_SPLIT, val);

            temp.set(param, item);
        }
    }

    // evaluate the block's value
    ValuePtr result = Nil::in(&scope);
    ExprPtr expr = val;
    while (expr)
    {
        if (expr->left->type == Expression::RETURN) return e->eval(expr->left->right, temp);
        if (expr->left->type == Expression::BREAK) return result;
        result = e->eval(expr->left, temp);
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
