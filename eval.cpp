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

ValuePtr Evaluator::eval(ExprPtr expr)
{
    if (expr->type == Expression::SET) return set(expr);
    else if (expr->type == Expression::CALL) return call(expr, nullptr);
    else if (expr->type == Expression::IF) return cond(expr);
    else if (expr->type == Expression::ACCESS) return access(expr);
    else return value(expr);
}

// ----------------------------

ValuePtr Evaluator::set(ExprPtr expr)
{
    ValuePtr obj = eval(expr->right);
    state->scope.set(expr->val, obj);
    return obj;
}

ValuePtr Evaluator::call(ExprPtr expr, ValuePtr caller)
{
    ValuePtr func = nullptr;
    std::vector<std::string> names;
    std::string name = "";
    for (auto& c : expr->val)
    {
        if (c == ' ')
        {
            names.push_back(name);
            continue;
        }
        name += c;
    }
    if (names.size() > 1) error("Expected '=' in assignment");

    if (caller) func = caller->table.find(names[0])->second;
    else func = state->scope.get(names[0]);

    if (!func) error("Undefined name '" + names[0] + "'");

    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func))
    {
        ValuePtr arg = nullptr;
        ValuePtr block = nullptr;
        if (expr->right) arg = eval(expr->right);
        if (expr->left) block = eval(expr->left);
        return static_cast<Func&>(*func).val(arg, caller, block);
    }
    if (TYPE_EQ(funcref, Block))
    {

    }

    return func;
}

ValuePtr Evaluator::cond(ExprPtr expr)
{
    return nullptr;
}

ValuePtr Evaluator::access(ExprPtr expr)
{
    return nullptr;
}

ValuePtr Evaluator::file(ExprPtr expr)
{
    return nullptr;
}

ValuePtr Evaluator::value(ExprPtr expr)
{
    ValuePtr result = nullptr;
    //result->type = expr->type;
    if (expr->type == Expression::TUP)
    {
        result = std::make_shared<Tuple>();
        uint counter = ARRAY_BEGIN_INDEX;
        while(expr && expr->left)
        {
            // unnamed value
            if (expr->val == "") expr->val = std::to_string(counter++);

            // add tuple value to object table
            result->table.emplace(expr->val, value(expr->left));
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

void Evaluator::error(const std::string& message)
{
    std::cout << message << "\n";
    exit(1);
}

OCA_END
