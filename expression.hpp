#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include "memory.hpp"
#include "colors.hpp"

namespace oca::internal
{

enum
{
    E_NUMBER,
    E_STRING,
    E_CALL,
    E_METHOD,
    E_ARG,
    E_NULL
};
const std::string E_TYPES[6]
{
    "number",
    "string",
    "call",
    "method",
    "arg",
    "null"
};

struct Expression
{
    int type;
    std::string value;
    Expression* left;
    Expression* right;
    Expression* other;

    Expression(int type, const std::string& value)
        : type(type), value(value), left(nullptr), right(nullptr), other(nullptr)
    {
        Memory::add('e');
    }

    Expression(int type, const std::string& value, Expression* l, Expression* r)
        : type(type), value(value), left(l), right(r), other(nullptr)
    {
        Memory::add('e');
    }

    ~Expression()
    {
        delete left;
        delete right;
        Memory::rem('e');
    }
};

inline void printTree(const Expression& e, std::ostream& stream, const std::string& branch, int indent = 0)
{
    std::string data = YELLOW + branch + RESET + "(\"" + CYAN + E_TYPES[e.type] + GREEN + " " + e.value + RESET + "\")\n";
    stream << std::setw(data.size() + indent) << data;
    if (e.left)
    {
        printTree(*e.left, stream, "L", indent + 2);
    }
    if (e.right)
    {
        printTree(*e.right, stream, "R", indent + 2);
    }
    if (e.other)
    {
        printTree(*e.other, stream, "O", indent + 2);
    }
}

inline std::ostream& operator<<(std::ostream& stream, const Expression& expr)
{
    printTree(expr, stream, "");
    return stream;
}

} // namespace oca::internal
