/* ollieberzs 2018
** tests.cpp
** oca testing
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "oca.hpp"

TEST_CASE("Evaluation of basic types", "[types]")
{
    oca::State state;

    // int
    REQUIRE(state.eval("820")->tos(true) == "<int>820");

    // real
    REQUIRE(state.eval("45.5")->tos(true) == "<real>45.5");

    // str
    REQUIRE(state.eval("'This is a string!'")->tos(true) == "<str>This is a string!");

    // bool
    REQUIRE(state.eval("true")->tos(true) == "<bool>true");

    // tuple
    REQUIRE(state.eval("(2, 3, (true, false))")->tos(true)
        == "<tup>([0]<int>2, [1]<int>3, [2]<tup>([0]<bool>true, [1]<bool>false))");
}

TEST_CASE("Variable setting and getting", "[variables]")
{
    oca::State state;

    // simple variable
    REQUIRE(state.eval("a = 5")->tos(false) == "5");
    REQUIRE(state.eval("a")->tos(false) == "5");
    state.eval("a = 7");
    REQUIRE(state.eval("a")->tos(false) == "7");

    // simple tuples
    state.eval("b = (1, 2, 3, 4)");
    state.eval("b[0] = 6");
    state.eval("bi = 3");
    REQUIRE(state.eval("b.2")->tos(false) == "3");
    REQUIRE(state.eval("b[1]")->tos(false) == "2");
    REQUIRE(state.eval("b[bi]")->tos(false) == "4");
    REQUIRE(state.eval("b.0")->tos(false) == "6");

    // named tuples
    state.eval("c = (x: 5, y: 6, z: 10)");
    state.eval("ci = 'y'");
    REQUIRE(state.eval("c.x")->tos(false) == "5");
    REQUIRE(state.eval("c['z']")->tos(false) == "10");
    REQUIRE(state.eval("c[ci]")->tos(false) == "6");

    // complex tuples
    state.eval("complex = (x: (1, 2, 3), y: 'hi')");
    state.eval("complex.x.1 = 6");
    REQUIRE(state.eval("complex.x.1")->tos(false) == "6");

    // blocks
    state.eval("ret = do with val return val");
    REQUIRE(state.eval("ret 6")->tos(false) == "6");
}

TEST_CASE("Operators", "operators")
{
    oca::State state;

    // int and int
    REQUIRE(state.eval("2 + 3")->tos(false) == "5");
    REQUIRE(state.eval("2 - 3")->tos(false) == "-1");
    REQUIRE(state.eval("2 * 3")->tos(false) == "6");
    REQUIRE(state.eval("6 / 3")->tos(false) == "2");
    REQUIRE(state.eval("10 % 4")->tos(false) == "2");
    REQUIRE(state.eval("2 ^ 3")->tos(false) == "8");
    REQUIRE(state.eval("2 == 3")->tos(false) == "false");
    REQUIRE(state.eval("2 < 3")->tos(false) == "true");
    REQUIRE(state.eval("2 > 3")->tos(false) == "false");
    REQUIRE(state.eval("2 <= 3")->tos(false) == "true");
    REQUIRE(state.eval("2 >= 3")->tos(false) == "false");
    REQUIRE(state.eval("2 .. 4")->tos(false) == "(2, 3, 4)");

    // int and real
    REQUIRE(state.eval("2 + 3.5")->tos(false) == "5.5");
    REQUIRE(state.eval("2 - 3.5")->tos(false) == "-1.5");
    REQUIRE(state.eval("2 * 3.5")->tos(false) == "7.0");
    REQUIRE(state.eval("5 / 2.5")->tos(false) == "2.0");
    REQUIRE(state.eval("2 ^ 3.0")->tos(false) == "8.0");
    REQUIRE(state.eval("2 < 3.5")->tos(false) == "true");
    REQUIRE(state.eval("2 > 3.5")->tos(false) == "false");

    // real and real
    REQUIRE(state.eval("2.0 + 3.5")->tos(false) == "5.5");
    REQUIRE(state.eval("2.0 - 3.5")->tos(false) == "-1.5");
    REQUIRE(state.eval("2.0 * 3.5")->tos(false) == "7.0");
    REQUIRE(state.eval("5.0 / 2.5")->tos(false) == "2.0");
    REQUIRE(state.eval("2.0 ^ 3.0")->tos(false) == "8.0");
    REQUIRE(state.eval("2.0 < 3.5")->tos(false) == "true");
    REQUIRE(state.eval("2.0 > 3.5")->tos(false) == "false");

    // real and int
    REQUIRE(state.eval("2.0 + 3")->tos(false) == "5.0");
    REQUIRE(state.eval("2.0 - 3")->tos(false) == "-1.0");
    REQUIRE(state.eval("2.0 * 3")->tos(false) == "6.0");
    REQUIRE(state.eval("5.0 / 2")->tos(false) == "2.5");
    REQUIRE(state.eval("2.0 ^ 3")->tos(false) == "8.0");
    REQUIRE(state.eval("2.0 < 3")->tos(false) == "true");
    REQUIRE(state.eval("2.0 > 3")->tos(false) == "false");

    // str and str
    REQUIRE(state.eval("'hi' == 'hello'")->tos(false) == "false");

    // str and any
    REQUIRE(state.eval("'hi' + 5")->tos(false) == "hi5");

    // bool and bool
    REQUIRE(state.eval("true == false")->tos(false) == "false");
}

TEST_CASE("Conditional evaluation", "[conditionals]")
{
    oca::State state;

    // if
    REQUIRE(state.eval("if true then 'yes'")->tos(false) == "yes");

    // if else
    REQUIRE(state.eval("if false then 'yes' else 'no'")->tos(false) == "no");
}

TEST_CASE("Evaluation of functions", "[functions]")
{
    oca::State state;

    // passthrough function
    state.bind("pass", "a", [](oca::Arg arg) -> oca::Ret { return arg.value; });
    REQUIRE(state.eval("pass 5")->tos(false) == "5");
}
