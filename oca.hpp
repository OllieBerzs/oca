#pragma once

#include <iostream>
#include <windows.h>
#include <fstream>
#include "interpret.hpp"
#include "errors.hpp"
#include "api.hpp"

namespace oca
{
    void script(State& state, const std::string& source, const std::string& filename)
    {
        internal::errors::Script::data = source;
        internal::errors::Script::file = filename;

        // Lexing
        std::vector<internal::Token> tokens;
        internal::lex(source, tokens);
        #ifdef OUT_TOKENS
            std::cout << "----------- TOKENS -----------\n";
            for (const Token& token : tokens) std::cout << token << "\n";
        #endif

        // Parsing
        std::vector<internal::Expression*> exprs;
        internal::parse(tokens, exprs);
        #ifdef OUT_TREE
            std::cout << "------------ TREE ------------\n";
            for (const internal::Expression* e : exprs) std::cout << *e << "\n";
        #endif

        // Evaluating
        std::cout << "Running ------------ " << filename << "\n";
        for (auto e : exprs)
        {
            internal::Value* val = internal::evaluate(&state, e);
            #ifdef OUT_VALUES
                if (val->type == "nil") std::cout << "->nil\n";
                else std::cout << "->" << val->expr->value << "\n";
            #endif
            if (val->refCount == 0) delete val;
        }
    }

    void scriptFile(State& state, const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open()) std::cout << "Could not open file " << path << "\n";
        std::string scriptPath((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        file.close();

        script(state, scriptPath, path);
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
