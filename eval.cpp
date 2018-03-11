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

    if (caller) func = caller->table.find(names[0])->second;
    if (!func) func = scope.get(names[0]);
    if (!func) error("Undefined name '" + names[0] + "'");

    ValuePtr arg = nullptr;
    ValuePtr block = nullptr;
    if (expr->right) arg = eval(expr->right, scope);
    if (expr->left) block = eval(expr->left, scope);

    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func))
    {
        return static_cast<Func&>(*func).val(arg, caller, block);
    }
    if (TYPE_EQ(funcref, Block))
    {
        ExprPtr exprs = static_cast<Block&>(*func).val;
        auto params = words(static_cast<Block&>(*func).val->val);

        // create block temp scope
        Scope blockScope(&scope);
        if (block) blockScope.set("yield", block);
        if (caller) blockScope.set("self", caller);

        // set parameters
        if (params.size() != 0 && !arg) error("Expected argument");
        if (params.size() == 1) blockScope.set(params[0], arg);
        else if (params.size() > 1) split(arg, params, blockScope);

        // evaluate block
        return doBlock(exprs, blockScope);
    }

    return func;
}

ValuePtr Evaluator::cond(ExprPtr expr, Scope& scope)
{
    return nullptr;
}

ValuePtr Evaluator::access(ExprPtr expr, Scope& scope)
{
    return nullptr;
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
        result = std::make_shared<Tuple>();
        uint counter = ARRAY_BEGIN_INDEX;
        while(expr && expr->left)
        {
            // unnamed value
            if (expr->val == "") expr->val = std::to_string(counter++);

            // add tuple value to object table
            result->table.emplace(expr->val, eval(expr->left, scope));
            expr = expr->right;
        }
    }
    else if (expr->type == Expression::BLOCK)
    {
        result = std::make_shared<Block>(expr);
    }
    else if (expr->type == Expression::STR)
    {
        result = std::make_shared<String>(expr);
    }
    else if (expr->type == Expression::INT)
    {
        result = std::make_shared<Integer>(expr);
    }
    else if (expr->type == Expression::REAL)
    {
        result = std::make_shared<Real>(expr);
    }
    else if (expr->type == Expression::BOOL)
    {
        result = std::make_shared<Bool>(expr);
    }
    return result;
}

// ---------------------------

ValuePtr Evaluator::doBlock(ExprPtr expr, Scope& scope)
{
    ValuePtr result = nullptr;
    while (expr && expr->left)
    {
        if (expr->left->type == Expression::RETURN)
        {
            result = eval(expr->left->right, scope);
            std::cout << "return " << result.get() << "\n";
            return result;
        }
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
        auto item = val->table.find(name);
        if (item == val->table.end()) item = val->table.find(std::to_string(counter++));
        if (item == val->table.end()) error("Cannot split value");

        scope.set(name, item->second);
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
