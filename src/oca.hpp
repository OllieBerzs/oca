#pragma once

#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "errors.hpp"

namespace oca
{

void script(const std::string& source, const std::string& filename = "")
{
  internal::errors::Script::data = source;
  internal::errors::Script::file = filename;

  // Lexing
  std::vector<internal::Token> tokens;
  internal::lex(source, tokens);
  #ifdef OUT_TOKENS
    std::cout << "------------TOKENS------------\n";
    for (const internal::Token& token : tokens) std::cout << token << "\n";
  #endif
  
  // Parsing
  std::vector<internal::Expression*> exprs;
  internal::parse(tokens, exprs);
  #ifdef OUT_TREE
    std::cout << "-------------TREE-------------\n";
    for (const internal::Expression* e : exprs) std::cout << *e << "\n";
  #endif
}

} // namespace oca
