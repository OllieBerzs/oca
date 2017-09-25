#pragma once

#include <vector>
#include "token.hpp"
#include "expression.hpp"

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions);
