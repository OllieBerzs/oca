#include "lexer.hpp"
#include "errors.hpp"

namespace oca::internal
{

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

unsigned int lexchar = 0;

bool isOf(char c, const std::string& str)
{
    for (char ch : str)
    {
        if (c == ch) return true;
    }
    return false;
}

std::string scanString(const std::string& script, unsigned int& index)
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

std::string scanNumber(const std::string& script, unsigned int& index)
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

std::string scanBool(const std::string& script, unsigned int& index, bool clear)
{
    unsigned int orig = index;
    std::string ret = "";

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

    if (ret != "true" && ret != "false")
    {
        ret = "";
        index = orig;
        lexchar = orig;
    }
    if (clear)
    {
        index = orig;
        lexchar = orig;
    }

    return ret;
}

std::string scanSymbol(const std::string& script, unsigned int& index)
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
    index--;
    lexchar = -1;
}

void newLine(std::vector<Token>& tokens)
{
    tokens.emplace_back(T_NEWLINE, "");
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
        else if (scanBool(script, index, true) != "") tokens.emplace_back(T_BOOL, scanBool(script, index, false));
        // Names
        else if (isOf(c, "+-*/%^=")) tokens.emplace_back(T_NAME, c);
        else if (isOf(c, "_" LETTERS)) tokens.emplace_back(T_NAME, scanSymbol(script, index));

        else ERR << "Unknown character " << lexchar << " \"" << script[index] << "\"";
        index++;
        lexchar++;
    }
}

} // namespace oca::internal
