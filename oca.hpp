#include "utils.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "native.hpp"

namespace oca
{
void script(const std::string& source, bool outTokens = false, bool outTree = false, bool outOutput = true, bool outMemory = false)
{
    std::vector<Token> tokens;
    std::vector<Expression*> exprs;

    lex(source, tokens);

    if (outTokens)
    {
        std::cout << "\n============TOKENS===========\n\n";
        for (const Token& token : tokens) std::cout << token << '\n';
    }

    parse(tokens, exprs);

    if (outTree)
    {
        std::cout << "\n============TREE=============\n\n";
        for (auto e : exprs) std::cout << *e << '\n';
    }

    /*std::cout << "=========================" << '\n';
    for (const Token& token : tokens) std::cout << token << '\n';
    std::cout << "-------------------------" << '\n';
    std::cout << "-------------------------" << '\n';
    for (auto e : exprs) std::cout << *e << '\n';
    std::cout << "-------------------------" << '\n';
    std::cout << "-------------------------" << '\n';
    for (auto e : exprs)
    {
    Object* o = evaluate(e, scope);
    if (o) std::cout << "=> " << o->toString() << '\n';
    else std::cout << "=> null" << '\n';
    }
    std::cout << "-------------------------" << '\n';
    std::cout << "-------------------------" << '\n';

    // Cleanup
    for (auto e : exprs) delete e;
    scope.clean();

    std::cout << "memory usage: [" << Memory::get() << "]\n";

    std::cout << "=========================" << '\n';*/
}
} // namespace oca
