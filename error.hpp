/* ollieberzs 2018
** error.hpp
** handle oca errors
*/

#include <tuple>
#include "common.hpp"
#include "value.hpp"

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
    TYPE_MISMATCH
};

enum ValueType
{
    INT = 0,
    REAL,
    STRING,
    BOOL
};

class Errors
{
    struct File
    {
        const std::string* path;
        const std::string* source;
        const std::vector<Token>* tokens;
        const Parser* parser;
    };
    std::vector<File> files;

    Errors() = default;

public:
    static Errors& instance();

    void begin(const std::string* path, const std::string* s,
        const std::vector<Token>* t, const Parser* p);
    void end();
    uint count();
    std::string folder();

    void panic(ErrorType type, ExprPtr expr = nullptr, const std::string& add = "");
};

OCA_END
