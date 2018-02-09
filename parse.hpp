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
};

struct ParseState
{
    uint current;                   // current token in script
    uint errorToken;                // token to blame for error
    LexState* ls;                   // info from lexer
    std::vector<ExprPtr> exprs;     // parsed ASTs so far
};

void parse(ParseState& state);

bool parseExpr(ExprPtr& out, ParseState& state);
bool parseCall(ExprPtr& out, ParseState& state);
bool parseAttach(ExprPtr& out, ParseState& state);
bool parseBlock(ExprPtr& out, ParseState& state);
bool parseDef(ExprPtr& out, ParseState& state);

bool parseArgs(ExprPtr& out, ParseState& state, uint line);
bool parseParams(ExprPtr& out, ParseState& state, uint line);

bool parseStr(ExprPtr& out, ParseState& state);
bool parseInt(ExprPtr& out, ParseState& state);
bool parseFloat(ExprPtr& out, ParseState& state);
bool parseBool(ExprPtr& out, ParseState& state);

const Token& getToken(ParseState& state);
bool unparse(uint orig, ParseState& state);

void parseError(const ParseState& state, const std::string& message);
void printExpr(const Expression& e, uint indent = 0);

OCA_END
