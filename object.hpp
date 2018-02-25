/* ollieberzs 2018
** object.hpp
** object for all oca types
*/

#pragma once

#include <string>
#include <map>

#include "common.hpp"
#include "scope.hpp"

OCA_BEGIN

struct Object
{
    std::string type;
    std::string val;
    NativeMethod nat;
    ExprPtr block;
    std::map<std::string, ObjectPtr> table;

    Object() = default;
    Object(const std::string& v);
    Object(int v);
    Object(float v);
    Object(bool v);
    Object(NativeMethod v);

    std::string tos(bool debug);
    int toi();
    float tof();
    bool tob();
};

OCA_END
