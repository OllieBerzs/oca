/* ollieberzs 2018
** error.cpp
** handle oca errors
*/

#include <iostream>
#include "error.hpp"
#include "lex.hpp"
#include "parse.hpp"

OCA_BEGIN

ErrorHandler::ErrorHandler(const std::string* p, const std::string* s,
        const std::vector<Token>* t, const std::vector<ExprPtr>* e)
    : path(p), source(s), tokens(t), exprs(e) {}

void ErrorHandler::error(ErrorType type)
{
    std::vector<std::string> typeStrings = {
        "UNKNOWN SYMBOL",
        "INDENTED FILE",
        "NOT AN EXPRESSION",
        "UNEXPECTED INDENT",
        "NO NEWLINE",
        "NO PARAMETER",
        "NOTHING TO SET",
        "NO CLOSING BRACE",
        "NO INDENT",
        "NO NAME",
        "NO ACCESS KEY",
        "NO ACCESS KEY CALL"
        "NO CONDITIONAL",
        "NO THEN",
        "NO RIGHT VALUE"
    };

    // error message config
    uint pos;
    uint width;
    std::string message;

    switch (type)
    {
    case UNKNOWN_SYMBOL:
        // last added token
        pos = tokenPos.back();
        width = 1;
        message = "This symbol is not supported by the Oca language.";
        break;

    case INDENTED_FILE:
        // first symbol in source
        pos = 0;
        width = 1;
        message = "The first line of the file must not be indented.";
        break;

    case NOT_AN_EXPRESSION:
        // current token
        pos = tokenPos.at(parser->index);
        width = tokens->at(parser->index).val.size();
        message = "This is not a valid start of an expression.";
        break;

    case UNEXPECTED_INDENT:
        // indent - previous token
        pos = tokenPos.at(parser->index - 1) + 1;
        width = tokens->at(parser->index - 1).val.size() - 1;
        message = "This indent is not supposed to be here.";
        break;

    case NO_NEWLINE:
        // current token
        pos = tokenPos.at(parser->index);
        width = tokens->at(parser->index).val.size();
        message = "There must be a newline here.";
        break;

    case NO_PARAMETER:
        // $ - previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "There must be a parameter name after this.";
        break;

    case NOTHING_TO_SET:
        // =/: - previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "Expected some value to be set.";
        break;

    case NO_CLOSING_BRACE:
        // previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "Expected a closing brace for tuple";
        break;

    case NO_INDENT:
        // current token
        pos = tokenPos.at(parser->index);
        width = tokens->at(parser->index).val.size();
        message = "There must be an indented block of code here.";
        break;

    case NO_NAME:
        // , - previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "Expected another variable.";
        break;

    case NO_ACCESS_KEY:
        // . - previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "Expected an accessor key.";
        break;

    case NO_ACCESS_KEY_CALL:
        // [ - previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "Expected an accessor key call.";
        break;

    case NO_CONDITIONAL:
        // if - previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "'if' must have a conditional expression.";
        break;

    case NO_THEN:
        // previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "'if' must have the 'then' keyword.";
        break;

    case NO_RIGHT_VALUE:
        // previous token
        pos = tokenPos.at(parser->index - 1);
        width = tokens->at(parser->index - 1).val.size();
        message = "Missing right value for operator.";
        break;
    }

    // get error line and the previous line if exists
    std::string prevline = "";
    std::string errline = "";
    uint lineNum = 1;
    uint colNum = 1;
    uint index = 0;

    char c = ' ';
    bool found = false;

    while (c != '\n' || !found)
    {
        c = (*source)[index];
        if (c == '\n' && !found)
        {
            lineNum++;
            prevline = errline;
            errline = "";
        }
        else errline += c;

        if (index == pos) // found error start position
        {
            found = true;
            colNum = errline.size() - 1;
        }
        ++index;
    }

    // split the error line
    std::string lineBeg = errline.substr(0, colNum);
    std::string lineMid = errline.substr(colNum, width);
    std::string lineEnd = errline.substr(colNum + width, errline.size() - (colNum + width));

    // header
    system("printf '\033[1A'");
    std::cout << "\033[38;5;14m";
    std::cout << "-- " << typeStrings[type] << " -------------------- " << *path << "\n";
    std::cout << "\033[0m";

    // error lines
    if (lineNum > 1)
    {
        std::cout << lineNum - 1 << "| " << prevline << "\n";
    }
    std::cout << lineNum << "| ";
    std::cout << "\033[38;5;15m";
    std::cout << lineBeg;
    std::cout << "\033[48;5;9m";
    std::cout << lineMid;
    std::cout << "\033[0m";
    std::cout << "\033[38;5;15m";
    std::cout << lineEnd << "\n";
    std::cout << "\033[0m";

    // message and suggestions
    std::cout << message << "\n";

    std::cin.get();
    exit(1);
}

OCA_END
