/* ollieberzs 2018
** eval.hpp
** evaluate AST to value
*/ 

#pragma once

#include <memory>
#include <string>
#include "common.hpp"

OCA_BEGIN

struct Evaluator
{
    Scope& scope;

    ObjectPtr eval(ExprPtr expr);

    ObjectPtr def(ExprPtr expr);
    ObjectPtr block(ExprPtr expr);
    ObjectPtr call(ExprPtr expr, ObjectPtr caller);
    ObjectPtr access(ExprPtr expr);
    ObjectPtr file(ExprPtr expr);
    ObjectPtr value(ExprPtr expr);

    void error(const std::string& message);
};

OCA_END