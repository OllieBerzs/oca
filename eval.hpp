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

struct Evaluator
{
    State* state;

    ObjectPtr eval(ExprPtr expr);

    ObjectPtr set(ExprPtr expr);
    ObjectPtr call(ExprPtr expr, ObjectPtr caller);
    ObjectPtr cond(ExprPtr expr);
    ObjectPtr access(ExprPtr expr);
    ObjectPtr file(ExprPtr expr);
    ObjectPtr value(ExprPtr expr);

    void error(const std::string& message);
};

OCA_END
