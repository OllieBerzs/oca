#pragma once

#include <ostream>
#include <string>

namespace oca::internal
{

  struct Token
  {
    std::string type;
    std::string value;

    unsigned int line;
    unsigned int column;
    unsigned int length;

    Token(const std::string& type, const std::string& value, 
      unsigned int line, unsigned int column, unsigned int length)
      : type(type), value(value), line(line), column(column), length(length) {}
  };

  inline std::ostream& operator<<(std::ostream& stream, const Token& token)
  {
    return stream << "(\"" << token.type << "\": \"" << token.value << "\")"
      << "[" << token.line << " " << token.column << "]";
  }

} // namespace oca::internal
