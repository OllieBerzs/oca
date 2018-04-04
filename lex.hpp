/* ollieberzs 2018
** lex.hpp
** lexing oca source into tokens
*/

#pragma once

#include <string>
#include <vector>
#include "common.hpp"

OCA_BEGIN

class Token
{
public:
    enum Type
    {
        STRING = 0, BINNUM, HEXNUM, SCIENTNUM, REAL, INTEGER, BOOLEAN, FILEPATH,
        KEYWORD, NAME, OPERATOR, PUNCTUATION,
        COMMENT, INDENT, WHITESPACE, INVALID, LAST
    };

    Type type;
    std::string val;
    uint pos;

    void print() const;
};

class Lexer
{
    State* state;

public:
    Lexer(State* state);
    void lex(const std::string& source, std::vector<Token>& tokens);
};

OCA_END
