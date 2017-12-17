#pragma once

#include <string>

typedef unsigned int uint;

struct Error {
  static std::string script;
  static std::string message;
  static uint        line;
  static uint        column;

  static std::string getLine();
  static void        panic();
};
