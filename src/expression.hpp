#pragma once

#include <iomanip>
#include <iostream>
#include <string>

namespace oca::internal 
{

struct Expression 
{
  std::string type;
  std::string value;
  Expression* left = nullptr;
  Expression* right = nullptr;

  Expression(const std::string& type, const std::string& value) : type(type), value(value) { }

  ~Expression() 
  {
    delete left;
    delete right;
  }
};

inline void printTree(const Expression& e, std::ostream& stream, const std::string& branch, 
  int indent = 0) 
{
  std::string data = branch + "(\"" + e.type;
  if (e.value != "") data += " " + e.value;
  data += "\")\n";

  stream << std::setw(data.size() + indent) << data;

  if (e.left) printTree(*e.left, stream, "<-", indent + 2);
  if (e.right) printTree(*e.right, stream, "->", indent + 2);
}

inline std::ostream& operator<<(std::ostream& stream, const Expression& expr) 
{
  printTree(expr, stream, "");
  return stream;
}

} // namespace oca::internal
