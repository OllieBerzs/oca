#pragma once

#include <string>
#include <ostream>

struct Token
{
  std::string type;
  std::string value;

  Token(const std::string& type, const std::string& value) : type(type), value(value) {}
};

inline std::ostream& operator<<(std::ostream& stream, const Token& token)
{
  return stream << "(\"" << token.type << "\": \"" << token.value << "\")";
}
