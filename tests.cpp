/* ollieberzs 2018
** tests.cpp
** oca testing
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "oca.hpp"

TEST_CASE("Evaluation of basic types", "[types]")
{
    oca::State oca;

    // int
    REQUIRE(oca.eval("820")->tos(true) == "<int>820");
    REQUIRE(oca.eval("0b0110")->tos(true) == "<int>6");
    REQUIRE(oca.eval("0xFF")->tos(true) == "<int>255");

    // real
    REQUIRE(oca.eval("45.5")->tos(true) == "<real>45.5");
    REQUIRE(oca.eval("2e2")->tos(true) == "<real>200.0");

    // str
    REQUIRE(oca.eval("'This is a string!'")->tos(true) == "<str>This is a string!");

    // bool
    REQUIRE(oca.eval("true")->tos(true) == "<bool>true");

    // tuple
    REQUIRE(oca.eval("(2, 3, (true, false))")->tos(true)
        == "<tup>([0]<int>2, [1]<int>3, [2]<tup>([0]<bool>true, [1]<bool>false))");
}

TEST_CASE("Variable setting and getting", "[variables]")
{
    oca::State oca;

    // simple variable
    REQUIRE(oca.eval("a = 5")->tos(false) == "5");
    REQUIRE(oca.eval("a")->tos(false) == "5");
    oca.eval("a = 7");
    REQUIRE(oca.eval("a")->tos(false) == "7");

    // simple tuples
    oca.eval("b = (1, 2, 3, 4)");
    oca.eval("b[0] = 6");
    oca.eval("bi = 3");
    REQUIRE(oca.eval("b.2")->tos(false) == "3");
    REQUIRE(oca.eval("b[1]")->tos(false) == "2");
    REQUIRE(oca.eval("b[bi]")->tos(false) == "4");
    REQUIRE(oca.eval("b.0")->tos(false) == "6");

    // named tuples
    oca.eval("c = (x: 5, y: 6, z: 10)");
    oca.eval("ci = 'y'");
    REQUIRE(oca.eval("c.x")->tos(false) == "5");
    REQUIRE(oca.eval("c['z']")->tos(false) == "10");
    REQUIRE(oca.eval("c[ci]")->tos(false) == "6");

    // complex tuples
    oca.eval("complex = (x: (1, 2, 3), y: 'hi')");
    oca.eval("complex.x.1 = 6");
    REQUIRE(oca.eval("complex.x.1")->tos(false) == "6");

    // blocks
    oca.eval("ret = do with val return val");
    REQUIRE(oca.eval("ret 6")->tos(false) == "6");
}

TEST_CASE("Conditional evaluation", "[conditionals]")
{
    oca::State oca;

    // if
    REQUIRE(oca.eval("if true then 'yes'")->tos(false) == "yes");

    // if else
    REQUIRE(oca.eval("if false then 'yes' else 'no'")->tos(false) == "no");
}

TEST_CASE("Operators", "operators")
{
    oca::State oca;

    // int and int
    REQUIRE(oca.eval("2 + 3")->tos(false) == "5");
    REQUIRE(oca.eval("2 - 3")->tos(false) == "-1");
    REQUIRE(oca.eval("2 * 3")->tos(false) == "6");
    REQUIRE(oca.eval("6 / 3")->tos(false) == "2");
    REQUIRE(oca.eval("10 % 4")->tos(false) == "2");
    REQUIRE(oca.eval("2 ^ 3")->tos(false) == "8");
    REQUIRE(oca.eval("2 == 3")->tos(false) == "false");
    REQUIRE(oca.eval("2 != 3")->tos(false) == "true");
    REQUIRE(oca.eval("2 < 3")->tos(false) == "true");
    REQUIRE(oca.eval("2 > 3")->tos(false) == "false");
    REQUIRE(oca.eval("2 <= 3")->tos(false) == "true");
    REQUIRE(oca.eval("2 >= 3")->tos(false) == "false");
    REQUIRE(oca.eval("2 .. 4")->tos(false) == "(2, 3, 4)");
    REQUIRE(oca.eval("0b0101 and 0b0110")->tos(false) == "4");
    REQUIRE(oca.eval("0b0101 or 0b0110")->tos(false) == "7");
    REQUIRE(oca.eval("0b0101 xor 0b0110")->tos(false) == "3");
    REQUIRE(oca.eval("0b0101 lsh 1")->tos(false) == "10");
    REQUIRE(oca.eval("0b0100 rsh 1")->tos(false) == "2");

    // int and real
    REQUIRE(oca.eval("2 + 3.5")->tos(false) == "5.5");
    REQUIRE(oca.eval("2 - 3.5")->tos(false) == "-1.5");
    REQUIRE(oca.eval("2 * 3.5")->tos(false) == "7.0");
    REQUIRE(oca.eval("5 / 2.5")->tos(false) == "2.0");
    REQUIRE(oca.eval("2 ^ 3.0")->tos(false) == "8.0");
    REQUIRE(oca.eval("2 < 3.5")->tos(false) == "true");
    REQUIRE(oca.eval("2 > 3.5")->tos(false) == "false");

    // real and real
    REQUIRE(oca.eval("2.0 + 3.5")->tos(false) == "5.5");
    REQUIRE(oca.eval("2.0 - 3.5")->tos(false) == "-1.5");
    REQUIRE(oca.eval("2.0 * 3.5")->tos(false) == "7.0");
    REQUIRE(oca.eval("5.0 / 2.5")->tos(false) == "2.0");
    REQUIRE(oca.eval("2.0 ^ 3.0")->tos(false) == "8.0");
    REQUIRE(oca.eval("2.0 < 3.5")->tos(false) == "true");
    REQUIRE(oca.eval("2.0 > 3.5")->tos(false) == "false");

    // real and int
    REQUIRE(oca.eval("2.0 + 3")->tos(false) == "5.0");
    REQUIRE(oca.eval("2.0 - 3")->tos(false) == "-1.0");
    REQUIRE(oca.eval("2.0 * 3")->tos(false) == "6.0");
    REQUIRE(oca.eval("5.0 / 2")->tos(false) == "2.5");
    REQUIRE(oca.eval("2.0 ^ 3")->tos(false) == "8.0");
    REQUIRE(oca.eval("2.0 < 3")->tos(false) == "true");
    REQUIRE(oca.eval("2.0 > 3")->tos(false) == "false");

    // str and str
    REQUIRE(oca.eval("'hi' == 'hello'")->tos(false) == "false");
    REQUIRE(oca.eval("'hi' != 'hello'")->tos(false) == "true");

    // str and any
    REQUIRE(oca.eval("'hi' + 5")->tos(false) == "hi5");

    // bool and bool
    REQUIRE(oca.eval("true == false")->tos(false) == "false");
    REQUIRE(oca.eval("true != false")->tos(false) == "true");
    REQUIRE(oca.eval("true and false")->tos(false) == "false");
    REQUIRE(oca.eval("true or false")->tos(false) == "true");
}
