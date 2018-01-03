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
    else if (boolean(script, index, tokens)) continue;
    else if (keyword(script, index, tokens)) continue;
    else if (name(script, index, tokens)) continue;
    else
    {
      std::cout << "Unknown symbol \"" << script[index] << "\" ["
        << lexLine << " " << lexColumn << "]\n";
      std::cin.get();
      exit(0);
    }
  }
}

bool isIn(char c, const std::string& str)
{
  for (char ch : str) if (c == ch) return true;
  return false;
}
bool match(const std::string& to, const std::string& script, unsigned int& index)
{
  unsigned int orig = index;
  unsigned int col = lexColumn;
  for (unsigned int i = 0; i < to.size(); i++)
  {
    if (to[i] != script[index])
    {
      index = orig;
      lexColumn = col;
      return false;
    }
    index++;
    lexColumn++;
  }
  return true;
}

bool whitespace(const std::string& script, unsigned int& index)
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

bool punctuation(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
{
  char c = script[index];
  if (isIn(c, ".,|()"))
  {
    std::string chr = "";
    chr += c;
    tokens.emplace_back(chr, "", lexLine, lexColumn, 1);
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
      if (isInt && index + 1 < script.length() && isIn(script[index + 1], NUMBERS)) isInt = false;
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
bool string(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
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
      std::cin.get();
      exit(0);
    }
    c = script[index];
  }
  index++;
  lexColumn++;
  tokens.emplace_back("string", str, lexLine, lexColumn, (unsigned int)str.size());
  return true;
}
bool boolean(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
{
  if (match("true", script, index))
  {
    tokens.emplace_back("boolean", "true", lexLine, lexColumn, 4);
    return true;
  }
  else if (match("false", script, index))
  { 
    tokens.emplace_back("boolean", "false", lexLine, lexColumn, 5);
    return true;
  }
  return false;
}
bool keyword(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
{
  if (match("def", script, index))
  {
    tokens.emplace_back("def", "", lexLine, lexColumn, 3);
    return true;
  }
  if (match("ext", script, index))
  {
    tokens.emplace_back("ext", "", lexLine, lexColumn, 3);
    return true;
  }
  if (match("return", script, index))
  {
    tokens.emplace_back("return", "", lexLine, lexColumn, 6);
    return true;
  }
  if (match("do", script, index))
  {
    tokens.emplace_back("do", "", lexLine, lexColumn, 2);
    return true;
  }
  if (match("end", script, index))
  {
    tokens.emplace_back("end", "", lexLine, lexColumn, 3);
    return true;
  }
  if (match("else", script, index))
  {
    tokens.emplace_back("else", "", lexLine, lexColumn, 4);
    return true;
  }
  return false;
}
bool name(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
{
  char c = script[index];
  if (isIn(c, "+-/*%^="))
  {
    std::string chr = "";
    chr += c;
    tokens.emplace_back("name", chr, lexLine, lexColumn, 1);
    index++;
    lexColumn++;
    return true;
  }
  std::string nam = "";
  while (isIn(c, LETTERS "_"))
  {
    nam += c;
    c = script[++index];
    lexColumn++;
  }
  if (nam.length() > 0)
  {
    tokens.emplace_back("name", nam, lexLine, lexColumn, (unsigned int)nam.length());
    return true;
  }
  return false;
}


} // namespace oca::internal
