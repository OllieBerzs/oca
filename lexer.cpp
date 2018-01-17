#include <iostream>
#include "lexer.hpp"

namespace oca::internal
{

#define NUMBERS "0123456789"
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

    unsigned int lexLine;
    unsigned int lexColumn;

    void lex(const std::string& script, std::vector<Token>& tokens)
    {
        unsigned int index = 0;

        lexLine = 1;
        lexColumn = 1;

        while (index < script.size())
        {
            if (whitespace(script, index)) continue;
            else if (punctuation(script, index, tokens)) continue;
            else if (number(script, index, tokens)) continue;
            else if (string(script, index, tokens)) continue;
            else if (boolean(script, index, tokens)) continue;
            else if (keyword(script, index, tokens)) continue;
            else if (dotless(script, index, tokens)) continue;
            else if (name(script, index, tokens)) continue;
            else
            {
                std::cout << "Unknown symbol \"" << script[index] << "\" ["
                << lexLine << " " << lexColumn << "]\n";
                std::cin.get();
                exit(0);
            }
        }
        tokens.emplace_back("__last__", "", 0, 0, 0);
    }

    bool isIn(char c, const std::string& str)
    {
        for (char ch : str) if (c == ch) return true;
        return false;
    }
    bool match(const std::string& to, const std::string& script, unsigned int& index)
    {
        unsigned int orig = index;
        unsigned int col = lexColumn;
        for (unsigned int i = 0; i < to.size(); i++)
        {
            if (to[i] != script[index])
            {
                index = orig;
                lexColumn = col;
                return false;
            }
            index++;
            lexColumn++;
        }
        return true;
    }

    bool whitespace(const std::string& script, unsigned int& index)
    {
        if (script[index] == ' ')
        {
            index++;
            lexColumn++;
            return true;
        }
        else if (isIn(script[index], "\n\r"))
        {
            index++;
            lexLine++;
            lexColumn = 1;
            return true;
        }
        return false;
    }

    bool punctuation(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        char c = script[index];
        if (isIn(c, ".,|()"))
        {
            std::string chr = "";
            chr += c;
            tokens.emplace_back(chr, "", lexLine, lexColumn, 1);
            index++;
            lexColumn++;
            return true;
        }
        return false;
    }
    bool number(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        char c = script[index];
        std::string num = "";
        bool isInt = true;

        // check if is number
        if (!isIn(c, NUMBERS)) return false;

        while (isIn(c, NUMBERS"."))
        {
            if (c == '.')
            {
                if (isInt && index + 1 < script.length() && isIn(script[index + 1], NUMBERS)) isInt = false;
                else break;
            }
            num += c;
            lexColumn++;
            c = script[++index];
        }

        unsigned int length = (unsigned int)num.size();
        if (isInt) tokens.emplace_back("integer", num, lexLine, lexColumn - length, length);
        else tokens.emplace_back("float", num, lexLine, lexColumn - length, length);

        return true;
    }
    bool string(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        char c = script[index];
        std::string str = "";

        if (c != '\'') return false;
        index++;
        lexColumn++;
        c = script[index];
        while (c != '\'')
        {
            str += c;
            index++;
            lexColumn++;
            if (index >= script.size())
            {
                std::cout << "Missing end quote [" << lexLine << " " << lexColumn << "]\n";
                std::cin.get();
                exit(0);
            }
            c = script[index];
        }
        index++;
        lexColumn++;
        unsigned int length = (unsigned int)str.size();
        tokens.emplace_back("string", str, lexLine, lexColumn - length, length);
        return true;
    }
    bool boolean(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        if (match("true", script, index))
        {
            tokens.emplace_back("boolean", "true", lexLine, lexColumn - 4, 4);
            return true;
        }
        else if (match("false", script, index))
        { 
            tokens.emplace_back("boolean", "false", lexLine, lexColumn - 5, 5);
            return true;
        }
        return false;
    }
    bool keyword(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        std::vector<std::string> names = {"def", "do", "else", "end"};
        for (std::string& name : names)
        {
            if (match(name, script, index))
            {
                unsigned int length = (unsigned int)name.length();
                tokens.emplace_back(name, "", lexLine, lexColumn - length, length);
                return true;
            }
        }
        return false;
    }
    bool dotless(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        std::vector<std::string> names = {"+", "-", "/", "*", "^", "%", "==", "or", "and"};
        for (std::string& name : names)
        {
            if (match(name, script, index))
            {
                unsigned int length = (unsigned int)name.length();
                tokens.emplace_back("dotless", name, lexLine, lexColumn - length, length);
                return true;
            }
        }
        return false;
    }
    bool name(const std::string& script, unsigned int& index, std::vector<Token>& tokens)
    {
        char c = script[index];
        std::string nam = "";
        while (isIn(c, LETTERS "_"))
        {
            nam += c;
            c = script[++index];
            lexColumn++;
        }
        unsigned int length = (unsigned int)nam.length();
        if (length > 0)
        {
            tokens.emplace_back("name", nam, lexLine, lexColumn - length, length);
            return true;
        }
        return false;
    }

} // namespace oca::internal