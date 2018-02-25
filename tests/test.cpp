#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../oca.hpp"
#include "../object.hpp"

TEST_CASE("Evaluation of basic types", "[types]")
{
    oca::State state;
    oca::ObjectPtr obj = nullptr;

    // integer
    REQUIRE((obj = state.eval("820")) != nullptr);
    REQUIRE(obj->tos(true) == "<int>820");

    // float
    REQUIRE((obj = state.eval("45.0003")) != nullptr);
    REQUIRE(obj->tos(true) == "<float>45.0003");

    // string
    REQUIRE((obj = state.eval("'This is a string!'")) != nullptr);
    REQUIRE(obj->tos(true) == "<str>This is a string!");

    // bool
    REQUIRE((obj = state.eval("true")) != nullptr);
    REQUIRE(obj->tos(true) == "<bool>true");

    // tuple
    REQUIRE((obj = state.eval("(2 3 (true false))")) != nullptr);
    REQUIRE(obj->tos(true) == "<tup>([1]<int>2 [2]<int>3 [3]<tup>([1]<bool>true [2]<bool>false))");
}
