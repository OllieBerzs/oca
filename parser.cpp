#include "parser.hpp"
#include "errors.hpp"

class Parser
{
  std::vector<Token> tokens;
  std::vector<std::string> stop;
  unsigned int* index;

public:
  Parser(const std::vector<Token>& tokens, const std::vector<std::string>& stop, unsigned int* index)
    : tokens(tokens), stop(stop), index(index) {}

  void bump()
  {
    (*index)++;
  }

  bool isEmpty()
  {
    return (*index) >= tokens.size();
  }

  Expression* multi(const std::string& sep, const std::string& end)
  {
    if (isEmpty()) ERR << "Parser: missing '" << end << "'";

    Expression* ret = nullptr;
    Expression* prev = nullptr;

    const Token& token = tokens[*index];
    if (token.type == end) bump();
    else
    {
      Parser argParser(tokens, {sep, end}, index);
      while (tokens[*index].type != end)
      {
        Expression* expr = argParser.next(new Expression);
        if (expr->type != "none")
        {
          if (!ret)
          {
            ret = new Expression("arg", "", expr, nullptr);
            prev = ret;
          }
          else
          {
            prev->right = new Expression("arg", "", expr, nullptr);
            prev = prev->right;
          }
        }
        if (tokens[(*index)].type != end) bump();
      }
    }
    bump();
    return ret;
  }

  Expression* next(Expression* prev)
  {
    if (isEmpty()) ERR << "Parser: ran out of tokens";

    const Token& token = tokens[*index];
    for (const std::string& s : stop)
    {
      if (token.type == s) return prev;
    }
    bump();

    if (prev->type == "none" && (token.type == "symbol" || token.type == "number" || token.type == "string"))
    {
      return next(new Expression(token.type, token.value));
    }
    else if (token.type == "operator")
    {
      Expression* nxt = next(new Expression);
      return next(new Expression("operation", token.value, prev, nxt));
    }
    else if (token.type == "(")
    {
      Expression* args = multi(",", ")");
      return next(new Expression("call", prev->value, nullptr, args));
    }
    else if (token.type == "{")
    {

    }
    else if (token.type == "=")
    {
      if (prev->type != "symbol") ERR << "Parser: Assignment to " << prev->type;
      Expression* nxt = next(new Expression);
      return next(new Expression("assignment", token.type, prev, nxt));
    }
    else
    {
      ERR << "Parser: Unknown token " << token;
    }
    return new Expression;
  }
};

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
  unsigned int* index = new unsigned int(0);
  Parser parser(tokens, {"end"}, index);
  while (!parser.isEmpty())
  {
    Expression* expr = parser.next(new Expression);
    if (expr->type != "none")
    {
      expressions.push_back(expr);
    }
    parser.bump();
  }
  delete index;
}
