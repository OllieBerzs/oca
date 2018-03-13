/* ollieberzs 2018
** parse.cpp
** parsing oca tokens into AST
*/

#include <iostream>
#include <fstream>
#include "parse.hpp"
#include "lex.hpp"

OCA_BEGIN

Expression::Expression(Expression::Type type, const std::string& val)
    : type(type), val(val) {}

void Expression::print(uint indent, char mod)
{
    std::vector<std::string> types =
    {
        "set", "call", "access", "if", "else", "next", "main",
        "branches", "part oper", "oper", "return", "break", "file", "str",
        "int", "real", "bool", "block", "tup", "name"
    };

    for (uint i = 0; i < indent; i++) std::cout << "  ";

    std::cout << mod << "<" << types[type] << ">" << val << "\n";

    if (left) left->print(indent + 1, 'L');
    if (right) right->print(indent + 1, 'R');
}

// ----------------------------

Parser::Parser(std::vector<Token>& ts, const std::string& path)
    : path(path), index(0), indent(0)
{
    tokens = std::move(ts);
}

std::vector<ExprPtr> Parser::parse()
{
    std::vector<ExprPtr> result;
    while (index < tokens.size() - 1)
    {
        //while (checkIndent(Indent::ANY)) {}
        if (expr())
        {
            result.push_back(cache.back());
            cache.pop_back();
        }
        else error("Not an expression");
        if (get().type == Token::LAST) break;
        if (checkIndent(Indent::MORE)) error("Unexpected indent");
        if (!checkIndent(Indent::SAME) && !checkIndent(Indent::LESS)) error("Expected a newline at end of expression");
    }
    return result;
}

// ----------------------------

Token& Parser::get()
{
    if (index < tokens.size() - 1) return tokens.at(index);
    else return tokens.back();
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
    if (set() || call() || value() || cond() || keyword() || file()) return true;
    return false;
}

bool Parser::set()
{
    if (!name()) return false;

    if (!lit("="))
    {
        cache.pop_back();
        --index;
        return false;
    }
    if (!set() && !call() &&
        !value() && !file() && !cond()) error("Expected value after '='");

    // assemble assignment
    ExprPtr s = std::make_shared<Expression>(Expression::SET, "");
    s->right = cache.back();
    cache.pop_back();
    s->val = cache.back()->val;
    cache.pop_back();
    cache.push_back(s);

    return true;
}

bool Parser::call()
{
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

    ExprPtr c = std::make_shared<Expression>(Expression::CALL, cache.back()->val);
    cache.pop_back();
    c->left = bl;
    c->right = arg;
    cache.push_back(c);

    access();
    oper();
    return true;
}

bool Parser::access()
{
    std::string mod = "";
    if (lit("."))
    {
        if (!integer() && !call()) error("No accessor call after '.'");
    }
    else if (lit("["))
    {
        mod = "[";
        if (!integer() && !string() && !call()) error("No key after '['");
        if (!lit("]")) error("Missing closing brace");
    }
    else return false;

    // assemble attachment
    ExprPtr next = cache.back();
    cache.pop_back();
    ExprPtr prev = cache.back();
    cache.pop_back();
    ExprPtr a = std::make_shared<Expression>(Expression::ACCESS, mod);
    a->left = prev;
    a->right = next;
    cache.push_back(a);
    return true;
}

bool Parser::cond()
{
    if (!lit("if")) return false;

    if (!set() && !call() && !value()) error("Expected conditional expression");
    if (!lit("then")) error("Expected 'then' in an 'if' expression");

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
        if (!expr()) error("Expected expression in 'if' main branch");
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
            if (!expr()) error("Expected expression in 'if' else branch");
        }
    }

    // assemble conditional
    ExprPtr els = std::make_shared<Expression>(Expression::ELSE, "");

    ExprPtr curr = els;
    for (uint i = elseCached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "");
            curr = curr->right;
        }
    }
    cache.resize(elseCached);

    ExprPtr mn = std::make_shared<Expression>(Expression::MAIN, "");

    curr = mn;
    for (uint i = cached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "");
            curr = curr->right;
        }
    }
    cache.resize(cached);

    ExprPtr ifer = std::make_shared<Expression>(Expression::IF, "");
    ifer->left = cache.back(); // condition
    cache.pop_back();
    ExprPtr branches = std::make_shared<Expression>(Expression::BRANCHES, "");
    branches->left = mn;
    if (hasElse) branches->right = els;
    ifer->right = branches;
    cache.push_back(ifer);

    return true;
}

