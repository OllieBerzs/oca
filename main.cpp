#define OUT_TOKENS

#include "oca.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    // TODO: Launch real-time interpreter
    exit(0);
  }

  // Scope scope(nullptr);
  // nativeMethods(scope);

  oca::script(file(argv[1]));
}
