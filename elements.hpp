#pragma once

#include <map>
#include <vector>
#include <string>
#include "expression.hpp"

struct Method;

struct Object
{
  std::map<std::string, Method*> names;
};

struct Number : public Object
{
  float num;

  Number(float num);
};

struct String : public Object
{
  std::string str;

  String(const std::string& str);
};

struct Method
{
  int exprCount;
  Expression* exprs;

  Method(int count, Expression* exprs);
};
