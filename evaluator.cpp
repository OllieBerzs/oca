#include "evaluator.hpp"
#include "errors.hpp"

namespace oca::internal
{

    Value* evaluate(Scope* scope, Expression* expr)
    {
        if (expr->type == "integer") return new Value("integer", expr);
        else if (expr->type == "float") return new Value("float", expr);
        else if (expr->type == "string") return new Value("string", expr);
        else if (expr->type == "boolean") return new Value("boolean", expr);
        else if (expr->type == "block") return new Value("block", expr);
        else if (expr->type == "def") return define(scope, expr);
        else if (expr->type == "call") return call(scope, expr);
        else return new Value("nil", nullptr);
    }

    Value* define(Scope* scope, Expression* expr)
    {
        Value* val = evaluate(scope, expr->right);
        val->refCount++;
        scope->set(expr->value, val);
        return val;
    }
    Value* call(Scope* scope, Expression* expr)
    {
        Value* method = scope->get(expr->value);

        // get arguments
        std::vector<Value*> args;
        Expression* arg = expr->right;
        while (arg && arg->left)
        {
            args.push_back(evaluate(scope, arg->left));
            arg = arg->right;
        }

        if (!method) errors::evalError("UNDEFINED", "Name '" + expr->value + "' is undefined");

        if (method->type == "native")
        {
            return method->native(args);
        }
        else if (method->type == "block")
        {
            Expression* block = method->expr;
            Expression* mainBody = block->left;
            //Expression* elseBody = block->right;
            Value* ret = nullptr;
            Scope methodScope(scope);

            // add parameters to scope
            Expression* params = mainBody->left;
            unsigned int index = 0;
            while (params)
            {
                methodScope.set(params->value, args[index++]);
                params = params->right;
            }

            // eval main body
            Expression* expr = mainBody->right;
            while (expr && expr->left)
            {
                ret = evaluate(&methodScope, expr->left);
                expr = expr->right;
            }
            ret->refCount++;
            methodScope.clean();
            ret->refCount--;
            return ret;
        }
        else
        {
            // it is not callable
            return method;
        }
        return new Value("nil", nullptr);
    }

} // namespace oca::internal