#pragma once

#include "scope.hpp"

void nativeMethods(Scope& scope)
{
  scope.set("print", new Method([](const Nargs& args) -> Object*
  {
    if (args[0]) std::cout << args[0]->toString() << '\n';
    else std::cout << "null" << '\n';
    return nullptr;
  }));
}