bool Parser::oper()
{
    uint cached = cache.size();

    if (get().type != Token::OPERATOR) return false;
    bool first = (cached < 2) || (cache[cached - 2]->type != Expression::PART_OPER);

    cache.push_back(std::make_shared<Expression>(Expression::PART_OPER, get().val));
    index++;
    if (!value() && !call()) error("Missing value after operator");
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

            ExprPtr o = std::make_shared<Expression>(Expression::OPER, (*it)->val);
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
        index++;
        ExprPtr r = std::make_shared<Expression>(Expression::RETURN, "");
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
        cache.push_back(std::make_shared<Expression>(Expression::BREAK, ""));
        return true;
    }
    return false;
}

bool Parser::file()
{
    if (get().type != Token::FILEPATH) return false;

    cache.push_back(std::make_shared<Expression>(Expression::FILE, get().val.substr(1)));
    index++;
    return true;
}

// ----------------------------

bool Parser::string()
{
    if (get().type != Token::STRING) return false;

    std::string val = get().val;
    cache.push_back(std::make_shared<Expression>(Expression::STR, val.substr(1, val.size() - 2)));
    index++;
    return true;
}

bool Parser::integer()
{
    if (get().type != Token::INTEGER) return false;

    cache.push_back(std::make_shared<Expression>(Expression::INT, get().val));
    index++;
    return true;
}

bool Parser::real()
{
    if (get().type != Token::REAL) return false;

    cache.push_back(std::make_shared<Expression>(Expression::REAL, get().val));
    index++;
    return true;
}

bool Parser::boolean()
{
    if (get().type != Token::BOOLEAN) return false;

    cache.push_back(std::make_shared<Expression>(Expression::BOOL, get().val));
    index++;
    return true;
}

bool Parser::block()
{
    if (!lit("do")) return false;

    bool hasParam = false;
    if (lit(":"))
    {
        if (!name()) error("No name provided for parameter after ':'");
        hasParam = true;
    }

    if (checkIndent(Indent::SAME)) error("Expected indented block");
    uint cached = cache.size();
    if (checkIndent(Indent::MORE))
    {
        while (expr()) if (!checkIndent(Indent::SAME)) break;
    }
    else if (!expr()) error("Expected expression for block");

    // assemble block
    ExprPtr bl = std::make_shared<Expression>(Expression::BLOCK, "");

    ExprPtr curr = bl;
    for (uint i = cached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>(Expression::NEXT, "");
            curr = curr->right;
        }
    }
    cache.resize(cached);

    if (hasParam)
    {
        bl->val = cache.back()->val;
        cache.pop_back();
    }
    cache.push_back(bl);

    return true;
}

// ----------------------------

bool Parser::value()
{
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
            if (checkIndent(Indent::MORE)) error("Unexpected indent as part of a tuple");
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
            if (!expr()) error("Expected expression as value");

            ExprPtr tup = std::make_shared<Expression>(Expression::TUP, nam);
            tup->left = cache.back();
            cache.pop_back();
            cache.push_back(tup);

            if (!lit(",")) break;
        }
        checkIndent(Indent::LESS);
        if (!lit(")")) error("Missing closing brace");

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
    std::string nam = "";

    bool trailComma = false;
    while (get().type == Token::NAME)
    {
        trailComma = false;
        nam += get().val + " ";
        ++index;
        if (!lit(",")) break;
        trailComma = true;
    }
    nam.pop_back();
    if (trailComma) --index;
    cache.push_back(std::make_shared<Expression>(Expression::NAME, nam));
    return true;
}

bool Parser::lit(const std::string& t)
{
    if (get().val != t) return false;

    index++;
    return true;
}

// ----------------------------

void Parser::error(const std::string& message)
{
    const Token& t = tokens[index];

    std::ifstream file(path);
    if (!file.is_open()) std::cout << "Could not open file " << path << "\n";
    std::string source((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    file.close();

    // get error line
    std::string errline = "";
    uint lineNum = 1;
    uint colNum = 1;
    uint index = 0;

    char c = 'a';
    bool found = false;

    while ((c = source[index]) != '\n' || !found)
    {
        if (c == '\n')
        {
            lineNum++;
            errline = "";
        }
        else errline += c;
        if (index == t.pos)
        {
            found = true;
            colNum = errline.size() - 1;
        }
        index++;
    }

    std::string lineStart = errline.substr(0, colNum);
    std::string lineEnd = colNum + t.val.size() < errline.size()
        ? errline.substr(colNum + t.val.size(), errline.size()) : "";

    system("printf '\033[1A'");
    std::cout << "\033[38;5;14m";
    std::cout << "-- ERROR -------------------- " << path << "\n";
    std::cout << "\033[0m";
    std::cout << lineNum << "| ";
    std::cout << "\033[38;5;15m";
    std::cout << lineStart;
    std::cout << "\033[48;5;9m";
    std::cout << t.val;
    std::cout << "\033[0m";
    std::cout << "\033[38;5;15m";
    std::cout << lineEnd << "\n";
    std::cout << "\033[0m";
    std::cout << message << "\n";

    std::cin.get();
    exit(1);
}

OCA_END
