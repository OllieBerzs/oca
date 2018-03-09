/* ollieberzs 2018
** parse.cpp
** parsing oca tokens into AST
*/

#include <iostream>
#include <fstream>
#include "parse.hpp"
#include "lex.hpp"

OCA_BEGIN

Expression::Expression(const std::string& type, const std::string& val)
    : type(type), val(val) {}

void Expression::print(uint indent)
{
    for (uint i = 0; i < indent; i++) std::cout << "  ";

    std::cout << "<" + type << ">" << val << "\n";

    if (left) left->print(indent + 1);
    if (right) right->print(indent + 1);
}

// ----------------------------

Parser::Parser(std::vector<Token>& ts, const std::string& path)
    : path(path), index(0), indent(0)
{
    tokens = std::move(ts);
}

// ----------------------------

Token& Parser::get()
{
    if (index < tokens.size() - 1) return tokens.at(index);
    else return tokens.back();
}

bool Parser::checkIndent(Indent ind)
{
    if (get().type != "INDENT") return false;

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
        if (get().type == "LAST") break;
        if (checkIndent(Indent::MORE)) error("Unexpected indent");
        if (!checkIndent(Indent::SAME) && !checkIndent(Indent::LESS)) error("Expected a newline at end of expression");
    }
    return result;
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
    if (!set() && !block() && !call() &&
        !value() && !file() && !cond()) error("Expected value after '='");

    // assemble assignment
    ExprPtr s = std::make_shared<Expression>("set", "");
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

    ExprPtr c = std::make_shared<Expression>("call", cache.back()->val);
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
    if (lit("."))
    {
        if (!integer() && !call()) error("No accessor call after '.'");
    }
    else return false;

    // assemble attachment
    ExprPtr next = cache.back();
    cache.pop_back();
    ExprPtr prev = cache.back();
    cache.pop_back();
    ExprPtr a = std::make_shared<Expression>("access", "");
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
    ExprPtr els = std::make_shared<Expression>("else", "");

    ExprPtr curr = els;
    for (uint i = elseCached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>("next", "");
            curr = curr->right;
        }
    }
    cache.resize(elseCached);

    ExprPtr mn = std::make_shared<Expression>("main", "");

    curr = mn;
    for (uint i = cached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>("next", "");
            curr = curr->right;
        }
    }
    cache.resize(cached);

    ExprPtr ifer = std::make_shared<Expression>("if", "");
    ifer->left = cache.back(); // condition
    cache.pop_back();
    ExprPtr branches = std::make_shared<Expression>("branches", "");
    branches->left = mn;
    if (hasElse) branches->right = els;
    ifer->right = branches;
    cache.push_back(ifer);

    return true;
}

bool Parser::oper()
{
    uint cached = cache.size();

    if (get().type != "OPERATOR") return false;
    bool first = (cached < 2) || (cache[cached - 2]->type != "operator");

    cache.push_back(std::make_shared<Expression>("operator", get().val));
    index++;
    if (!value() && !call()) error("Missing value after operator");
    oper();

    if (!first) return true;

    // assemble operator
    for (int p = 3; p >= 0; p--)
    {
        for (auto it = cache.begin() + cached - 1; it != cache.end(); it++)
        {
            if ((*it)->type != "operator") continue;

            // set priority
            int priority = 1;
            char op = (*it)->val[0];
            if (op == '=' || op == '<' || op == '>') priority = 0;
            else if (op == '*' || op == '/' || op == '%') priority = 2;
            else if (op == '^') priority = 3;

            if (priority != p) continue;

            ExprPtr o = std::make_shared<Expression>("oper", (*it)->val);
            o->left = *(it - 1);
            o->right = *(it + 1);
            cache.erase(it - 1, it + 2);
            cache.insert(it - 1, o);
            it--;
        }
    }
    return true;
}

bool Parser::block()
{
    if (!lit("do")) return false;

    uint cached = cache.size();

    bool hasParam = false;
    if (lit(":"))
    {
        if (!name()) error("No name provided for parameter after ':'");
        hasParam = true;
        if (!checkIndent(Indent::MORE)) error("Expected indentation at the start of a block");

        cached = cache.size();
        while (expr())
        {
            if (!checkIndent(Indent::SAME)) break;
        }
    }
    else
    {
        if (checkIndent(Indent::SAME)) error("Expected indented block");
        checkIndent(Indent::MORE);
        cached = cache.size();
        if (!expr()) error("Expected expression for block");
    }

    // assemble block
    ExprPtr bl = std::make_shared<Expression>("block", "");

    ExprPtr curr = bl;
    for (uint i = cached; i < cache.size(); ++i)
    {
        curr->left = cache[i];
        if (i < cache.size() - 1)
        {
            curr->right = std::make_shared<Expression>("next", "");
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

bool Parser::keyword()
{
    if (get().val == "return")
    {
        index++;
        ExprPtr r = std::make_shared<Expression>("return", "");
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
        cache.push_back(std::make_shared<Expression>("break", ""));
        return true;
    }
    return false;
}

bool Parser::file()
{
    if (get().type != "FILEPATH") return false;

    cache.push_back(std::make_shared<Expression>("file", get().val.substr(1)));
    index++;
    return true;
}

// ----------------------------

bool Parser::string()
{
    if (get().type != "STRING") return false;

    std::string val = get().val;
    cache.push_back(std::make_shared<Expression>("str", val.substr(1, val.size() - 2)));
    index++;
    return true;
}

bool Parser::integer()
{
    if (get().type != "INTEGER") return false;

    cache.push_back(std::make_shared<Expression>("int", get().val));
    index++;
    return true;
}

bool Parser::floatnum()
{
    if (get().type != "FLOAT") return false;

    cache.push_back(std::make_shared<Expression>("float", get().val));
    index++;
    return true;
}

bool Parser::boolean()
{
    if (get().type != "BOOLEAN") return false;

    cache.push_back(std::make_shared<Expression>("bool", get().val));
    index++;
    return true;
}

// ----------------------------

bool Parser::value()
{
    uint cached = cache.size();

    if (string() || integer() || floatnum() || boolean()) // single value
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

            ExprPtr tup = std::make_shared<Expression>("tup", nam);
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
    if (get().type != "NAME") return false;
    std::string nam = "";

    bool trailComma = false;
    while (get().type == "NAME")
    {
        trailComma = false;
        nam += get().val + " ";
        ++index;
        if (!lit(",")) break;
        trailComma = true;
    }
    if (trailComma) --index;
    cache.push_back(std::make_shared<Expression>("name", nam));
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
    std::cout << (t.val == "" ? t.type : t.val);
    std::cout << "\033[0m";
    std::cout << "\033[38;5;15m";
    std::cout << lineEnd << "\n";
    std::cout << "\033[0m";
    std::cout << message << "\n";

    std::cin.get();
    exit(1);
}

OCA_END
