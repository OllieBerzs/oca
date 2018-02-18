//#define OUT_TOKENS
#define OUT_AST
//#define OUT_VALUES

#include <iostream>
#include "oca.hpp"

oca::Ret print(oca::Args args)
{
    std::cout << args[0]->tos() << "\n";
    return nullptr;
}

oca::Ret pause(oca::Args args)
{
    std::cin.get();
    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc < 2) std::cout << "No file provided\n";

    oca::Scope scope(nullptr);

    //define methods in scope
    oca::def(scope, "print", print);
    oca::def(scope, "pause", pause);

    oca::scriptFile(scope, argv[1]);

    std::cin.get();

    return 0;
}
