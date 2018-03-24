/* ollieberzs 2018
** error.hpp
** handle oca errors
*/

#include "common.hpp"

OCA_BEGIN

enum ErrorType
{
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

    // Evaluating
    NEW_TUPLE_KEY,
    CANNOT_SPLIT,
    UNDEFINED_OPERATOR,
    IF_BOOL,
    UNDEFINED_IN_TUPLE,
    NO_ARGUMENT,
    UNDEFINED
};

class ErrorHandler
{
    const std::string* path;

    const std::string* source;
    const std::vector<Token>* tokens;

public:
    Lexer* lexer;
    Parser* parser;

    ErrorHandler(const std::string* p, const std::string* s, const std::vector<Token>* t);

    void error(ErrorType type, ExprPtr expr = nullptr);
};

OCA_END
