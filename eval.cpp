/* ollieberzs 2018
** eval.cpp
** evaluate AST to value
*/

#include <iostream>

#include "eval.hpp"
#include "parse.hpp"
#include "value.hpp"
#include "oca.hpp"

OCA_BEGIN

ValuePtr Evaluator::eval(ExprPtr expr, Scope& scope)
{
    if (expr->type == Expression::SET) return set(expr, scope);
    else if (expr->type == Expression::CALL) return call(expr, nullptr, scope);
    else if (expr->type == Expression::IF) return cond(expr, scope);
    else if (expr->type == Expression::ACCESS) return access(expr, scope);
    else return value(expr, scope);
}

// ----------------------------

ValuePtr Evaluator::set(ExprPtr expr, Scope& scope)
{
    auto names = words(expr->val);
    ValuePtr obj = eval(expr->right, scope);

    if (names.size() == 1) scope.set(names[0], obj);
    else split(obj, names, scope);

    return obj;
}

ValuePtr Evaluator::call(ExprPtr expr, ValuePtr caller, Scope& scope)
{
    ValuePtr func = nullptr;
    auto names = words(expr->val);
    if (names.size() > 1) error("Expected '=' in assignment");

    if (caller) func = caller->scope.get(names[0]);
    if (!func) func = scope.get(names[0]);
    if (!func) error("Undefined name '" + names[0] + "'");

    ValuePtr arg = nullptr;
    ValuePtr block = nullptr;
    if (expr->right) arg = eval(expr->right, scope);
    if (expr->left) block = eval(expr->left, scope);

    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func)) return static_cast<Func&>(*func).val(arg, caller, block);
    if (TYPE_EQ(funcref, Block)) return callBlock(func, arg, caller, block, scope);

    return func;
}

ValuePtr Evaluator::cond(ExprPtr expr, Scope& scope)
{
    ValuePtr conditional = eval(expr->left, scope);
    Value& b = *conditional;
    if (!(TYPE_EQ(b, Bool))) error("Expected a boolean value in 'if'");
    bool trueness = static_cast<Bool&>(*conditional).val;

    if (trueness)
        return callBlock(eval(expr->right->left, scope), nullptr, nullptr, nullptr, scope);
    else if (expr->right->right)
        return callBlock(eval(expr->right->right, scope), nullptr, nullptr, nullptr, scope);

    return nullptr;
}

ValuePtr Evaluator::access(ExprPtr expr, Scope& scope)
{
    ValuePtr caller = eval(expr->left, scope);
    auto data = caller->scope.get(expr->right->val);

    if (!data)
        error("Undefined name '" + expr->right->val + "' for '" + expr->left->val + "'");

    Value& val = *data;
    if (TYPE_EQ(val, Block)) return callBlock
        (data, eval(expr->right->right, scope), caller, eval(expr->right->left, scope), scope);
    if (TYPE_EQ(val, Func)) return static_cast<Func&>
        (val).val(eval(expr->right->right, scope), caller, eval(expr->right->left, scope));
    else return data;
}

ValuePtr Evaluator::file(ExprPtr expr, Scope& scope)
{
    return nullptr;
}

ValuePtr Evaluator::value(ExprPtr expr, Scope& scope)
{
    ValuePtr result = nullptr;
    if (expr->type == Expression::TUP)
    {
        result = std::make_shared<Tuple>(&scope);
        uint counter = ARRAY_BEGIN_INDEX;
        while(expr && expr->left)
        {
            // unnamed value
            if (expr->val == "") expr->val = std::to_string(counter++);

            // add tuple value to object table
            result->scope.set(expr->val, eval(expr->left, scope));
            expr = expr->right;
        }
    }
    else if (expr->type == Expression::BLOCK)
    {
        result = std::make_shared<Block>(expr, &scope);
    }
    else if (expr->type == Expression::STR)
    {
        result = std::make_shared<String>(expr, &scope);
    }
    else if (expr->type == Expression::INT)
    {
        result = std::make_shared<Integer>(expr, &scope);
    }
    else if (expr->type == Expression::REAL)
    {
        result = std::make_shared<Real>(expr, &scope);
    }
    else if (expr->type == Expression::BOOL)
    {
        result = std::make_shared<Bool>(expr, &scope);
    }
    return result;
}

// ---------------------------

ValuePtr Evaluator::callBlock(ValuePtr val, ValuePtr arg, ValuePtr caller, ValuePtr block, Scope& scope)
{
    Block& b = static_cast<Block&>(*val);
    auto params = words(b.val->val);

    // create temp scope
    Scope temp(&scope);
    if (block) temp.set("yield", block);
    if (caller) temp.set("self", caller);

    // set parameters
    if (params.size() != 0 && !arg) error("Expected argument");
    if (params.size() == 1) temp.set(params[0], arg);
    else split(arg, params, temp);

    // evaluate the block's value
    ValuePtr result = nullptr;
    ExprPtr expr = b.val;
    while (expr && expr->left)
    {
        if (expr->left->type == Expression::RETURN) return eval(expr->left->right, scope);
        if (expr->left->type == Expression::BREAK) return result;
        result = eval(expr->left, scope);
        expr = expr->right;
    }
    return result;
}

// ----------------------------

void Evaluator::split(ValuePtr val, const std::vector<std::string>& names, Scope& scope)
{
    uint counter = ARRAY_BEGIN_INDEX;
    for (auto& name : names)
    {
        ValuePtr item = nullptr;
        if (!(item = val->scope.get(name))) item = val->scope.get(std::to_string(counter++));
        if (!item) error("Cannot split value");

        scope.set(name, item);
    }
}

std::vector<std::string> Evaluator::words(const std::string& str)
{
    std::vector<std::string> result;
    std::string word = "";
    for (auto& c : str)
    {
        if (c == ' ')
        {
            result.push_back(word);
            word = "";
            continue;
        }
        word += c;
    }
    return result;
}

// ----------------------------

void Evaluator::error(const std::string& message)
{
    std::cout << message << "\n";
    exit(1);
}

OCA_END
