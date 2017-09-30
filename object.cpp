#include "object.hpp"
#include "memory.hpp"

std::string Object::toString() { return "err"; }

Number::Number(float num) : num(num)
{
  Memory::add('n');
  //TODO: Define base methods
}
Number::~Number() { Memory::rem('n'); }
std::string Number::toString()
{
  return std::to_string(num);
}

String::String(const std::string& str) : str(str)
{
  Memory::add('s');
  //TODO: Define base methods
}
String::~String() { Memory::rem('s'); }
std::string String::toString()
{
  return str;
}

Method::Method(const std::vector<Expression*>& exprs, Scope* e)
  : exprs(exprs), native(false)
{
  env = e;
  Memory::add('m');
}
Method::Method(const Nfunc& function)
  : function(function), native(true)
{
  Memory::add('m');
}
Method::~Method()
{
  Memory::rem('m');
  for (auto expr : exprs) delete expr;
}
