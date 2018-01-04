#include "parser.hpp"
#include "errors.hpp"

namespace oca::internal 
{
unsigned int errorToken = 0;

const Token& tokenAt(unsigned int i, const std::vector<Token>& tokens)
{
  if (i < tokens.size() - 1) return tokens[i];
  else return tokens[tokens.size() - 1];
}
bool back(unsigned int& i, unsigned int orig) 
{
  errorToken = i;
  i = orig;
  return false;
}
void next(unsigned int& i)
{
  errorToken++;
  i++;
}

bool isOper(const std::string& name)
{
  return name == "+" || name == "-" || name == "*" || name == "/" || name == "%" || name == "^";
}

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
  unsigned int i = 0;
  while (i < tokens.size() - 1) 
  {
    Expression* e = nullptr;
    if (expr(e, i, tokens)) expressions.push_back(e);
    else errors::parseError(tokenAt(i, tokens), "INVALID TOKEN", "Invalid syntax token");
  }
}

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (tokenAt(i, tokens).type == "(")
  {
    Expression* intern = nullptr;
    next(i);
    if (!expr(intern, i, tokens)) return back(i, orig);
    if (tokenAt(i, tokens).type != ")")
    {
      errors::parseError(tokens[i], "MISSING TOKEN", "Missing ')' before token");
    }
    next(i);
    out = new Expression("outer", "");
    out->left = intern;
  } 
  else if (!block(out, i, tokens) && !call(out, i, tokens) && !string(out, i, tokens) 
    && !integer(out, i, tokens) && !floating(out, i, tokens) && !boolean(out, i, tokens))
  {
    return back(i, orig);
  }

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

  if (tokenAt(i, tokens).type != "name") return back(i, orig);
  std::string name = tokens[i].value;
  unsigned int line = tokens[i].line;
  next(i);

  // check for assignment
  if (tokenAt(i, tokens).value == "=")
  {
    name += "=";
    next(i);
  }

  bool hasParen = false;
  if (tokenAt(i, tokens).type == "(")
  {
    next(i);
    hasParen = true;
  }

  Expression* arg = nullptr;
  if (!args(arg, i, tokens, line)) return back(i, orig);

  if (tokenAt(i, tokens).type != ")" && hasParen)
  {
    errors::parseError(tokens[i], "MISSING TOKEN", "Missing ')' before token");
  }
  else if (hasParen) next(i);

  out = new Expression("call", name);
  out->right = arg;

  // check for block
  Expression* blck = nullptr;
  if (block(blck, i, tokens))
  {
    Expression* temp = new Expression("block", "attach");
    temp->left = out;
    temp->right = blck;
    out = temp;
  }

  return true;
}
bool attachment(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  if (tokenAt(i, tokens).type == ".")
  {
    next(i);
    if (call(out, i, tokens)) return true;
  }
  // Check for arithmetic methods
  if (tokenAt(i, tokens).type == "name" && isOper(tokens[i].value) && call(out, i, tokens))
  {
    return true;
  }

  return back(i, orig);
}
bool block(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  // check if is block
  if (tokenAt(i, tokens).type != "do") return back(i, orig);
  next(i);

  out = new Expression("block", "head");
  Expression* mainBlock = new Expression("block", "body");
  Expression* elseBlock = nullptr;
 
  // get parameters
  Expression* par = nullptr;
  if (tokenAt(i, tokens).type == "|")
  {
    next(i);
    if (!params(par, i, tokens, tokenAt(i, tokens).line))
    {
      errors::parseError(tokenAt(i, tokens), "EMPTY PIPES", "No parameters for block with '||'");
    }
    if (tokenAt(i, tokens).type != "|")
    {
      errors::parseError(tokenAt(i - 1, tokens), "MISSING TOKEN", "Expected '|' after token");
    }
    next(i);
  }

  // get body
  Expression* exprs = nullptr;
  if (!expr(exprs, i, tokens))
  {
    errors::parseError(tokens[i], "MISSING EXPRESSION", "No expression provided to block");
  }
  while (tokenAt(i, tokens).type != "end" && tokenAt(i, tokens).type != "else")
  {
    Expression* anotherExpr = nullptr;
    if (!expr(anotherExpr, i, tokens))
    {
      errors::parseError(tokens[i], "NOT AN EXPRESSION", "Expected an expression");
    }
    exprs->right = anotherExpr;
  }

  // get else body
  if (tokenAt(i, tokens).type == "else")
  {
    next(i);
    if (!expr(elseBlock, i, tokens))
    {
      errors::parseError(tokens[i], "MISSING EXPRESSION", "No expression provided to else block");
    }
    while (tokenAt(i, tokens).type != "end")
    {
      Expression* anotherExpr = nullptr;
      if (!expr(anotherExpr, i, tokens))
      {
        errors::parseError(tokens[i], "NOT AN EXPRESSION", "Expected an expression");
      }
      elseBlock->right = anotherExpr;
    }
  }
  next(i);

  // build block
  mainBlock->left = par;
  mainBlock->right = exprs;
  out->left = mainBlock;
  out->right = elseBlock;
  return true;
}

bool args(Expression*& out, unsigned int& i, const std::vector<Token>& tokens, unsigned int line)
{
  unsigned int orig = i;

  Expression* arg = new Expression("arg", "");
  if (tokenAt(i, tokens).line == line && !isOper(tokens[i].value) 
    && tokens[i].type != "do" && expr(arg->left, i, tokens))
  {
    // check for more arguments if followed by comma
    Expression* anotherArg = nullptr;
    if (tokenAt(i, tokens).type == ",")
    {
      next(i);
      if (!args(anotherArg, i, tokens, line))
      {
        errors::parseError(tokens[i], "MISSING TOKEN", "Expected an argument after ','");
      }
    }
    arg->right = anotherArg;
  }

  out = arg;
  return true;
}
bool params(Expression*& out, unsigned int& i, const std::vector<Token>& tokens, unsigned int line)
{
  unsigned int orig = i;

  Expression* param = nullptr;
  if (tokenAt(i, tokens).line == line && !isOper(tokens[i].value) && tokens[i].type == "name")
  {
    param = new Expression("param", tokens[i].value);
    next(i);
    // check for more parameters if followed by comma
    Expression* anotherParam = nullptr;
    if (tokenAt(i, tokens).type == ",")
    {
      next(i);
      if (!params(anotherParam, i, tokens, line))
      {
        errors::parseError(tokens[i], "MISSING TOKEN", "Expected a parameter after ','");
      }
    }
    param->right = anotherParam;
  }

  if (!param) return back(i, orig);
  out = param;
  return true;
}

bool string(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  if (tokenAt(i, tokens).type != "string") return back(i, orig);
  std::string value = tokens[i].value;
  next(i);

  out = new Expression("string", value);
  return true;
}
bool integer(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (tokenAt(i, tokens).type != "integer") return back(i, orig);
  std::string value = tokens[i].value;
  next(i);
  
  out = new Expression("integer", value);
  return true;
}
bool floating(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
  unsigned int orig = i;

  if (tokenAt(i, tokens).type != "float") return back(i, orig);
  std::string value = tokens[i].value;
  next(i);

  out = new Expression("float", value);
  return true;
}
bool boolean(Expression*& out, unsigned int& i, const std::vector<Token>& tokens) 
{
  unsigned int orig = i;

  if (tokenAt(i, tokens).type != "boolean") return back(i, orig);
  std::string value = tokens[i].value;
  next(i);

  out = new Expression("boolean", value);
  return true;
}

} // namespace oca::internal
