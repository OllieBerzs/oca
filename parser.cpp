#include "parser.hpp"
#include "errors.hpp"

namespace oca::internal
{

void parse(const std::vector<Token>& tokens, std::vector<Expression*>& expressions)
{
    Error::lineNum = 1;

    unsigned int i = 0;
    while (i < tokens.size())
    {
        // skip newlines
        while (tokens[i].type == T_NEWLINE && i < tokens.size())
        {
            i++;
            Error::lineNum++;
        }
        if (i == tokens.size()) break;

        Expression* e = nullptr;
        if (expr(e, i, tokens)) expressions.push_back(e);
        else ERR << "Invalid syntax";
    }
}

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (!call(out, i, tokens) && !string(out, i, tokens) && !number(out, i, tokens))
    {
        i = orig;
        return false;
    }

    //TODO: add support for parentheses
    // Check for attachables
    Expression* attach = nullptr;
    attachment(attach, i, tokens);
    out->left = attach;

    return true;
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

    out = new Expression(E_CALL, name, nullptr, a);
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

    out = new Expression(E_STRING, value);
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

    out = new Expression(E_NUMBER, value);
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

bool attachment(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (tokens[i].type == T_DOT)
    {
        i++;
        if (call(out, i, tokens))
        {
            return true;
        }
    }

    // Check for arithmetic methods
    if (tokens[i].type == T_NAME &&
        (tokens[i].value == "+"
        || tokens[i].value == "-"
        || tokens[i].value == "*"
        || tokens[i].value == "/"
        || tokens[i].value == "%"
        || tokens[i].value == "^"))
    {
        if (call(out, i, tokens))
        {
            return true;
        }
    }

    i = orig;
    return false;
}

} // namespace oca::internal
