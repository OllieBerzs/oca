#include "oca.hpp"

int main(int argc, char** argv) {
    oca::State oca;

    if (argc > 1)
        oca.runFile(argv[1]);
    else
        oca.runREPL();

    return 0;
}
