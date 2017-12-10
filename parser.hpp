#pragma once

#include "expression.hpp"
#include "token.hpp"
#include <vector>

namespace oca::internal {

void parse(const std::vector<Token>& tokens,
           std::vector<Expression*>& expressions);

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);
bool call(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);
bool args(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);
bool attachment(Expression*& out, unsigned int& i,
                const std::vector<Token>& tokens);
bool block(Expression*& out, unsigned int& i, const std::vector<Token>& tokens);

bool string(Expression*& out, unsigned int& i,
            const std::vector<Token>& tokens);
bool number(Expression*& out, unsigned int& i,
            const std::vector<Token>& tokens);
bool boolean(Expression*& out, unsigned int& i,
             const std::vector<Token>& tokens);

} // namespace oca::internal
