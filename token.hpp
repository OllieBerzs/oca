#pragma once

#include <string>
#include <ostream>

enum
{
  T_NEWLINE,
  T_LPAREN,
  T_RPAREN,
  T_DOT,
  T_COMMA,
  T_STRING,
  T_NUMBER,
  T_NAME
};
const std::string T_TYPES[8]
{
  "newline",
  "lparen",
  "rparen",
  "dot",
  "comma",
  "string",
  "number",
  "name"
};

struct Token
{
  int type;
  std::string value;

  Token(int type, const std::string& value) : type(type), value(value) {}
  Token(int type, char c) : type(type), value(1, c) {}
};

inline std::ostream& operator<<(std::ostream& stream, const Token& token)
{
    return stream << "(\"" << T_TYPES[token.type] << "\": \"" << token.value << "\")";
}
