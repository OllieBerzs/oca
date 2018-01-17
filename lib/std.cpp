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
            if (args.size() < 2) 
            {
                std::cout << "method '+' requires 2 arguments";
                exit(0);
            }
            if (args[0]->type == "integer" && args[1]->type == "integer")
            {
                int result = integer(args[0]) + integer(args[1]);
                internal::Expression* e = new internal::Expression("integer", std::to_string(result));
                return new internal::Value("integer", e);
            }
            return NIL;
        });
    }
}
