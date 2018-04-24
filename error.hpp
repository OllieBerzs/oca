/* ollieberzs 2018
** error.hpp
** handle oca errors
*/

#pragma once

#include "common.hpp"

OCA_BEGIN

enum ErrorType {
    // Lexing
    UNKNOWN_SYMBOL = 0,
    INDENTED_FILE,

    // Parsing
    NOT_AN_EXPRESSION,
    UNEXPECTED_INDENT,
    NO_NEWLINE,
    NO_PARAMETER,
    NOTHING_TO_SET,
    NO_CLOSING_BRACE,
    NO_INDENT,
    NO_NAME,
    NO_ACCESS_KEY,
    NO_ACCESS_KEY_CALL,
    NO_CONDITIONAL,
    NO_THEN,
    NO_RIGHT_VALUE,
    NOTHING_TO_INJECT,

    // Evaluating
    NEW_TUPLE_KEY,
    CANNOT_SPLIT,
    UNDEFINED_OPERATOR,
    IF_BOOL,
    UNDEFINED_IN_TUPLE,
    NO_ARGUMENT,
    SMALL_TUPLE,
    UNDEFINED,
    TYPE_MISMATCH,
    NOT_PUBLIC,
    CUSTOM_ERROR
};

enum ValueType { INT = 0, REAL, STRING, BOOL };

class Error {
public:
    ErrorType type;
    ExprPtr expr;
    std::string detail;

    Error(ErrorType type, const std::string& detail = "", ExprPtr expr = nullptr);
};

class ErrorHandler {
public:
    const State* state;
    const std::string* path{};
    const std::string* source{};
    const std::vector<Token>* tokens{};

    explicit ErrorHandler(const State* state);
    void panic(Error error) const;
};

OCA_END
