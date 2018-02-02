/* ollieberzs 2018
** value.cpp
** value for all oca types
*/ 

#include "value.hpp"
#include "parse.hpp"

OCA_BEGIN

Value::Value() : table(nullptr) {}
// ----------------------------

ValuePtr Value::makeStr(ExprPtr v)
{
    ValuePtr vp = std::make_shared<Value>();
    vp->type = "str";
    vp->val = v;
    return vp;
}

ValuePtr Value::makeInt(ExprPtr v)
{
    ValuePtr vp = std::make_shared<Value>();
    vp->type = "int";
    vp->val = v;
    return vp;
}

ValuePtr Value::makeFloat(ExprPtr v)
{
    ValuePtr vp = std::make_shared<Value>();
    vp->type = "float";
    vp->val = v;
    return vp;
}

ValuePtr Value::makeBool(ExprPtr v)
{
    ValuePtr vp = std::make_shared<Value>();
    vp->type = "bool";
    vp->val = v;
    return vp;
}

ValuePtr Value::makeBlock(ExprPtr v)
{
    ValuePtr vp = std::make_shared<Value>();
    vp->type = "block";
    vp->val = v;
    return vp;
}

ValuePtr Value::makeMeth(NativeMethod v)
{
    ValuePtr vp = std::make_shared<Value>();
    vp->type = "native";
    vp->native = v;
    return vp;
}
// ----------------------------

std::string Value::tos()
{
    if (type == "str") return val->val;
    else return "";
}

int Value::toi()
{
    if (type == "int") return std::stoi(val->val);
    else return 0;
}

float Value::tof()
{
    if (type == "float") return std::stof(val->val);
    else return 0.0f;
}

bool Value::tob()
{
    if (type == "bool") return val->val == "true";
    else return false;
}

OCA_END