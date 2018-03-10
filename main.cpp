#include <iostream>
#include "oca.hpp"

oca::Ret print(oca::Arg arg, oca::ValuePtr block, oca::ValuePtr caller)
{
    std::cout << arg->toStr(false) << "\n";
    return nullptr;
}

oca::Ret pause(oca::Arg arg, oca::ValuePtr block, oca::ValuePtr caller)
{
    std::cin.get();
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
    state.set("pause", pause);
    state.script(argv[1]);

    return 0;
}
