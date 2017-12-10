#include "evaluator.hpp"

namespace oca::internal {

void getArgs(Expression* expr, Scope& env, std::vector<Object*>& args) {
  // Expression* arg = expr->right;
  // while (arg)
  //{
  //    args.push_back(evaluate(arg->left, env));
  //    arg = arg->right;
  //}
}

Object* call(Expression* expr, Scope& env) {
  const std::string& name = expr->value;
  // const Expression* caller = expr->left;
  std::vector<Object*> args;
  getArgs(expr, env, args);

  Method* method = env.get(name);
  if (!method) ERR << "No method named '" << name << "'";

  if (method->native) {
    Object* ret = method->function(args);
    for (auto a : args) delete a;
    return ret;
  } else {
  }
  return nullptr;
}

Object* evaluate(Expression* expr, Scope& env) {
  int type = expr->type;
  if (type == E_NUMBER) {
    return new Number(std::stof(expr->value));
  } else if (type == E_STRING) {
    return new String(expr->value);
  } else if (type == E_CALL) {
    return call(expr, env);
  } else if (type == E_METHOD) {
    return nullptr;
  }
  return nullptr;
}

} // namespace oca::internal
