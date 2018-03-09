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
        STRING = 0, REAL, INTEGER, BOOLEAN, FILEPATH,
        KEYWORD, NAME, OPERATOR, PUNCTUATION,
        COMMENT, INDENT, WHITESPACE, INVALID, LAST
    };

    Type type;
    std::string val;
    uint pos;

    void print();
};

class Lexer
{
    std::string source;
    std::string path;

public:
    Lexer(const std::string& s, const std::string& p);
    std::vector<Token> lex();

private:
    void error(const std::string& message, const Token t);
};

OCA_END
