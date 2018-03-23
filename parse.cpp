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

Parser::Parser(ErrorHandler* er)
    : er(er), index(0), indent(0), inAccess(false)
{
    er->parser = this;
}

void Parser::parse(const std::vector<Token>& tokens, std::vector<ExprPtr>& exprs)
{
    this->tokens = &tokens;
    while (index < tokens.size() - 1)
    {
        if (expr())
        {
            exprs.push_back(cache.back());
            cache.pop_back();
        }
        else er->error(NOT_AN_EXPRESSION);
        if (get().type == Token::LAST) break;
        if (checkIndent(Indent::MORE)) er->error(UNEXPECTED_INDENT);
        if (!checkIndent(Indent::SAME) && !checkIndent(Indent::LESS)) er->error(NO_NEWLINE);
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
    if (get().type != Token::INDENT) return false;

    uint size = get().val.size();
    if (get().val[0] == '\n') --size;

    if (ind == Indent::LESS) if (size >= indent) return false;
    if (ind == Indent::SAME) if (size > indent || size < indent) return false;
    if (ind == Indent::MORE) if (size <= indent) return false;

    indent = size;
    ++index;
    return true;
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
    if (!lit("=")) return false;

    if (!call() && !value() &&
        !file() && !cond()) er->error(NOTHING_TO_SET);

    // assemble assignment
    ExprPtr s = std::make_shared<Expression>(Expression::SET, "", orig);
    s->right = cache.back();
    cache.pop_back();
    s->left = cache.back();
    cache.pop_back();
    cache.push_back(s);

    return true;
}

bool Parser::call()
{
    uint orig = index;
    if (!name()) return false;

    bool hasArg = value() || call();
    bool hasBlock = block();

    // assemble call
    ExprPtr bl = nullptr;
    ExprPtr arg = nullptr;

    if (hasBlock)
    {
        bl = cache.back();
        cache.pop_back();
    }

    if (hasArg)
    {
        arg = cache.back();
        cache.pop_back();
    }

    ExprPtr c = std::make_shared<Expression>
        (Expression::CALL, cache.back()->val, orig);
    cache.pop_back();
    c->left = bl;
    c->right = arg;
    cache.push_back(c);

    if (!inAccess) access();
    if (lit(","))
    {
        if (!call()) er->error(NO_NAME);
        ExprPtr calls = std::make_shared<Expression>
            (Expression::CALLS, "", index);
        calls->right = cache.back();
        cache.pop_back();
        calls->left = cache.back();
        cache.pop_back();
        cache.push_back(calls);
    }

    bool hasSet = false;
    if (cache.size() == 1) hasSet = set();
    if (!hasSet) oper();
    return true;
}

bool Parser::access()
{
    uint orig = index;
    inAccess = true;
    std::string mod = "";
    if (lit("."))
    {
        if (!integer() && !call()) er->error(NO_ACCESS_KEY);
    }
    else if (lit("["))
    {
        mod = "[]";
        if (!integer() && !string() && !call()) er->error(NO_ACCESS_KEY_CALL);
        if (!lit("]")) er->error(NO_CLOSING_BRACE);
    }
    else
    {
        inAccess = false;
        return false;
    }

    // assemble access
    ExprPtr next = cache.back();
    cache.pop_back();
    ExprPtr prev = cache.back();
    cache.pop_back();
    ExprPtr a = std::make_shared<Expression>(Expression::ACCESS, mod, orig);
    a->left = prev;
    a->right = next;
    cache.push_back(a);

    // additional access
    access();
    inAccess = false;

    return true;
}

bool Parser::cond()
{
    uint orig = index;
    if (!lit("if")) return false;

    if (!set() && !call() && !value()) er->error(NO_CONDITIONAL);
    if (!lit("then")) er->error(NO_THEN);

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
        if (!expr()) er->error(NOT_AN_EXPRESSION);
        checkIndent(Indent::SAME);
    }

    uint elseCached = cache.size();
    bool hasElse = false;
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
            if (!expr()) er->error(NOT_AN_EXPRESSION);
        }
    }

    // assemble conditional
    ExprPtr els = std::make_shared<Expression>(Expression::ELSE, "", orig);

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

    ExprPtr mn = std::make_shared<Expression>(Expression::MAIN, "", orig);

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
    ExprPtr branches = std::make_shared<Expression>
        (Expression::BRANCHES, "", orig);
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

    cache.push_back(std::make_shared<Expression>
        (Expression::PART_OPER, get().val, orig));
    index++;
    if (!value() && !call()) er->error(NO_RIGHT_VALUE);
    oper();

    if (!first) return true;

    // assemble operator
    for (int p = 3; p >= 0; p--)
    {
        for (auto it = cache.begin() + cached - 1; it != cache.end(); it++)
        {
            if ((*it)->type != Expression::PART_OPER) continue;

            // set priority
            int priority = 1;
            char op = (*it)->val[0];
            if (op == '=' || op == '<' || op == '>') priority = 0;
            else if (op == '*' || op == '/' || op == '%') priority = 2;
            else if (op == '^') priority = 3;

            if (priority != p) continue;

            ExprPtr o = std::make_shared<Expression>(Expression::OPER, (*it)->val, orig);
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
    uint orig = index;
    if (get().val == "return")
    {
        index++;
        ExprPtr r = std::make_shared<Expression>(Expression::RETURN, "", orig);
        if (expr())
        {
            r->right = cache.back();
            cache.pop_back();
        }
        cache.push_back(r);
        return true;
    }
    else if (get().val == "break")
    {
        index++;
        cache.push_back(std::make_shared<Expression>(Expression::BREAK, "", orig));
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

    std::string val = get().val;
    cache.push_back(std::make_shared<Expression>
        (Expression::STR, val.substr(1, val.size() - 2), index));
    index++;
    return true;
}

bool Parser::integer()
{
    if (get().type != Token::INTEGER) return false;

    cache.push_back(std::make_shared<Expression>(Expression::INT, get().val, index));
    index++;
    return true;
}

bool Parser::real()
{
    if (get().type != Token::REAL) return false;

    cache.push_back(std::make_shared<Expression>(Expression::REAL, get().val, index));
    index++;
    return true;
}

bool Parser::boolean()
{
    if (get().type != Token::BOOLEAN) return false;

    cache.push_back(std::make_shared<Expression>(Expression::BOOL, get().val, index));
    index++;
    return true;
}

bool Parser::block()
{
    uint orig = index;
    if (!lit("do")) return false;

    // checking for parameters
    std::string params = "";
    if (lit("$"))
    {
        while (name())
        {
            params += cache.back()->val;
            cache.pop_back();
            if (!lit(",")) break;
        }
        if (params == "") er->error(NO_PARAMETER);
    }

    // getting the expression block
    if (checkIndent(Indent::SAME)) er->error(NO_INDENT);
    uint cached = cache.size();
    if (checkIndent(Indent::MORE))
    {
        while (expr()) if (!checkIndent(Indent::SAME)) break;
    }
    else if (!expr()) er->error(NOT_AN_EXPRESSION);

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
    uint orig = index;
    uint cached = cache.size();

    if (string() || integer() || real() || boolean() || block()) // single value
    {
        access();
        oper();
        return true;
    }
    else if (lit("(")) // tuple
    {
        checkIndent(Indent::MORE);
        while (true)
        {
            if (checkIndent(Indent::MORE)) er->error(UNEXPECTED_INDENT);
            checkIndent(Indent::SAME);
            std::string nam = "";
            if (name())
            {
                if (lit(":"))
                {
                    nam = cache.back()->val;
                    cache.pop_back();
                }
                else
                {
                    cache.pop_back();
                    --index;
                }

            }
            if (!expr()) er->error(NOTHING_TO_SET);

            ExprPtr tup = std::make_shared<Expression>(Expression::TUP, nam, orig);
            tup->left = cache.back();
            cache.pop_back();
            cache.push_back(tup);

            if (!lit(",")) break;
        }
        checkIndent(Indent::LESS);
        if (!lit(")")) er->error(NO_CLOSING_BRACE);

        // assemble tuple
        ExprPtr tup = cache[cached];
        for (uint i = cached; i < cache.size() - 1; i++)
            cache[i]->right = cache[i + 1];
        cache.resize(cached);
        cache.push_back(tup);

        access();
        oper();
        return true;
    }
    return false;
}

bool Parser::name()
{
    if (get().type != Token::NAME) return false;

    cache.push_back(std::make_shared<Expression>(Expression::NAME, get().val, index));
    ++index;
    return true;
}

bool Parser::lit(const std::string& t)
{
    if (get().val != t) return false;

    index++;
    return true;
}

OCA_END
