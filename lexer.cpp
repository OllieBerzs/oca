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

bool match(const std::string& script, unsigned int index, const std::vector<std::string>& lookFor)
{
    for (const std::string& s : lookFor)
    {
        unsigned int i = index;
        bool same = false;
        for (char c : s)
        {
            if (script[i] != c)
            {
                same = false;
                break;
            }
            i++;
            same = true;
        }
        if (same)
        {
            return true;
        }
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

std::string scanBool(const std::string& script, unsigned int& index)
{
    std::string ret = "";

    bool end = false;
    while (!end)
    {
        char c = script[index];
        if (isOf(c, LETTERS))
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
    Error::line++;
    index--;
    lexchar = -1;
}

void newLine(std::vector<Token>& tokens)
{
    tokens.emplace_back(T_NEWLINE, "");
    Error::line++;
    lexchar = -1;
}

void lex(const std::string& script, std::vector<Token>& tokens)
{
    Error::line = 1;

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
        // Keywords
        else if (match(script, index, {"do"})) { tokens.emplace_back(T_DO, ""); index += 2; }
        else if (match(script, index, {"end"})) { tokens.emplace_back(T_END, ""); index += 3; }
        // Types
        else if (isOf(c, "'\"")) tokens.emplace_back(T_STRING, scanString(script, index));
        else if (isOf(c, NUMBERS)) tokens.emplace_back(T_NUMBER, scanNumber(script, index));
        else if (match(script, index, {"true", "false"})) tokens.emplace_back(T_BOOL, scanBool(script, index));
        // Names
        else if (isOf(c, "+-*/%^=")) tokens.emplace_back(T_NAME, c);
        else if (isOf(c, "_" LETTERS)) tokens.emplace_back(T_NAME, scanSymbol(script, index));

        else
        {
            Error::message = "Unknown character " + std::to_string(lexchar) + " \"" + std::to_string(script[index]) + "\"";
            Error::panic();
        }
        index++;
        lexchar++;
    }
}

} // namespace oca::internal
