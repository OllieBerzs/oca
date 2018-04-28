/* ollieberzs 2018
** parse.cpp
** parsing oca tokens into AST
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include "oca.hpp"

OCA_BEGIN

Expression::Expression(Expression::Type type, const std::string& val, uint index)
    : type(type), val(val), index(index) {}

void Expression::print(uint indent, char mod) {
    std::vector<std::string> typestrings = {
        "set",      "call",      "access", "if",     "else",  "next", "main",
        "branches", "part oper", "oper",   "return", "break", "file", "str",
        "int",      "real",      "bool",   "block",  "tup",   "name", "calls"};

    for (uint i = 0; i < indent; i++)
        std::cout << "  ";

    std::cout << mod << "<" << typestrings[type] << ">" << val << "\n";

    if (left)
        left->print(indent + 1, 'L');
    if (right)
        right->print(indent + 1, 'R');
}

// ----------------------------

std::vector<ExprPtr> Parser::makeAST(const std::vector<Token>& tokens) {
    index = 0;
    indent = 0;

    this->tokens = &tokens;
    while (checkIndent(Indent::SAME))
        ;
    std::vector<ExprPtr> ast;
    while (index < tokens.size() - 1) {
        if (expr()) {
            ast.push_back(cache.back());
            cache.pop_back();
        } else
            throw Error(NOT_AN_EXPRESSION);

        if (get().type == Token::LAST)
            break;
        if (checkIndent(Indent::MORE))
            throw Error(UNEXPECTED_INDENT);
        if (!checkIndent(Indent::SAME) && !checkIndent(Indent::LESS))
            throw Error(NO_NEWLINE);
    }

    return ast;
}

// ----------------------------

const Token& Parser::get() {
    if (index < tokens->size() - 1)
        return tokens->at(index);
    else
        return tokens->back();
}

bool Parser::checkIndent(Indent ind) {
    uint size = get().val.size() - 1;

    if (get().type != Token::INDENT)
        return false;

    if (ind == Indent::LESS)
        if (size >= indent)
            return false;

    if (ind == Indent::SAME)
        if (size > indent || size < indent)
            return false;

    if (ind == Indent::MORE)
        if (size <= indent)
            return false;

    indent = size;
    ++index;
    return true;
}

ExprPtr Parser::uncache() {
    ExprPtr result = cache.back();
    cache.pop_back();
    return result;
}

// ----------------------------

bool Parser::expr() {
    if (set() || call() || value() || block() || cond() || keyword() || file())
        return true;
    return false;
}

bool Parser::set() {
    uint orig = index;

    if (!call())
        return false;

    if (!lit("=")) {
        uncache();
        index = orig;
        return false;
    }

    if (!call() && !value() && !block() && !file() && !cond())
        throw Error(NOTHING_TO_SET);

    // assemble assignment
    ExprPtr expr = std::make_shared<Expression>(Expression::SET, "", orig);
    expr->right = uncache();
    expr->left = uncache();
    cache.push_back(expr);

    return true;
}

bool Parser::call(bool inDot) {
    uint orig = index;

    if (!name())
        return false;

    bool hasArg = value() || call() || file();
    bool hasYield = block();

    // assemble call
    ExprPtr yield = (hasYield) ? uncache() : nullptr;
    ExprPtr arg = (hasArg) ? uncache() : nullptr;

    ExprPtr c = std::make_shared<Expression>(Expression::CALL, uncache()->val, orig);
    c->left = yield;
    c->right = arg;
    cache.push_back(c);

    // check for accessor
    if (!inDot)
        access();

    if (cache.size() == 1 && lit(",")) {
        uint origc = index;
        if (!call())
            throw Error(NO_NAME);
        ExprPtr calls = std::make_shared<Expression>(Expression::CALLS, "", origc);
        calls->right = uncache();
        calls->left = uncache();
        cache.push_back(calls);
    }

    if (!inDot)
        oper();

    return true;
}

bool Parser::access() {
    uint orig = index;
    if (!lit("."))
        return false;
    // pass true, so the next call doesn't parse access
    if (!call(true) && !integer())
        throw Error(NO_ACCESS_KEY);

    // assemble dot access
    ExprPtr a = std::make_shared<Expression>(Expression::ACCESS, "", orig);
    a->right = uncache();
    a->left = uncache();
    cache.push_back(a);

    access();

    return true;
}

bool Parser::cond() {
    uint orig = index;
    if (!lit("if"))
        return false;

    if (!set() && !call() && !value())
        throw Error(NO_CONDITIONAL);
    if (!lit("then"))
        throw Error(NO_THEN);

    uint origt = index;
    uint cached = cache.size();
    if (checkIndent(Indent::MORE)) {
        while (expr()) {
            if (!checkIndent(Indent::SAME))
                break;
        }
    } else if (!expr())
        throw Error(NOT_AN_EXPRESSION);

    uint elseCached = cache.size();
    bool hasElse = false;
    uint orige = index;
    bool preelseIndent = checkIndent(Indent::SAME);
    if (lit("else")) {
        hasElse = true;
        if (checkIndent(Indent::MORE)) {
            while (expr()) {
                if (!checkIndent(Indent::SAME))
                    break;
            }
        } else if (!expr())
            throw Error(NOT_AN_EXPRESSION);
    } else if (preelseIndent)
        --index;

    // assemble conditional
    ExprPtr els = std::make_shared<Expression>(Expression::ELSE, "", orige);
    ExprPtr curr = els;
    for (uint i = elseCached; i < cache.size(); ++i) {
        curr->left = cache[i];
        if (i < cache.size() - 1) {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "", orig);
            curr = curr->right;
        }
    }
    cache.resize(elseCached);

    ExprPtr mn = std::make_shared<Expression>(Expression::MAIN, "", origt);
    curr = mn;
    for (uint i = cached; i < cache.size(); ++i) {
        curr->left = cache[i];
        if (i < cache.size() - 1) {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "", orig);
            curr = curr->right;
        }
    }
    cache.resize(cached);

    ExprPtr ifer = std::make_shared<Expression>(Expression::IF, "", orig);
    ifer->left = uncache(); // condition
    ExprPtr branches = std::make_shared<Expression>(Expression::BRANCHES, "", orig);
    branches->left = mn;
    if (hasElse)
        branches->right = els;
    ifer->right = branches;
    cache.push_back(ifer);

    return true;
}

bool Parser::oper() {
    uint cached = cache.size();

    if (get().type != Token::OPERATOR)
        return false;
    cache.push_back(std::make_shared<Expression>(Expression::PART_OPER, get().val, index));
    ++index;

    if (!value() && !call())
        throw Error(NO_RIGHT_VALUE);
    oper();

    if (!(cached < 2) || (cache[cached - 2]->type != Expression::PART_OPER))
        return true;

    // assemble operator
    for (int p = 3; p >= 0; p--) {
        for (auto it = cache.begin() + cached - 1; it != cache.end(); ++it) {
            if ((*it)->type != Expression::PART_OPER)
                continue;
            uint origp = (*it)->index;

            int priority = 1;
            char op = (*it)->val[0];
            if (op == '=' || op == '<' || op == '>')
                priority = 0;
            else if (op == '*' || op == '/' || op == '%')
                priority = 2;
            else if (op == '^')
                priority = 3;

            if (priority != p)
                continue;

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

bool Parser::keyword() {
    if (get().val == "return") {
        ExprPtr r = std::make_shared<Expression>(Expression::RETURN, "", index);
        ++index;
        if (expr())
            r->right = uncache();
        cache.push_back(r);
        return true;
    } else if (get().val == "break") {
        cache.push_back(std::make_shared<Expression>(Expression::BREAK, "", index));
        ++index;
        return true;
    }
    return false;
}

bool Parser::file() {
    if (get().type != Token::FILEPATH)
        return false;

    cache.push_back(std::make_shared<Expression>(Expression::FILE, get().val.substr(1), index));
    ++index;
    return true;
}

// ----------------------------

bool Parser::string() {
    if (get().type != Token::STRING)
        return false;

    std::string s = get().val.substr(1, get().val.size() - 2);
    cache.push_back(std::make_shared<Expression>(Expression::STR, s, index));
    ++index;
    return true;
}

bool Parser::integer() {
    if (get().type == Token::BINNUM) {
        int num = 0;
        std::string bin = get().val;
        for (uint i = bin.size() - 1; i > 1; --i) {
            if (bin[i] == '1')
                num += std::pow(2, bin.size() - i - 1);
        }
        cache.push_back(std::make_shared<Expression>(Expression::INT, std::to_string(num), index));
        ++index;
        return true;
    }

    if (get().type == Token::HEXNUM) {
        int num = std::stoi(get().val, 0, 16);
        cache.push_back(std::make_shared<Expression>(Expression::INT, std::to_string(num), index));
        ++index;
        return true;
    }

    bool minus = false;
    if (get().val == "-") {
        const Token& prev = (index == 0) ? tokens->back() : tokens->at(index - 1);
        if (prev.type == Token::INDENT || prev.val == "(" || prev.val == ":" || prev.val == "=") {
            ++index;
            minus = true;
        }
    }
    if (get().type != Token::INTEGER) {
        if (minus)
            --index;
        return false;
    }

    std::string val = minus ? "-" + get().val : get().val;
    cache.push_back(std::make_shared<Expression>(Expression::INT, val, index));
    ++index;
    return true;
}

bool Parser::real() {
    bool minus = false;
    if (get().val == "-") {
        const Token& prev = (index == 0) ? tokens->back() : tokens->at(index - 1);
        if (prev.type == Token::INDENT || prev.val == "(" || prev.val == ":" || prev.val == "=") {
            ++index;
            minus = true;
        }
    }

    if (get().type == Token::SCIENTNUM) {
        auto e = get().val.find("e");
        if (e == std::string::npos)
            e = get().val.find("E");
        float base = std::stof(get().val.substr(0, e));
        float power = std::stof(get().val.substr(e + 1, get().val.size() - 1));
        std::string num = std::to_string(base * std::pow(10, power));

        std::string val = minus ? "-" + num : num;
        cache.push_back(std::make_shared<Expression>(Expression::REAL, val, index));
        ++index;
        return true;
    }

    if (get().type != Token::REAL) {
        if (minus)
            --index;
        return false;
    }

    std::string val = minus ? "-" + get().val : get().val;
    cache.push_back(std::make_shared<Expression>(Expression::REAL, val, index));
    ++index;
    return true;
}

bool Parser::boolean() {
    if (get().type != Token::BOOLEAN)
        return false;

    cache.push_back(std::make_shared<Expression>(Expression::BOOL, get().val, index));
    ++index;
    return true;
}

bool Parser::block() {
    uint orig = index;
    if (!lit("do"))
        return false;

    std::string params = "";
    if (lit("with")) {
        while (name()) {
            params += uncache()->val + " ";
            if (!lit(","))
                break;
        }
        params.pop_back();
        if (params == "")
            throw Error(NO_PARAMETER);
    }

    if (checkIndent(Indent::SAME))
        throw Error(NO_INDENT);
    uint cached = cache.size();
    if (checkIndent(Indent::MORE)) {
        while (expr())
            if (!checkIndent(Indent::SAME))
                break;
    } else if (!expr())
        throw Error(NOT_AN_EXPRESSION);

    // assemble block
    ExprPtr bl = std::make_shared<Expression>(Expression::BLOCK, params, orig);

    ExprPtr curr = bl;
    for (uint i = cached; i < cache.size(); ++i) {
        curr->left = cache[i];
        if (i < cache.size() - 1) {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "", orig);
            curr = curr->right;
        }
    }
    cache.resize(cached);

    cache.push_back(bl);

    return true;
}

// ----------------------------

bool Parser::value() {
    uint cached = cache.size();

    if (string() || integer() || real() || boolean()) {
        access();
        oper();
        return true;
    } else if (lit("(")) {
        checkIndent(Indent::MORE);
        while (true) {
            if (checkIndent(Indent::MORE))
                throw Error(UNEXPECTED_INDENT);
            checkIndent(Indent::SAME);
            uint origt = index;
            std::string nam = "";
            bool pub = lit("pub");
            if (name()) {
                if (lit(":"))
                    nam = (pub ? "pub " : "") + uncache()->val;
                else {
                    cache.pop_back();
                    --index;
                    if (pub)
                        --index;
                }
            }
            if (!expr())
                throw Error(NOTHING_TO_SET);

            ExprPtr tup = std::make_shared<Expression>(Expression::TUP, nam, origt);
            tup->left = uncache();
            cache.push_back(tup);

            if (!lit(","))
                break;
        }
        checkIndent(Indent::LESS);
        if (!lit(")"))
            throw Error(NO_CLOSING_BRACE);

        // assemble tuple
        ExprPtr tup = cache[cached];
        for (uint i = cached; i < cache.size() - 1; ++i)
            cache[i]->right = cache[i + 1];
        cache.resize(cached);
        cache.push_back(tup);

        access();
        oper();
        return true;
    }
    return false;
}

bool Parser::name() {
    if (get().type != Token::NAME)
        return false;

    cache.push_back(std::make_shared<Expression>(Expression::NAME, get().val, index));
    ++index;
    return true;
}

bool Parser::lit(const std::string& t) {
    if (get().val != t)
        return false;
    ++index;
    return true;
}

OCA_END
