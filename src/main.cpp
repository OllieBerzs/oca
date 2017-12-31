#define OUT_TOKENS

#include "oca.hpp"
#include "utils.hpp"

int main(int argc, char** argv)
{
  if (argc < 2) std::cout << "No file provided\n";
  oca::script(file(argv[1]));
  std::cin.get();
}
