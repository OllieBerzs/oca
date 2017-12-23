/*#include "colorput.hpp"
#include "errors.hpp"

std::string Error::getLine() {
  std::string  ret = "";
  unsigned int ln = 1;

  for (char c : script) {
    if (c == '\n') {
      if (ln == line) break;
      line++;
      ret = "";
    } else
      ret += c;
  }
  return ret;
}

void Error::panic() {
  colorize(RED);
  std::cout << line << " |  " << getLine() << "\n-- " << message << "\n";
  colorize(RESET);

  exit(0);
}*/
