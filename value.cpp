/* ollieberzs 2018
** value.cpp
** all oca types
*/

#include <iostream>
#include <sstream>
#include "value.hpp"
#include "parse.hpp"

OCA_BEGIN

bool Value::isNil()
{
    return false;
}

// ---------------------------------

Integer::Integer(ExprPtr expr, Scope* parent)
{
    scope.parent = parent;
    val = std::stoi(expr->val);

    // add functions
    scope.set("__add", std::make_shared<Func>(
        [](oca::Arg arg, oca::ValuePtr caller, oca::ValuePtr block) -> oca::Ret
        {
            Value& left = *caller;
            Value& right = *arg;
            if (!TYPE_EQ(left, Integer)) std::cout << "Left is not an int\n";
            if (!TYPE_EQ(right, Integer)) std::cout << "Right is not an int\n";

            int lint = static_cast<Integer&>(left).val;
            int rint = static_cast<Integer&>(right).val;
            int result = lint + rint;

            return std::make_shared<Integer>(
                std::make_shared<Expression>(
                    Expression::INT, std::to_string(result), 0), nullptr);
        },
        &scope
    ));
}

std::string Integer::toStr(bool debug)
{
    std::string result = "";
    if (debug) result += "<int>";
    result += std::to_string(val);
    return result;
}

// ----------------------------------

Real::Real(ExprPtr expr, Scope* parent)
{
    scope.parent = parent;
    val = std::stof(expr->val);
    // add functions
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

String::String(ExprPtr expr, Scope* parent)
{
    scope.parent = parent;
    val = expr->val;
    // add functions
}

std::string String::toStr(bool debug)
{
    std::string result = "";
    if (debug) result += "<str>";
    result += val;
    return result;
}

// ---------------------------------

Bool::Bool(ExprPtr expr, Scope* parent)
{
    scope.parent = parent;
    val = (expr->val == "true");
    // add functions
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

Nil::Nil(Scope* parent)
{
    scope.parent = parent;
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
