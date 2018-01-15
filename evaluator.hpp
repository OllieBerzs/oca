#pragma once

#include "value.hpp"
#include "scope.hpp"

namespace oca::internal
{

    Value* evaluate(Scope* scope, Expression* expr);

    Value* define(Scope* scope, Expression* expr);
    Value* call(Scope* scope, Expression* expr);

} // namespace oca::internal