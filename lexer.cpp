#include "lexer.hpp"
#include "errors.hpp"

namespace oca::internal
{

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

unsigned int lexchar = 0;

bool isOf(char c, std::string str)
{
    for (char ch : str)
    {
        if (c == ch) return true;
    }
    return false;
}

std::string scanString(std::string script, unsigned int& index)
{
    std::string ret;

    char quote = script[index];
    // Put index after the first quote
    index++;

    bool end = false;
    while (!end)
    {
        char c = script[index];
        if (c == quote) end = true;
        else
        {
            ret += c;
            index++;
            lexchar++;
        }
    }

    return ret;
}

std::string scanNumber(std::string script, unsigned int& index)
{
    std::string ret;

    bool noDot = true;
    bool end = false;
    while (!end)
    {
        char c = script[index];
        if (isOf(c, NUMBERS))
        {
            ret += c;
            index++;
            lexchar++;
        }
        else if (c == '.' && isOf(script[index + 1], NUMBERS))
        {
            if (noDot)
            {
                ret += c;
                index++;
                noDot = false;
            }
            else
            {
                end = true;
                index--;
                lexchar--;
            }
        }
        else
        {
            end = true;
            index--;
            lexchar--;
        }
    }

    return ret;
}

std::string scanSymbol(std::string script, unsigned int& index)
{
    std::string ret;

    // First character cannot be a number
    ret += script[index];
    index++;
    lexchar++;

    bool end = false;
    while (!end)
    {
        char c = script[index];
        if (isOf(c, "_" LETTERS NUMBERS))
        {
            ret += c;
            index++;
            lexchar++;
        }
        else
        {
            end = true;
            index--;
            lexchar--;
        }
    }

    return ret;
}

void skipLine(const std::string& script, unsigned int& index)
{
    while (script[index] != '\n') index++;
    Error::lineNum++;
    lexchar = -1;
}

void newLine(std::vector<Token>& tokens)
{
    std::cout << lexchar;
    if (lexchar != 0) tokens.emplace_back(T_NEWLINE, "");
    Error::lineNum++;
    lexchar = -1;
}

void lex(const std::string& script, std::vector<Token>& tokens)
{
    Error::lineNum = 1;

    unsigned int index = 0;
    while (index < script.size())
    {
        char c = script[index];

        if (isOf(c, " \r")); // Skip space
        else if (c == '#') skipLine(script, index);
        else if (c == '\n') newLine(tokens);
        else if (c == '(') tokens.emplace_back(T_LPAREN, "");
        else if (c == ')') tokens.emplace_back(T_RPAREN, "");
        else if (c == '.') tokens.emplace_back(T_DOT, "");
        else if (c == ',') tokens.emplace_back(T_COMMA, "");
        // Types
        else if (isOf(c, "'\"")) tokens.emplace_back(T_STRING, scanString(script, index));
        else if (isOf(c, NUMBERS)) tokens.emplace_back(T_NUMBER, scanNumber(script, index));
        // Names
        else if (isOf(c, "+-*/%^=")) tokens.emplace_back(T_NAME, c);
        else if (isOf(c, "_" LETTERS)) tokens.emplace_back(T_NAME, scanSymbol(script, index));

        else ERR << "Unknown character " << lexchar << " \"" << script[index] << "\"";
        index++;
        lexchar++;
    }
}

} // namespace oca::internal
