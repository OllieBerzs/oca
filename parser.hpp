#pragma once

#include "expression.hpp"
#include "token.hpp"
#include <vector>

namespace oca::internal {

typedef unsigned int             uint;
typedef std::vector<Token>       Tokens;
typedef std::vector<Expression*> Expressions;

void parse(const Tokens& tokens, Expressions& expressions);

bool expr(Expression*& out, uint& i, const Tokens& tokens);
bool call(Expression*& out, uint& i, const Tokens& tokens);
bool args(Expression*& out, uint& i, const Tokens& tokens);
bool attachment(Expression*& out, uint& i, const Tokens& tokens);
bool block(Expression*& out, uint& i, const Tokens& tokens);

bool string(Expression*& out, uint& i, const Tokens& tokens);
bool number(Expression*& out, uint& i, const Tokens& tokens);
bool boolean(Expression*& out, uint& i, const Tokens& tokens);

} // namespace oca::internal
