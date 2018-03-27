/* ollieberzs 2018
** eval.cpp
** evaluate AST to value
*/

#include <iostream>

#include "eval.hpp"
#include "parse.hpp"
#include "value.hpp"
#include "oca.hpp"
#include "error.hpp"

OCA_BEGIN

Evaluator::Evaluator(State* state) : state(state) {}

ValuePtr Evaluator::eval(ExprPtr expr, Scope& scope)
{
    if (expr->type == Expression::SET) return set(expr, scope);
    else if (expr->type == Expression::CALL) return call(expr, Nil::in(&scope), scope);
    else if (expr->type == Expression::IF) return cond(expr, scope);
    else if (expr->type == Expression::ACCESS) return access(expr, scope);
    else if (expr->type == Expression::OPER) return oper(expr, scope);
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
        std::string name = leftExpr->val;
        ValuePtr leftVal = Nil::in(&scope);

        if (leftExpr->type == Expression::ACCESS)
        {
            leftVal = eval(leftExpr, scope);
            if (leftVal->isNil()) Errors::instance().panic(NEW_TUPLE_KEY, leftExpr);

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

        if (lefts.size() == 1)
        {
            leftVal->scope.parent->set(name, rightVal);
        }
        else // split right tuple
        {
            ValuePtr rightValPart = rightVal->scope.get(std::to_string(counter++));
            if (rightValPart->isNil()) Errors::instance().panic(CANNOT_SPLIT, expr->right);

            leftVal->scope.parent->set(name, rightValPart);
        }
    }

    return rightVal;
}

ValuePtr Evaluator::call(ExprPtr expr, ValuePtr caller, Scope& scope)
{
    // TODO: check if requires argument
    ValuePtr func = Nil::in(&scope);

    /*std::cout << "------ " << expr->val << " ------\n";
    std::cout << "caller: ";
    caller->scope.print();
    std::cout << "global: ";
    state->global.print();
    std::cout << "this: ";
    scope.print();*/

    if (caller) func = caller->scope.get(expr->val); // type specific
    if (func->isNil()) func = state->global.get(expr->val); // global
    if (func->isNil()) func = scope.get(expr->val); // in this scope
    if (func->isNil() && expr->val != "super") Errors::instance().panic(UNDEFINED, expr);

    ValuePtr arg = Nil::in(&scope);
    ValuePtr block = Nil::in(&scope);
    if (expr->right) arg = eval(expr->right, scope);
    if (expr->left) block = eval(expr->left, scope);

    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func)) return static_cast<Func&>(*func).val({caller, arg, block});
    if (TYPE_EQ(funcref, Block)) return static_cast<Block&>(*func)(caller, arg, block, this);

    return func;
}

ValuePtr Evaluator::oper(ExprPtr expr, Scope& scope)
{
    std::map<std::string, std::string> operFuncs = {
        {"+", "__add"}, {"-", "__sub"}, {"*", "__mul"}, {"/", "__div"},
        {"==", "__eq"}
    };

    ValuePtr left = eval(expr->left, scope);
    ValuePtr right = eval(expr->right, scope);
    ValuePtr func = left->scope.get(operFuncs[expr->val]);
    if (func->isNil()) Errors::instance().panic(UNDEFINED_OPERATOR, expr);

    // call the operator
    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func)) return static_cast<Func&>(*func).val({left, right, Nil::in(&scope)});
    if (TYPE_EQ(funcref, Block)) return static_cast<Block&>(*func)(left, right, Nil::in(&scope), this);

    return func;
}

ValuePtr Evaluator::cond(ExprPtr expr, Scope& scope)
{
    ValuePtr conditional = eval(expr->left, scope);
    Value& b = *conditional;
    if (!(TYPE_EQ(b, Bool))) Errors::instance().panic(IF_BOOL, expr->left);
    bool trueness = static_cast<Bool&>(*conditional).val;

    ValuePtr block = Nil::in(&scope);
    if (trueness) block = eval(expr->right->left, scope);
    else if (expr->right->right) block = eval(expr->right->right, scope);

    // evaluate the branch
    ValuePtr result = Nil::in(&scope);
    ExprPtr it = static_cast<Block&>(*block).val;
    while (it && it->left)
    {
        if (it->left->type == Expression::RETURN) return eval(it->left->right, scope);
        if (it->left->type == Expression::BREAK) return result;
        result = eval(it->left, scope);
        it = it->right;
    }
    return result;
}

ValuePtr Evaluator::access(ExprPtr expr, Scope& scope)
{
    ValuePtr left = eval(expr->left, scope);
    ValuePtr right = Nil::in(&scope);

    std::string name = "";
    if (expr->val == "[]") name = eval(expr->right, scope)->toStr(false);
    else name = expr->right->val;

    right = left->scope.get(name);
    if (expr->left->val == "super" && right->isNil()) right = left->scope.parent->get(name);
    if (right->isNil()) Errors::instance().panic(UNDEFINED_IN_TUPLE, expr->right);

    ValuePtr arg = Nil::in(&scope);
    ValuePtr block = Nil::in(&scope);
    if (expr->right->right) arg = eval(expr->right->right, scope);
    if (expr->right->left) block = eval(expr->right->left, scope);

    Value& val = *right;
    if (TYPE_EQ(val, Func)) return static_cast<Func&>(val).val({left, arg, block});
    if (TYPE_EQ(val, Block)) return static_cast<Block&>(val)(left, arg, block, this);
    else return right;
}

ValuePtr Evaluator::file(ExprPtr expr, Scope& scope)
{
    return Nil::in(&scope);
}

ValuePtr Evaluator::value(ExprPtr expr, Scope& scope)
{
    ValuePtr result = Nil::in(&scope);
    if (expr->type == Expression::TUP)
    {
        // if tuple has only one member, open it up
        if (expr->right == nullptr) return eval(expr->left, scope);

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

OCA_END
