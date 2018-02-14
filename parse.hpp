/* ollieberzs 2018
** parse.hpp
** parsing oca tokens into AST
*/ 

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "common.hpp"

OCA_BEGIN

struct Token;
struct LexState;
struct Expression;

typedef std::shared_ptr<Expression> ExprPtr;

struct Expression
{
    std::string type;
    std::string val;
    ExprPtr left;
    ExprPtr right;

    Expression(const std::string& type, const std::string& val);
    void print(uint indent = 0);
};

struct ParseState
{
    uint current;                   // current token in script
    uint errorToken;                // token to blame for error
    LexState* ls;                   // info from lexer
    std::vector<ExprPtr> exprs;     // parsed ASTs so far

    const Token& get();
    void next();
    bool unparse(uint orig);
};

void parse(ParseState& state);

bool parseExpr(ExprPtr& out, ParseState& state);
bool parseCall(ExprPtr& out, ParseState& state);
bool parseAttach(ExprPtr& out, ParseState& state);
bool parseBlock(ExprPtr& out, ParseState& state);
bool parseDef(ExprPtr& out, ParseState& state);

bool parseVal(ExprPtr& out, ParseState& state);
bool parseName(ExprPtr& out, ParseState& state);

bool parseStr(ExprPtr& out, ParseState& state);
bool parseInt(ExprPtr& out, ParseState& state);
bool parseFloat(ExprPtr& out, ParseState& state);
bool parseBool(ExprPtr& out, ParseState& state);

void parseError(const ParseState& state, const std::string& message);

OCA_END
