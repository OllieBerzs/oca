/* ollieberzs 2018
** common.hpp
** defines and typedefs that should be included in every file
*/

#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <typeindex>

#include "ocaconf.hpp"

#define OCA_BEGIN namespace oca {
#define OCA_END }
#define DLLEXPORT __declspec(dllexport) void
#define TYPE_EQ(a, b) std::type_index(typeid(a)).name() == std::type_index(typeid(b)).name()
#define NIL(scope) std::make_shared<oca::Nil>(scope)

OCA_BEGIN

class Token;
class Lexer;
class Expression;
class Value;
class Scope;

typedef unsigned int uint;

typedef std::shared_ptr<Expression> ExprPtr;
typedef std::shared_ptr<Value> ValuePtr;
typedef std::function<ValuePtr(ValuePtr, ValuePtr, ValuePtr)> CPPFunc;

typedef void(*DLLfunc)(Scope&);
typedef ValuePtr Arg;
typedef ValuePtr Ret;

OCA_END
