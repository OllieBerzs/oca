/* ollieberzs 2018
** error.cpp
** handle oca errors
*/

#include "error.hpp"
#include "oca.hpp"
#include "utils.hpp"
#include <iostream>

OCA_BEGIN

Error::Error(ErrorType type, ExprPtr expr, const std::string& detail)
    : type(type), expr(expr), detail(detail) {}

ErrorHandler::ErrorHandler(const State* state) : state(state) {}

void ErrorHandler::panic(Error error) const {
    std::vector<std::string> typestrings = {"UNKNOWN SYMBOL",
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
                                            "NO ACCESS KEY CALL",
                                            "NO CONDITIONAL",
                                            "NO THEN",
                                            "NO RIGHT VALUE",
                                            "NOTHING TO INJECT",
                                            "NEW TUPLE KEY",
                                            "CANNOT SPLIT",
                                            "UNDEFINED OPERATOR",
                                            "IF BOOL",
                                            "UNDEFINED IN TUPLE",
                                            "NO ARGUMENT",
                                            "SMALL TUPLE",
                                            "UNDEFINED",
                                            "TYPE MISMATCH",
                                            "NOT PUBLIC",
                                            "ERROR"};

    // error message config
    uint position;
    uint width;
    std::string message;

    switch (error.type) {
    case UNKNOWN_SYMBOL:
        // last added token
        position = std::stoi(error.detail);
        width = 1;
        message = "This symbol is not supported by the Oca language.";
        break;

    case INDENTED_FILE:
        // first symbol in source
        position = 0;
        width = 1;
        message = "The first line of the file must not be indented.";
        break;

    case NOT_AN_EXPRESSION:
        // current token
        position = tokens->at(state->parser.index).pos;
        width = tokens->at(state->parser.index).val.size();
        message = "This is not a valid start of an expression.";
        break;

    case UNEXPECTED_INDENT:
        // indent - previous token
        position = tokens->at(state->parser.index - 1).pos + 1;
        width = tokens->at(state->parser.index - 1).val.size() - 1;
        message = "This indent is not supposed to be here.";
        break;

    case NO_NEWLINE:
        // current token
        position = tokens->at(state->parser.index).pos;
        width = tokens->at(state->parser.index).val.size();
        message = "There must be a newline here.";
        break;

    case NO_PARAMETER:
        // $ - previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "There must be a parameter name after this.";
        break;

    case NOTHING_TO_SET:
        // =/: - previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Expected some value to be set.";
        break;

    case NO_CLOSING_BRACE:
        // previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Expected a closing brace for tuple";
        break;

    case NO_INDENT:
        // current token
        position = tokens->at(state->parser.index).pos;
        width = tokens->at(state->parser.index).val.size();
        message = "There must be an indented block of code here.";
        break;

    case NO_NAME:
        // , - previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Expected another variable.";
        break;

    case NO_ACCESS_KEY:
        // . - previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Expected an accessor key.";
        break;

    case NO_ACCESS_KEY_CALL:
        // [ - previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Expected an accessor key call.";
        break;

    case NO_CONDITIONAL:
        // if - previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "'if' must have a conditional expression.";
        break;

    case NO_THEN:
        // previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "'if' must have the 'then' keyword.";
        break;

    case NO_RIGHT_VALUE:
        // previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Missing right value for operator.";
        break;

    case NOTHING_TO_INJECT:
        // previous token
        position = tokens->at(state->parser.index - 1).pos;
        width = tokens->at(state->parser.index - 1).val.size();
        message = "Missing file to inject.";
        break;

    case NEW_TUPLE_KEY:
        // tuple - left branch
        position = tokens->at(error.expr->left->index).pos;
        width = tokens->at(error.expr->left->index).val.size();
        message = "You cannot error.detail a new key to a tuple.";
        break;

    case CANNOT_SPLIT:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "This value cannot be split into the variables on the left.";
        break;

    case UNDEFINED_OPERATOR:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "The operator doesn't exist for the type.";
        break;

    case IF_BOOL:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "The conditional for 'if' must evaluate to a boolean value.";
        break;

    case UNDEFINED_IN_TUPLE:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "Undefined name in tuple.";
        break;

    case NO_ARGUMENT:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "This block requires an argument to be called.";
        break;

    case SMALL_TUPLE:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "The tuple is too small of an argument for function " + error.detail;
        break;

    case UNDEFINED:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "Undefined variable.";
        break;

    case TYPE_MISMATCH:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = "This function got " + error.detail;
        break;

    case NOT_PUBLIC:
        position = tokens->at(state->evaler.current->index).pos;
        width = tokens->at(state->evaler.current->index).val.size();
        message = "This is not a public member.";
        break;

    case CUSTOM_ERROR:
        // current expression
        position = tokens->at(error.expr->index).pos;
        width = tokens->at(error.expr->index).val.size();
        message = error.detail;
        break;
    }

    enableANSI();

    // header
    std::string filename = "";
    if (path)
        filename = *path;

    std::cout << ESC "38;5;14m";
    std::cout << "-- " << typestrings[error.type] << " -------------------- " << filename << "\n";
    std::cout << ESC "0m";

    // get error line and the previous line if exists
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

        if (index == position) {
            found = true;
            colNum = errline.size() - 1;
        }
        ++index;
    }

    // split the error line
    std::string lineBeg = errline.substr(0, colNum);
    std::string lineMid = errline.substr(colNum, width);
    std::string lineEnd = errline.substr(colNum + width, errline.size() - (colNum + width));

    // error lines
    if (lineNum > 1) {
        std::cout << lineNum - 1 << "| " << prevline << "\n";
    }
    std::cout << lineNum << "| " << ESC "38;5;15m" << lineBeg << ESC "48;5;9m" << lineMid
              << ESC "0m" << ESC "38;5;15m" << lineEnd << "\n"
              << ESC "0m" << message << "\n";

    exit(1);
}

OCA_END
