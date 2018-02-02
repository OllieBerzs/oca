//#define OUT_TOKENS
//#define OUT_AST
//#define OUT_VALUES

#include <iostream>
#include "oca.hpp"

oca::Ret print(oca::Args args)
{
    std::cout << args[0]->tos() << "\n";
    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc < 2) std::cout << "No file provided\n";

    oca::Scope scope(nullptr);

    //print
    def(scope, "print", print);

    oca::scriptFile(scope, argv[1]);

    std::cin.get();
    return 0;
}
