#include <iostream>
#include "oca.hpp"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "No file provided\n";
        exit(1);
    }

    oca::State state;
    state.script(argv[1]);

    return 0;
}
