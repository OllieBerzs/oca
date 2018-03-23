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
    NO_RIGHT_VALUE
};

class ErrorHandler
{
    const std::string* path;

    const std::string* source;
    const std::vector<Token>* tokens;
    const std::vector<ExprPtr>* exprs;

public:
    Lexer* lexer;
    Parser* parser;

    std::vector<uint> tokenPos;
    std::vector<std::pair<uint, uint>> exprPos;

    ErrorHandler(const std::string* p, const std::string* s,
        const std::vector<Token>* t, const std::vector<ExprPtr>* e);

    void error(ErrorType type);
};

OCA_END
