#pragma once

#include <vector>
#include "token.hpp"

struct Expression {};

struct Assignment : Expression
{

};

struct Call : Expression
{

};

struct Operation : Expression
{

};

struct Function : Expression
{

};

void parse(const std::vector<Token>& tokens, std::vector<Expression>& expressions);
