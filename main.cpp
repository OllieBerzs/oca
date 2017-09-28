#include "utils.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"

int main(int argc, char** argv)
{
  if (argc < 2) ERR << "No script file provided\n";
  std::string script = file(argv[1]);

  std::vector<Token> tokens;
  lex(script, tokens);

  std::cout << "-------------------------" << '\n';
  std::cout << "---------TOKENS----------" << '\n';
  std::cout << "-------------------------" << '\n';

  for (const Token& token : tokens)
  {
    std::cout << token << '\n';
  }

  std::cout << "-------------------------" << '\n';
  std::cout << "-------EXPRESSIONS-------" << '\n';
  std::cout << "-------------------------" << '\n';

  std::vector<Expression*> expressions;
  parse(tokens, expressions);

  for (auto e : expressions)
  {
    std::cout << *e;
    std::cout << "-------------------------" << '\n';
  }

  std::cout << "---------VALUES----------" << '\n';
  std::cout << "-------------------------" << '\n';

  Scope scope(nullptr);

  for (auto e : expressions)
  {
    Value* val = evaluate(e, scope);
    std::cout << *val << "\n";
  }

  std::cout << "-------------------------" << '\n';
  std::cout << "---------MEMORY----------" << '\n';
  std::cout << "-------------------------" << '\n';

  unsigned int exprCount = expressions.size();
  int total = Memory::get();

  // cleanup
  for (auto e : expressions) delete e;
  expressions.clear();
  scope.clean();

  int remaining = Memory::get();
  std::cout << "memory usage: " << total << " -> " << remaining << '\n';
  std::cout << "Expressions in script: " << exprCount << '\n';
}
