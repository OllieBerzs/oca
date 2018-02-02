/* ollieberzs 2018
** eval.cpp
** evaluate AST to value
*/ 

#include <iostream>

#include "eval.hpp"
#include "parse.hpp"
#include "value.hpp"

OCA_BEGIN

ValuePtr eval(Scope& scope, ExprPtr expr)
{
    if (expr->type == "int") return Value::makeInt(expr);
    else if (expr->type == "float") return Value::makeFloat(expr);
    else if (expr->type == "str") return Value::makeStr(expr);
    else if (expr->type == "bool") return Value::makeBool(expr);
    else if (expr->type == "block") return Value::makeBlock(expr);
    else if (expr->type == "def") return define(scope, expr);
    else if (expr->type == "call") return call(scope, expr);
    else return nullptr;
}
// ----------------------------

ValuePtr define(Scope& scope, ExprPtr expr)
{
    ValuePtr val = eval(scope, expr->right);
    scope.set(expr->val, val);
    return val;
}

ValuePtr call(Scope& scope, ExprPtr expr)
{
    // for functions and operators check if argument count and type/scope are the same
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
        ExprPtr block = method->val;
        ExprPtr mainBody = block->left;
        //Expression* elseBody = block->right;
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
            ret = eval(methodScope, expr->left);
            expr = expr->right;
        }
        methodScope.clean();
        return ret;
    }
    else
    {
        // it is not callable
        return method;
    }
    return nullptr;
}
// ----------------------------

void evalError(ExprPtr expr, const std::string& message)
{
    //const Token& errTok = *expr->token;

    // get error line
    /*std::string line = "";
    uint count = 1;
    for (char c : state.ls->source)
    {
        if (c == '\n')
        {
            if (count == errTok.line) break;
            count++;
            line = "";
        }
        else line += c;
    }*/

    system("printf ''");
    std::cout << "\033[38;5;14m";
    std::cout << "-- ERROR -------------------- " << "\n"; //state.ls->sourceName << "\n";
    std::cout << "\033[0m";
    //std::cout << errTok.line << "| ";
    //std::cout << "\033[38;5;15m";
    //std::cout << lineStart;
    //std::cout << "\033[48;5;9m";
    //std::cout << (errTok.val == "" ? errTok.type : errTok.val);
    //std::cout << "\033[0m";
    //std::cout << "\033[38;5;15m";
    //std::cout << lineEnd << "\n";
    //std::cout << "\033[0m";
    std::cout << message << "\n";

    std::cin.get();
    exit(1);
}

OCA_END