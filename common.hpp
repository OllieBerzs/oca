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
#define TYPE_EQ(a, b) (std::type_index(typeid(a)).name() == std::type_index(typeid(b)).name())

OCA_BEGIN

class Token;
class Lexer;
class Expression;
class Parser;
class Value;
class Scope;
class Evaluator;
class ErrorHandler;
class State;
struct Arg;
class ValueCast;

typedef unsigned int uint;

typedef std::shared_ptr<Expression> ExprPtr;
typedef std::shared_ptr<Value> ValuePtr;
typedef void(*DLLfunc)(Scope&);
typedef ValuePtr Ret;
typedef Ret(*CPPFunc)(Arg);

OCA_END
