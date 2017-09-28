#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include "memory.hpp"

struct Expression
{
  std::string type;
  std::string value;
  Expression* left;
  Expression* right;

  Expression(const std::string& type, const std::string& value)
    : type(type), value(value), left(nullptr), right(nullptr)
  {
    Memory::add();
  }

  Expression(const std::string& type, const std::string& value, Expression* l, Expression* r)
    : type(type), value(value), left(l), right(r)
  {
    Memory::add();
  }

  ~Expression()
  {
    delete left;
    delete right;
    Memory::rem();
  }
};

inline void printTree(const Expression& e, std::ostream& stream, int indent = 0)
{
  std::string data = "(\"" + e.type + " " + e.value + "\")\n";
  stream << std::setw(data.size() + indent) << data;
  if (e.left) printTree(*e.left, stream, indent + 2);
  if (e.right) printTree(*e.right, stream, indent + 2);
}

inline std::ostream& operator<<(std::ostream& stream, const Expression& expr)
{
  printTree(expr, stream);
  return stream;
}
