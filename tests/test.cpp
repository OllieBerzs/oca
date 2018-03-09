/* ollieberzs 2018
** test.cpp
** oca testing
*/

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../oca.hpp"

TEST_CASE("Evaluation of basic types", "[types]")
{
    oca::State state;
    oca::ValuePtr obj = nullptr;

    // integer
    REQUIRE((obj = state.eval("820")) != nullptr);
    REQUIRE(obj->toStr(true) == "<int>820");

    // real
    REQUIRE((obj = state.eval("45.5")) != nullptr);
    REQUIRE(obj->toStr(true) == "<real>45.5");

    // string
    REQUIRE((obj = state.eval("'This is a string!'")) != nullptr);
    REQUIRE(obj->toStr(true) == "<str>This is a string!");

    // bool
    REQUIRE((obj = state.eval("true")) != nullptr);
    REQUIRE(obj->toStr(true) == "<bool>true");

    // tuple
    REQUIRE((obj = state.eval("(2, 3, (true, false))")) != nullptr);
    REQUIRE(obj->toStr(true) == "<tup>([1]<int>2, [2]<int>3, [3]<tup>([1]<bool>true, [2]<bool>false))");
}

TEST_CASE("Evaluation of functions", "[functions]")
{
    oca::State state;

    state.set("passback", [](oca::Arg arg, oca::ValuePtr block, oca::ValuePtr caller) -> oca::Ret
    {
        return arg;
    });

    oca::ValuePtr obj = nullptr;

    REQUIRE((obj = state.eval("passback 5")) != nullptr);
    REQUIRE(obj->toStr(true) == "<int>5");
}
