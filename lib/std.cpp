#include "../api.hpp"

using namespace oca;
extern "C"
{
    DLLEXPORT load(State& state)
    {
        // print
        def(state, "print", [](auto args) -> auto
        {
            std::cout << args[0]->expr->value << "\n";
            return NIL;
        });
    }
}
