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
    uint line;
    uint col;
};

struct LexState
{
    std::string source;        // oca source code
    std::string sourceName;    // source file name
    uint current;              // current char in script
    uint lineNum;              // source line counter
    uint colNum;               // source column counter
    std::vector<Token> tokens; // lexed tokens so far
};

void lex(LexState& state);

bool matchPunct(LexState& state);
bool matchNum(LexState& state);
bool matchStr(LexState& state);
bool matchBool(LexState& state);
bool matchKeyword(LexState& state);
bool matchOper(LexState& state);
bool matchName(LexState& state);

void skipSpace(LexState& state);
bool charIsIn(char c, const std::string& str);
bool match(const std::string& to, LexState& state);

void lexError(const LexState& state, const std::string& message);
void printToken(const Token& token);

OCA_END
