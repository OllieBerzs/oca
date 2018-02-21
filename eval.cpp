/* ollieberzs 2018
** eval.cpp
** evaluate AST to value
*/ 

#include <iostream>

#include "eval.hpp"
#include "parse.hpp"
#include "object.hpp"

OCA_BEGIN

ObjectPtr Evaluator::eval(ExprPtr expr)
{
    if (expr->type == "def") return def(expr);
    else if (expr->type == "block") return block(expr);
    else if (expr->type == "call") return call(expr, nullptr);
    else if (expr->type == "access") return access(expr);
    else return value(expr);
}

// ----------------------------

ObjectPtr Evaluator::def(ExprPtr expr)
{
    return nullptr;
}

ObjectPtr Evaluator::block(ExprPtr expr)
{
    return nullptr;
}

ObjectPtr Evaluator::call(ExprPtr expr, ObjectPtr caller)
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


/*ValuePtr evalDef(Scope& scope, ExprPtr expr)
{
    ValuePtr val = nullptr;
    if (expr->right->type == "block")
    {
        val = evalValue(scope, expr->right);
    }
    else
    {
        val = eval(scope, expr->right);
    }
    scope.set(expr->val, val);
    return val;
}

ValuePtr evalBlock(Scope& scope, ExprPtr expr)
{
    ExprPtr mainBody = expr->left;
    ExprPtr elseBody = expr->right;

    ValuePtr ret = nullptr;
    Scope blockScope(scope);

    ExprPtr curr = mainBody->right;
    while (curr && curr->left)
    {
        if (curr->left->val == "break")
        {
            if (!elseBody->right) return ret;
            curr = elseBody->right;
        }
        if (curr->left->val == "return")
        {
            return eval(blockScope, curr->left->right);
        }
        ret = eval(blockScope, curr->left);
        curr = curr->right;
    }
    return ret;
}

ValuePtr evalCall(Scope& scope, ExprPtr expr, ValuePtr caller)
{
    ValuePtr method = scope.get(expr->val);

    // get arguments
    std::vector<ValuePtr> args;
    ExprPtr arg = expr->right;
    while (arg && arg->left)
    {
        args.push_back(eval(scope, arg->left));
        arg = arg->right;
    }

    if (!method) evalError(expr, "Name '" + expr->val + "' is undefined");

    if (method->type == "native")
    {
        return method->native(args);
    }
    else if (method->type == "block")
    {
        ExprPtr block = method->block;
        ExprPtr mainBody = block->left;
        ExprPtr elseBody = block->right;

        ValuePtr ret = nullptr;
        Scope methodScope(scope);

        // add parameters to scope
        ExprPtr params = mainBody->left;
        uint index = 0;
        while (params)
        {
            methodScope.set(params->val, args[index++]);
            params = params->right;
        }

        // eval main body
        ExprPtr expr = mainBody->right;
        while (expr && expr->left)
        {
            if (expr->left->val == "break")
        {
            if (!elseBody->right) return ret;
            expr = elseBody->right;
        }
        if (expr->left->val == "return")
        {
            return eval(methodScope, expr->left->right);
        }
        ret = eval(methodScope, expr->left);
        expr = expr->right;
        }
        return ret;
    }
    else
    {
        // it is not callable
        return method;
    }
    return nullptr;
}

ValuePtr evalAttach(Scope& scope, ExprPtr expr)
{
    //ValuePtr prev = eval(scope, expr->left);
    //ExprPtr arg = prev->val;

    // add argument
    //ExprPtr call = expr->right->left;
    //ExprPtr args = call->right;
    //ExprPtr temp = std::make_shared<Expression>("tup", "");
    //temp->left = arg;
    //temp->right = args;
    //args = temp;

    // eval attachment
    //return eval(prev->table, call);
    return nullptr;
}

ValuePtr evalValue(Scope& scope, ExprPtr expr)
{
    ValuePtr value = std::make_shared<Value>();
    value->type = expr->type;
    if (expr->type == "tup")
    {
        value->table = std::make_shared<std::map<std::string, ValuePtr>>();
        ExprPtr curr = expr;
        uint counter = ARRAY_BEGIN_INDEX;
        while (curr && curr->left)
        {
            if (curr->val == "")
            {
                curr->val = std::to_string(counter);
                counter++;
            }
            value->table->emplace(curr->val, eval(scope, curr->left));
            curr = curr->right;
        }
    }
    else
    {
        // pull in the table of variables and methods for type
        value->val = expr->val;
    }
    return value;
}*/

ObjectPtr Evaluator::value(ExprPtr expr)
{
    ObjectPtr result = std::make_shared<Object>();
    if (expr->type == "tup")
    {
        ExprPtr curr = expr;
        uint counter = ARRAY_BEGIN_INDEX;
        while(curr && curr->left)
        {
            if (curr->val == "") // unnamed
            {
                curr->val = std::to_string(counter);
                ++counter;
            }

            ExprPtr value = curr->left;
            Value v;
            v.type = value->type;
            if (value->type == "block") v.block = value;
            else v.val = value->val;
            result->table.emplace(curr->val, v);

            curr = curr->right;
        }
    }
    else
    {
        Value v;
        v.type = expr->type;
        v.val = expr->val;
        result->table.emplace("self", v);

        // add type specific native methods
    }
    return result;
}

// ----------------------------

void Evaluator::error(const std::string& message)
{

}

OCA_END