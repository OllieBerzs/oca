#include "lexer.hpp"
#include "errors.hpp"

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

bool isOf(char c, std::string str)
{
    for (char ch : str)
    {
        if (c == ch) return true;
    }
    return false;
}

std::string scanString(std::string script, unsigned int& index, unsigned int& num)
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
            num++;
        }
    }

    return ret;
}

std::string scanNumber(std::string script, unsigned int& index, unsigned int& num)
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
            num++;
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
                num--;
            }
        }
        else
        {
            end = true;
            index--;
            num--;
        }
    }

    return ret;
}

std::string scanSymbol(std::string script, unsigned int& index, unsigned int& num)
{
    std::string ret;

    // First character cannot be a number
    ret += script[index];
    index++;
    num++;

    bool end = false;
    while (!end)
    {
        char c = script[index];
        if (isOf(c, "_" LETTERS NUMBERS))
        {
            ret += c;
            index++;
            num++;
        }
        else
        {
            end = true;
            index--;
            num--;
        }
    }

    return ret;
}

void skipLine(const std::string& script, unsigned int& index, unsigned int& line, unsigned int& num)
{
    while (script[index] != '\n') index++;
    index--;
    line++;
    num = 0;
}

void lex(const std::string& script, std::vector<Token>& tokens)
{
    unsigned int index = 0;
    unsigned int line = 1;
    unsigned int num = 0;
    while (index < script.size())
    {
        char c = script[index];

        if (isOf(c, " \r")); // Skip space
        else if (c == '#') skipLine(script, index, line, num);
        else if (c == '\n') { tokens.emplace_back(T_NEWLINE, ""); line++; num = 0; }
        else if (c == '(') tokens.emplace_back(T_LPAREN, "");
        else if (c == ')') tokens.emplace_back(T_RPAREN, "");
        else if (c == '.') tokens.emplace_back(T_DOT, "");
        else if (c == ',') tokens.emplace_back(T_COMMA, "");
        // Types
        else if (isOf(c, "'\"")) tokens.emplace_back(T_STRING, scanString(script, index, num));
        else if (isOf(c, NUMBERS)) tokens.emplace_back(T_NUMBER, scanNumber(script, index, num));
        // Names
        else if (isOf(c, "+-*/=")) tokens.emplace_back(T_NAME, c);
        else if (isOf(c, "_" LETTERS)) tokens.emplace_back(T_NAME, scanSymbol(script, index, num));

        else ERR << "Unknown character " << num << " on line " << line;
        index++;
        num++;
    }
}
