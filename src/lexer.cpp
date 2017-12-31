#include <iostream>
#include "lexer.hpp"

namespace oca::internal
{

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

unsigned int lexLine;
unsigned int lexColumn;

void lex(const std::string& script, std::vector<Token>& tokens)
{
  unsigned int index = 0;

  lexLine = 1;
  lexColumn = 1;

  while (index < script.size())
  {
    if (whitespace(script, index)) continue;
    else if (punctuation(script, index, tokens)) continue;
    else if (number(script, index, tokens)) continue;
    else if (string(script, index, tokens)) continue;
    else
    {
      std::cout << "Unknown symbol \"" << script[index] << "\" ["
        << lexLine << " " << lexColumn << "]\n";
      exit(0);
    }
  }
}

bool isIn(char c, const std::string& str)
{
  for (char ch : str) if (c == ch) return true;
  return false;
}

bool whitespace(const std::string &script, unsigned int &index)
{
  if (script[index] == ' ')
  {
    index++;
    lexColumn++;
    return true;
  }
  else if (isIn(script[index], "\n\r"))
  {
    index++;
    lexLine++;
    lexColumn = 1;
    return true;
  }
  return false;
}

bool punctuation(const std::string &script, unsigned int &index, std::vector<Token> &tokens)
{
  char c = script[index];
  if (c == '.')
  {
    tokens.emplace_back("dot", "", lexLine, lexColumn, 1);
    index++;
    lexColumn++;
    return true;
  }
  if (c == ',')
  {
    tokens.emplace_back("comma", "", lexLine, lexColumn, 1);
    index++;
    lexColumn++;
    return true;
  }
  if (c == '|')
  {
    tokens.emplace_back("pipe", "", lexLine, lexColumn, 1);
    index++;
    lexColumn++;
    return true;
  }
  if (c == '(')
  {
    tokens.emplace_back("lparen", "", lexLine, lexColumn, 1);
    index++;
    lexColumn++;
    return true;
  }
  if (c == ')')
  {
    tokens.emplace_back("rparen", "", lexLine, lexColumn, 1);
    index++;
    lexColumn++;
    return true;
  }
  return false;
}

bool number(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
{
  char c = script[index];
  std::string num = "";
  bool isInt = true;

  // check if is number
  if (!isIn(c, NUMBERS)) return false;

  while (isIn(c, NUMBERS"."))
  {
    if (c == '.')
    {
      if (isInt) isInt = false;
      else break;
    }
    num += c;
    lexColumn++;
    c = script[++index];
  }

  if (isInt) tokens.emplace_back("integer", num, lexLine, lexColumn, 0);
  else tokens.emplace_back("float", num, lexLine, lexColumn, 0);

  return true;
}

bool string(const std::string &script, unsigned int &index, std::vector<Token> &tokens)
{
  char c = script[index];
  std::string str = "";

  if (c != '\'') return false;
  index++;
  lexColumn++;
  c = script[index];
  while (c != '\'')
  {
    str += c;
    index++;
    lexColumn++;
    if (index >= script.size())
    {
      std::cout << "Missing end quote [" << lexLine << " " << lexColumn << "]\n";
    }
    c = script[index];
  }
  tokens.emplace_back("string", str, lexLine, lexColumn, str.size());
  return true;
}


} // namespace oca::internal
