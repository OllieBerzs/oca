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

  scope.set("print", new Method([](const Nargs& args) -> Object*
  {
    if (args[0]) std::cout << args[0]->toString() << '\n';
    else std::cout << "null" << '\n';
    return nullptr;
  }));

  for (auto e : expressions)
  {
    Object* obj = evaluate(e, scope);
    if (obj) std::cout << "=> " << obj->toString() << "\n";
    else std::cout << "=> " << "null" << "\n";
    delete e;
  }

  scope.clean();

  std::cout << "-------------------------" << '\n';
  std::cout << "---------MEMORY----------" << '\n';
  std::cout << "-------------------------" << '\n';

  std::cout << "memory usage: [" << Memory::get() << "]\n";
}
