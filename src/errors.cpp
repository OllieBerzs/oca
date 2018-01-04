#include "errors.hpp"
#include <cmath>

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  constexpr int COLORS[3] = { 0x07, 0x04, 0x03 };
#else
  constexpr int COLORS[3] = { 0, 31, 36 };
#endif

// Define colors
#define RESET 0
#define RED 1
#define CYAN 2

void colorize(int color) {
  #if defined(_WIN32) || defined(_WIN64)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLORS[color]);
  #else
    std::cout << "\003[" << COLORS[color] << "m";
  #endif
}

namespace oca::internal::errors
{
  std::string Script::file = "";
  std::string Script::data = "";

  void parseError(const Token& token, const std::string& type, const std::string& message)
  {
    // get error line
    std::string line = "";
    unsigned int lineNum = token.line;
    unsigned int count = 1;
    for (char c : Script::data)
    {
      if (c == '\n')
      {
        if (count == lineNum) break;
        count++;
        line = "";
      }
      else line += c;
    }

    // prepare error indicator
    std::string indicator = "";
    unsigned int digits = lineNum > 0 ? (int)log10((double)lineNum) + 1 : 1;
    for (unsigned int i = 0; i < digits + 2; i++) indicator += ' ';
    for (unsigned int i = 0; i < token.column - 1; i++) indicator += ' ';
    for (unsigned int i = 0; i < token.length; i++) indicator += '^';

    // print error
    colorize(CYAN);
    std::cout << "-- " << type << " ---------------------- " << Script::file << "\n\n";
    colorize(RESET);
    std::cout << lineNum << "| " << line << "\n";
    colorize(RED);
    std::cout << indicator << "\n";
    colorize(RESET);
    std::cout << message << "\n";
    std::cout << token << "\n";

    // exit
    std::cin.get();
    exit(0);
  }
}