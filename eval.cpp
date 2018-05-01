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

ValuePtr Evaluator::eval(ExprPtr expr, Scope& scope) {
    if (expr->type == Expression::SET)
        return set(expr, scope);
    else if (expr->type == Expression::CALL)
        return call(expr, scope);
    else if (expr->type == Expression::IF)
        return cond(expr, scope);
    else if (expr->type == Expression::ACCESS)
        return access(expr, scope);
    else if (expr->type == Expression::OPER)
        return oper(expr, scope);
    else if (expr->type == Expression::FILE)
        return file(expr, scope);
    else
        return value(expr, scope);
}

// ----------------------------

ValuePtr Evaluator::set(ExprPtr expr, Scope& scope) {
    auto tracker = current;
    current = expr;

    std::vector<ExprPtr> lefts;
    ValuePtr rightVal = eval(expr->right, scope);

    ExprPtr it = expr->left;
    if (it->type == Expression::CALL)
        lefts.push_back(it);
    else {
        while (it->type == Expression::CALLS) {
            lefts.push_back(it->left);
            it = it->right;
        }
        lefts.push_back(it);
    }

    uint counter = ARRAY_BEGIN_INDEX;
    for (auto& leftExpr : lefts) {
        std::string name = leftExpr->val;
        ValuePtr leftVal = Nil::in(&scope);

        if (leftExpr->type == Expression::ACCESS) {
            leftVal = eval(leftExpr, scope);
            if (leftVal->isNil())
                throw Error(NEW_TUPLE_KEY);
            name = leftVal->scope.parent->find(leftVal);
        }

        if (lefts.size() == 1)
            leftVal->scope.parent->set(name, rightVal, true);
        else {
            ValuePtr rightValPart = rightVal->scope.get(std::to_string(counter), false);
            ++counter;
            if (rightValPart->isNil())
                throw Error(CANNOT_SPLIT);

            leftVal->scope.parent->set(name, rightValPart, true);
        }
    }

    current = tracker;
    return rightVal;
}

ValuePtr Evaluator::call(ExprPtr expr, Scope& scope) {
    auto tracker = current;
    current = expr;

    #ifdef OUT_SCOPES
    std::cout << "Call '" << expr->val << "' scopes:\n";
    std::cout << "-- function scope ";
    scope.print();
    std::cout << "-- global scope ";
    state->global.print();
    #endif

    ValuePtr val = scope.get(expr->val, true);
    if (val->isNil())
        val = state->global.get(expr->val, true);
    if (val->isNil())
        throw Error(UNDEFINED);
    current = tracker;

    ValuePtr arg = Nil::in(&scope);
    ValuePtr block = Nil::in(&scope);
    if (expr->right)
        arg = eval(expr->right, scope);
    if (expr->left)
        block = eval(expr->left, scope);

    Value& vref = *val;
    if (TYPE_EQ(vref, Func))
        return static_cast<Func&>(vref)(Tuple::from(scope), arg, block);
    if (TYPE_EQ(vref, Block))
        return static_cast<Block&>(vref)(Tuple::from(scope), arg, block);
    return val;
}

ValuePtr Evaluator::oper(ExprPtr expr, Scope& scope) {
    auto tracker = current;
    current = expr;

    std::map<std::string, std::string> operFuncs = {
        {"+", "__add"},   {"-", "__sub"},   {"*", "__mul"},  {"/", "__div"},   {"%", "__mod"},
        {"^", "__pow"},   {"==", "__eq"},   {"!=", "__neq"}, {">", "__gr"},    {"<", "__ls"},
        {">=", "__geq"},  {"<=", "__leq"},  {"..", "__ran"}, {"and", "__and"}, {"or", "__or"},
        {"xor", "__xor"}, {"lsh", "__lsh"}, {"rsh", "__rsh"}};

    ValuePtr left = eval(expr->left, scope);
    ValuePtr right = eval(expr->right, scope);
    ValuePtr func = left->scope.get(operFuncs[expr->val], false);
    if (func->isNil())
        throw Error(UNDEFINED_OPERATOR);
    current = tracker;

    Value& funcref = *func;
    if (TYPE_EQ(funcref, Func))
        return static_cast<Func&>(funcref)(left, right, Nil::in(&scope));
    if (TYPE_EQ(funcref, Block))
        return static_cast<Block&>(funcref)(left, right, Nil::in(&scope));

    return func;
}

