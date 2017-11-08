#include "utils.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "native.hpp"

int main(int argc, char** argv)
{
  if (argc < 2) ERR << "No script file provided";

  std::vector<Token> tokens;
  std::vector<Expression*> exprs;

  std::string script = file(argv[1]);

  //Scope scope(nullptr);
  //nativeMethods(scope);

  std::cout << "=========================" << '\n';

  lex(script, tokens);

  for (const Token& token : tokens) std::cout << token << '\n';
  std::cout << "-------------------------" << '\n';
  std::cout << "-------------------------" << '\n';

  parse(tokens, exprs);

  for (auto e : exprs) std::cout << *e << '\n';
  std::cout << "-------------------------" << '\n';
  std::cout << "-------------------------" << '\n';

  /*std::cout << "=========================" << '\n';
  for (const Token& token : tokens) std::cout << token << '\n';
  std::cout << "-------------------------" << '\n';
  std::cout << "-------------------------" << '\n';
  for (auto e : exprs) std::cout << *e << '\n';
  std::cout << "-------------------------" << '\n';
  std::cout << "-------------------------" << '\n';
  for (auto e : exprs)
  {
    Object* o = evaluate(e, scope);
    if (o) std::cout << "=> " << o->toString() << '\n';
    else std::cout << "=> null" << '\n';
  }
  std::cout << "-------------------------" << '\n';
  std::cout << "-------------------------" << '\n';

  // Cleanup
  for (auto e : exprs) delete e;
  scope.clean();

  std::cout << "memory usage: [" << Memory::get() << "]\n";

  std::cout << "=========================" << '\n';*/
}
