#include "parser.hpp"
#include "errors.hpp"

namespace oca::internal 
{
unsigned int errorToken = 0;

bool isOper(const std::string& name) 
{
  return name == "+" || name == "-" || name == "*" || name == "/" || name == "%" || name == "^";
}
bool unmatch(unsigned int& i, unsigned int orig) 
{
  errorToken = i;
  i = orig;
  return false;
}

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
  unsigned int i = 0;
  while (i < tokens.size()) 
  {
    Expression* e = nullptr;
    if (expr(e, i, tokens)) expressions.push_back(e);
    else errors::parseError(tokens[i], "INVALID TOKEN", "Invalid syntax token");
  }
}

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type == "(")
  {
    Expression* intern = nullptr;
    i++;
    if (!expr(intern, i, tokens)) return unmatch(i, orig);
    if (i < tokens.size() && tokens[i].type != ")") return unmatch(i, orig);
    i++;
    out = new Expression("outer", "");
    out->left = intern;
  } 
  else if (!call(out, i, tokens) && !string(out, i, tokens) && !integer(out, i, tokens)
    && !floating(out, i, tokens) && !boolean(out, i, tokens)) return unmatch(i, orig);

  // Check for attachables
  Expression* attach = nullptr;
  while (attachment(attach, i, tokens))
  {
    Expression* temp = new Expression("attach", "");
    temp->left = out;
    temp->right = attach;
    out = temp;
  }

  return true;
}
bool call(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type != "name") return unmatch(i, orig);
  std::string name = tokens[i].value;
  i++;

  // check for assignment
  if (i < tokens.size() && tokens[i].value == "=")
  {
    name += "=";
    i++;
  }

  bool hasParen = true;
  if (i < tokens.size() && tokens[i].type != "(")
  {
    i--;
    hasParen = false;
  }
  i++;

  Expression* arg = nullptr;
  if (!args(arg, i, tokens)) return unmatch(i, orig);

  if (i < tokens.size() && tokens[i].type != ")" && hasParen) return unmatch(i, orig);
  else if (!hasParen) i--;
  i++;

  out = new Expression("call", name);
  out->right = arg;
  return true;
}
bool args(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  Expression* arg = nullptr;
  if (i < tokens.size() && !isOper(tokens[i].value) && expr(arg, i, tokens))
  {
    // check for more arguments if followed by comma
    Expression* anotherArg = nullptr;
    if (i < tokens.size() && tokens[i].type == ",")
    {
      i++;
      if (!args(anotherArg, i, tokens)) return unmatch(i, orig);
    }
    arg->right = anotherArg;
  }

  out = arg;
  return true;
}
bool attachment(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type == ".")
  {
    i++;
    if (call(out, i, tokens)) return true;
  }
  // Check for arithmetic methods
  if (i < tokens.size() && tokens[i].type == "name" 
    && isOper(tokens[i].value) && call(out, i, tokens)) return true;

  return unmatch(i, orig);
}
bool block(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  return false;
}

bool string(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type != "string") return unmatch(i, orig);
  std::string value = tokens[i].value;
  i++;

  out = new Expression("string", value);
  return true;
}
bool integer(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type != "integer") return unmatch(i, orig);
  std::string value = tokens[i].value;
  i++;
  
  out = new Expression("integer", value);
  return true;
}
bool floating(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type != "float") return unmatch(i, orig);
  std::string value = tokens[i].value;
  i++;

  out = new Expression("float", value);
  return true;
}
bool boolean(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (i < tokens.size() && tokens[i].type != "boolean") return unmatch(i, orig);
  std::string value = tokens[i].value;
  i++;

  out = new Expression("boolean", value);
  return true;
}

} // namespace oca::internal
