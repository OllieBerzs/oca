/* ollieberzs 2018
** value.cpp
** all oca types
*/

#include <iostream>
#include <sstream>
#include <cmath>
#include "value.hpp"
#include "parse.hpp"
#include "oca.hpp"
#include "error.hpp"
#include "eval.hpp"

OCA_BEGIN

bool Value::isNil()
{
    return false;
}

void Value::bind(const std::string& name, const std::string& args, CPPFunc func)
{
    scope.set(name, std::make_shared<Func>(func, args, &scope, state));
}

ValueCast Value::operator[](const std::string& name)
{
    ValuePtr var = scope.get(name);
    if (var->isNil())
    {
        scope.set(name, Nil::in(&scope));
        return ValueCast(scope.get(name), name, state);
    }
    return ValueCast(var, name, state);
}

int Value::toi()
{
    if (TYPE_EQ(*this, Integer))
    {
        return static_cast<Integer&>(*this).val;
    }
    else return 0;
}

float Value::tor()
{
    if (TYPE_EQ(*this, Real))
    {
        return static_cast<Real&>(*this).val;
    }
    else return 0.0f;
}

bool Value::tob()
{
    if (TYPE_EQ(*this, Bool))
    {
        return static_cast<Bool&>(*this).val;
    }
    else return false;
}

bool Value::isi()
{
    if (TYPE_EQ(*this, Integer)) return true;
    else return false;
}

bool Value::isr()
{
    if (TYPE_EQ(*this, Real)) return true;
    else return false;
}

bool Value::isb()
{
    if (TYPE_EQ(*this, Bool)) return true;
    else return false;
}

bool Value::iss()
{
    if (TYPE_EQ(*this, String)) return true;
    else return false;
}

bool Value::ist()
{
    if (TYPE_EQ(*this, Tuple)) return true;
    else return false;
}

// ---------------------------------

Integer::Integer(ExprPtr expr, Scope* parent, State* state) : Integer(std::stoi(expr->val), parent, state) {}

Integer::Integer(int val, Scope* parent, State* state) : val(val)
{
    this->state = state;
    scope.parent = parent;

    // functions
    bind("__add", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left + arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left + arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__sub", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left - arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left - arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__mul", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left * arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left * arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__div", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left / arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left / arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__mod", "i", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return arg.state->cast(left % right);
    });

    bind("__pow", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        ValuePtr right = arg.value;
        if (right->isi()) return arg.state->cast(static_cast<int>(std::pow(left, right->toi())));
        if (right->isr()) return arg.state->cast(static_cast<float>(std::pow(left, right->tor())));
        return Nil::in(nullptr);
    });

    bind("__eq", "i", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return arg.state->cast(left == right);
    });

    bind("__gr", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left > arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left > arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__ls", "n", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        if (arg.value->isi()) return arg.state->cast(left < arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left < arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__geq", "i", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return arg.state->cast(left >= right);
    });

    bind("__leq", "i", [](Arg arg) -> Ret
    {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return arg.state->cast(left <= right);
    });

    bind("__ran", "i", [](Arg arg) -> Ret
    {
        int begin = arg.caller->toi();
        int end = arg.value->toi();
        std::vector<int> vec(end - begin + 1);
        int counter = 0;
        for (int i = begin; i <= end; ++i)
        {
            vec[counter] = i;
            ++counter;
        }
        return arg.state->cast(vec);
    });

    bind("times", "", [](Arg arg) -> Ret
    {
        int times = arg.caller->toi();
        Block& yield = static_cast<Block&>(*arg.yield);
        for (int i = 0; i < times; ++i)
        {
            yield(Nil::in(arg.caller->scope.parent), arg.state->cast(i), Nil::in(nullptr));
        }
        return Nil::in(nullptr);
    });
}

std::string Integer::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<int>";
    result += std::to_string(val);
    return result;
}

// ----------------------------------

Real::Real(ExprPtr expr, Scope* parent, State* state) : Real(std::stof(expr->val), parent, state) {}

Real::Real(float val, Scope* parent, State* state) : val(val)
{
    this->state = state;
    scope.parent = parent;

    // functions
    bind("__add", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        if (arg.value->isi()) return arg.state->cast(left + arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left + arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__sub", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        if (arg.value->isi()) return arg.state->cast(left - arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left - arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__mul", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        if (arg.value->isi()) return arg.state->cast(left * arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left * arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__div", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        if (arg.value->isi()) return arg.state->cast(left / arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left / arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__pow", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        ValuePtr right = arg.value;
        if (right->isi()) return arg.state->cast(static_cast<float>(std::pow(left, right->toi())));
        if (right->isr()) return arg.state->cast(static_cast<float>(std::pow(left, right->tor())));
        return Nil::in(nullptr);
    });

    bind("__gr", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        if (arg.value->isi()) return arg.state->cast(left > arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left > arg.value->tor());
        return Nil::in(nullptr);
    });

    bind("__ls", "n", [](Arg arg) -> Ret
    {
        float left = arg.caller->tor();
        if (arg.value->isi()) return arg.state->cast(left < arg.value->toi());
        if (arg.value->isr()) return arg.state->cast(left < arg.value->tor());
        return Nil::in(nullptr);
    });
}

std::string Real::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<real>";
    std::stringstream ss;
    ss << val;
    result += ss.str();
    return result;
}

// ---------------------------------

String::String(ExprPtr expr, Scope* parent, State* state) : String(expr->val, parent, state) {}

