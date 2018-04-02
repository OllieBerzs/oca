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

Evaluator::Evaluator(State* state) : state(state), current(nullptr) {}

ValuePtr Evaluator::eval(ExprPtr expr, Scope& scope)
{
    current = expr;
    if (expr->type == Expression::SET) return set(expr, scope);
    else if (expr->type == Expression::CALL) return call(expr, Nil::in(&scope), scope);
    else if (expr->type == Expression::IF) return cond(expr, scope);
    else if (expr->type == Expression::ACCESS) return access(expr, scope);
    else if (expr->type == Expression::OPER) return oper(expr, scope);
    else if (expr->type == Expression::FILE) return file(expr, scope);
    else if (expr->type == Expression::INJECT) return inject(expr, scope);
    else return value(expr, scope);
}

// ----------------------------

ValuePtr Evaluator::set(ExprPtr expr, Scope& scope)
{
    current = expr;
    std::vector<ExprPtr> lefts;
    ValuePtr rightVal = eval(expr->right, scope);

    // get variables from left side
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

    // split the right value into all variables on the left
    uint counter = ARRAY_BEGIN_INDEX;
    for (auto& leftExpr : lefts)
    {
        std::string name = leftExpr->val;
        ValuePtr leftVal = Nil::in(&scope);

        // get the name if it is a tuple member
        if (leftExpr->type == Expression::ACCESS)
        {
            leftVal = eval(leftExpr, scope);
            if (leftVal->isNil()) Errors::instance().panic(NEW_TUPLE_KEY, leftExpr);

            // find the variable in parent scope
            for (auto& var : leftVal->scope.parent->names)
            {
                if (var.second.get() != leftVal.get()) continue;
                name = var.first;
                break;
            }
        }

        // set the left variable to the right value
        if (lefts.size() == 1) leftVal->scope.parent->set(name, rightVal);
        else
        {
            // get the right value based on the index
            ValuePtr rightValPart = rightVal->scope.get(std::to_string(counter));
            ++counter;
            if (rightValPart->isNil()) Errors::instance().panic(CANNOT_SPLIT, expr->right);

            leftVal->scope.parent->set(name, rightValPart);
        }
    }

    return rightVal;
}

ValuePtr Evaluator::call(ExprPtr expr, ValuePtr caller, Scope& scope)
{
    current = expr;
    ValuePtr func = Nil::in(&scope);
    ValuePtr arg = Nil::in(&scope);
    ValuePtr block = Nil::in(&scope);

    // get the function from one of the scopes
    if (caller) func = caller->scope.get(expr->val); // type specific
    if (func->isNil()) func = state->global.get(expr->val); // global
    if (func->isNil()) func = scope.get(expr->val); // in this scope
    if (func->isNil() && expr->val != "super") Errors::instance().panic(UNDEFINED, expr);

    // get the argument and yield block
    if (expr->right) arg = eval(expr->right, scope);
    if (expr->left) block = eval(expr->left, scope);

    // call the function
    Value& f = *func;
    if (TYPE_EQ(f, Func)) return static_cast<Func&>(f)(caller, arg, block);
    if (TYPE_EQ(f, Block)) return static_cast<Block&>(f)(caller, arg, block);
    return func;
}

ValuePtr Evaluator::oper(ExprPtr expr, Scope& scope)
{
    current = expr;
    std::map<std::string, std::string> operFuncs = {
        {"+", "__add"}, {"-", "__sub"}, {"*", "__mul"}, {"/", "__div"}, {"%", "__mod"},
        {"^", "__pow"}, {"==", "__eq"}, {">", "__gr"}, {"<", "__ls"}, {">=", "__geq"},
        {"<=", "__leq"}, {"..", "__ran"}    
    };

    ValuePtr left = eval(expr->left, scope);
    ValuePtr right = eval(expr->right, scope);
    ValuePtr func = left->scope.get(operFuncs[expr->val]);
    if (func->isNil()) Errors::instance().panic(UNDEFINED_OPERATOR, expr);

    // call the operator
    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func)) return static_cast<Func&>(*func)(left, right, Nil::in(&scope));
    if (TYPE_EQ(funcref, Block)) return static_cast<Block&>(*func)(left, right, Nil::in(&scope));
    return func;
}

