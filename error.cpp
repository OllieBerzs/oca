/* ollieberzs 2018
** error.cpp
** handle oca errors
*/

#include <iostream>
#include "error.hpp"
#include "oca.hpp"
#include "utils.hpp"

OCA_BEGIN

Error::Error(ErrorType type, const std::string& detail) : type(type), detail(detail) {}

ErrorHandler::ErrorHandler(const State* state) : state(state) {}

void ErrorHandler::panic(const Error& error) const {
    std::cout << "-";
    enableANSI();
    auto info = getErrorInfo(error);

    std::string filename = "";
    if (path)
        filename = *path;

    std::string prevline = "";
    std::string errline = "";
    uint lineNum = 1;
    uint colNum = 1;
    uint index = 0;

    char c = ' ';
    bool found = false;
    while ((c != '\n' && index < (*source).size()) || !found) {
        c = (*source)[index];
        if ((c == '\n' || index == (*source).size() - 1) && !found) {
            ++lineNum;
            prevline = errline;
            errline = "";
        } else
            errline += c;

        if (index == info.position) {
            found = true;
            colNum = errline.size() - 1;
        }
        ++index;
    }

    uint colEnd = colNum + info.width;
    std::string lineBeg = errline.substr(0, colNum);
    std::string lineMid = errline.substr(colNum, info.width);
    std::string lineEnd = errline.substr(colEnd, errline.size() - colEnd);

    std::cout << ESC "38;5;14m"
              << "- " << info.typestring << " -------------------- " << filename << "\n"
              << ESC "0m";

    if (lineNum > 1)
        std::cout << lineNum - 1 << "| " << prevline << "\n";

    std::cout << lineNum << "| " << ESC "38;5;15m" << lineBeg << ESC "48;5;9m" << lineMid
              << ESC "0m" << ESC "38;5;15m" << lineEnd << "\n"
              << ESC "0m" << info.message << "\n";
}

// -------------------------------------------

ErrorInfo ErrorHandler::getErrorInfo(const Error& error) const {
    if (error.type <= INDENTED_FILE)
        return getLexErrorInfo(error);
    if (error.type <= NOTHING_TO_INJECT)
        return getParseErrorInfo(error);
    return getEvalErrorInfo(error);
}

ErrorInfo ErrorHandler::getLexErrorInfo(const Error& error) const {
    switch (error.type) {
    case UNKNOWN_SYMBOL:
        return {static_cast<uint>(std::stoi(error.detail)), 1,
                "This symbol is not supported by the Oca language.", "UNKNOWN SYMBOL"};

    case INDENTED_FILE:
        return {0, 1, "The first line of the file must not be indented.", "INDENTED FILE"};

    default: return {0, 0, "", ""};
    }
}

ErrorInfo ErrorHandler::getParseErrorInfo(const Error& error) const {
    uint tokenIndex = state->parser.index;
    switch (error.type) {
    case NOT_AN_EXPRESSION:
        return {tokens->at(tokenIndex).pos, static_cast<uint>(tokens->at(tokenIndex).val.size()),
                "This is not a valid start of an expression.", "NOT AN EXPRESSION"};

    case UNEXPECTED_INDENT:
        return {tokens->at(tokenIndex - 1).pos + 1,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()) - 1,
                "This indent is not supposed to be here.", "UNEXPECTED INDENT"};

    case NO_NEWLINE:
        return {tokens->at(tokenIndex).pos, static_cast<uint>(tokens->at(tokenIndex).val.size()),
                "There must be a newline here.", "NO NEWLINE"};

    case NO_PARAMETER:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "There must be a parameter name after this.", "NO PARAMETER"};

    case NOTHING_TO_SET:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "Expected some value to be set.", "NOTHING TO SET"};

    case NO_CLOSING_BRACE:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "Expected a closing brace for table", "NO CLOSING BRACE"};

    case NO_INDENT:
        return {tokens->at(tokenIndex).pos, static_cast<uint>(tokens->at(tokenIndex).val.size()),
                "There must be an indented block of code here.", "NO INDENT"};

    case NO_NAME:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "Expected another variable.", "NO NAME"};

    case NO_ACCESS_KEY:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "Expected an accessor key.", "NO ACCESS KEY"};

    case NO_ACCESS_KEY_CALL:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "Expected an accessor key call.", "NO ACCESS KEY CALL"};

    case NO_CONDITIONAL:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "'if' must have a conditional expression.", "NO CONDITIONAL"};

    case NO_THEN:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "'if' must have the 'then' keyword.", "NO THEN"};

    case NO_RIGHT_VALUE:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()),
                "Missing right value for operator.", "NO RIGHT VALUE"};

    case NOTHING_TO_INJECT:
        return {tokens->at(tokenIndex - 1).pos,
                static_cast<uint>(tokens->at(tokenIndex - 1).val.size()), "Missing file to inject.",
                "NOTHING TO INJECT"};

    default: return {0, 0, "", ""};
    }
}

ErrorInfo ErrorHandler::getEvalErrorInfo(const Error& error) const {
    auto currentExpr = state->evaler.current;
    if (!currentExpr)
        std::cout << "Error: null current expr\n";
    switch (error.type) {
    case NEW_TABLE_KEY:
        return {tokens->at(currentExpr->left->index).pos,
                static_cast<uint>(tokens->at(currentExpr->left->index).val.size()),
                "You cannot error.detail a new key to a table.", "NEW TABLE KEY"};

    case CANNOT_SPLIT:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "This value cannot be split into the variables on the left.", "CANNOT SPLIT"};

    case UNDEFINED_OPERATOR:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "The operator doesn't exist for the type.", "UNDEFINED OPERATOR"};

    case IF_BOOL:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "The conditional for 'if' must evaluate to a boolean value.", "IF BOOL"};

    case UNDEFINED_IN_TABLE:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "Undefined name in table.", "UNDEFINED IN TABLE"};

    case NO_ARGUMENT:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "This block requires an argument to be called.", "NO ARGUMENT"};

    case SMALL_TABLE:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "The table is too small of an argument for function " + error.detail,
                "SMALL TABLE"};

    case UNDEFINED:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()), "Undefined variable.",
                "UNDEFINED"};

    case TYPE_MISMATCH:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "This function got " + error.detail, "TYPE MISMATCH"};

    case NOT_PUBLIC:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()),
                "This is not a public member.", "NOT PUBLIC"};

    case CUSTOM_ERROR:
        return {tokens->at(currentExpr->index).pos,
                static_cast<uint>(tokens->at(currentExpr->index).val.size()), error.detail,
                "ERROR"};

    default: return {0, 0, "", ""};
    }
}

OCA_END
