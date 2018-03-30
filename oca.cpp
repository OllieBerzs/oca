/* ollieberzs 2018
** oca.cpp
** oca api
*/

#include <iostream>
#include <fstream>
#include "oca.hpp"
#include "lex.hpp"
#include "parse.hpp"
#include "eval.hpp"
#include "error.hpp"

OCA_BEGIN

ValueCast::ValueCast(ValuePtr val, const std::string& name, Evaluator* e)
    : val(val), name(name), evaler(e) {}

// ---------------------------------------

void ValueCast::operator=(int v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Integer>(v, parent, evaler));
}

void ValueCast::operator=(float v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Real>(v, parent, evaler));
}

void ValueCast::operator=(const std::string& v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<String>(v, parent, evaler));
}

void ValueCast::operator=(bool v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Bool>(v, parent, evaler));
}

void ValueCast::operator=(CPPFunc v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Func>(v, parent));
}

ValueCast ValueCast::operator[](const std::string& name)
{
    ValuePtr var = val->scope.get(name);
    if (var->isNil())
    {
        val->scope.set(name, Nil::in(&val->scope));
        return ValueCast(val->scope.get(name), name, evaler);
    }
    return ValueCast(var, name, evaler);
}

// ---------------------------------------

State::State() : evaler(this)
{
    // add base functions
    (*this)["print"] = [](Arg arg) -> Ret
    {
        std::cout << arg.value->toStr(false) << "\n";
        return NIL;
    };
}

State::~State()
{
    if (Errors::instance().count() == 1) Errors::instance().end();
}

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
    std::vector<Token> tokens;
    std::vector<ExprPtr> ast;

    // error handling
    if (path != "" || Errors::instance().count() == 0)
        Errors::instance().begin(&path, &source, &tokens, &parser);

    // Lexing
    lex(source, tokens);

    #ifdef OUT_TOKENS
    std::cout << "----------- TOKENS -----------\n";
    for (Token& t : tokens) t.print();
    #endif

    // Parsing
    parser.parse(tokens, ast);

    #ifdef OUT_AST
    std::cout << "------------ AST -------------\n";
    for (ExprPtr e : ast) e->print();
    #endif

    // Evaluating
    #ifdef OUT_VALUES
    std::cout << "------------ EVAL ------------\n";
    #endif

    ValuePtr obj = nullptr;
    for (ExprPtr e : ast)
    {
        obj = evaler.eval(e, scope);
        #ifdef OUT_VALUES
        if (obj == nullptr) std::cout << "->nil\n";
        else std::cout << "->" << obj->toStr(true) << "\n";
        #endif
    }

    // pop file
    if (path != "") Errors::instance().end();

    return obj;
}

// ---------------------------------------

ValueCast State::operator[](const std::string& name)
{
    ValuePtr var = global.get(name);
    if (var->isNil())
    {
        global.set(name, Nil::in(&global));
        return ValueCast(global.get(name), name, &evaler);
    }
    return ValueCast(var, name, &evaler);
}

// ---------------------------------------

std::shared_ptr<Integer> State::cast(int val)
{
    return std::make_shared<Integer>(val, nullptr, &evaler);
}

std::shared_ptr<Real> State::cast(float val)
{
    return std::make_shared<Real>(val, nullptr, &evaler);
}

std::shared_ptr<Bool> State::cast(bool val)
{
    return std::make_shared<Bool>(val, nullptr, &evaler);
}

std::shared_ptr<String> State::cast(std::string val)
{
    return std::make_shared<String>(val, nullptr, &evaler);
}

OCA_END
