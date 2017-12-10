#pragma once

#include <string>

typedef unsigned int uint;
using std::string;

struct Error {
  static string script;
  static string message;
  static uint   line;
  static uint   column;

  static string getLine();
  static void   panic();
};
