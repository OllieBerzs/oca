#include "../api.hpp"

using namespace oca;
extern "C"
{
    DLLEXPORT load(State& state)
    {
        //print
        def(state, "print", [](auto args) -> auto
        {
            std::cout << args[0]->expr->value << "\n";
            return NIL;
        });

        //arithmetics
        def(state, "+", [](auto args) -> auto
        {
            if (checkArgs("ii", args))
            {
                int ret = toi(args[0]) + toi(args[1]);
                return tov(ret);
            }
            return NIL;
        });
    }
}
