#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "expression.hpp"

struct Method;
class Scope;

struct Object
{
  Scope* env = nullptr;

  virtual ~Object();

  virtual std::string toString();
};

struct Number : public Object
{
  float num;

  Number(float num);
  ~Number();

  std::string toString();
};

struct String : public Object
{
  std::string str;

  String(const std::string& str);
  ~String();

  std::string toString();
};

// Typedefs for native functions
typedef std::vector<Object*> Nargs;
typedef std::function<Object*(const std::vector<Object*>&)> Nfunc;

struct Method : public Object
{
  Nfunc function;
  std::vector<Expression*> exprs;
  bool native;

  Method(const std::vector<Expression*>& exprs, Scope* env);
  Method(const Nfunc& function);
  ~Method();
};
