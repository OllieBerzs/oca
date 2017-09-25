#pragma once

#include <string>
#include <iostream>
#include <memory>

struct Expression
{
  std::string type = "---";
  std::string value = "---";
  //std::shared_ptr<Expression> left;
  //std::shared_ptr<Expression> right;
  Expression* left;
  Expression* right;

  Expression(const std::string& type, const std::string& value) : type(type), value(value)
  {
    left = nullptr;
    right = nullptr;
  }

  //Expression(const std::string& type, const std::string& value, const Expression& l, const Expression& r) : type(type), value(value)
  Expression(const std::string& type, const std::string& value, Expression* l, Expression* r) : type(type), value(value)
  {
    //left = std::make_shared<Expression>(l.type, l.value, *l.left, *l.right);
    //right = std::make_shared<Expression>(r.type, r.value, *r.left, *r.right);
    left = l;
    right = r;
  }

  ~Expression()
  {
    delete left;
    delete right;
  }

  void print()
  {
    std::cout << "=====\n";
    std::cout << "[" << type << " " << value << "]\n";
    if (left)
    {
      std::cout << "[left]\n";
      left->print();
    }
    if (right)
    {
      std::cout << "[right]\n";
      right->print();
    }
  }
};

inline std::ostream& operator<<(std::ostream& stream, const Expression* expr)
{
  stream << "[" << expr->type << " " << expr->value << "]\n";
  if (expr->left)
  {
    stream << "[left]\n" << expr->left << "\n";
  }
  if (expr->right)
  {
    stream << "[right]\n" << expr->right << "\n";
  }
  return stream;
}