ValuePtr Evaluator::cond(ExprPtr expr, Scope& scope)
{
    current = expr;
    ValuePtr result = Nil::in(&scope);
    ValuePtr conditional = eval(expr->left, scope);
    ValuePtr branch = Nil::in(&scope);
    bool trueness;

    // evaluate the conditional
    Value& c = *conditional;
    if (!(TYPE_EQ(c, Bool))) Errors::instance().panic(IF_BOOL, expr->left);
    trueness = static_cast<Bool&>(c).val;

    // set the appropriate branch based on the conditional
    if (trueness) branch = eval(expr->right->left, scope);
    else if (expr->right->right) branch = eval(expr->right->right, scope);

    // evaluate the branch
    if (branch->isNil()) return branch;
    ExprPtr it = static_cast<Block&>(*branch).val;
    while (it && it->left)
    {
        if (it->left->type == Expression::RETURN)
        {
            returning = true;
            return eval(it->left->right, scope);
        }
        if (it->left->type == Expression::BREAK) return result;
        result = eval(it->left, scope);
        it = it->right;
    }
    return result;
}

ValuePtr Evaluator::access(ExprPtr expr, Scope& scope)
{
    current = expr;
    ValuePtr left = eval(expr->left, scope);
    ValuePtr right = Nil::in(&scope);
    ValuePtr arg = Nil::in(&scope);
    ValuePtr block = Nil::in(&scope);
    std::string name = "";

    // get the entry name
    if (expr->val == "[]") name = eval(expr->right, scope)->tos(false);
    else name = expr->right->val;

    // get the data member
    right = left->scope.get(name);
    if (expr->left->val == "super" && right->isNil()) right = left->scope.parent->get(name);
    if (right->isNil()) Errors::instance().panic(UNDEFINED_IN_TUPLE, expr->right);

    // get the argument and yield block
    if (expr->right->right) arg = eval(expr->right->right, scope);
    if (expr->right->left) block = eval(expr->right->left, scope);

    // call the data member
    Value& val = *right;
    if (TYPE_EQ(val, Func)) return static_cast<Func&>(val)(left, arg, block);
    if (TYPE_EQ(val, Block)) return static_cast<Block&>(val)(left, arg, block);
    else return right; // if is not callable
}

ValuePtr Evaluator::file(ExprPtr expr, Scope& scope)
{
    current = expr;
    std::string folder = Errors::instance().folder();
    return state->script(folder + expr->val + ".oca", true);
}

ValuePtr Evaluator::inject(ExprPtr expr, Scope& scope)
{
    current = expr;
    ValuePtr tuple = file(expr->right, scope);
    for (auto var : tuple->scope.names)
    {
        scope.set(var.first, var.second);
    }
    return Nil::in(&scope);
}

ValuePtr Evaluator::value(ExprPtr expr, Scope& scope)
{
    current = expr;
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
            if (expr->val == "")
            {
                expr->val = std::to_string(counter);
                ++counter;
                ++static_cast<Tuple&>(*result).count;
            }

            // add tuple value to object table
            result->scope.set(expr->val, eval(expr->left, result->scope));
            expr = expr->right;
        }
    }
    else if (expr->type == Expression::BLOCK || expr->type == Expression::MAIN
        || expr->type == Expression::ELSE)
    {
        result = std::make_shared<Block>(expr, &scope, this);
    }
    else if (expr->type == Expression::STR)
    {
        result = std::make_shared<String>(expr, &scope, this);
    }
    else if (expr->type == Expression::INT)
    {
        result = std::make_shared<Integer>(expr, &scope, this);
    }
    else if (expr->type == Expression::REAL)
    {
        result = std::make_shared<Real>(expr, &scope, this);
    }
    else if (expr->type == Expression::BOOL)
    {
        result = std::make_shared<Bool>(expr, &scope, this);
    }
    return result;
}

OCA_END
