#include "parser.hpp"
#include "errors.hpp"

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
    unsigned int i = 0;
    unsigned int line = 1;
    while (i < tokens.size())
    {
        Expression* e = nullptr;
        if (expr(e, i, tokens))
        {
            expressions.push_back(e);
            if (tokens[i].type == T_NEWLINE)
            {
                i++;
            }
            else
            {
                ERR << "No newline after expression on line " << line;
            }
        }
        else
        {
            ERR << "Invalid syntax on line " << line;
        }
        line++;
    }
}

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (call(out, i, tokens))
    {
        return true;
    }

    if (tokens[i].type == T_STRING)
    {
        out = new Expression(E_STRING, tokens[i].value);
        i++;
        return true;
    }

    if (tokens[i].type == T_NUMBER)
    {
        out = new Expression(E_NUMBER, tokens[i].value);
        i++;
        return true;
    }

    i = orig;
    return false;
}

bool call(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (tokens[i].type != T_NAME)
    {
        i = orig;
        return false;
    }
    std::string name = tokens[i].value;
    i++;

    if (tokens[i].type != T_LPAREN)
    {
        i = orig;
        return false;
    }
    i++;

    Expression* a = nullptr;
    if (!args(a, i, tokens))
    {
        i = orig;
        return false;
    }

    if (tokens[i].type != T_RPAREN)
    {
        i = orig;
        return false;
    }
    i++;

    out = new Expression(E_CALL, name, nullptr, a);
    return true;
}

bool args(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    Expression* e = nullptr;
    if (!expr(e, i, tokens))
    {
        i = orig;
        return false;
    }

    out = e;
    return true;
}
