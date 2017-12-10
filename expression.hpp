#pragma once

#include "colors.hpp"
#include "memory.hpp"
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

using std::string;

namespace oca::internal {

enum { E_NUMBER, E_STRING, E_BOOL, E_CALL, E_METHOD, E_ARG, E_CASE, E_NULL };
const string E_TYPES[8]{"number", "string", "boolean", "call",
                        "method", "arg",    "case",    "null"};

struct Expression {
  int         type;
  string      value;
  Expression* attachment = nullptr;
  Expression* next = nullptr;
  Expression* argument = nullptr;
  Expression* content = nullptr;
  Expression* block = nullptr;

  Expression(int type, const string& value) : type(type), value(value) {
    Memory::add('e');
  }

  ~Expression() {
    delete attachment;
    delete next;
    delete argument;
    delete content;
    delete block;
    Memory::rem('e');
  }
};

inline void printTree(const Expression& e, std::ostream& stream,
                      const string& branch, int indent = 0) {
  string data = YELLOW + branch + RESET + "(\"" + CYAN + E_TYPES[e.type] + GREEN
                + " " + e.value + RESET + "\")\n";
  stream << std::setw(data.size() + indent) << data;

  if (e.content) printTree(*e.content, stream, "()", indent + 2);
  if (e.argument) printTree(*e.argument, stream, "args", indent + 2);
  if (e.next) printTree(*e.next, stream, "->", indent + 2);
  if (e.attachment) printTree(*e.attachment, stream, ".", indent + 2);
}

inline std::ostream& operator<<(std::ostream& stream, const Expression& expr) {
  printTree(expr, stream, "");
  return stream;
}

} // namespace oca::internal
