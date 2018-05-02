/* ollieberzs 2018
** oca.cpp
** oca api
*/

#include <fstream>
#include <iostream>
#include "oca.hpp"
#include "utils.hpp"

OCA_BEGIN

ValuePtr Arg::operator[](uint i) {
    return value->scope.get(std::to_string(i), false);
}

// ---------------------------------------

State::State()
    : global(nullptr), scope(nullptr), evaler(this), eh(this), lextime(0), parsetime(0),
      evaltime(0) {
    bind("print", "a", [&] CPPFUNC {
        std::cout << arg.value->tos() << "\n";
        return NIL;
    });

    bind("debug", "a", [&] CPPFUNC {
        std::cout << arg.value->tos() << " of type " << arg.value->typestr() << " at "
                  << arg.value.get() << "\n";
        return NIL;
    });

    bind("input", "", [&] CPPFUNC {
        std::string result;
        std::cin >> result;
        return cast(result);
    });

    bind("pause", "", [&] CPPFUNC {
        std::cin.get();
        return NIL;
    });

    bind("assert", "bs", [&] CPPFUNC {
        bool cond = arg[0]->tob();
        std::string message = arg[1]->tos();
        if (!cond)
            throw Error(CUSTOM_ERROR, message);
        return NIL;
    });

    bind("type", "a", [&] CPPFUNC {
        std::string str = arg.value->typestr();
        return cast(str);
    });

    bind("inject", "t", [&, this] CPPFUNC {
        auto rightScope = dynamic_cast<Tuple&>(*arg.value).scope;
        scope.add(rightScope);
        return NIL;
    });
}

State::~State() {
    // output times
    #ifdef OUT_TIMES

    enableANSI();

    std::cout << ESC "38;5;15m";
    std::cout << "Interpreter time: " << (lextime + parsetime + evaltime).count() << "ms";
    std::cout << " (L:";
    if (lextime.count() >= parsetime.count() && lextime.count() >= evaltime.count())
        std::cout << ESC "38;5;11m";
    std::cout << lextime.count() << "ms";
    std::cout << ESC "38;5;15m";
    std::cout << ", P:";
    if (parsetime.count() >= lextime.count() && parsetime.count() >= evaltime.count())
        std::cout << ESC "38;5;11m";
    std::cout << parsetime.count() << "ms";
    std::cout << ESC "38;5;15m";
    std::cout << ", E:";
    if (evaltime.count() >= parsetime.count() && evaltime.count() >= lextime.count())
        std::cout << ESC "38;5;11m";
    std::cout << evaltime.count() << "ms";
    std::cout << ESC "38;5;15m";
    std::cout << ")\n";
    std::cout << ESC "0m";
    #endif
}

ValuePtr State::runFile(const std::string& path, bool asTuple) {
    std::ifstream file(path);
    if (!file.is_open())
        std::cout << "Could not open file " << path << "\n";
    auto begin = std::istreambuf_iterator<char>(file);
    auto end = std::istreambuf_iterator<char>();
    std::string source(begin, end);
    file.close();

    eh.path = &path;
    return runString(source, asTuple);
}

ValuePtr State::runString(const std::string& source, bool asTuple) {
    try {
        eh.source = &source;
        auto tokens = lex(source);
        auto ast = parse(tokens);
        return evaluate(ast, asTuple);
    } catch (Error& e) {
        eh.panic(e);
        return NIL;
    }
}

void State::runREPL() {
    enableANSI();
    while (true) {
        std::cout << ESC "38;5;15m"
                  << "[] " << ESC "0m";

        std::string input;
        while (true) {
            std::string line;
            std::getline(std::cin, line);

            if (line.back() == '`') {
                line.pop_back();
                input += line + '\n';
                std::cout << ESC "38;5;15m"
                          << "-] " << ESC "0m";
            } else {
                input += line + '\n';
                break;
            }
        }

        if (input == "exit\n")
            return;

        auto val = runString(input);

        std::cout << ESC "38;5;8m" << val->tos() << ESC "0m\n";
    }
}

// ---------------------------------------

void State::bind(const std::string& name, const std::string& params, CPPFunc func) {
    global.set(name, std::make_shared<Func>(func, params, &global), true);
}

// ---------------------------------------

std::vector<Token> State::lex(const std::string& source) {
    #ifdef OUT_TIMES
    auto lstart = std::chrono::high_resolution_clock::now();
    #endif

    auto tokens = lexer.tokenize(source);
    eh.tokens = &tokens;

    #ifdef OUT_TIMES
    auto lend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> lduration = lend - lstart;
    lextime += std::chrono::duration_cast<std::chrono::milliseconds>(lduration);
    #endif

    #ifdef OUT_TOKENS
    std::cout << "----------- TOKENS -----------\n";
    for (Token& t : tokens)
        t.print();
    #endif

    return tokens;
}

std::vector<ExprPtr> State::parse(const std::vector<Token>& tokens) {
    #ifdef OUT_TIMES
    auto pstart = std::chrono::high_resolution_clock::now();
    #endif

    auto ast = parser.makeAST(tokens);

    #ifdef OUT_TIMES
    auto pend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> pduration = pend - pstart;
    parsetime += std::chrono::duration_cast<std::chrono::milliseconds>(pduration);
    #endif

    #ifdef OUT_AST
    std::cout << "------------ AST -------------\n";
    for (ExprPtr e : ast)
        e->print();
    #endif

    return ast;
}

ValuePtr State::evaluate(const std::vector<ExprPtr>& ast, bool asTuple) {
    #ifdef OUT_VALUES
    std::cout << "------------ EVAL ------------\n";
    #endif

    #ifdef OUT_TIMES
    auto estart = std::chrono::high_resolution_clock::now();
    #endif

    ValuePtr val = nullptr;
    for (ExprPtr e : ast) {
        val = evaler.eval(e, scope);

        #ifdef OUT_VALUES
        std::cout << "->" << val->tos(true) << "\n";
        #endif
    }

    #ifdef OUT_TIMES
    auto eend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> eduration = eend - estart;
    evaltime += std::chrono::duration_cast<std::chrono::milliseconds>(eduration);
    #endif

    if (asTuple)
        return Tuple::from(scope);
    return val;
}

OCA_END
