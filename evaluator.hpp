#pragma once

#include "scope.hpp"
#include "expression.hpp"

namespace oca::internal
{

Object* evaluate(Expression* expr, Scope& env);

} // namespace oca::internal
