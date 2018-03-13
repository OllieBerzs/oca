/* ollieberzs 2018
** oca.cpp
** oca api
*/

#include <iostream>
//#include <windows.h>
#include <fstream>

#include "oca.hpp"
#include "lex.hpp"
#include "parse.hpp"
#include "eval.hpp"

OCA_BEGIN

ValuePtr State::script(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) std::cout << "Could not open file " << path << "\n";
    std::string source((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    file.close();

    return eval(source, path);
}

ValuePtr State::eval(const std::string& source, const std::string& path)
{
    // Lexing
    auto tokens = Lexer{source, path}.lex();

    #ifdef OUT_TOKENS
    std::cout << "----------- TOKENS -----------\n";
    for (Token& t : tokens) t.print();
    #endif

    // Parsing
    auto ast = Parser(tokens, path).parse();

    #ifdef OUT_AST
    std::cout << "------------ AST -------------\n";
    for (ExprPtr e : ast) e->print();
    #endif

    // Evaluating
    #ifdef OUT_VALUES
    std::cout << "------------ EVAL ------------\n";
    #endif

    Evaluator ev;
    ValuePtr obj = nullptr;
    for (ExprPtr e : ast)
    {
        obj = ev.eval(e, scope);
        #ifdef OUT_VALUES
        if (obj == nullptr) std::cout << "->nil\n";
        else std::cout << "->" << obj->toStr(true) << "\n";
        #endif
    }

    return obj;
}

// ---------------------------------------

/*void State::load(const std::string& lib)
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
    function(scope);

    // free DLL
    //FreeLibrary(DLL);
}*/

void State::set(const std::string& name, CPPFunc func)
{
    ValuePtr val = std::make_shared<Func>(func, &scope);
    scope.set(name, val);
}

OCA_END
