/* ollieberzs 2018
** lex.cpp
** lexing oca source into tokens
*/

#include <iostream>
#include <regex>
#include "oca.hpp"

OCA_BEGIN

void Token::print() const {
    std::vector<std::string> types = {
        "string",  "binnum",   "hexnum",     "scientnum", "real",     "integer",
        "boolean", "filepath", "keyword",    "name",      "operator", "punctuation",
        "comment", "indent",   "whitespace", "invalid",   "last"};
    std::cout << "<" << types[type] << ">";
    if (type != Type::INDENT)
        std::cout << val;
    std::cout << "\n";
}

//-----------------------------

Lexer::Lexer() {
    // calculate the capture group count for each syntax element
    captureGroupCounts.reserve(syntax.size());
    for (auto element : syntax) {
        std::regex regex(element.second + "|.*");
        std::string blank = "";
        auto matches = std::sregex_iterator(blank.begin(), blank.end(), regex);
        auto match = *matches;
        captureGroupCounts.push_back(match.size());
    }
}

void Lexer::lex(const std::string& source, std::vector<Token>& tokens) {
    if (source[0] == ' ')
        throw Error(INDENTED_FILE);

    std::string fullRegexString = "";
    for (const auto& element : syntax)
        fullRegexString += "(" + element.second + ")|";
    fullRegexString.pop_back();

    std::regex regex(fullRegexString);
    auto matches = std::sregex_iterator(source.begin(), source.end(), regex);

    for (auto it = matches; it != std::sregex_iterator(); ++it) {
        uint pos = static_cast<uint>(it->position());
        for (uint i = 0; i < it->size(); ++i) {
            if (it->str(i + 1).empty())
                continue;
            uint index = indexFromGroup(i);
            if (syntax[index].first == Token::WHITESPACE)
                continue;
            if (syntax[index].first == Token::COMMENT)
                continue;
            tokens.push_back({syntax[index].first, it->str(), pos});
            if (syntax[index].first == Token::INVALID)
                throw Error(UNKNOWN_SYMBOL);
            break;
        }
    }

    tokens.push_back({Token::LAST, "", static_cast<uint>(source.size())});
}

uint Lexer::indexFromGroup(uint group) {
    uint sum = 0;
    uint index = 0;
    while (sum < group) {
        sum += captureGroupCounts[index];
        ++index;
    }
    return index;
}

OCA_END
