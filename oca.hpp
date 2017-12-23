#include "errors.hpp"
#include "evaluator.hpp"
#include "lexer.hpp"
#include "native.hpp"
#include "parser.hpp"
#include "utils.hpp"

namespace oca {

void script(const std::string& source) {

  std::vector<internal::Token>       tokens;
  std::vector<internal::Expression*> exprs;

  lex(source, tokens);

#ifdef OUT_TOKENS
  std::cout << "\n============TOKENS===========\n\n";
  for (const internal::Token& token : tokens) std::cout << token << '\n';
#endif

  parse(tokens, exprs);

#ifdef OUT_TREE
  std::cout << "\n============TREE=============\n\n";
  for (auto e : exprs) std::cout << *e << '\n';
#endif
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

} // namespace oca
