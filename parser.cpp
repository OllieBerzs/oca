#include "parser.hpp"
#include "errors.hpp"

class Parser
{
  std::vector<Token> _tokens;
  std::vector<std::string> _stop;
  unsigned int* _index;

public:
  Parser(const std::vector<Token>& tokens, const std::vector<std::string>& stop, unsigned int* index)
    : _tokens(tokens), _stop(stop), _index(index) {}

  void bump()
  {
    (*_index)++;
  }

  bool isEmpty()
  {
    return (*_index) >= _tokens.size();
  }

  Expression* multi(const std::string& sep, const std::string& end)
  {
    if (isEmpty()) ERR << "Parser: missing '" << end << "'";

    Expression* ret = nullptr;
    Expression* prev = nullptr;

    const Token& token = _tokens[*_index];
    if (token.type != end)
    {
      Parser argParser(_tokens, {sep, end}, _index);
      while (_tokens[*_index].type != end)
      {
        Expression* expr = argParser.next(nullptr);
        if (expr != nullptr)
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
        if (_tokens[(*_index)].type != end) bump();
      }
    }
    bump();
    return ret;
  }

  Expression* findArgs()
  {
    if (_tokens[*_index].type == "(")
    {
      bump();
      return multi(",", ")");
    }
    return nullptr;
  }

  Expression* next(Expression* prev)
  {
    if (isEmpty()) ERR << "Parser: ran out of tokens";

    const Token& token = _tokens[*_index];
    for (const std::string& s : _stop)
    {
      if (token.type == s) return prev;
    }
    bump();

    if (prev == nullptr && (token.type == "symbol" || token.type == "number" || token.type == "string"))
    {
      return next(new Expression(token.type, token.value));
    }
    else if (token.type == "operator")
    {
      Expression* nxt = next(nullptr);
      return next(new Expression("operation", token.value, prev, nxt));
    }
    else if (token.type == "(")
    {
      Expression* args = multi(",", ")");
      return next(new Expression("call", "", new Expression("symbol", prev->value), args));
    }
    else if (token.type == "{")
    {
      Expression* args = findArgs();
      Expression* body = multi("end", "}");
      return next(new Expression("function", "", args, body));
    }
    else if (token.type == "=")
    {
      if (prev->type != "symbol") ERR << "Parser: Assignment to " << prev->type;
      Expression* nxt = next(nullptr);
      return next(new Expression("assignment", token.type, prev, nxt));
    }
    else
    {
      ERR << "Parser: Unknown token " << token;
    }
    return nullptr;
  }
};

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
  unsigned int* index = new unsigned int(0);
  Parser parser(tokens, {"end"}, index);
  while (!parser.isEmpty())
  {
    Expression* expr = parser.next(nullptr);
    if (expr != nullptr)
    {
      expressions.push_back(expr);
    }
    parser.bump();
  }
  delete index;
}
