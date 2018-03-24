/* ollieberzs 2018
** lex.cpp
** lexing oca source into tokens
*/

#include <iostream>
#include <regex>
#include "lex.hpp"
#include "error.hpp"

OCA_BEGIN

std::vector<std::pair<Token::Type, std::string>> syntax
{

    {Token::STRING,          "()('[^']*')"},
    {Token::REAL,            "()([0-9]+\\.[0-9]+)"},
    {Token::INTEGER,         "()([0-9]+)"},
    {Token::BOOLEAN,         "()\\b(true|false)\\b"},
    {Token::FILEPATH,        "()@(.)+"},
    {Token::KEYWORD,         "()\\b(do|if|then|else|return|break)\\b"},
    {Token::NAME,            "([A-Za-z_]+)([0-9]*)"},
    {Token::COMMENT,         "()--(.|\\n)*--"},
    {Token::OPERATOR,        "()(\\+|-|\\*|\\/|%|\\^|<|>)"},
    {Token::PUNCTUATION,     "()(\\.|\\$|:|\\(|\\)|,|=|\\[|\\])"},
    {Token::INDENT,          "(\\n *)(?=\\S)"},
    {Token::WHITESPACE,      "()(\\n *| +)"},
    {Token::INVALID,         "()(.)+"}
};

void Token::print()
{
    std::vector<std::string> types =
    {
        "string", "real", "integer", "boolean", "filepath",
        "keyword", "name", "operator", "punctuation",
        "comment", "indent", "whitespace", "invalid", "last"
    };
    std::cout << "<" << types[type] << ">";
    if (type != Type::INDENT) std::cout << val;
    std::cout << "\n";
}

//-----------------------------

Lexer::Lexer(ErrorHandler* er) : er(er)
{
    er->lexer = this;
}

void Lexer::lex(const std::string& source, std::vector<Token>& tokens)
{
    if (source[0] == ' ') er->error(INDENTED_FILE);

    std::string reg;
    for (const auto& r : syntax) reg += r.second + "|";
    reg.pop_back();

    std::regex regex(reg);
    auto matches = std::sregex_iterator(source.begin(), source.end(), regex);

    for (auto it = matches; it != std::sregex_iterator(); it++)
    {
        uint pos = static_cast<uint>(it->position());
        for (uint i = 0; i < it->size(); ++i)
        {
            if (it->str(i + 1).empty()) continue;
            uint index = i / 2;
            if (syntax[index].first == Token::WHITESPACE) continue;
            if (syntax[index].first == Token::COMMENT) continue;
            tokens.push_back({syntax[index].first, it->str(), pos});
            if (syntax[index].first == Token::INVALID) er->error(UNKNOWN_SYMBOL);
            break;
        }
    }

    tokens.push_back({Token::LAST, "", static_cast<uint>(source.size())});
}

OCA_END
