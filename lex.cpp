/* ollieberzs 2018
** lex.cpp
** lexing oca source into tokens
*/

#include <iostream>
#include <regex>

#include "lex.hpp"

OCA_BEGIN

std::vector<std::pair<std::string, std::string>> syntax
{

    {"STRING",          "'(.)*'"},
    {"FLOAT",           "([0-9]+\\.[0-9]+)"},
    {"INTEGER",         "([0-9]+)"},
    {"BOOLEAN",         "\\b(true|false)\\b"},
    {"FILEPATH",        "\\$(.)+"},

    {"KEYWORD",         "\\b(do|if|then|else|return|break)\\b"},
    {"NAME",            "([A-Za-z]+)"},
    {"OPERATOR",        "(\\+|-|\\*|\\/|%|\\^|&|=|!|<|>|~)+"},

    {"PUNCTUATION",     "(\\.|:|\\(|\\)|,)"},

    {"COMMENT",         "#(.)*"},
    {"INDENT",          "(^ +|\\n *)"},
    {"WHITESPACE",      "(\\n| )+"},
    {"INVALID",         "(.)+"}
};

void Token::print()
{
    std::cout << "<" << type << "(" << pos << ")" << ">";
    if (type != "INDENT") std::cout << val;
    std::cout << "\n";
}

//-----------------------------

std::vector<Token> Lexer::lex()
{
    std::vector<Token> result;

    std::string reg;
    for (const auto& r : syntax) reg += r.second + "|";
    reg.pop_back();

    std::regex regex(reg);
    auto matches = std::sregex_iterator(source.begin(), source.end(), regex);

    for (auto it = matches; it != std::sregex_iterator(); it++)
    {
        uint pos = static_cast<uint>(it->position());
        for (uint i = 0; i < it->size(); i++)
        {
            if (it->str(i + 1).empty()) continue;
            if (syntax[i].first == "WHITESPACE") continue;
            if (syntax[i].first == "COMMENT") continue;
            if (syntax[i].first == "INVALID") error("Unknown symbol", {syntax[i].first, it->str(), pos});
            result.push_back({syntax[i].first, it->str(), pos});
            break;
        }
    }

    result.push_back({"LAST", "", static_cast<uint>(source.size())});
    return result;
}

void Lexer::error(const std::string& message, const Token t)
{
    // get error line
    std::string errline = "";
    uint lineNum = 1;
    uint colNum = 1;
    uint index = 0;

    char c = 'a';
    bool found = false;

    while (c != '\n' || !found)
    {
        c = source[index];
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

    std::string lineStart = errline.substr(0, colNum - 1);
    std::string lineEnd = colNum < errline.size()
        ? errline.substr(colNum, errline.size()) : "";

    system("printf '\033[1A'");
    std::cout << "\033[38;5;14m";
    std::cout << "-- ERROR -------------------- " << path << "\n";
    std::cout << "\033[0m";
    std::cout << lineNum << "| ";
    std::cout << "\033[38;5;15m";
    std::cout << lineStart;
    std::cout << "\033[48;5;9m";
    std::cout << (t.val == "\n" ? " " : t.val);
    std::cout << "\033[0m";
    std::cout << "\033[38;5;15m";
    std::cout << lineEnd << "\n";
    std::cout << "\033[0m";
    std::cout << message << "\n";

    std::cin.get();
    exit(1);
}

OCA_END
