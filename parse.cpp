/* ollieberzs 2018
** parse.cpp
** parsing oca tokens into AST
*/

#include <iostream>
#include <fstream>
#include "parse.hpp"
#include "lex.hpp"
#include "error.hpp"

OCA_BEGIN

Expression::Expression(Expression::Type type, const std::string& val, uint index)
    : type(type), val(val), index(index) {}

void Expression::print(uint indent, char mod)
{
    std::vector<std::string> types =
    {
        "set", "call", "access", "if", "else", "next", "main",
        "branches", "part oper", "oper", "return", "break", "file", "str",
        "int", "real", "bool", "block", "tup", "name", "calls"
    };

    for (uint i = 0; i < indent; i++) std::cout << "  ";

    std::cout << mod << "<" << types[type] << ">" << val << "\n";

    if (left) left->print(indent + 1, 'L');
    if (right) right->print(indent + 1, 'R');
}

// ----------------------------

void Parser::parse(const std::vector<Token>& tokens, std::vector<ExprPtr>& exprs)
{
    index = 0;
    indent = 0;
    inDot = false;

    this->tokens = &tokens;
    while (index < tokens.size() - 1)
    {
        if (expr())
        {
            exprs.push_back(cache.back());
            cache.pop_back();
        }
        else Errors::instance().panic(NOT_AN_EXPRESSION);
        if (get().type == Token::LAST) break;
        if (checkIndent(Indent::MORE)) Errors::instance().panic(UNEXPECTED_INDENT);
        if (!checkIndent(Indent::SAME) && !checkIndent(Indent::LESS)) Errors::instance().panic(NO_NEWLINE);
    }
}

// ----------------------------

const Token& Parser::get()
{
    if (index < tokens->size() - 1) return tokens->at(index);
    else return tokens->back();
}

bool Parser::checkIndent(Indent ind)
{
    uint size = get().val.size() - 1;

    if (get().type != Token::INDENT) return false;
    if (ind == Indent::LESS) if (size >= indent) return false;
    if (ind == Indent::SAME) if (size > indent || size < indent) return false;
    if (ind == Indent::MORE) if (size <= indent) return false;

    indent = size;
    ++index;
    return true;
}

ExprPtr Parser::uncache()
{
    ExprPtr result = cache.back();
    cache.pop_back();
    return result;
}

// ----------------------------

bool Parser::expr()
{
    if (call() || value() || cond() || keyword() || file()) return true;
    return false;
}

bool Parser::set()
{
    uint orig = index;

    // assignment starts with '='
    if (!lit("=")) return false;

    // has to have a value after
    if (!call() && !value() &&
        !file() && !cond()) Errors::instance().panic(NOTHING_TO_SET);

    // assemble assignment
    ExprPtr s = std::make_shared<Expression>(Expression::SET, "", orig);
    s->right = uncache();
    s->left = uncache();
    cache.push_back(s);

    return true;
}

bool Parser::call()
{
    uint orig = index;

    // call has to start with a name
    if (!name()) return false;

    // can have an argument and/or a yield block
    bool hasArg = value() || call();
    bool hasYield = block();

    // assemble call
    ExprPtr yield = (hasYield) ? uncache() : nullptr;
    ExprPtr arg = (hasArg) ? uncache() : nullptr;

    ExprPtr c = std::make_shared<Expression>(Expression::CALL, uncache()->val, orig);
    c->left = yield;
    c->right = arg;
    cache.push_back(c);

    // check for accessors
    if (!access() && !inDot) dotaccess();

    if (cache.size() == 1 && lit(","))
    {
        uint origc = index;
        if (!call()) Errors::instance().panic(NO_NAME);
        ExprPtr calls = std::make_shared<Expression>(Expression::CALLS, "", origc);
        calls->right = uncache();
        calls->left = uncache();
        cache.push_back(calls);
    }

    if (!set()) oper();
    return true;
}

bool Parser::access()
{
    uint orig = index;
    if (!lit("[")) return false;
    if (!integer() && !string() && !call()) Errors::instance().panic(NO_ACCESS_KEY_CALL);
    if (!lit("]")) Errors::instance().panic(NO_CLOSING_BRACE);

    // assemble access
    ExprPtr a = std::make_shared<Expression>(Expression::ACCESS, "[]", orig);
    a->right = uncache();
    a->left = uncache();
    cache.push_back(a);

    // additional access
    access();

    return true;
}

