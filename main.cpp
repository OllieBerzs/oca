//#define OUT_TOKENS
//#define OUT_AST
//#define OUT_VALUES

#include <iostream>
#include "oca.hpp"

oca::Ret print(oca::Args args)
{
    std::cout << args[0]->val->val << "\n";
    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc < 2) std::cout << "No file provided\n";

    // each type should have it's own scope with methods and variables
    // state is a collection of named scopes for each type which is copied over to types unique scope at instantiation
    oca::Scope s(nullptr);

    //print
    def(s, "print", print);

    //oca::loadLib(state, "std");
    oca::scriptFile(s, argv[1]);

    std::cin.get();
    return 0;
}
