#include "scope.hpp"
#include "memory.hpp"

std::string Object::toString() const { return "err"; }

Number::Number(float num) : num(num)
{
  Memory::add('n');
  //TODO: Define base methods
}

Number::~Number() { Memory::rem('n'); }

std::string Number::toString() const { return std::to_string(num); }

String::String(const std::string& str) : str(str)
{
  Memory::add('s');
  //TODO: Define base methods
}

String::~String() { Memory::rem('s'); }

std::string String::toString() const { return str; }

Method::Method(const std::vector<Expression*>& exprs, Scope* scope)
  : exprs(exprs), native(false), scope(scope) { Memory::add('m'); }

Method::Method(const Nfunc& function)
  : function(function), native(true), scope(nullptr) { Memory::add('m'); }

Method::~Method()
{
  Memory::rem('m');
  for (auto expr : exprs) delete expr;
  if (scope) delete scope;
}

std::string Method::toString() const { return "<method: " + std::to_string(native) + ">"; }
