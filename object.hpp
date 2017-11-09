#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "expression.hpp"

namespace oca::internal
{

struct Method;
class Scope;

struct Object
{
    std::map<std::string, Method*> methods;

    virtual ~Object() {};

    virtual std::string toString() const;
};

struct Number : public Object
{
    float num;

    Number(float num);
    ~Number();

    std::string toString() const;
};

struct String : public Object
{
    std::string str;

    String(const std::string& str);
    ~String();

    std::string toString() const;
};

// Typedefs for native functions
typedef std::vector<Object*> Nargs;
typedef std::function<Object*(const std::vector<Object*>&)> Nfunc;

struct Method : public Object
{
    Nfunc function;
    std::vector<Expression*> exprs;
    bool native;
    Scope* scope; // Delete from outside

    Method(const std::vector<Expression*>& exprs, Scope* scope);
    Method(const Nfunc& function);
    ~Method();

    std::string toString() const;
};

} // namespace oca::internal
