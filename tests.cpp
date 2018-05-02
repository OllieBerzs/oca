/* ollieberzs 2018
** tests.cpp
** oca testing
*/

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "oca.hpp"

TEST_CASE("Evaluation of basic types") {
    oca::State oca;

    auto dec = oca.runString("820");
    REQUIRE(dec->typestr() == "int");
    REQUIRE(dec->tos() == "820");

    auto bin = oca.runString("0b0110");
    REQUIRE(bin->typestr() == "int");
    REQUIRE(bin->tos() == "6");

    auto hex = oca.runString("0xFF");
    REQUIRE(hex->typestr() == "int");
    REQUIRE(hex->tos() == "255");

    auto real = oca.runString("45.5");
    REQUIRE(real->typestr() == "real");
    REQUIRE(real->tos() == "45.5");

    auto scient = oca.runString("2e2");
    REQUIRE(scient->typestr() == "real");
    REQUIRE(scient->tos() == "200.0");

    auto str = oca.runString("'This is a string!'");
    REQUIRE(str->typestr() == "str");
    REQUIRE(str->tos() == "This is a string!");

    auto boolean = oca.runString("true");
    REQUIRE(boolean->typestr() == "bool");
    REQUIRE(boolean->tos() == "true");

    auto tuple = oca.runString("(2, 3, (true, false))");
    REQUIRE(tuple->typestr() == "(int, int, (bool, bool))");
    REQUIRE(tuple->tos() == "(0: 2, 1: 3, 2: (0: true, 1: false))");
}

TEST_CASE("Variable setting and getting") {
    oca::State oca;

    // simple variable
    REQUIRE(oca.runString("a = 5")->tos() == "5");
    REQUIRE(oca.runString("a")->tos() == "5");
    oca.runString("a = 7");
    REQUIRE(oca.runString("a")->tos() == "7");

    // simple tuples
    oca.runString("b = (1, 2, 3, 4)");
    REQUIRE(oca.runString("b.2")->tos() == "3");

    // named tuples
    oca.runString("c = (pub x: 5, pub y: 6, pub z: 10)");
    REQUIRE(oca.runString("c.x")->tos() == "5");

    // complex tuples
    oca.runString("complex = (pub x: (1, 2, 3), pub y: 'hi')");
    oca.runString("complex.x.1 = 6");
    REQUIRE(oca.runString("complex.x.1")->tos() == "6");

    // blocks
    oca.runString("ret = do with val return val");
    REQUIRE(oca.runString("ret 6")->tos() == "6");
}

TEST_CASE("Conditional evaluation") {
    oca::State oca;

    // if
    REQUIRE(oca.runString("if true then 'yes'")->tos() == "yes");

    // if else
    REQUIRE(oca.runString("if false then 'yes' else 'no'")->tos() == "no");
}

TEST_CASE("Operators") {
    oca::State oca;

    // int and int
    REQUIRE(oca.runString("2 + 3")->tos() == "5");
    REQUIRE(oca.runString("2 - 3")->tos() == "-1");
    REQUIRE(oca.runString("2 * 3")->tos() == "6");
    REQUIRE(oca.runString("6 / 3")->tos() == "2");
    REQUIRE(oca.runString("10 % 4")->tos() == "2");
    REQUIRE(oca.runString("2 ^ 3")->tos() == "8");
    REQUIRE(oca.runString("2 == 3")->tos() == "false");
    REQUIRE(oca.runString("2 != 3")->tos() == "true");
    REQUIRE(oca.runString("2 < 3")->tos() == "true");
    REQUIRE(oca.runString("2 > 3")->tos() == "false");
    REQUIRE(oca.runString("2 <= 3")->tos() == "true");
    REQUIRE(oca.runString("2 >= 3")->tos() == "false");
    REQUIRE(oca.runString("2 .. 4")->tos() == "(0: 2, 1: 3, 2: 4)");
    REQUIRE(oca.runString("0b0101 and 0b0110")->tos() == "4");
    REQUIRE(oca.runString("0b0101 or 0b0110")->tos() == "7");
    REQUIRE(oca.runString("0b0101 xor 0b0110")->tos() == "3");
    REQUIRE(oca.runString("0b0101 lsh 1")->tos() == "10");
    REQUIRE(oca.runString("0b0100 rsh 1")->tos() == "2");

    // int and real
    REQUIRE(oca.runString("2 + 3.5")->tos() == "5.5");
    REQUIRE(oca.runString("2 - 3.5")->tos() == "-1.5");
    REQUIRE(oca.runString("2 * 3.5")->tos() == "7.0");
    REQUIRE(oca.runString("5 / 2.5")->tos() == "2.0");
    REQUIRE(oca.runString("2 ^ 3.0")->tos() == "8.0");
    REQUIRE(oca.runString("2 < 3.5")->tos() == "true");
    REQUIRE(oca.runString("2 > 3.5")->tos() == "false");

    // real and real
    REQUIRE(oca.runString("2.0 + 3.5")->tos() == "5.5");
    REQUIRE(oca.runString("2.0 - 3.5")->tos() == "-1.5");
    REQUIRE(oca.runString("2.0 * 3.5")->tos() == "7.0");
    REQUIRE(oca.runString("5.0 / 2.5")->tos() == "2.0");
    REQUIRE(oca.runString("2.0 ^ 3.0")->tos() == "8.0");
    REQUIRE(oca.runString("2.0 < 3.5")->tos() == "true");
    REQUIRE(oca.runString("2.0 > 3.5")->tos() == "false");

    // real and int
    REQUIRE(oca.runString("2.0 + 3")->tos() == "5.0");
    REQUIRE(oca.runString("2.0 - 3")->tos() == "-1.0");
    REQUIRE(oca.runString("2.0 * 3")->tos() == "6.0");
    REQUIRE(oca.runString("5.0 / 2")->tos() == "2.5");
    REQUIRE(oca.runString("2.0 ^ 3")->tos() == "8.0");
    REQUIRE(oca.runString("2.0 < 3")->tos() == "true");
    REQUIRE(oca.runString("2.0 > 3")->tos() == "false");

    // str and str
    REQUIRE(oca.runString("'hi' == 'hello'")->tos() == "false");
    REQUIRE(oca.runString("'hi' != 'hello'")->tos() == "true");

    // str and int
    REQUIRE(oca.runString("'hi ' * 3")->tos() == "hi hi hi ");

    // str and any
    REQUIRE(oca.runString("'hi' + 5")->tos() == "hi5");

    // bool and bool
    REQUIRE(oca.runString("true == false")->tos() == "false");
    REQUIRE(oca.runString("true != false")->tos() == "true");
    REQUIRE(oca.runString("true and false")->tos() == "false");
    REQUIRE(oca.runString("true or false")->tos() == "true");
}
