#include <iostream>
#include "oca.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) std::cout << "No file provided\n";

    oca::State state;

    state.script(argv[1]);

    std::cin.get();

    return 0;
}
