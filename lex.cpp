/* ollieberzs 2018
** lex.cpp
** lexing oca source into tokens
*/

#include <iostream>
#include <regex>
#include "oca.hpp"

OCA_BEGIN

std::vector<std::pair<Token::Type, std::string>> syntax
{
    {Token::STRING,          "()()('[^']*')"},
    {Token::BINNUM,          "()()(0b[01]+)"},
    {Token::HEXNUM,          "()()(0x[0-9A-Fa-f]+)"},
    {Token::SCIENTNUM,       "([0-9]+(\\.[0-9]+)?[eE]-?[0-9]+(\\.[0-9]+)?)"},
    {Token::REAL,            "()()([0-9]+\\.[0-9]+)"},
    {Token::INTEGER,         "()()([0-9]+)"},
    {Token::BOOLEAN,         "()()\\b(true|false)\\b"},
    {Token::FILEPATH,        "()()(\\$.+)"},
    {Token::KEYWORD,         "()()\\b(do|if|then|else|return|break|with|inject)\\b"},
    {Token::OPERATOR,        "()()(\\+|-|\\*|\\/|%|\\^|<=|>=|==|!=|<|>|\\.\\.)"},
    {Token::COMMENT,         "()(#(.)*)"},
    {Token::NAME,            "()([A-Za-z_])([A-Za-z_0-9]*)"},
    {Token::PUNCTUATION,     "()()(\\.|:|\\(|\\)|,|=|\\[|\\])"},
    {Token::INDENT,          "()(\\n *)(?=[^\\s^#])"},
    {Token::WHITESPACE,      "()()(\\n *| +)"},
    {Token::INVALID,         "()()(.)+"}
};

void Token::print() const
{
    std::vector<std::string> types =
    {
        "string", "binnum", "hexnum", "scientnum", "real", "integer", "boolean", "filepath",
        "keyword", "name", "operator", "punctuation",
        "comment", "indent", "whitespace", "invalid", "last"
    };
    std::cout << "<" << types[type] << ">";
    if (type != Type::INDENT) std::cout << val;
    std::cout << "\n";
}

//-----------------------------

Lexer::Lexer(State* state) : state(state) {}

void Lexer::lex(const std::string& source, std::vector<Token>& tokens)
{
    if (source[0] == ' ') state->err.panic(INDENTED_FILE);

    std::string reg;
    for (const auto& r : syntax) reg += r.second + "|";
    reg.pop_back();

    std::regex regex(reg);
    auto matches = std::sregex_iterator(source.begin(), source.end(), regex);

    for (auto it = matches; it != std::sregex_iterator(); ++it)
    {
        uint pos = static_cast<uint>(it->position());
        for (uint i = 0; i < it->size(); ++i)
        {
            if (it->str(i + 1).empty()) continue;
            uint index = i / 3;
            if (syntax[index].first == Token::WHITESPACE) continue;
            if (syntax[index].first == Token::COMMENT) continue;
            tokens.push_back({syntax[index].first, it->str(), pos});
            if (syntax[index].first == Token::INVALID) state->err.panic(UNKNOWN_SYMBOL);
            break;
        }
    }

    tokens.push_back({Token::LAST, "", static_cast<uint>(source.size())});
}

OCA_END
