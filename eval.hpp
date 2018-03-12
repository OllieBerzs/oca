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
    ValuePtr eval(ExprPtr expr, Scope& scope);

private:
    ValuePtr set(ExprPtr expr, Scope& scope);
    ValuePtr call(ExprPtr expr, ValuePtr caller, Scope& scope);
    ValuePtr cond(ExprPtr expr, Scope& scope);
    ValuePtr access(ExprPtr expr, Scope& scope);
    ValuePtr file(ExprPtr expr, Scope& scope);
    ValuePtr value(ExprPtr expr, Scope& scope);

    ValuePtr callBlock(ValuePtr val, ValuePtr arg, ValuePtr caller, ValuePtr block, Scope& scope);

    void split(ValuePtr val, const std::vector<std::string>& names, Scope& scope);
    std::vector<std::string> words(const std::string& str);

    void error(const std::string& message);
};

OCA_END
