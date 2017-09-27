#include "evaluator.hpp"

Value* operation(Expression* expr, Table& env)
{
  Value* arg1 = evaluate(expr->left, env);
  Value* arg2 = evaluate(expr->right, env);
  if (expr->value == "+")
  {
    return new Value(arg1->num + arg2->num);
  }
  else if (expr->value == "-")
  {
    return new Value(arg1->num - arg2->num);
  }
  else if (expr->value == "*")
  {
    return new Value(arg1->num * arg2->num);
  }
  else if (expr->value == "/")
  {
    return new Value(arg1->num / arg2->num);
  }
  else
  {
    ERR << "Unknown operator '" << expr->value << "'";
  }
  return new Value();
}

Value* getArgs(Expression* expr, Table& env, int& count)
{
  Expression* arg = expr->right;
  while (arg)
  {
    count++;
    arg = arg->right;
  }
  Value* args = new Value[count];
  arg = expr->right;
  for (int i = 0; i < count; i++)
  {
    args[i] = *evaluate(arg->left, env);
    arg = arg->right;
  }
  return args;
}

Value* call(Expression* expr, Table& env)
{
  Value* fn = evaluate(expr->left, env);
  int argCount = 0;
  Value* args = getArgs(expr, env, argCount);
  if (fn->type == "fun")
  {
    // TODO: get parameters from function

  }
  else if (fn->type == "native")
  {

  }
  else
  {
    ERR << "Not a function: " << expr;
  }
  return nullptr;
}

Value* evaluate(Expression* expr, Table& env)
{
  const std::string& type = expr->type;
  if (type == "number")
  {
    return new Value(std::stof(expr->value));
  }
  else if (type == "string")
  {
    return new Value(expr->value);
  }
  else if (type == "null")
  {
    return new Value();
  }
  else if (type == "operation")
  {
    return operation(expr, env);
  }
  else if (type == "symbol")
  {
    return env.get(expr->value);
  }
  else if (type == "assignment")
  {
    const std::string& name = expr->left->value;
    Value* val = evaluate(expr->right, env);
    env.set(name, val);
    return val;
  }
  else if (type == "call")
  {
    return call(expr, env);
  }
  else if (type == "function")
  {
    return new Value(expr, new Table(&env));
  }
  else
  {

  }
  return nullptr;
}
