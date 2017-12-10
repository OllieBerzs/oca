#pragma once

#include "expression.hpp"
#include "scope.hpp"

namespace oca::internal {

Object* evaluate(Expression* expr, Scope& env);

} // namespace oca::internal
