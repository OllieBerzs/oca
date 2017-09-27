#pragma once

#include <string>
#include <vector>
#include "expression.hpp"
#include "memory.hpp"

class Table;
struct Value
{
  std::string type;

  // Number
  float num;

  // String
  std::string str;

  // Function
  Expression* fun;
  Table* env;

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

  Value(Expression* fun, Table* env) : type("fun"), fun(fun), env(env)
  {
    Memory::add();
  }

  ~Value();
};