bool Parser::dotaccess()
{
    uint orig = index;
    if (!lit(".")) return false;
    inDot = true; // so the next call doesn't parse dotaccess
    if (!call()) Errors::instance().panic(NO_ACCESS_KEY);

    // assemble dot access
    ExprPtr a = std::make_shared<Expression>(Expression::ACCESS, ".", orig);
    a->right = uncache();
    a->left = uncache();
    cache.push_back(a);

    // additional access
    dotaccess();

    return true;
}

bool Parser::cond()
{
    uint orig = index;
    if (!lit("if")) return false;

    if (!set() && !call() && !value()) Errors::instance().panic(NO_CONDITIONAL);
    if (!lit("then")) Errors::instance().panic(NO_THEN);

    uint origt = index;
    uint cached = cache.size();
    if (checkIndent(Indent::MORE))
    {
        while (expr())
        {
            if (!checkIndent(Indent::SAME)) break;
        }
        checkIndent(Indent::LESS);
    }
    else
    {
        if (!expr()) Errors::instance().panic(NOT_AN_EXPRESSION);
        checkIndent(Indent::SAME);
    }

    uint elseCached = cache.size();
    bool hasElse = false;
    uint orige = index;
    if (lit("else"))
    {
        hasElse = true;
        if (checkIndent(Indent::MORE))
        {
            while (expr())
            {
                if (!checkIndent(Indent::SAME)) break;
            }
        }
        else
        {
            if (!expr()) Errors::instance().panic(NOT_AN_EXPRESSION);
        }
    }

    // assemble conditional
    ExprPtr els = std::make_shared<Expression>(Expression::ELSE, "", orige);

    ExprPtr curr = els;
    for (uint i = elseCached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>
                (Expression::NEXT, "", orig);
            curr = curr->right;
        }
    }
    cache.resize(elseCached);

    ExprPtr mn = std::make_shared<Expression>(Expression::MAIN, "", origt);

    curr = mn;
    for (uint i = cached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>
                (Expression::NEXT, "", orig);
            curr = curr->right;
        }
    }
    cache.resize(cached);

    ExprPtr ifer = std::make_shared<Expression>(Expression::IF, "", orig);
    ifer->left = cache.back(); // condition
    cache.pop_back();
    ExprPtr branches = std::make_shared<Expression>(Expression::BRANCHES, "", orig);
    branches->left = mn;
    if (hasElse) branches->right = els;
    ifer->right = branches;
    cache.push_back(ifer);

    return true;
}

bool Parser::oper()
{
    uint orig = index;
    uint cached = cache.size();

    if (get().type != Token::OPERATOR) return false;
    bool first = (cached < 2) || (cache[cached - 2]->type != Expression::PART_OPER);

    cache.push_back(std::make_shared<Expression>(Expression::PART_OPER, get().val, orig));
    index++;
    if (!value() && !call()) Errors::instance().panic(NO_RIGHT_VALUE);
    oper();

    if (!first) return true;

    // assemble operator
    for (int p = 3; p >= 0; p--)
    {
        for (auto it = cache.begin() + cached - 1; it != cache.end(); it++)
        {
            if ((*it)->type != Expression::PART_OPER) continue;
            uint origp = (*it)->index;

            // set priority
            int priority = 1;
            char op = (*it)->val[0];
            if (op == '=' || op == '<' || op == '>') priority = 0;
            else if (op == '*' || op == '/' || op == '%') priority = 2;
            else if (op == '^') priority = 3;

            if (priority != p) continue;

            ExprPtr o = std::make_shared<Expression>(Expression::OPER, (*it)->val, origp);
            o->left = *(it - 1);
            o->right = *(it + 1);
            cache.erase(it - 1, it + 2);
            cache.insert(it - 1, o);
            it--;
        }
    }
    return true;
}

