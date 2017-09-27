#pragma once

#include "table.hpp"
#include "expression.hpp"

Value* evaluate(Expression* expr, Table& env);
