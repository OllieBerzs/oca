//#define OUT_TOKENS
//define OUT_TREE
//#define OUT_VALUES

#include <iostream>
#include "oca.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) std::cout << "No file provided\n";

    oca::State state;
    oca::loadLib(state, "std");
    oca::scriptFile(state, argv[1]);

    std::cin.get();
    return 0;
}
