#pragma once

#include <iostream>
#include "lexer.hpp"

namespace oca
{

void script(const std::string& source)
{
  std::vector<internal::Token> tokens;

  internal::lex(source, tokens);

  #ifdef OUT_TOKENS
    std::cout << "------------TOKENS------------\n";
    for (const internal::Token& token : tokens) std::cout << token << '\n';
  #endif
}

} // namespace oca
