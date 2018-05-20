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

class Expression {
public:
    enum Type {
        SET,
        CALL,
        ACCESS,
        IF,
        ELSE,
        NEXT,
        MAIN,
        BRANCHES,
        PART_OPER,
        OPER,
        RETURN,
        BREAK,
        FILE,
        STR,
        FSTR,
        INT,
        REAL,
        BOOL,
        BLOCK,
        TABL,
        EMPTY_TABL,
        NAME,
        CALLS
    };

    Type type;
    std::string val;
    ExprPtr left;
    ExprPtr right;
    uint index;

    Expression(Type type, const std::string& val, uint index);
    void print(uint indent = 0, char mod = '.');
};

class Parser {
    const std::vector<Token>* tokens;
    std::vector<ExprPtr> cache;
    uint index;
    uint indent;

public:
    Parser() = default;
    std::vector<ExprPtr> makeAST(const std::vector<Token>& tokens);

private:
    const Token& get();
    ExprPtr uncache();

    bool expr();
    bool set();
    bool call(bool inDot = false);
    bool access();
    bool cond();
    bool oper();
    bool keyword();
    bool file();
    bool name();
    bool value();
    bool string();
    bool fstring();
    bool integer();
    bool real();
    bool boolean();
    bool block();

    bool checkLit(const std::string& t);
    bool checkIndent(Indent ind);

    friend class ErrorHandler;
};

OCA_END
