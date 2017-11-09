#include "oca.hpp"

int main(int argc, char** argv)
{
    if (argc < 2) ERR << "No script file provided";

    //Scope scope(nullptr);
    //nativeMethods(scope);

    oca::script(file(argv[1]), false, true);
}
