/* ollieberzs 2018
** common.hpp 
** defines and typedefs that should be included in every file
*/ 

#pragma once

#include <vector>
#include <functional>
#include <memory>

#define OCA_BEGIN namespace oca {
#define OCA_END }
#define DLLEXPORT __declspec(dllexport) void
#define ARRAY_BEGIN_INDEX 1

OCA_BEGIN

struct Token;
struct Lexer;
struct Expression;
struct Object;
struct Scope;

typedef unsigned int uint;

typedef std::shared_ptr<Expression> ExprPtr;
typedef std::shared_ptr<Object> ObjectPtr;
typedef std::shared_ptr<Scope> ScopePtr;
typedef std::function<ObjectPtr(ObjectPtr)> NativeMethod;

typedef void(*DLLfunc)(Scope&);
typedef ObjectPtr Arg;
typedef ObjectPtr Ret;

OCA_END
