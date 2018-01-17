#pragma once

#include "interpret.hpp"

namespace oca::internal
{
    Value* define(Scope* scope, Expression* expr);
    Value* call(Scope* scope, Expression* expr);
} // namespace oca::internal