bool Parser::keyword()
{
    if (get().val == "return")
    {
        ExprPtr r = std::make_shared<Expression>(Expression::RETURN, "", index++);
        if (expr()) r->right = uncache();
        cache.push_back(r);
        return true;
    }
    else if (get().val == "break")
    {
        cache.push_back(std::make_shared<Expression>(Expression::BREAK, "", index++));
        return true;
    }
    return false;
}

bool Parser::file()
{
    if (get().type != Token::FILEPATH) return false;

    cache.push_back(std::make_shared<Expression>(Expression::FILE, get().val.substr(1), index));
    index++;
    return true;
}

// ----------------------------

bool Parser::string()
{
    if (get().type != Token::STRING) return false;

    std::string s = get().val.substr(1, get().val.size() - 2);
    cache.push_back(std::make_shared<Expression>(Expression::STR, s, index++));
    return true;
}

bool Parser::integer()
{
    if (get().type != Token::INTEGER) return false;

    cache.push_back(std::make_shared<Expression>(Expression::INT, get().val, index++));
    return true;
}

bool Parser::real()
{
    if (get().type != Token::REAL) return false;

    cache.push_back(std::make_shared<Expression>(Expression::REAL, get().val, index++));
    return true;
}

bool Parser::boolean()
{
    if (get().type != Token::BOOLEAN) return false;

    cache.push_back(std::make_shared<Expression>(Expression::BOOL, get().val, index++));
    return true;
}

bool Parser::block()
{
    uint orig = index;
    if (!lit("do")) return false;

    // check for parameters
    std::string params = "";
    if (lit("with"))
    {
        while (name())
        {
            params += uncache()->val + " ";
            if (!lit(",")) break;
        }
        params.pop_back();
        if (params == "") Errors::instance().panic(NO_PARAMETER);
    }

    // getting the expression block
    if (checkIndent(Indent::SAME)) Errors::instance().panic(NO_INDENT);
    uint cached = cache.size();
    if (checkIndent(Indent::MORE))
    {
        while (expr()) if (!checkIndent(Indent::SAME)) break;
    }
    else if (!expr()) Errors::instance().panic(NOT_AN_EXPRESSION);

    // assemble block
    ExprPtr bl = std::make_shared<Expression>(Expression::BLOCK, params, orig);

    ExprPtr curr = bl;
    for (uint i = cached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "", orig);
            curr = curr->right;
        }
    }
    cache.resize(cached);

    cache.push_back(bl);

    return true;
}

// ----------------------------

bool Parser::value()
{
    uint cached = cache.size();

    if (string() || integer() || real() || boolean() || block()) // single value
    {
        // check for accessor
        if (!access() && !inDot) dotaccess();
        oper();
        return true;
    }
    else if (lit("(")) // tuple
    {
        checkIndent(Indent::MORE);
        while (true)
        {
            if (checkIndent(Indent::MORE)) Errors::instance().panic(UNEXPECTED_INDENT);
            checkIndent(Indent::SAME);
            uint origt = index;
            std::string nam = "";
            if (name())
            {
                if (lit(":")) nam = uncache()->val;
                else
                {
                    cache.pop_back();
                    --index;
                }

            }
            if (!expr()) Errors::instance().panic(NOTHING_TO_SET);

            ExprPtr tup = std::make_shared<Expression>(Expression::TUP, nam, origt);
            tup->left = uncache();
            cache.push_back(tup);

            if (!lit(",")) break;
        }
        checkIndent(Indent::LESS);
        if (!lit(")")) Errors::instance().panic(NO_CLOSING_BRACE);

        // assemble tuple
        ExprPtr tup = cache[cached];
        for (uint i = cached; i < cache.size() - 1; ++i) cache[i]->right = cache[i + 1];
        cache.resize(cached);
        cache.push_back(tup);

        // check for accessor
        if (!access() && !inDot) dotaccess();
        oper();
        return true;
    }
    return false;
}

bool Parser::name()
{
    if (get().type != Token::NAME) return false;

    cache.push_back(std::make_shared<Expression>(Expression::NAME, get().val, index++));
    return true;
}

bool Parser::lit(const std::string& t)
{
    if (get().val != t) return false;
    ++index;
    return true;
}

OCA_END
