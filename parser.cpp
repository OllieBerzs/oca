#include "parser.hpp"
#include "errors.hpp"

//Expression next(const std::vector<Token>& tokens, unsigned int& index, const Expression& prev)
Expression* next(const std::vector<Token>& tokens, unsigned int& index, Expression* prev)
{
  const Token& token = tokens[index];
  if (token.type == "end") return prev;
  index++;

  //if (prev.type == "none" && (token.type == "symbol" || token.type == "number" || token.type == "string"))
  if (prev->type == "none" && (token.type == "symbol" || token.type == "number" || token.type == "string"))
  {
    //Expression e = next(tokens, index, Expression(token.type, token.value));
    Expression* e = next(tokens, index, new Expression(token.type, token.value));
    return e;
  }
  else if (token.type == "operator")
  {
    //Expression nxt = next(tokens, index, Expression("none", ""));
    Expression* nxt = next(tokens, index, new Expression("none", ""));
    //return next(tokens, index, Expression("operation", token.value, prev, nxt));
    return next(tokens, index, new Expression("operation", token.value, prev, nxt));
  }
  else if (token.type == "=")
  {
    //if (prev.type != "symbol") ERR << "Parser: Assignment to " << prev.type;
    if (prev->type != "symbol") ERR << "Parser: Assignment to " << prev->type;
    //Expression nxt = next(tokens, index, Expression("none", ""));
    Expression* nxt = next(tokens, index, new Expression("none", ""));
    //return next(tokens, index, Expression("assignment", token.type, prev, nxt));
    return next(tokens, index, new Expression("assignment", token.type, prev, nxt));
  }
  else
  {
    ERR << "Parser: Unknown token " << token;
  }
  //return Expression("", "");
  return new Expression("", "");
}

//void parse(const std::vector<Token>& tokens, std::vector<Expression>& expressions)
void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
  unsigned int index = 0;
  while (index < tokens.size())
  {
    //Expression expr = next(tokens, index, Expression("none", ""));
    Expression* expr = next(tokens, index, new Expression("none", ""));
    //if (expr.type != "none")
    if (expr->type != "none")
    {
      expressions.push_back(expr);
    }
    index++;
  }
}
