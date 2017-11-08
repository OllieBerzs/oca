#pragma once

#include <vector>
#include "token.hpp"
#include "expression.hpp"

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions);

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);
bool call(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);
bool args(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);
