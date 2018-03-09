#include <iostream>
#include "oca.hpp"

oca::Ret print(oca::Arg arg, oca::ObjectPtr block, oca::ObjectPtr caller)
{
    std::cout << arg->tos(false) << "\n";
    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "No file provided\n";
        exit(1);
    }

    oca::State state;

    state.set("print", print);

    state.script(argv[1]);

    std::cin.get();

    return 0;
}
