#pragma once

#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
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
      std::cout << "----------- TOKENS -----------\n";
      for (const internal::Token& token : tokens) std::cout << token << "\n";
    #endif
  
    // Parsing
    std::vector<internal::Expression*> exprs;
    internal::parse(tokens, exprs);
    #ifdef OUT_TREE
      std::cout << "------------ TREE ------------\n";
      for (const internal::Expression* e : exprs) std::cout << *e << "\n";
    #endif

    // Evaluating
    std::cout << "Running ------------ " << filename << "\n";

    // Create scope
    internal::Scope scope(nullptr);

    internal::Value* print = new internal::Value("native", nullptr, true);
    print->native = [](std::vector<internal::Value*> vals) -> internal::Value*
    {
      std::cout << vals[0]->expr->value << "\n";
      return new internal::Value("nil", nullptr, false);
    };
    scope.set("print", print);

    for (auto e : exprs)
    {
      internal::Value* val = internal::evaluate(&scope, e);
      if (val->type == "nil") std::cout << "->nil\n";
      else std::cout << "->" << val->expr->value << "\n";
      if (!val->isNamed) delete val;
    }
  }

} // namespace oca
