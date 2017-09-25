#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <memory>

struct Expression
{
  std::string type;
  std::string value;
  Expression* left;
  Expression* right;

  Expression() : type("none"), value(""), left(nullptr), right(nullptr) {}

  Expression(const std::string& type, const std::string& value)
    : type(type), value(value), left(nullptr), right(nullptr) {}

  Expression(const std::string& type, const std::string& value, Expression* l, Expression* r)
    : type(type), value(value), left(l), right(r) {}

  ~Expression()
  {
    delete left;
    delete right;
  }
};

inline void printTree(const Expression* e, std::ostream& stream, int indent = 0)
{
  if (e)
  {
    stream << e->type << " " << e->value << "\n ";
    if (e->left) printTree(e->left, stream, indent + 4);
    if (e->right) printTree(e->right, stream, indent + 4);
    if (indent) stream << std::setw(indent) << ' ';
  }
}

inline std::ostream& operator<<(std::ostream& stream, const Expression* expr)
{
  printTree(expr, stream);
  stream << '\n';
  return stream;
}
