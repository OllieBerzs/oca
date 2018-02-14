/* ollieberzs 2018
** oca.hpp
** oca api
*/ 

#pragma once

#include <iostream>
#include <windows.h>
#include <fstream>

#include "common.hpp"
#include "lex.hpp"
#include "parse.hpp"
#include "scope.hpp"
#include "value.hpp"
#include "eval.hpp"

OCA_BEGIN

#define DLLEXPORT __declspec(dllexport) void

typedef void(*DLLfunc)(Scope&);
typedef std::vector<ValuePtr> Args;
typedef ValuePtr Ret;

void script(Scope& s, const std::string& source, const std::string& filename)
{
    // Lexing
    LexState ls;
    ls.source = source;
    ls.sourceName = filename;

    lex(ls);
    #ifdef OUT_TOKENS
    std::cout << "----------- TOKENS -----------\n";
    for (const Token& token : ls.tokens) printToken(token);
    #endif

    // Parsing
    ParseState ps;
    ps.ls = &ls;

    parse(ps);
    #ifdef OUT_AST
    std::cout << "------------ AST -------------\n";
    for (ExprPtr e : ps.exprs) e->print();
    #endif

    // Evaluating
    #ifdef OUT_VALUES
    std::cout << "------------ EVAL ------------\n";
    #endif
    for (ExprPtr e : ps.exprs)
    {
        ValuePtr val = eval(s, e);
        #ifdef OUT_VALUES
        if (val == nullptr) std::cout << "->nil\n";
        else 
        {
            std::cout << "->";
            val->print(true);
            std::cout << "\n";
        }
        #endif
    }
}

void scriptFile(Scope& s, const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) std::cout << "Could not open file " << path << "\n";
    std::string scriptPath((std::istreambuf_iterator<char>(file)),(std::istreambuf_iterator<char>()));
    file.close();

    script(s, scriptPath, path);
}


void loadLib(Scope& s, const std::string& lib)
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
    function(s);

    // free DLL
    //FreeLibrary(DLL);
}

void def(Scope& scope, const std::string& name, NativeMethod native)
{
    ValuePtr val = std::make_shared<Value>();
    val->type = "native";
    val->native = native;
    scope.set(name, val);
}

bool checkArgs(const std::string& pattern, const std::vector<ValuePtr>& args)
{
    if (pattern.size() > args.size() || args.size() > pattern.size())
    {
        std::cout << "method expected " << pattern.size() << " arguments " << args.size() << " were given\n";
        exit(0);
    }
    for (unsigned int i = 0; i < pattern.size(); i++)
    {
        if (pattern[i] == 'i' && args[i]->type != "int") return false;
        else if (pattern[i] == 'f' && args[i]->type != "float") return false;
        else if (pattern[i] == 'b' && args[i]->type != "bool") return false;
        else if (pattern[i] == 's' && args[i]->type != "str") return false;
    }
    return true;
}

OCA_END
