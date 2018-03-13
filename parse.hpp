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

enum class Indent { SAME, MORE, LESS, ANY };

class Expression
{
public:
    enum Type
    {
        SET, CALL, ACCESS, IF, ELSE, NEXT, MAIN,
        BRANCHES, PART_OPER, OPER, RETURN, BREAK, FILE, STR,
        INT, REAL, BOOL, BLOCK, TUP, NAME
    };

    Type type;
    std::string val;
    ExprPtr left;
    ExprPtr right;

    Expression(Type type, const std::string& val);
    void print(uint indent = 0, char mod = '.');
};

class Parser
{
    std::string path;
    std::vector<Token> tokens;
    std::vector<ExprPtr> cache;
    uint index;
    uint indent;

public:
    Parser(std::vector<Token>& ts, const std::string& path);

    std::vector<ExprPtr> parse();

private:
    Token& get();
    bool checkIndent(Indent ind);

    bool expr();
    bool set();
    bool call();
    bool access();
    bool cond();
    bool oper();
    bool keyword();
    bool file();

    bool string();
    bool integer();
    bool real();
    bool boolean();
    bool block();

    bool value();
    bool name();
    bool lit(const std::string& t);

    void error(const std::string& message);
};

OCA_END
