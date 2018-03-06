/* ollieberzs 2018
** eval.cpp
** evaluate AST to value
*/

#include <iostream>

#include "eval.hpp"
#include "parse.hpp"
#include "object.hpp"
#include "oca.hpp"

OCA_BEGIN

ObjectPtr Evaluator::eval(ExprPtr expr)
{
    if (expr->type == "set") return set(expr);
    else if (expr->type == "call") return call(expr, nullptr);
    else if (expr->type == "if") return cond(expr);
    else if (expr->type == "access") return access(expr);
    else return value(expr);
}

// ----------------------------

ObjectPtr Evaluator::set(ExprPtr expr)
{
    ObjectPtr obj = eval(expr->right);
    state->scope.set(expr->val, obj);
    return obj;
}

ObjectPtr Evaluator::call(ExprPtr expr, ObjectPtr caller)
{
    ObjectPtr func = nullptr;
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

    if (caller) func = caller->table.find(names[0])->second;
    else func = state->scope.get(names[0]);

    if (!func) error("Undefined name '" + names[0] + "'");

    if (func->type == "native")
    {
        ObjectPtr arg = nullptr;
        ObjectPtr block = nullptr;
        if (expr->right) arg = eval(expr->right);
        if (expr->left) block = eval(expr->left);
        return func->nat(arg, caller, block);
    }
    if (func->type == "block")
    {

    }

    return func;
}

ObjectPtr Evaluator::cond(ExprPtr expr)
{
    return nullptr;
}

ObjectPtr Evaluator::access(ExprPtr expr)
{
    return nullptr;
}

ObjectPtr Evaluator::file(ExprPtr expr)
{
    return nullptr;
}

ObjectPtr Evaluator::value(ExprPtr expr)
{
    ObjectPtr result = std::make_shared<Object>();
    result->type = expr->type;
    if (expr->type == "tup")
    {
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
    else if (result->type == "block")
    {
        result->block = expr;
    }
    else
    {
        result->val = expr->val;
        // add type specific native methods
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
