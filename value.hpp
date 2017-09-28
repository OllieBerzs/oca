#pragma once

#include <string>
#include <vector>
#include "expression.hpp"
#include "memory.hpp"

class Scope;
struct Value
{
  std::string type;

  // Number
  float num = 0.0f;

  // String
  std::string str = "";

  // Function
  Expression* fun = nullptr;
  Scope* env = nullptr;

  Value() : type("null")
  {
    Memory::add();
  }

  Value(float num) : type("num"), num(num)
  {
    Memory::add();
  }

  Value(const std::string& str) : type("str"), str(str)
  {
    Memory::add();
  }

  Value(Expression* fun, Scope* env) : type("fun"), fun(fun), env(env)
  {
    Memory::add();
  }

  ~Value();
};

inline std::ostream& operator<<(std::ostream& stream, const Value& val)
{
  if (val.type == "num")
  {
    stream << val.num;
  }
  else if (val.type == "str")
  {
    stream << val.str;
  }
  else if (val.type == "fun")
  {
    stream << "< function: " << val.fun << " >";
  }
  else if (val.type == "null")
  {
    stream << "null";
  }
  return stream;
}
