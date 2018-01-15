#pragma once

#include <iostream>
#include <windows.h>
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "errors.hpp"
#include "utils.hpp"

namespace oca
{
    namespace internal
    {
        void runScript(Scope& scope, const std::string& source, const std::string& filename)
        {
            errors::Script::data = source;
            errors::Script::file = filename;

            // Lexing
            std::vector<Token> tokens;
            lex(source, tokens);
#ifdef OUT_TOKENS
            std::cout << "----------- TOKENS -----------\n";
            for (const Token& token : tokens) std::cout << token << "\n";
#endif

            // Parsing
            std::vector<Expression*> exprs;
            parse(tokens, exprs);
#ifdef OUT_TREE
            std::cout << "------------ TREE ------------\n";
            for (const Expression* e : exprs) std::cout << *e << "\n";
#endif

            // Evaluating
            std::cout << "Running ------------ " << filename << "\n";
            for (auto e : exprs)
            {
                Value* val = evaluate(&scope, e);
#ifdef OUT_VALUES
                if (val->type == "nil") std::cout << "->nil\n";
                else std::cout << "->" << val->type << " " << val->expr->value << "\n";
#endif
                if (val->refCount == 0) delete val;
            }
        }
    }

    // api
    typedef internal::Scope State;
    #define NIL new internal::Value("nil", nullptr)

    void script(State& state, const std::string& source)
    {
        internal::runScript(state, source, "");
    }
    void scriptFile(State& state, const std::string& path)
    {
        internal::runScript(state, internal::readFile(path), path);
    }

    void def(State& state, const std::string& name, internal::NativeMethod native)
    {
        internal::Value* val = new internal::Value("native", nullptr);
        val->refCount++;
        val->native = native;
        state.set(name, val);
    }

    typedef void(*DLLfunc)();
    void loadLib(State& state)
    {
        HINSTANCE DLL = LoadLibrary("lib/lib.ocalib");

        if (!DLL)
        {
            std::cout << "could not load the dynamic library\n";
            std::cin.get();
            exit(1);
        }

        // resolve function address here
        DLLfunc function = (DLLfunc)GetProcAddress(DLL, "function");

        // call function
        if (function) function();

        // free DLL
        FreeLibrary(DLL);

        // print
        /*def(state, "print", [](auto args) -> auto
        {
            std::cout << args[0]->expr->value << "\n";
            return NIL;
        });*/
        
    }
} // namespace oca
