/* ollieberzs 2018
** lex.hpp
** lexing oca source into tokens
*/

#pragma once

#include <string>
#include <vector>
#include "common.hpp"

OCA_BEGIN

class Token {
public:
    enum Type {
        STRING = 0,
        BINNUM,
        HEXNUM,
        SCIENTNUM,
        REAL,
        INTEGER,
        BOOLEAN,
        FILEPATH,
        KEYWORD,
        NAME,
        OPERATOR,
        PUNCTUATION,
        COMMENT,
        INDENT,
        WHITESPACE,
        INVALID,
        LAST
    };

    Type type;
    std::string val;
    uint pos;

    void print() const;
};

class Lexer {
    std::vector<int> captureGroupCounts;
    std::vector<std::pair<Token::Type, std::string>> syntax = {
        {Token::STRING, "'[^']*'"},
        {Token::BINNUM, "0b[01]+"},
        {Token::HEXNUM, "0x[0-9A-Fa-f]+"},
        {Token::SCIENTNUM, "[0-9]+(\\.[0-9]+)?[eE]-?[0-9]+(\\.[0-9]+)?"},
        {Token::REAL, "[0-9]+\\.[0-9]+"},
        {Token::INTEGER, "[0-9]+"},
        {Token::BOOLEAN, "\\b(true|false)\\b"},
        {Token::FILEPATH, "\\$.+"},
        {Token::KEYWORD, "\\b(do|if|then|else|return|break|with|pub)\\b"},
        {Token::OPERATOR, "\\+|-|\\*|\\/|%|\\^|<=|>=|==|!=|<|>|\\.\\.|and|or|xor|lsh|rsh"},
        {Token::COMMENT, "#(.)*"},
        {Token::NAME, "[A-Za-z_][A-Za-z_0-9]*"},
        {Token::PUNCTUATION, "\\.|:|\\(|\\)|,|="},
        {Token::INDENT, "\\n *(?=[^\\s^#])"},
        {Token::WHITESPACE, "\\n *| +"},
        {Token::INVALID, ".+"}};

public:
    explicit Lexer();
    void lex(const std::string& source, std::vector<Token>& tokens);

private:
    uint indexFromGroup(uint group);
};

OCA_END
