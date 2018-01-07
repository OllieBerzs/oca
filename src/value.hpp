#pragma once

#include <string>
#include <vector>
#include "expression.hpp"

namespace oca::internal
{

  struct Value
  {
    std::string type;
    Expression* expr;
    bool isNamed;
    Value* (*native)(std::vector<Value*>);

    Value(const std::string& type, Expression* expr, bool n) 
      : type(type), expr(expr), isNamed(n) {}
    ~Value()
    {
      if (expr) delete expr;
    }
  };

} // namespace oca::internal