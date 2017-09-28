#pragma once

#include "scope.hpp"
#include "expression.hpp"

Value* evaluate(Expression* expr, Scope& env);
