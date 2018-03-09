/* ollieberzs 2018
** eval.hpp
** evaluate AST to value
*/

#pragma once

#include <memory>
#include <string>
#include "common.hpp"

OCA_BEGIN

class State;

class Evaluator
{
public:
    State* state;

    ValuePtr eval(ExprPtr expr);

private:
    ValuePtr set(ExprPtr expr);
    ValuePtr call(ExprPtr expr, ValuePtr caller);
    ValuePtr cond(ExprPtr expr);
    ValuePtr access(ExprPtr expr);
    ValuePtr file(ExprPtr expr);
    ValuePtr value(ExprPtr expr);

    void error(const std::string& message);
};

OCA_END
