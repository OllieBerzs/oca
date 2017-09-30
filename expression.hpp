#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include "memory.hpp"

enum
{
  E_NUMBER,
  E_STRING,
  E_CALL,
  E_METHOD,
  E_ARG,
  E_NULL
};
const std::string E_TYPES[6]
{
  "number",
  "string",
  "call",
  "method",
  "arg",
  "null"
};

struct Expression
{
  int type;
  std::string value;
  Expression* left;
  Expression* right;

  Expression(int type, const std::string& value)
    : type(type), value(value), left(nullptr), right(nullptr)
  {
    Memory::add('e');
  }

  Expression(int type, const std::string& value, Expression* l, Expression* r)
    : type(type), value(value), left(l), right(r)
  {
    Memory::add('e');
  }

  ~Expression()
  {
    delete left;
    delete right;
    Memory::rem('e');
  }
};

inline void printTree(const Expression& e, std::ostream& stream, int indent = 0)
{
  std::string data = "(\"" + E_TYPES[e.type] + " " + e.value + "\")\n";
  stream << std::setw(data.size() + indent) << data;
  if (e.left) printTree(*e.left, stream, indent + 2);
  if (e.right) printTree(*e.right, stream, indent + 2);
}

inline std::ostream& operator<<(std::ostream& stream, const Expression& expr)
{
  printTree(expr, stream);
  return stream;
}
