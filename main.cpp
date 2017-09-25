#include "utils.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <iomanip>

void printTree(Expression* e, int indent = 0)
{
  if (e)
  {
    std::cout << e->type << " " << e->value << "\n ";
    if (e->left) printTree(e->left, indent + 4);
    if (e->right) printTree(e->right, indent + 4);
    //if (indent) std::cout << std::setw(indent) << ' ';
  }
}

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

  std::cout << "----------------------------------------" << '\n';

  //std::vector<Expression> expressions;
  std::vector<Expression*> expressions;
  parse(tokens, expressions);

  printTree(expressions[0]);
  //Expression* test = expressions[0]->right->left->left;
  //std::cout << (test != nullptr) << '\n';
  //std::cout << test->type << " " << test->value << '\n';

  /*std::vector<Expression*> exs;
  Expression* e = expressions[0];
  bool a = true;

  while (a)
  {
    exs.push_back(e);
    if (e->left)
    {
      e = e->left;
    }
    else a = false;
  }

  for (auto ex : exs)
  {
    std::cout << ex->type << " " << ex->value << "\n";
  }*/
}
