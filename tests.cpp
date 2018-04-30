/* ollieberzs 2018
** tests.cpp
** oca testing
*/

#define CATCH_CONFIG_MAIN
#include "catch/catch-mini.hpp"

#include "oca.hpp"

TEST_CASE("Evaluation of basic types") {
    oca::State oca;

    // int
    REQUIRE(oca.runString("820")->tos(true) == "<int>820");
    REQUIRE(oca.runString("0b0110")->tos(true) == "<int>6");
    REQUIRE(oca.runString("0xFF")->tos(true) == "<int>255");

    // real
    REQUIRE(oca.runString("45.5")->tos(true) == "<real>45.5");
    REQUIRE(oca.runString("2e2")->tos(true) == "<real>200.0");

    // str
    REQUIRE(oca.runString("'This is a string!'")->tos(true) == "<str>This is a string!");

    // bool
    REQUIRE(oca.runString("true")->tos(true) == "<bool>true");

    // tuple
    REQUIRE(
        oca.runString("(2, 3, (true, false))")->tos(true) ==
        "<tup>([0]<int>2, [1]<int>3, [2]<tup>([0]<bool>true, [1]<bool>false))");
}

TEST_CASE("Variable setting and getting") {
    oca::State oca;

    // simple variable
    REQUIRE(oca.runString("a = 5")->tos(false) == "5");
    REQUIRE(oca.runString("a")->tos(false) == "5");
    oca.runString("a = 7");
    REQUIRE(oca.runString("a")->tos(false) == "7");

    // simple tuples
    oca.runString("b = (1, 2, 3, 4)");
    REQUIRE(oca.runString("b.2")->tos(false) == "3");

    // named tuples
    oca.runString("c = (pub x: 5, pub y: 6, pub z: 10)");
    REQUIRE(oca.runString("c.x")->tos(false) == "5");

    // complex tuples
    oca.runString("complex = (pub x: (1, 2, 3), pub y: 'hi')");
    oca.runString("complex.x.1 = 6");
    REQUIRE(oca.runString("complex.x.1")->tos(false) == "6");

    // blocks
    oca.runString("ret = do with val return val");
    REQUIRE(oca.runString("ret 6")->tos(false) == "6");
}

TEST_CASE("Conditional evaluation") {
    oca::State oca;

    // if
    REQUIRE(oca.runString("if true then 'yes'")->tos(false) == "yes");

    // if else
    REQUIRE(oca.runString("if false then 'yes' else 'no'")->tos(false) == "no");
}

TEST_CASE("Operators") {
    oca::State oca;

    // int and int
    /*REQUIRE(oca.runString("2 + 3")->tos(false) == "5");
    REQUIRE(oca.runString("2 - 3")->tos(false) == "-1");
    REQUIRE(oca.runString("2 * 3")->tos(false) == "6");
    REQUIRE(oca.runString("6 / 3")->tos(false) == "2");
    REQUIRE(oca.runString("10 % 4")->tos(false) == "2");
    REQUIRE(oca.runString("2 ^ 3")->tos(false) == "8");
    REQUIRE(oca.runString("2 == 3")->tos(false) == "false");
    REQUIRE(oca.runString("2 != 3")->tos(false) == "true");
    REQUIRE(oca.runString("2 < 3")->tos(false) == "true");
    REQUIRE(oca.runString("2 > 3")->tos(false) == "false");
    REQUIRE(oca.runString("2 <= 3")->tos(false) == "true");
    REQUIRE(oca.runString("2 >= 3")->tos(false) == "false");
    REQUIRE(oca.runString("2 .. 4")->tos(false) == "(2, 3, 4)");
    REQUIRE(oca.runString("0b0101 and 0b0110")->tos(false) == "4");
    REQUIRE(oca.runString("0b0101 or 0b0110")->tos(false) == "7");
    REQUIRE(oca.runString("0b0101 xor 0b0110")->tos(false) == "3");
    REQUIRE(oca.runString("0b0101 lsh 1")->tos(false) == "10");
    REQUIRE(oca.runString("0b0100 rsh 1")->tos(false) == "2");

    // int and real
    REQUIRE(oca.runString("2 + 3.5")->tos(false) == "5.5");
    REQUIRE(oca.runString("2 - 3.5")->tos(false) == "-1.5");
    REQUIRE(oca.runString("2 * 3.5")->tos(false) == "7.0");
    REQUIRE(oca.runString("5 / 2.5")->tos(false) == "2.0");
    REQUIRE(oca.runString("2 ^ 3.0")->tos(false) == "8.0");
    REQUIRE(oca.runString("2 < 3.5")->tos(false) == "true");
    REQUIRE(oca.runString("2 > 3.5")->tos(false) == "false");

    // real and real
    REQUIRE(oca.runString("2.0 + 3.5")->tos(false) == "5.5");
    REQUIRE(oca.runString("2.0 - 3.5")->tos(false) == "-1.5");
    REQUIRE(oca.runString("2.0 * 3.5")->tos(false) == "7.0");
    REQUIRE(oca.runString("5.0 / 2.5")->tos(false) == "2.0");
    REQUIRE(oca.runString("2.0 ^ 3.0")->tos(false) == "8.0");
    REQUIRE(oca.runString("2.0 < 3.5")->tos(false) == "true");
    REQUIRE(oca.runString("2.0 > 3.5")->tos(false) == "false");

    // real and int
    REQUIRE(oca.runString("2.0 + 3")->tos(false) == "5.0");
    REQUIRE(oca.runString("2.0 - 3")->tos(false) == "-1.0");
    REQUIRE(oca.runString("2.0 * 3")->tos(false) == "6.0");
    REQUIRE(oca.runString("5.0 / 2")->tos(false) == "2.5");
    REQUIRE(oca.runString("2.0 ^ 3")->tos(false) == "8.0");
    REQUIRE(oca.runString("2.0 < 3")->tos(false) == "true");
    REQUIRE(oca.runString("2.0 > 3")->tos(false) == "false");

    // str and str
    REQUIRE(oca.runString("'hi' == 'hello'")->tos(false) == "false");
    REQUIRE(oca.runString("'hi' != 'hello'")->tos(false) == "true");

    // str and int
    REQUIRE(oca.runString("'hi ' * 3")->tos(false) == "hi hi hi ");

    // str and any
    REQUIRE(oca.runString("'hi' + 5")->tos(false) == "hi5");

    // bool and bool
    REQUIRE(oca.runString("true == false")->tos(false) == "false");
    REQUIRE(oca.runString("true != false")->tos(false) == "true");
    REQUIRE(oca.runString("true and false")->tos(false) == "false");
    REQUIRE(oca.runString("true or false")->tos(false) == "true");*/
}
