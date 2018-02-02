/* ollieberzs 2018
** lex.cpp
** lexing oca source into tokens
*/ 

#include <iostream>
#include "lex.hpp"

OCA_BEGIN

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

void lex(LexState& state)
{
    state.current = 0;
    state.lineNum = 1;
    state.colNum = 1;

    while (state.current < state.source.size())
    {
        skipSpace(state);
        if (state.current >= state.source.size()) break;

        if (matchPunct(state)) continue;
        else if (matchNum(state)) continue;
        else if (matchStr(state)) continue;
        else if (matchBool(state)) continue;
        else if (matchKeyword(state)) continue;
        else if (matchOper(state)) continue;
        else if (matchName(state)) continue;
        else lexError(state, "Unknown symbol");
    }
    state.tokens.push_back(Token {"last", "", state.lineNum, state.colNum});
}
// ----------------------------

bool matchPunct(LexState& state)
{
    char c = state.source[state.current];
    if (charIsIn(c, ".,|()"))
    {
        std::string chr = "";
        chr += c;
        state.tokens.push_back(Token {chr, "", state.lineNum, state.colNum});
        state.current++;
        state.colNum++;
        return true;
    }
    return false;
}

bool matchNum(LexState& state)
{
    char c = state.source[state.current];
    std::string num = "";
    bool isInt = true;

    // check if is number
    if (!charIsIn(c, NUMBERS)) return false;

    while (charIsIn(c, NUMBERS"."))
    {
        if (c == '.')
        {
            if (isInt && state.current + 1 < state.source.size()
                && charIsIn(state.source[state.current + 1], NUMBERS))
            {
                isInt = false;
            }
            else break;
        }
        num += c;
        state.colNum++;
        c = state.source[++state.current];
    }

    if (isInt) state.tokens.push_back(Token {"int", num, state.lineNum, state.colNum});
    else state.tokens.push_back(Token {"float", num, state.lineNum, state.colNum});
    return true;
}

bool matchStr(LexState& state)
{
    char c = state.source[state.current];
    std::string str = "";

    if (c != '\'') return false;
    state.current++;
    state.colNum++;

    c = state.source[state.current];
    while (c != '\'')
    {
        if (c == '\n') lexError(state, "Missing end quote");

        str += c;
        state.current++;
        state.colNum++;
        
        if (state.current >= state.source.size()) lexError(state, "Missing end quote");

        c = state.source[state.current];
    }
    state.current++;
    state.colNum++;
    state.tokens.push_back(Token {"str", str, state.lineNum, state.colNum});
    return true;
}

bool matchBool(LexState& state)
{
    if (match("true", state))
    {
        state.tokens.push_back(Token {"bool", "true", state.lineNum, state.colNum});
        return true;
    }
    else if (match("false", state))
    { 
        state.tokens.push_back(Token {"bool", "false", state.lineNum, state.colNum});
        return true;
    }
    return false;
}

bool matchKeyword(LexState& state)
{
    std::vector<std::string> names = {"def", "do", "else", "end", "break", "yield"};
    for (std::string& name : names)
    {
        if (match(name, state))
        {
            state.tokens.push_back(Token {name, "", state.lineNum, state.colNum});
            return true;
        }
    }
    return false;
}

bool matchOper(LexState& state)
{
    std::vector<std::string> names = {"+", "-", "/", "*", "^", "%", "==", "|", "&"};
    for (std::string& name : names)
    {
        if (match(name, state))
        {
            state.tokens.push_back(Token {"oper", name, state.lineNum, state.colNum});
            return true;
        }
    }
    return false;
}

bool matchName(LexState& state)
{
    char c = state.source[state.current];
    std::string nam = "";

    if (!charIsIn(c, LETTERS"_")) return false;

    while (charIsIn(c, LETTERS"_"))
    {
        nam += c;
        c = state.source[++state.current];
        state.colNum++;
    }

    state.tokens.push_back(Token {"name", nam, state.lineNum, state.colNum});
    return true;
}
// ----------------------------

void skipSpace(LexState& state)
{
    while (match("\n\r", state) || match("\r\n", state) 
        || match("\n", state) || match("\r", state) || match(" ", state))
    {
        if (state.source[state.current - 1] != ' ')
        {
            state.lineNum++;
            state.colNum = 1;
        }
    }
}

bool charIsIn(char c, const std::string& str)
{
    for (char ch : str) if (c == ch) return true;
    return false;
}

bool match(const std::string& to, LexState& state)
{
    uint orig = state.current;
    uint origcol = state.colNum;

    for (uint i = 0; i < to.size(); i++)
    {
        if (to[i] != state.source[state.current])
        {
            state.current = orig;
            state.colNum = origcol;
            return false;
        }
        state.current++;
        state.colNum++;
    }
    return true;
}
// ----------------------------

void lexError(const LexState& state, const std::string& message)
{
    // get error line
    std::string line = "";
    uint count = 1;
    for (char c : state.source)
    {
        if (c == '\n')
        {
            if (count == state.lineNum) break;
            count++;
            line = "";
        }
        else line += c;
    }

    char current = state.source[state.current];
    std::string lineStart = line.substr(0, state.colNum - 1);
    std::string lineEnd = state.colNum < line.size() 
        ? line.substr(state.colNum, line.size()) : "";

    system("printf '\033[1A'");
    std::cout << "\033[38;5;14m";
    std::cout << "-- ERROR -------------------- " << state.sourceName << "\n";
    std::cout << "\033[0m";    
    std::cout << state.lineNum << "| ";
    std::cout << "\033[38;5;15m";
    std::cout << lineStart;
    std::cout << "\033[48;5;9m";
    std::cout << (current == '\n' ? ' ' : current);
    std::cout << "\033[0m";
    std::cout << "\033[38;5;15m";
    std::cout << lineEnd << "\n";
    std::cout << "\033[0m";
    std::cout << message << "\n";
    
    std::cin.get();
    exit(1);
}

void printToken(const Token& token)
{
    std::cout << "<" << token.type << ">" << token.val << "\n";
}

OCA_END
