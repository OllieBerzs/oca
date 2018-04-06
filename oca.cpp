/* ollieberzs 2018
** oca.cpp
** oca api
*/

#include <iostream>
#include <fstream>
#include "oca.hpp"

OCA_BEGIN

ValuePtr Arg::operator[](uint i)
{
    return value->scope.get(std::to_string(i));
}

// ---------------------------------------

State::State()
    : scope(nullptr), global(nullptr), lexer(this), parser(this)
    , evaler(this), err(this), lextime(0), parsetime(0), evaltime(0)
{
    // add base functions
    bind("print", "a", CPPFUNC
    {
        std::cout << arg.value->tos(false) << "\n";
        return NIL;
    });

    bind("input", "", CPPFUNC
    {
        std::string result;
        std::cin >> result;
        return cast(result);
    });

    bind("pause", "", CPPFUNC
    {
        std::cin.get();
        return NIL;
    });

    bind("assert", "bs", CPPFUNC
    {
        bool cond = arg[0]->tob();
        std::string message = arg[1]->tos(false);
        if (!cond) err.panic(ERROR, evaler.current, message);
        return NIL;
    });

    bind("type", "a", CPPFUNC
    {
        std::string str = arg.value->tos(true);
        auto end = str.find(">");
        return cast(str.substr(1, end - 1));
    });
}

State::~State()
{
    // output times
    #ifdef OUT_TIMES
    system("printf ''");
    std::cout << "\033[38;5;15m";
    std::cout << "Interpreter time: " << (lextime + parsetime + evaltime).count() << "ms";
    std::cout << " (L:";
    if (lextime.count() >= parsetime.count() &&
        lextime.count() >= evaltime.count()) std::cout << "\033[38;5;11m";
    std::cout << lextime.count() << "ms";
    std::cout << "\033[38;5;15m";
    std::cout << ", P:";
    if (parsetime.count() >= lextime.count() &&
        parsetime.count() >= evaltime.count()) std::cout << "\033[38;5;11m";
    std::cout << parsetime.count() << "ms";
    std::cout << "\033[38;5;15m";
    std::cout << ", E:";
    if (evaltime.count() >= parsetime.count() &&
        evaltime.count() >= lextime.count()) std::cout << "\033[38;5;11m";
    std::cout << evaltime.count() << "ms";
    std::cout << "\033[38;5;15m";
    std::cout << ")\n";
    std::cout << "\033[0m";
    #endif
}

ValuePtr State::script(const std::string& path, bool asTuple)
{
    std::ifstream file(path);
    if (!file.is_open()) std::cout << "Could not open file " << path << "\n";
    std::string source((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    file.close();

    return eval(source, path, asTuple);
}

ValuePtr State::eval(const std::string& source, const std::string& path, bool asTuple)
{
    std::vector<Token> tokens;
    std::vector<ExprPtr> ast;

    // error handling
    err.path = &path;
    err.source = &source;
    err.tokens = &tokens;

    // Lexing
    #ifdef OUT_TIMES
    auto lstart = std::chrono::high_resolution_clock::now();
    #endif
    lexer.lex(source, tokens);
    #ifdef OUT_TIMES
    auto lend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> lduration = lend - lstart;
    lextime += std::chrono::duration_cast<std::chrono::milliseconds>(lduration);
    #endif

    #ifdef OUT_TOKENS
    std::cout << "----------- TOKENS -----------\n";
    for (Token& t : tokens) t.print();
    #endif

    // Parsing
    #ifdef OUT_TIMES
    auto pstart = std::chrono::high_resolution_clock::now();
    #endif
    parser.parse(tokens, ast);
    #ifdef OUT_TIMES
    auto pend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> pduration = pend - pstart;
    parsetime += std::chrono::duration_cast<std::chrono::milliseconds>(pduration);
    #endif

    #ifdef OUT_AST
    std::cout << "------------ AST -------------\n";
    for (ExprPtr e : ast) e->print();
    #endif

    // Evaluating
    #ifdef OUT_VALUES
    std::cout << "------------ EVAL ------------\n";
    #endif

    #ifdef OUT_TIMES
    auto estart = std::chrono::high_resolution_clock::now();
    #endif
    ValuePtr val = nullptr;
    for (ExprPtr e : ast)
    {
        val = evaler.eval(e, scope);
        #ifdef OUT_VALUES
        if (val == nullptr) std::cout << "->nullptr\n";
        else std::cout << "->" << val->tos(true) << "\n";
        #endif
    }
    #ifdef OUT_TIMES
    auto eend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> eduration = eend - estart;
    evaltime += std::chrono::duration_cast<std::chrono::milliseconds>(eduration);
    #endif

    // return as tuple
    if (asTuple)
    {
        auto tuple = std::make_shared<Tuple>(nullptr);
        tuple->scope = scope;
        return tuple;
    }

    // return as last value
    return val;
}

// ---------------------------------------

void State::bind(const std::string& name, const std::string& params, CPPFunc func)
{
    global.set(name, std::make_shared<Func>(func, params, &global, this));
}

// ---------------------------------------

ValuePtr State::cast(std::any val)
{
    if (val.type() == typeid(int))
    {
        return std::make_shared<Integer>(std::any_cast<int>(val), nullptr, this);
    }
    else if (val.type() == typeid(float))
    {
        return std::make_shared<Real>(std::any_cast<float>(val), nullptr, this);
    }
    else if (val.type() == typeid(bool))
    {
        return std::make_shared<Bool>(std::any_cast<bool>(val), nullptr, this);
    }
    else if (val.type() == typeid(std::string))
    {
        return std::make_shared<String>(std::any_cast<std::string>(val), nullptr, this);
    }
    else if (val.type() == typeid(std::vector<int>))
    {
        auto vec = std::any_cast<std::vector<int>>(val);
        auto tuple = std::make_shared<Tuple>(nullptr);
        for (uint i = 0; i < vec.size(); ++i)
        {
            ++static_cast<Tuple&>(*tuple).count;
            tuple->scope.set(std::to_string(i + ARRAY_BEGIN_INDEX),
                std::make_shared<Integer>(vec[i], &tuple->scope, this));
        }
        return tuple;
    }
    else return NIL;
}

OCA_END
