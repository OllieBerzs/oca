#include "parser.hpp"
#include "errors.hpp"

namespace oca::internal
{

unsigned int errorToken = 0;

bool isArithmetic(const std::string& name)
{
    return name == "+" || name == "-" || name == "*" || name == "/" || name == "%" || name == "^";
}

bool unmatch(unsigned int& i, unsigned int orig)
{
    errorToken = i;
    i = orig;
    return false;
}

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
        else ERR << "Invalid syntax " << tokens[errorToken];
    }
}

bool expr(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    Expression* content = nullptr;
    if (tokens[i].type == T_LPAREN)
    {
        i++;
        if (!expr(content, i, tokens))
        {
            return unmatch(i, orig);
        }
        if (tokens[i].type != T_RPAREN)
        {
            return unmatch(i, orig);
        }
        i++;
        out = new Expression(E_CASE, "");
    }
    else if (!call(out, i, tokens) && !string(out, i, tokens) && !number(out, i, tokens))
    {
        return unmatch(i, orig);
    }

    //TODO: add support for parentheses
    // Check for attachables
    Expression* attach = nullptr;
    attachment(attach, i, tokens);
    out->attachment = attach;
    out->content = content;

    return true;
}

bool call(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (tokens[i].type != T_NAME)
    {
        return unmatch(i, orig);
    }
    std::string name = tokens[i].value;
    i++;

    // check for assignment
    if (tokens[i].value == "=")
    {
        name += "=";
        i++;
    }

    bool hasParen = true;
    if (tokens[i].type != T_LPAREN)
    {
        i--;
        hasParen = false;
    }
    i++;

    Expression* arg = nullptr;
    if (!args(arg, i, tokens))
    {
        return unmatch(i, orig);
    }

    if (tokens[i].type != T_RPAREN && hasParen)
    {
        return unmatch(i, orig);
    }
    else if (!hasParen)
    {
        i--;
    }
    i++;

    out = new Expression(E_CALL, name);
    out->argument = arg;
    return true;
}

bool string(Expression*& out, unsigned int& i, const std::vector<Token>& tokens)
{
    unsigned int orig = i;

    if (tokens[i].type != T_STRING)
    {
        return unmatch(i, orig);
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
        return unmatch(i, orig);
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
    if (isArithmetic(tokens[i].value)); //skip
    else if (expr(arg, i, tokens))
    {
        // check for more arguments if followed by comma
        Expression* anotherArg = nullptr;
        if (tokens[i].type == T_COMMA)
        {
            i++;
            if (!args(anotherArg, i, tokens))
            {
                return unmatch(i, orig);
            }
        }
        arg->next = anotherArg;
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
    if (tokens[i].type == T_NAME && isArithmetic(tokens[i].value))
    {
        if (call(out, i, tokens))
        {
            return true;
        }
    }

    return unmatch(i, orig);
}

} // namespace oca::internal
