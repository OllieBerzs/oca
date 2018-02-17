/* ollieberzs 2018
** lex.hpp
** lexing oca source into tokens
*/ 

#pragma once

#include <string>
#include <vector>
#include "common.hpp"

OCA_BEGIN

struct Token
{
    std::string type;
    std::string val;
    uint pos;

    void print();
};

struct Lexer
{
    std::string source;
    std::string path;

    std::vector<Token> lex();
    void error(const std::string& message, const Token t);
};

OCA_END
