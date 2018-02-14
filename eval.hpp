/* ollieberzs 2018
** eval.hpp
** evaluate AST to value
*/ 

#pragma once

#include <memory>
#include <string>
#include "common.hpp"

OCA_BEGIN

struct Expression;
struct Value;
struct Scope;

typedef std::shared_ptr<Expression> ExprPtr;
typedef std::shared_ptr<Value> ValuePtr;

ValuePtr eval(Scope& scope, ExprPtr expr);

ValuePtr evalDef(Scope& scope, ExprPtr expr);
ValuePtr evalBlock(Scope& scope, ExprPtr expr);
ValuePtr evalCall(Scope& scope, ExprPtr expr, ValuePtr caller);
ValuePtr evalAttach(Scope& scope, ExprPtr expr);
ValuePtr evalFile(Scope& scope, ExprPtr expr);
ValuePtr evalValue(Scope& scope, ExprPtr expr);

void evalError(ExprPtr expr, const std::string& message);

OCA_END