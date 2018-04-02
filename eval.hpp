/* ollieberzs 2018
** eval.hpp
** evaluate AST to value
*/

#pragma once

#include <memory>
#include <string>
#include "common.hpp"

OCA_BEGIN

class Evaluator
{
public:
    State* state;
    ExprPtr current;

    Evaluator(State* state);
    ValuePtr eval(ExprPtr expr, Scope& scope);

private:
    ValuePtr set(ExprPtr expr, Scope& scope);
    ValuePtr call(ExprPtr expr, ValuePtr caller, Scope& scope);
    ValuePtr oper(ExprPtr expr, Scope& scope);
    ValuePtr cond(ExprPtr expr, Scope& scope);
    ValuePtr access(ExprPtr expr, Scope& scope);
    ValuePtr file(ExprPtr expr, Scope& scope);
    ValuePtr value(ExprPtr expr, Scope& scope);
};

OCA_END
