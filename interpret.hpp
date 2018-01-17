#pragma once

#include "scope.hpp"
#include "token.hpp"

namespace oca::internal
{
    void lex(const std::string& script, std::vector<Token>& tokens);
    void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions);
    Value* evaluate(Scope* scope, Expression* expr);
}