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
    for (uint i = 0; i < indent; i++) std::cout << "|   ";

    std::cout << "<" + type << ">" << val << "\n";

    if (left) left->print(indent + 1);
    if (right) right->print(indent + 1);
}

// ----------------------------

Parser::Parser(std::vector<Token>& ts, const std::string& path) 
    : path(path), index(0)
{
    tokens = std::move(ts);
}

// ----------------------------

const Token& Parser::get()
{
    if (index < tokens.size() - 1) return tokens.at(index);
    else return tokens.back();
}

// ----------------------------

std::vector<ExprPtr> Parser::parse()
{
    std::vector<ExprPtr> result;
    while (index < tokens.size() - 1)
    {
        if (expr()) 
        {
            result.push_back(cache.back());
            cache.pop_back();
        }
        else error("Not an expression");
    }
    return std::move(result);
}

// ----------------------------

bool Parser::expr()
{
    if (block() || call() || def() || value()) return true;
    return false;
}

bool Parser::call()
{
    if (!name()) return false;

    bool hasArg = false;
    if (lit(":"))
    {
        if (!expr()) error("No argument provided for function call with ':'");
        hasArg = true;
    }
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
        if (!integer() && !call()) error("No function call after '.'");
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

bool Parser::oper()
{
    uint cached = cache.size();

    if (get().type != "OPERATOR") return false;
    bool first = (cached < 2) || (cache[cached - 2]->type != "operator"); 
    
    cache.push_back(std::make_shared<Expression>("operator", get().val));
    index++;
    if (!expr()) error("Missing expression after operator");
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

    bool hasParam = false;
    if (lit(":"))
    {
        if (!name()) error("No name provided for parameter after ':'");
        hasParam = true;
    }

    uint cached = cache.size();

    uint els = 0;
    uint counter = 0;

    while (!lit("end"))
    {
        if (lit("else")) els = counter;
        if (!expr()) error("Not an expression");
        counter++;
    }

    // assemble block
    ExprPtr bl = std::make_shared<Expression>("block", "");
    ExprPtr mainBody = std::make_shared<Expression>("main", "");
    ExprPtr elseBody = std::make_shared<Expression>("else", "");

    if (els > 0)
    {
        ExprPtr curr = elseBody;
        for (uint i = cached + els; i < cache.size(); i++)
        {
            curr->left = cache[i];
            curr->right = std::make_shared<Expression>("next", "");
            curr = curr->right;
        }
        cache.resize(cached + els);
    }
    ExprPtr curr = mainBody;
    for (uint i = cached; i < cache.size(); i++)
    {
        curr->left = cache[i];
        curr->right = std::make_shared<Expression>("next", "");
        curr = curr->right;
    }
    cache.resize(cached);

    if (hasParam)
    {
        mainBody->left = cache.back();
        cache.pop_back();
    }

    bl->left = mainBody;
    bl->right = elseBody;
    cache.push_back(bl);

    return true;
}

bool Parser::def()
{
    if (lit("def"))
    {
        if (get().type != "OPERATOR")
        {
            if (!name()) error("No name provided for definition");
        }
        else
        {
            cache.push_back(std::make_shared<Expression>("name", get().val));
            index++;
        }
        if (!expr()) error("No expression provided for definition");
    }
    else return false;

    // assemble definition
    ExprPtr d = std::make_shared<Expression>("def", "");
    d->right = cache.back();
    cache.pop_back();
    d->val = cache.back()->val;
    cache.pop_back();
    cache.push_back(d);
    
    return true;
}

// ----------------------------

bool Parser::string()
{
    if (get().type != "STRING") return false;

    cache.push_back(std::make_shared<Expression>("str", get().val));
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
        while (true)
        {
            std::string nam = "";
            if (lit("[") && name())
            {
                if (!lit("]")) error("Missing closing brace");
                nam = cache.back()->val;
                cache.pop_back();
            }
            if (!expr()) break;

            ExprPtr tup = std::make_shared<Expression>("tup", nam);
            tup->left = cache.back();
            cache.pop_back();
            cache.push_back(tup);
        }
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
    
    cache.push_back(std::make_shared<Expression>("name", get().val));
    index++;
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
