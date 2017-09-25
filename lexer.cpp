#include "lexer.hpp"
#include "errors.hpp"

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

bool isOf(char c, std::string str)
{
  for (char ch : str)
  {
    if (c == ch) return true;
  }
  return false;
}

std::string scanString(std::string script, unsigned int& index)
{
  std::string ret;

  // Put index after the single quote
  index++;

  bool end = false;
  while (!end)
  {
    char c = script[index];
    if (c == '\'') end = true;
    else
    {
      ret += c;
      index++;
    }
  }

  return ret;
}

std::string scanNumber(std::string script, unsigned int& index)
{
  std::string ret;

  bool end = false;
  while (!end)
  {
    char c = script[index];
    if (isOf(c, "." NUMBERS))
    {
      ret += c;
      index++;
    }
    else
    {
      end = true;
      index--;
    }
  }

  return ret;
}

std::string scanSymbol(std::string script, unsigned int& index)
{
  std::string ret;

  // First character cannot be a number
  ret += script[index];
  index++;

  bool end = false;
  while (!end)
  {
    char c = script[index];
    if (isOf(c, "_" LETTERS NUMBERS))
    {
      ret += c;
      index++;
    }
    else
    {
      end = true;
      index--;
    }
  }

  return ret;
}

void lex(const std::string& script, std::vector<Token>& tokens)
{
  unsigned int index = 0;
  while (index < script.size())
  {
    char c = script[index];
    if (isOf(c, " \n"));
    else if (isOf(c, ";")) tokens.emplace_back("end", "");
    else if (isOf(c, "+-*/")) tokens.emplace_back("operator", std::string(1, c));
    else if (isOf(c, "(){},=:")) tokens.emplace_back(std::string(1, c), "");
    else if (isOf(c, "'")) tokens.emplace_back("string", scanString(script, index));
    else if (isOf(c, "." NUMBERS)) tokens.emplace_back("number", scanNumber(script, index));
    else if (isOf(c, "_" LETTERS)) tokens.emplace_back("symbol", scanSymbol(script, index));
    else ERR << "Lexer: Unknown character at index " << index;
    index++;
  }
}
