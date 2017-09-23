#pragma once

#include <string>
#include <ostream>

struct Token
{
  std::string type;
  std::string data;

  Token(const std::string& type, const std::string& data) : type(type), data(data) {}
};

inline std::ostream& operator<<(std::ostream& stream, const Token& token)
{
  return stream << "(\"" << token.type << "\": \"" << token.data << "\")";
}
