/* ollieberzs 2018
** oca.cpp
** oca api
*/

#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
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
    begin = std::chrono::high_resolution_clock::now();

    bind("print", "a", [&] CPPFUNC {
        std::cout << arg.value->tos();
        return NIL;
    });

    bind("println", "a", [&] CPPFUNC {
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

    bind("error", "s", [&] CPPFUNC {
        std::string message = arg.value->tos();
        throw Error(CUSTOM_ERROR, message);
        return NIL;
    });

    bind("type", "a", [&] CPPFUNC {
        std::string str = arg.value->typestr();
        return cast(str);
    });

    bind("abs", "n", [&] CPPFUNC {
        if (arg.value->isi()) {
            auto& val = static_cast<Integer&>(*arg.value);
            return cast(std::abs(val.val));
        } else {
            auto& val = static_cast<Real&>(*arg.value);
            return cast(std::abs(val.val));
        }
    });

    bind("acos", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::acos(val.val));
    });

    bind("asin", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::asin(val.val));
    });

    bind("atan", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::atan(val.val));
    });

    bind("acot", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::atan(1 / val.val));
    });

    bind("cos", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::cos(val.val));
    });

    bind("sin", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::sin(val.val));
    });

    bind("tan", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(std::tan(val.val));
    });

    bind("cot", "r", [&] CPPFUNC {
        auto& val = static_cast<Real&>(*arg.value);
        return cast(1 / std::tan(val.val));
    });

    bind("max", "nn", [&] CPPFUNC {
        if (arg[0]->isi()) {
            if (!arg[1]->isi())
                throw Error(CUSTOM_ERROR, "Expected similar types.");
            return cast(std::max(arg[0]->toi(), arg[1]->toi()));
        } else {
            if (!arg[1]->isr())
                throw Error(CUSTOM_ERROR, "Expected similar types.");
            return cast(std::fmax(arg[0]->tor(), arg[1]->tor()));
        }
    });

    bind("min", "nn", [&] CPPFUNC {
        if (arg[0]->isi()) {
            if (!arg[1]->isi())
                throw Error(CUSTOM_ERROR, "Expected similar types.");
            return cast(std::min(arg[0]->toi(), arg[1]->toi()));
        } else {
            if (!arg[1]->isr())
                throw Error(CUSTOM_ERROR, "Expected similar types.");
            return cast(std::fmin(arg[0]->tor(), arg[1]->tor()));
        }
    });

    bind("rad", "r", [&] CPPFUNC {
        oca_real halfc = 3.14159265358979323846 / 180.0;
        return cast(arg.value->tor() * halfc);
    });

    bind("deg", "r", [&] CPPFUNC {
        oca_real halfc = 3.14159265358979323846 / 180.0;
        return cast(arg.value->tor() / halfc);
    });

    bind("pi", "", [&] CPPFUNC { return cast(3.14159265358979323846); });

    bind("log", "rr", [&] CPPFUNC {
        oca_real base = arg[0]->tor();
        oca_real x = arg[1]->tor();
        return cast(std::log(x) / std::log(base));
    });

    bind("ln", "r", [&] CPPFUNC {
        oca_real x = arg.value->tor();
        return cast(std::log(x));
    });

    bind("lg", "r", [&] CPPFUNC {
        oca_real x = arg.value->tor();
        return cast(std::log10(x));
    });

    bind("random", "", [&] CPPFUNC {
        oca_real r = static_cast<oca_real>(std::rand()) / static_cast<oca_real>(RAND_MAX);
        return cast(r);
    });

    bind("seed", "i", [&] CPPFUNC {
        std::srand(arg.value->toi());
        return NIL;
    });

    bind("sqrt", "r", [&] CPPFUNC { return cast(std::sqrt(arg.value->tor())); });

    bind("cbrt", "r", [&] CPPFUNC { return cast(std::cbrt(arg.value->tor())); });

    bind("read", "s", [&] CPPFUNC {
        std::string path = arg.value->tos();
        std::ifstream file(path);
        if (!file.is_open())
            throw Error(CUSTOM_ERROR, "Could not open file '" + path + "'.");
        auto begin = std::istreambuf_iterator<char>(file);
        auto end = std::istreambuf_iterator<char>();
        std::string string(begin, end);
        file.close();

        return cast(string);
    });

    bind("write", "ss", [&] CPPFUNC {
        std::string path = arg[0]->tos();
        std::string string = arg[1]->tos();
        std::ofstream file(path);
        if (!file.is_open())
            throw Error(CUSTOM_ERROR, "Could not open file '" + path + "'.");
        file << string;
        file.close();

        return NIL;
    });

    bind("date", "", [&] CPPFUNC {
        auto now = std::chrono::system_clock::now();
        auto nowt = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&nowt), "%d-%m-%Y %X");
        return cast(ss.str());
    });

    bind("clock", "", [&] CPPFUNC {
        auto dur = std::chrono::high_resolution_clock::now() - begin;
        int milli = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        return cast(milli);
    });

    bind("execute", "s", [&] CPPFUNC {
        std::system(&arg.value->tos()[0]);
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

ValuePtr State::runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        std::cout << "Could not open file " << path << "\n";
    auto begin = std::istreambuf_iterator<char>(file);
    auto end = std::istreambuf_iterator<char>();
    std::string source(begin, end);
    file.close();

    eh.path = &path;
    return runString(source);
}

ValuePtr State::runString(const std::string& source) {
    try {
        eh.source = &source;
        auto tokens = lex(source);
        auto ast = parse(tokens);
        return evaluate(ast);
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

        if (input == "\n")
            continue;
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

ValuePtr State::evaluate(const std::vector<ExprPtr>& ast) {
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
        std::cout << "->" << val->tos() << "\n";
        #endif
    }

    #ifdef OUT_TIMES
    auto eend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> eduration = eend - estart;
    evaltime += std::chrono::duration_cast<std::chrono::milliseconds>(eduration);
    #endif

    return val;
}

OCA_END
