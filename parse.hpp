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
        INT, REAL, BOOL, BLOCK, TUP, NAME, CALLS
    };

    Type type;
    std::string val;
    ExprPtr left;
    ExprPtr right;
    uint index;

    Expression(Type type, const std::string& val, uint index);
    void print(uint indent = 0, char mod = '.');
};

class Parser
{
    const std::vector<Token>* tokens;
    std::vector<ExprPtr> cache;
    uint index;
    uint indent;
    bool inDot;

public:
    void parse(const std::vector<Token>& tokens, std::vector<ExprPtr>& exprs);

private:
    const Token& get();
    bool checkIndent(Indent ind);
    ExprPtr uncache();

    bool expr();
    bool set();
    bool call();
    bool access();
    bool dotaccess();
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

    friend class Errors;
};

OCA_END
