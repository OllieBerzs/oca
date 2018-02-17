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
struct Lexer;
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

struct Parser
{
    std::string path;
    std::vector<Token> tokens;
    std::vector<ExprPtr> cache;
    std::vector<ExprPtr> result;
    uint index;
    uint errorToken;

    Parser(std::vector<Token>& ts);

    const Token& get();
    void next();
    bool unparse(uint orig);

    std::vector<ExprPtr> parse();

    bool expr();
    bool call();
    bool attach();
    bool block();
    bool def();

    bool string();
    bool integer();
    bool floatnum();
    bool boolean();

    bool value();
    bool name();
    bool lit(const std::string& t);

    void error(const std::string& message);
};

OCA_END
