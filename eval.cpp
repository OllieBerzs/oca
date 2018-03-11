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

    if (names.size() == 1) // set value
    {
        scope.set(names[0], obj);
    }
    else // split values
    {
        uint counter = ARRAY_BEGIN_INDEX;
        for (auto& n : names)
        {
            auto item = obj->table.find(n);
            if (item == obj->table.end()) item = obj->table.find(std::to_string(counter++));
            if (item == obj->table.end()) error("Cannot split value");

            scope.set(n, item->second);
        }
    }


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
        Scope blockScope(&scope);
        if (block) blockScope.set("yield", block);
        if (caller) blockScope.set("self", caller);
        // set parameters
        if (params.size() == 1)
        {
            if (!arg) error("Expected argument");
            blockScope.set(params[0], arg);
        }
        else if (params.size() > 1)
        {
            if (!arg) error("Expected argument");
            uint counter = ARRAY_BEGIN_INDEX;
            for (auto& par : params)
            {
                auto item = arg->table.find(par);
                if (item == arg->table.end()) item = arg->table.find(std::to_string(counter++));
                if (item == arg->table.end()) error("Cannot split value");

                blockScope.set(par, item->second);
            }
        }

        // evaluate block
        ValuePtr result = nullptr;
        while (exprs && exprs->left)
        {
            result = eval(exprs->left, blockScope);
            exprs = exprs->right;
        }
        return result;
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

// ----------------------------

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