String::String(const std::string& val, Scope* parent, State* state) : val(val)
{
    this->state = state;
    scope.parent = parent;

    // functions
    bind("__add", "a", [](Arg arg) -> Ret
    {
        std::string left = arg.caller->tos(false);
        std::string right = arg.value->tos(false);
        return arg.state->cast(left + right);
    });

    bind("__eq", "s", [](Arg arg) -> Ret
    {
        std::string left = arg.caller->tos(false);
        std::string right = arg.value->tos(false);
        return arg.state->cast(left == right);
    });
}

std::string String::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<str>";
    result += val;
    return result;
}

// ---------------------------------

Bool::Bool(ExprPtr expr, Scope* parent, State* state) : Bool((expr->val == "true"), parent, state) {}

Bool::Bool(bool val, Scope* parent, State* state) : val(val)
{
    this->state = state;
    scope.parent = parent;

    bind("__eq", "b", [](Arg arg) -> Ret
    {
        bool left = arg.caller->tob();
        bool right = arg.value->tob();
        return arg.state->cast(left == right);
    });
}

std::string Bool::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<bool>";
    result += (val) ? "true" : "false";
    return result;
}

// ---------------------------------

Block::Block(ExprPtr expr, Scope* parent, State* state)
{
    this->state = state;
    scope.parent = parent;
    val = expr;
    // add functions
}

std::string Block::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<block>";
    const void* address = static_cast<const void*>(&*val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

ValuePtr Block::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block)
{
    Scope temp(&scope);
    std::vector<std::string> params;
    ValuePtr result = Nil::in(&scope);

    // get parameters
    std::string word = "";
    for (auto& c : val->val)
    {
        if (c == ' ')
        {
            params.push_back(word);
            word = "";
        }
        else word += c;
    }
    if (word != "") params.push_back(word);

    // set super and yield in scope
    if (!block->isNil()) temp.set("yield", block);
    temp.set("super", caller);

    // get argument count
    uint argc = 0;
    if (arg->ist()) argc = static_cast<Tuple&>(*arg).count;
    else if (!arg->isNil()) argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0) state->err.panic(NO_ARGUMENT, val);
    if (argc < params.size())
    {
        state->err.panic(SMALL_TUPLE, val,
            "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");
    }

    // set parameters
    if (params.size() == 1) temp.set(params[0], arg);
    else
    {
        uint counter = ARRAY_BEGIN_INDEX;
        for (auto& param : params)
        {
            ValuePtr item = arg->scope.get(param);
            if (item->isNil())
            {
                item = arg->scope.get(std::to_string(counter));
                ++counter;
            }
            if (item->isNil()) state->err.panic(CANNOT_SPLIT, val);

            temp.set(param, item);
        }
    }

    // evaluate the block's value
    ExprPtr expr = val;
    while (expr)
    {
        if (expr->left->type == Expression::RETURN) return state->evaler.eval(expr->left->right, temp);
        if (expr->left->type == Expression::BREAK) return result;
        result = state->evaler.eval(expr->left, temp);
        if (state->evaler.returning)
        {
            state->evaler.returning = false;
            return result;
        }
        expr = expr->right;
    }
    return result;
}

// ---------------------------------

Tuple::Tuple(Scope* parent)
{
    scope.parent = parent;
}

std::string Tuple::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<tup>";
    result += "(";
    for (auto& i : scope.names)
    {
        if (debug) result += "[" + i.first + "]";
        result += i.second->tos(debug);
        result += ", ";
    }
    result.pop_back();
    result.pop_back();
    result += ")";
    return result;
}

// ---------------------------------

Func::Func(CPPFunc func, const std::string& params, Scope* parent, State* state)
    : val(func), params(params)
{
    this->state = state;
    scope.parent = parent;
}

std::string Func::tos(bool debug)
{
    std::string result = "";
    if (debug) result += "<cppfunc>";
    const void* address = static_cast<const void*>(&val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

ValuePtr Func::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block)
{
    // get argument count
    uint argc = 0;
    if (arg->ist()) argc = static_cast<Tuple&>(*arg).count;
    else if (!arg->isNil()) argc = 1;
    if (arg->ist() && argc == 0) argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0) state->err.panic(NO_ARGUMENT, state->evaler.current);
    if (argc < params.size())
    {
        state->err.panic(SMALL_TUPLE, state->evaler.current,
            "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");
    }
    // if argc >= args.size ok!

    // check argument types
    for (uint i = 0; i < params.size(); ++i)
    {
        ValuePtr v = arg;
        if (argc > 1) v = arg->scope.get(std::to_string(i));
        switch (params[i])
        {
        case 'i':
            if (!v->isi()) state->err.panic(TYPE_MISMATCH, state->evaler.current,
                v->tos(true) + " wanted int.");
            break;
        case 'r':
            if (!v->isr()) state->err.panic(TYPE_MISMATCH, state->evaler.current,
                v->tos(true) + " wanted real.");
            break;
        case 'n':
            if (!v->isi() && !v->isr()) state->err.panic(TYPE_MISMATCH, state->evaler.current,
                v->tos(true) + " wanted int/real.");
            break;
        case 'b':
            if (!v->isb()) state->err.panic(TYPE_MISMATCH, state->evaler.current,
                    v->tos(true) + " wanted bool.");
            break;
        case 's':
            if (!v->iss()) state->err.panic(TYPE_MISMATCH, state->evaler.current,
                    v->tos(true) + " wanted str.");
            break;
        case 't':
            if (!v->ist()) state->err.panic(TYPE_MISMATCH, state->evaler.current,
                    v->tos(true) + " wanted tuple.");
            break;
        }
    }

    return val({caller, arg, block, state->evaler.state});
}

// ---------------------------------

std::shared_ptr<Nil> Nil::in(Scope* parent)
{
    auto n = std::make_shared<Nil>();
    n->scope.parent = parent;
    return n;
}

std::string Nil::tos(bool debug)
{
    return "nil";
}

bool Nil::isNil()
{
    return true;
}

OCA_END
