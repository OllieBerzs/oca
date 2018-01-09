#pragma once

#include <string>
#include <vector>
#include <functional>
#include "expression.hpp"

namespace oca::internal
{
  struct Value;
  typedef std::function<Value*(std::vector<Value*>)> NativeMethod;
  struct Value
  {
    std::string type;
    Expression* expr;
    bool isNamed;
    NativeMethod native;

    Value(const std::string& type, Expression* expr, bool n) 
      : type(type), expr(expr), isNamed(n) {}
    ~Value()
    {
      if (expr) delete expr;
    }
  };

} // namespace oca::internal