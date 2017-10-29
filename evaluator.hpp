#pragma once

#include "scope.hpp"
#include "expression.hpp"

Object* evaluate(Expression* expr, Scope& env);