ValuePtr Evaluator::cond(ExprPtr expr, Scope& scope) {
    auto tracker = current;
    current = expr;

    ValuePtr conditional = eval(expr->left, scope);
    Value& cref = *conditional;
    if (!(TYPE_EQ(cref, Bool)))
        throw Error(IF_BOOL);
    bool trueness = static_cast<Bool&>(cref).val;
    current = tracker;

    ValuePtr branch = Nil::in(&scope);
    if (trueness)
        branch = eval(expr->right->left, scope);
    else if (expr->right->right)
        branch = eval(expr->right->right, scope);

    if (branch->isNil())
        return branch;

    ValuePtr result = Nil::in(&scope);
    ExprPtr it = static_cast<Block&>(*branch).val;
    while (it && it->left) {
        if (it->left->type == Expression::RETURN) {
            returning = true;
            return eval(it->left->right, scope);
        }
        if (it->left->type == Expression::BREAK)
            return result;
        result = eval(it->left, scope);
        it = it->right;
    }
    return result;
}

ValuePtr Evaluator::access(ExprPtr expr, Scope& scope) {
    auto tracker = current;
    current = expr->right;

    std::string name = expr->right->val;
    ValuePtr left = eval(expr->left, scope);

    bool super = expr->left->val == "super";
    ValuePtr right = left->scope.get(name, super);

    #ifdef OUT_SCOPES
    std::cout << "Access '" << expr->right->val << "' scopes:\n";
    std::cout << "-- left scope ";
    left->scope.print();
    #endif

    if (right->isNil())
        throw Error(UNDEFINED_IN_TUPLE);
    current = tracker;

    ValuePtr arg = Nil::in(&scope);
    ValuePtr block = Nil::in(&scope);
    if (expr->right->right)
        arg = eval(expr->right->right, scope);
    if (expr->right->left)
        block = eval(expr->right->left, scope);

    Value& val = *right;
    if (TYPE_EQ(val, Func))
        return static_cast<Func&>(val)(left, arg, block);
    if (TYPE_EQ(val, Block))
        return static_cast<Block&>(val)(left, arg, block);
    return right;
}

ValuePtr Evaluator::file(ExprPtr expr, Scope& scope) {
    auto oldPath = state->eh.path;
    auto oldSource = state->eh.source;
    auto oldTokens = state->eh.tokens;
    auto oldScope = state->scope;

    std::string folder = "";
    if (state->eh.path) {
        uint slash = state->eh.path->find_last_of("/");
        folder = state->eh.path->substr(0, slash + 1);
    }

    state->scope = Scope(nullptr);
    ValuePtr val = state->runFile(folder + expr->val + ".oca", true);

    state->eh.path = oldPath;
    state->eh.source = oldSource;
    state->eh.tokens = oldTokens;
    state->scope = oldScope;

    return val;
}

ValuePtr Evaluator::value(ExprPtr expr, Scope& scope) {
    ValuePtr result = Nil::in(&scope);
    if (expr->type == Expression::TUP) {
        if (expr->right == nullptr && expr->val == "")
            return eval(expr->left, scope);

        result = std::make_shared<Tuple>(&scope);
        uint counter = ARRAY_BEGIN_INDEX;
        while (expr && expr->left) {
            std::string nam = "";
            bool pub = (expr->val.find("pub ") != std::string::npos);

            if (expr->val == "") {
                pub = true;
                nam = std::to_string(counter);
                ++counter;
                ++static_cast<Tuple&>(*result).count;
            }
            if (pub && expr->val != "")
                nam = expr->val.substr(4);
            if (nam == "")
                nam = expr->val;

            result->scope.set(nam, eval(expr->left, scope), pub);
            expr = expr->right;
        }
    } else if (
        expr->type == Expression::BLOCK || expr->type == Expression::MAIN ||
        expr->type == Expression::ELSE) {
        result = std::make_shared<Block>(expr, &scope, this);
    } else if (expr->type == Expression::STR) {
        result = std::make_shared<String>(expr->val, &scope);
    } else if (expr->type == Expression::INT) {
        result = std::make_shared<Integer>(std::stoi(expr->val), &scope);
    } else if (expr->type == Expression::REAL) {
        result = std::make_shared<Real>(std::stof(expr->val), &scope);
    } else if (expr->type == Expression::BOOL) {
        result = std::make_shared<Bool>(expr->val == "true", &scope);
    }
    return result;
}

OCA_END
