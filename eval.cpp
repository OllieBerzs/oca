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
    else if (expr->type == Expression::CALL) return call(expr, NIL(&scope), scope);
    else if (expr->type == Expression::IF) return cond(expr, scope);
    else if (expr->type == Expression::ACCESS) return access(expr, scope);
    else return value(expr, scope);
}

// ----------------------------

ValuePtr Evaluator::set(ExprPtr expr, Scope& scope)
{
    // getting variables from left side
    std::vector<ExprPtr> lefts;
    ExprPtr it = expr->left;
    if (it->type == Expression::CALL) lefts.push_back(it);
    else
    {
        while (it->type == Expression::CALLS)
        {
            lefts.push_back(it->left);
            it = it->right;
        }
        lefts.push_back(it);
    }

    ValuePtr rightVal = eval(expr->right, scope);

    uint counter = ARRAY_BEGIN_INDEX;
    for (auto& leftExpr : lefts)
    {
        ValuePtr leftVal = eval(leftExpr, scope);

        // get variable name
        std::string name = "";
        if (leftVal->isNil()) // variable does not exist
        {
            if (leftExpr->type == Expression::ACCESS) error("Cannot add new key to tuple");
            name = leftExpr->val;
        }
        else // variable exists
        {
            // find the variable in parent scope
            for (auto& var : leftVal->scope.parent->names)
            {
                if (var.second.get() == leftVal.get())
                {
                    name = var.first;
                    break;
                }
            }
        }

        if (lefts.size() == 1) leftVal->scope.parent->set(name, rightVal);
        else // split right tuple
        {
            ValuePtr rightValPart = rightVal->scope.get(std::to_string(counter++));
            if (rightValPart->isNil()) error("Could not split value");

            leftVal->scope.parent->set(name, rightValPart);
        }
    }

    return rightVal;
}

ValuePtr Evaluator::call(ExprPtr expr, ValuePtr caller, Scope& scope)
{
    ValuePtr func = NIL(&scope);

    if (caller) func = caller->scope.get(expr->val);
    if (func->isNil()) func = scope.get(expr->val);
    if (func->isNil()) return func;

    ValuePtr arg = NIL(&scope);
    ValuePtr block = NIL(&scope);
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
        return callBlock(eval(expr->right->left, scope), NIL(&scope), NIL(&scope), NIL(&scope), scope);
    else if (expr->right->right)
        return callBlock(eval(expr->right->right, scope), NIL(&scope), NIL(&scope), NIL(&scope), scope);

    return NIL(&scope);
}

ValuePtr Evaluator::access(ExprPtr expr, Scope& scope)
{
    ValuePtr caller = eval(expr->left, scope);
    ValuePtr data = NIL(&scope);

    if (expr->val == "[")
        data = caller->scope.get(eval(expr->right, scope)->toStr(false));
    else
        data = caller->scope.get(expr->right->val);

    if (data->isNil())
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
    return NIL(&scope);
}

ValuePtr Evaluator::value(ExprPtr expr, Scope& scope)
{
    ValuePtr result = NIL(&scope);
    if (expr->type == Expression::TUP)
    {
        result = std::make_shared<Tuple>(&scope);
        uint counter = ARRAY_BEGIN_INDEX;
        while(expr && expr->left)
        {
            // unnamed value
            if (expr->val == "") expr->val = std::to_string(counter++);

            // add tuple value to object table
            result->scope.set(expr->val, eval(expr->left, result->scope));
            expr = expr->right;
        }
    }
    else if (expr->type == Expression::BLOCK || expr->type == Expression::MAIN
        || expr->type == Expression::ELSE)
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

    // split parameters
    std::vector<std::string> params;
    std::string word = "";
    for (auto& c : b.val->val)
    {
        if (c == ' ')
        {
            params.push_back(word);
            word = "";
            continue;
        }
        word += c;
    }
    if (word != "") params.push_back(word);

    // create temp scope
    Scope temp(&scope);
    if (!block->isNil()) temp.set("yield", block);
    if (!caller->isNil()) temp.set("self", caller);

    // set parameters
    if (params.size() != 0 && arg->isNil()) error("Expected argument");
    if (params.size() == 1) temp.set(params[0], arg);
    else
    {
        uint counter = ARRAY_BEGIN_INDEX;
        for (auto& param : params)
        {
            ValuePtr item = NIL(&scope);
            if ((item = arg->scope.get(param))->isNil()) item = arg->scope.get(std::to_string(counter++));
            if (item->isNil()) error("Cannot split value");

            temp.set(param, item);
        }
    }

    // evaluate the block's value
    ValuePtr result = NIL(&scope);
    ExprPtr expr = b.val;
    while (expr && expr->left)
    {
        if (expr->left->type == Expression::RETURN) return eval(expr->left->right, temp);
        if (expr->left->type == Expression::BREAK) return result;
        result = eval(expr->left, temp);
        expr = expr->right;
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
