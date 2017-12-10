#pragma once

#include "colors.hpp"
#include <ostream>
#include <string>

namespace oca::internal {

enum {
  T_NEWLINE,
  T_LPAREN,
  T_RPAREN,
  T_DOT,
  T_COMMA,
  T_STRING,
  T_NUMBER,
  T_BOOL,
  T_NAME,
  T_DO,
  T_END
};
const std::string T_TYPES[11]{"newline", "lparen", "rparen", "dot",
                              "comma",   "string", "number", "boolean",
                              "name",    "do",     "end"};

struct Token {
  int         type;
  std::string value;

  unsigned int line;
  unsigned int column;
  unsigned int length;

  Token(int type, const std::string& value, unsigned int line,
        unsigned int column, unsigned int length)
    : type(type), value(value), line(line), column(column), length(length) {
  }
};

inline std::ostream& operator<<(std::ostream& stream, const Token& token) {
  return stream << RESET << "(\"" << CYAN << T_TYPES[token.type] << RESET
                << "\": \"" << GREEN << token.value << RESET << "\")";
}

} // namespace oca::internal
