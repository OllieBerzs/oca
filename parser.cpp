#include "parser.hpp"
#include "errors.hpp"

class Parser
{
  std::vector<Token> _tokens;
  std::vector<int> _stop;
  unsigned int* _index;

public:
  Parser(const std::vector<Token>& tokens, const std::vector<int>& stop, unsigned int* index)
    : _tokens(tokens), _stop(stop), _index(index) {}

  void bump()
  {
    (*_index)++;
  }

  bool isEmpty()
  {
    return (*_index) >= _tokens.size();
  }

  Expression* multi(int sep, int end)
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
            ret = new Expression(E_ARG, "", expr, nullptr);
            prev = ret;
          }
          else
          {
            prev->right = new Expression(E_ARG, "", expr, nullptr);
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
    if (_tokens[*_index].type == T_LBRACKET)
    {
      bump();
      return multi(T_COMMA, T_RBRACKET);
    }
    return nullptr;
  }

  Expression* next(Expression* prevExpr)
  {
    if (isEmpty()) ERR << "Parser: ran out of tokens";

    const Token& prevToken = _tokens[(*_index) - 1];
    const Token& token = _tokens[*_index];
    const Token& nextToken = _tokens[(*_index) + 1];

    for (int i : _stop) if (token.type == i) return prevExpr;
    bump();

    if (token.type == T_NUMBER)
    {
      return next(new Expression(E_NUMBER, token.value));
    }
    else if (token.type == T_STRING)
    {
      return next(new Expression(E_STRING, token.value));
    }
    else if (token.type == T_DOT)
    {
      return next(prevExpr);
    }
    else if (token.type == T_NAME)
    {
      std::string name = token.value;
      if (nextToken.value == "=")
      {
        name += nextToken.value; // Setter method
        bump();
      }

      Expression* args = nullptr;
      if (_tokens[*_index].type == T_LBRACKET)
      {
        bump();
        args = multi(T_COMMA, T_RBRACKET);
      }

      if (prevToken.type == T_DOT) return next(new Expression(E_CALL, name, prevExpr, args));
      else return next(new Expression(E_CALL, name, nullptr, args));
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
  Parser parser(tokens, {T_NEWLINE}, index);
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
