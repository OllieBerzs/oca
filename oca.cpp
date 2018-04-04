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

ValueCast::ValueCast(ValuePtr val, const std::string& name, State* state)
    : val(val), name(name), state(state) {}

// ---------------------------------------

void ValueCast::operator=(int v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Integer>(v, parent, state));
}

void ValueCast::operator=(float v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Real>(v, parent, state));
}

void ValueCast::operator=(const std::string& v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<String>(v, parent, state));
}

void ValueCast::operator=(bool v)
{
    Scope* parent = val->scope.parent;
    parent->set(name, std::make_shared<Bool>(v, parent, state));
}

ValueCast ValueCast::operator[](const std::string& name)
{
    ValuePtr var = val->scope.get(name);
    if (var->isNil())
    {
        val->scope.set(name, Nil::in(&val->scope));
        return ValueCast(val->scope.get(name), name, state);
    }
    return ValueCast(var, name, state);
}


// ---------------------------------------

ValuePtr Arg::operator[](uint i)
{
    return value->scope.get(std::to_string(i));
}

// ---------------------------------------

State::State() : lexer(this), parser(this), evaler(this), err(this)
{
    // add base functions
    bind("print", "a", [](Arg arg) -> Ret
    {
        std::cout << arg.value->tos(false) << "\n";
        return NIL;
    });
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
    lexer.lex(source, tokens);

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

    ValuePtr val = nullptr;
    for (ExprPtr e : ast)
    {
        val = evaler.eval(e, scope);
        #ifdef OUT_VALUES
        if (val == nullptr) std::cout << "->nullptr\n";
        else std::cout << "->" << val->tos(true) << "\n";
        #endif
    }

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

ValueCast State::operator[](const std::string& name)
{
    ValuePtr var = global.get(name);
    if (var->isNil())
    {
        global.set(name, Nil::in(&global));
        return ValueCast(global.get(name), name, this);
    }
    return ValueCast(var, name, this);
}

// ---------------------------------------

std::shared_ptr<Integer> State::cast(int val)
{
    return std::make_shared<Integer>(val, nullptr, this);
}

std::shared_ptr<Real> State::cast(float val)
{
    return std::make_shared<Real>(val, nullptr, this);
}

std::shared_ptr<Bool> State::cast(bool val)
{
    return std::make_shared<Bool>(val, nullptr, this);
}

std::shared_ptr<String> State::cast(const std::string& val)
{
    return std::make_shared<String>(val, nullptr, this);
}

std::shared_ptr<Tuple> State::cast(const std::vector<int>& val)
{
    auto tuple = std::make_shared<Tuple>(nullptr);
    for (uint i = 0; i < val.size(); ++i)
    {
        ++static_cast<Tuple&>(*tuple).count;
        tuple->scope.set(std::to_string(i + ARRAY_BEGIN_INDEX),
            std::make_shared<Integer>(val[i], &tuple->scope, this));
    }
    return tuple;
}

OCA_END
