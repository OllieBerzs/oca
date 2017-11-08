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
                ERR << "No newline after expression on line " << line << "\n- last token " << tokens[i];
            }
        }
        else
        {
            ERR << "Invalid syntax on line " << line << "\n- last token " << tokens[i];
        }
        line++;
    }
}

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (call(out, i, tokens) || string(out, i, tokens) || number(out, i, tokens))
    {
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

    bool hasParen = true;
    if (tokens[i].type != T_LPAREN)
    {
        i--;
        hasParen = false;
    }
    i++;

    Expression* a = nullptr;
    if (!args(a, i, tokens))
    {
        i = orig;
        return false;
    }

    if (tokens[i].type != T_RPAREN && hasParen)
    {
        i = orig;
        return false;
    }
    else if (!hasParen)
    {
        i--;
    }
    i++;

    // Check if has dot after, if does attach another method
    Expression* dot = nullptr;
    if (tokens[i].type == T_DOT)
    {
        i++;
        if (!call(dot, i, tokens))
        {
            i = orig;
            return false;
        }
    }

    // Check for arithmetic methods
    if (tokens[i].type == T_NAME &&
        (tokens[i].value == "+"
        || tokens[i].value == "-"
        || tokens[i].value == "*"
        || tokens[i].value == "/"))
    {
        if (!call(dot, i, tokens))
        {
            i = orig;
            return false;
        }
    }

    out = new Expression(E_CALL, name, dot, a);
    return true;
}

bool string(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (tokens[i].type != T_STRING)
    {
        i = orig;
        return false;
    }
    std::string value = tokens[i].value;
    i++;

    // Check if has dot after, if does attach another method
    Expression* dot = nullptr;
    if (tokens[i].type == T_DOT)
    {
        i++;
        if (!call(dot, i, tokens))
        {
            i = orig;
            return false;
        }
    }

    // Check for arithmetic methods
    if (tokens[i].type == T_NAME &&
        (tokens[i].value == "+"
        || tokens[i].value == "-"
        || tokens[i].value == "*"
        || tokens[i].value == "/"))
    {
        if (!call(dot, i, tokens))
        {
            i = orig;
            return false;
        }
    }

    out = new Expression(E_STRING, value, dot, nullptr);
    return true;
}

bool number(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (tokens[i].type != T_NUMBER)
    {
        i = orig;
        return false;
    }
    std::string value = tokens[i].value;
    i++;

    // Check if has dot after, if does attach another method
    Expression* dot = nullptr;
    if (tokens[i].type == T_DOT)
    {
        i++;
        if (!call(dot, i, tokens))
        {
            i = orig;
            return false;
        }
    }

    // Check for arithmetic methods
    if (tokens[i].type == T_NAME &&
        (tokens[i].value == "+"
        || tokens[i].value == "-"
        || tokens[i].value == "*"
        || tokens[i].value == "/"))
    {
        if (!call(dot, i, tokens))
        {
            i = orig;
            return false;
        }
    }

    out = new Expression(E_NUMBER, value, dot, nullptr);
    return true;
}

bool args(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    Expression* arg = nullptr;
    if (expr(arg, i, tokens))
    {
        // check for more arguments if followed by comma
        Expression* anotherArg = nullptr;
        if (tokens[i].type == T_COMMA)
        {
            i++;
            if (!args(anotherArg, i, tokens))
            {
                i = orig;
                return false;
            }
        }
        arg->other = anotherArg;
    }

    out = arg;
    return true;
}
