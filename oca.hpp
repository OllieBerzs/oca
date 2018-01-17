#pragma once

#include <iostream>
#include <windows.h>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"

#include "errors.hpp"
#include "utils.hpp"
#include "api.hpp"

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
    } // namespace internal

    void script(State& state, const std::string& source)
    {
        internal::runScript(state, source, "");
    }
    void scriptFile(State& state, const std::string& path)
    {
        internal::runScript(state, internal::readFile(path), path);
    }

    typedef void(*DLLfunc)(State&);
    void loadLib(State& state, const std::string& lib)
    {
        char path[MAX_PATH];
        GetFullPathNameA((lib + ".ocalib").c_str(), MAX_PATH, path, nullptr);
        HINSTANCE DLL = LoadLibraryA(path);

        if (!DLL)
        {
            std::cout << "could not load " << path << "\n";
            std::cin.get();
            exit(1);
        }

        // resolve function address here
        DLLfunc function = (DLLfunc)GetProcAddress(DLL, "load");

        // call function
        if (!function) 
        {
            std::cout << "load function not found\n";
            std::cin.get();
            exit(1);
        }
        function(state);

        // free DLL
        //FreeLibrary(DLL);
    }
} // namespace oca
