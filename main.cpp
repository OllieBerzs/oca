#include "utils.hpp"
#include "errors.hpp"
#include "lexer.hpp"

int main(int argc, char** argv)
{
  if (argc < 2) ERR << "No script file provided\n";
  std::string script = file(argv[1]);

  std::vector<Token> tokens;
  lex(script, tokens);

  for (const Token& token : tokens)
  {
    std::cout << token << '\n';
  }
}
