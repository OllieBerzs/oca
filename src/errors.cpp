#include "errors.hpp"

namespace oca::internal::errors
{
  std::string Script::data = "";

  void parseError(const Token& token, const std::string& message)
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
    for (unsigned int i = 0; i < (lineNum / 10) + 2; i++) indicator += ' ';
    for (unsigned int i = 0; i < token.column; i++) indicator += ' ';
    for (unsigned int i = 0; i < token.length; i++) indicator += '^';

    // print error
    std::cout << "----------------------error\n";
    std::cout << lineNum << "| " << line << "\n";
    std::cout << indicator << "\n";
    std::cout << "  " << message << " " << token << "\n";

    // exit
    std::cin.get();
    exit(0);
  }
}