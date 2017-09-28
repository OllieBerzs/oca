#include "evaluator.hpp"

Value* operation(Expression* expr, Scope& env)
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

Value* getArgs(Expression* expr, Scope& env, int& count)
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

Value* call(Expression* expr, Scope& env)
{
  Value* fn = evaluate(expr->left, env);
  int argCount = 0;
  Value* args = getArgs(expr, env, argCount);
  if (fn->type == "fun")
  {

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

Value* evaluate(Expression* expr, Scope& env)
{
  const std::string& type = expr->type;
  if (type == "number")
  {
    std::cout << "number" << '\n';
    return new Value(std::stof(expr->value));
  }
  else if (type == "string")
  {
    std::cout << "string" << '\n';
    return new Value(expr->value);
  }
  else if (type == "null")
  {
    std::cout << "null" << '\n';
    return new Value();
  }
  else if (type == "operation")
  {
    std::cout << "operation" << '\n';
    return operation(expr, env);
  }
  else if (type == "symbol")
  {
    std::cout << "symbol" << '\n';
    return env.get(expr->value);
  }
  else if (type == "assignment")
  {
    std::cout << "assignment" << '\n';
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
    return new Value(expr, new Scope(&env));
  }
  else
  {

  }
  return nullptr;
}
