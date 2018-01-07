#include "evaluator.hpp"
#include "errors.hpp"

namespace oca::internal
{

  Value* evaluate(Scope* scope, Expression* expr)
  {
    if (expr->type == "integer") return new Value("integer", expr, false);
    else if (expr->type == "float") return new Value("float", expr, false);
    else if (expr->type == "string") return new Value("string", expr, false);
    else if (expr->type == "boolean") return new Value("boolean", expr, false);
    else if (expr->type == "def") return define(scope, expr);
    else if (expr->type == "call") return call(scope, expr);
    else return new Value("nil", nullptr, false);
  }

  Value* define(Scope* scope, Expression* expr)
  {
    Value* val = evaluate(scope, expr->right);
    val->isNamed = true;
    scope->set(expr->value, val);
    return val;
  }
  Value* call(Scope* scope, Expression* expr)
  {
    Value* method = scope->get(expr->value);

    // get arguments
    std::vector<Value*> args;
    Expression* arg = expr->right;
    while (arg)
    {
      args.push_back(evaluate(scope, arg->left));
      arg = arg->right;
    }

    if (!method) errors::evalError("UNDEFINED", "Name " + expr->value + " is undefined");

    if (method->type == "native")
    {
      return method->native(args);
    }
    return new Value("nil", nullptr, false);
  }

} // namespace oca::internal