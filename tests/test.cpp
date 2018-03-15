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

    // integer
    REQUIRE(state.eval("820")->toStr(true) == "<int>820");

    // real
    REQUIRE(state.eval("45.5")->toStr(true) == "<real>45.5");

    // string
    REQUIRE(state.eval("'This is a string!'")->toStr(true) == "<str>This is a string!");

    // bool
    REQUIRE(state.eval("true")->toStr(true) == "<bool>true");

    // tuple
    REQUIRE(state.eval("(2, 3, (true, false))")->toStr(true) == "<tup>([0]<int>2, [1]<int>3, [2]<tup>([0]<bool>true, [1]<bool>false))");
}

TEST_CASE("Variable setting and getting", "[variables]")
{
    oca::State state;

    // simple variable
    REQUIRE(state.eval("a = 5")->toStr(false) == "5");
    REQUIRE(state.eval("a")->toStr(false) == "5");
    state.eval("a = 7");
    REQUIRE(state.eval("a")->toStr(false) == "7");

    // simple tuples
    state.eval("b = (1, 2, 3, 4)");
    state.eval("bi = 3");
    REQUIRE(state.eval("b.2")->toStr(false) == "3");
    REQUIRE(state.eval("b[1]")->toStr(false) == "2");
    REQUIRE(state.eval("b[bi]")->toStr(false) == "4");
    state.eval("b[0] = 6");
    REQUIRE(state.eval("b.0")->toStr(false) == "6");

    // named tuples
    state.eval("c = (x: 5, y: 6, z: 10)");
    state.eval("ci = 'y'");
    REQUIRE(state.eval("c.x")->toStr(false) == "5");
    REQUIRE(state.eval("c['z']")->toStr(false) == "10");
    REQUIRE(state.eval("c[ci]")->toStr(false) == "6");

    // complex tuples
    state.eval("complex = (x: (1, 2, 3), y: 'hi')");
    state.eval("complex.x.1 = 6");
    REQUIRE(state.eval("complex.x.1")->toStr(false) == "6");

    // blocks
    state.eval("ret = do $val return val");
    REQUIRE(state.eval("ret 6")->toStr(false) == "6");
}

TEST_CASE("Conditional evaluation", "[conditionals]")
{
    oca::State state;

    // if
    REQUIRE(state.eval("if true then 'yes'")->toStr(false) == "yes");

    // if else
    REQUIRE(state.eval("if false then 'yes' else 'no'")->toStr(false) == "no");

    // if else with variable
    state.eval("a = true");
    REQUIRE(state.eval("if a then 'yes' else 'no'")->toStr(false) == "yes");
}

TEST_CASE("Evaluation of functions", "[functions]")
{
    oca::State state;

    // passthrough function
    state.set("pass", [](oca::Arg arg, oca::ValuePtr block, oca::ValuePtr caller) -> oca::Ret
    {
        return arg;
    });
    REQUIRE(state.eval("pass 5")->toStr(false) == "5");
}
