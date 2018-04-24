/* ollieberzs 2018
** value.cpp
** all oca types
*/

#include <iostream>
#include <sstream>
#include <cmath>
#include <cctype>
#include "oca.hpp"
#include "utils.hpp"

OCA_BEGIN

bool Value::isNil() {
    return false;
}

void Value::bind(const std::string& name, const std::string& args, CPPFunc func) {
    scope.set(name, std::make_shared<Func>(func, args, &scope), true);
}

int Value::toi() {
    if (TYPE_EQ(*this, Integer))
        return static_cast<Integer&>(*this).val;
    else
        return 0;
}

float Value::tor() {
    if (TYPE_EQ(*this, Real))
        return static_cast<Real&>(*this).val;
    else
        return 0.0f;
}

bool Value::tob() {
    if (TYPE_EQ(*this, Bool))
        return static_cast<Bool&>(*this).val;
    else
        return false;
}

bool Value::isi() {
    if (TYPE_EQ(*this, Integer))
        return true;
    else
        return false;
}

bool Value::isr() {
    if (TYPE_EQ(*this, Real))
        return true;
    else
        return false;
}

bool Value::isb() {
    if (TYPE_EQ(*this, Bool))
        return true;
    else
        return false;
}

bool Value::iss() {
    if (TYPE_EQ(*this, String))
        return true;
    else
        return false;
}

bool Value::ist() {
    if (TYPE_EQ(*this, Tuple))
        return true;
    else
        return false;
}

// ---------------------------------

Integer::Integer(int val, Scope* parent) : val(val) {
    scope = Scope(parent);

    // functions
    bind("__add", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left + arg.value->toi());
        if (arg.value->isr())
            return cast(left + arg.value->tor());
        return NIL;
    });

    bind("__sub", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left - arg.value->toi());
        if (arg.value->isr())
            return cast(left - arg.value->tor());
        return NIL;
    });

    bind("__mul", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left * arg.value->toi());
        if (arg.value->isr())
            return cast(left * arg.value->tor());
        return NIL;
    });

    bind("__div", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left / arg.value->toi());
        if (arg.value->isr())
            return cast(left / arg.value->tor());
        return NIL;
    });

    bind("__mod", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left % right);
    });

    bind("__pow", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        ValuePtr right = arg.value;
        if (right->isi())
            return cast(static_cast<int>(std::pow(left, right->toi())));
        if (right->isr())
            return cast(static_cast<float>(std::pow(left, right->tor())));
        return NIL;
    });

    bind("__eq", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left == right);
    });

    bind("__neq", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left != right);
    });

    bind("__gr", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left > arg.value->toi());
        if (arg.value->isr())
            return cast(left > arg.value->tor());
        return NIL;
    });

    bind("__ls", "n", [&] CPPFUNC {
        int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left < arg.value->toi());
        if (arg.value->isr())
            return cast(left < arg.value->tor());
        return NIL;
    });

    bind("__geq", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left >= right);
    });

    bind("__leq", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left <= right);
    });

    bind("__ran", "i", [&] CPPFUNC {
        int begin = arg.caller->toi();
        int end = arg.value->toi();
        std::vector<int> vec(end - begin + 1);
        int counter = 0;
        for (int i = begin; i <= end; ++i) {
            vec[counter] = i;
            ++counter;
        }
        return cast(vec);
    });

    bind("__and", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left & right);
    });

    bind("__or", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left | right);
    });

    bind("__xor", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left ^ right);
    });

    bind("__lsh", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left << right);
    });

    bind("__rsh", "i", [&] CPPFUNC {
        int left = arg.caller->toi();
        int right = arg.value->toi();
        return cast(left >> right);
    });

    bind("times", "", [&] CPPFUNC {
        int times = arg.caller->toi();
        Block& yield = static_cast<Block&>(*arg.yield);
        for (int i = 0; i < times; ++i) {
            yield(Nil::in(arg.caller->scope.parent), cast(i), NIL);
        }
        return NIL;
    });
}

std::string Integer::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<int>";
    result += std::to_string(val);
    return result;
}

// ----------------------------------

Real::Real(float val, Scope* parent) : val(val) {
    scope = Scope(nullptr);
    scope.parent = parent;

    // functions
    bind("__add", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left + arg.value->toi());
        if (arg.value->isr())
            return cast(left + arg.value->tor());
        return NIL;
    });

    bind("__sub", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left - arg.value->toi());
        if (arg.value->isr())
            return cast(left - arg.value->tor());
        return NIL;
    });

    bind("__mul", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left * arg.value->toi());
        if (arg.value->isr())
            return cast(left * arg.value->tor());
        return NIL;
    });

    bind("__div", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left / arg.value->toi());
        if (arg.value->isr())
            return cast(left / arg.value->tor());
        return NIL;
    });

    bind("__pow", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        ValuePtr right = arg.value;
        if (right->isi())
            return cast(static_cast<float>(std::pow(left, right->toi())));
        if (right->isr())
            return cast(static_cast<float>(std::pow(left, right->tor())));
        return NIL;
    });

    bind("__gr", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left > arg.value->toi());
        if (arg.value->isr())
            return cast(left > arg.value->tor());
        return NIL;
    });

    bind("__ls", "n", [&] CPPFUNC {
        float left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left < arg.value->toi());
        if (arg.value->isr())
            return cast(left < arg.value->tor());
        return NIL;
    });
}

std::string Real::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<real>";

    // format real
    std::string str = std::to_string(val);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (str.back() == '.')
        str += '0';

    result += str;
    return result;
}

// ---------------------------------

String::String(const std::string& val, Scope* parent) : val(val) {
    scope = Scope(parent);

    // functions
    bind("__add", "a", [&] CPPFUNC {
        std::string left = arg.caller->tos(false);
        std::string right = arg.value->tos(false);
        return cast(left + right);
    });

    bind("__mul", "i", [&] CPPFUNC {
        std::string left = arg.caller->tos(false);
        int right = arg.value->toi();
        std::string result = "";
        if (right <= 0)
            return cast(result); // TODO: should error
        for (int i = 0; i < right; ++i) {
            result += left;
        }
        return cast(result);
    });

    bind("__eq", "s", [&] CPPFUNC {
        std::string left = arg.caller->tos(false);
        std::string right = arg.value->tos(false);
        return cast(left == right);
    });

    bind("__neq", "s", [&] CPPFUNC {
        std::string left = arg.caller->tos(false);
        std::string right = arg.value->tos(false);
        return cast(left != right);
    });

    bind("len", "", [&] CPPFUNC {
        std::string str = arg.caller->tos(false);
        return cast(static_cast<int>(str.size()));
    });

    bind("upcase", "", [&] CPPFUNC {
        std::string str = arg.caller->tos(false);
        std::string result;
        for (char c : str)
            result += std::toupper(c);
        return cast(result);
    });

    bind("lowcase", "", [&] CPPFUNC {
        std::string str = arg.caller->tos(false);
        std::string result;
        for (char c : str)
            result += std::tolower(c);
        return cast(result);
    });

    bind("int", "", [&] CPPFUNC {
        std::string str = arg.caller->tos(false);
        return cast(std::stoi(str));
    });

    bind("real", "", [&] CPPFUNC {
        std::string str = arg.caller->tos(false);
        return cast(std::stof(str));
    });
}

std::string String::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<str>";
    result += val;
    return result;
}

// ---------------------------------

Bool::Bool(bool val, Scope* parent) : val(val) {
    scope = Scope(nullptr);
    scope.parent = parent;

    bind("__eq", "b", [&] CPPFUNC {
        bool left = arg.caller->tob();
        bool right = arg.value->tob();
        return cast(left == right);
    });

    bind("__neq", "b", [&] CPPFUNC {
        bool left = arg.caller->tob();
        bool right = arg.value->tob();
        return cast(left != right);
    });

    bind("__and", "b", [&] CPPFUNC {
        bool left = arg.caller->tob();
        bool right = arg.value->tob();
        return cast(left && right);
    });

    bind("__or", "b", [&] CPPFUNC {
        bool left = arg.caller->tob();
        bool right = arg.value->tob();
        return cast(left || right);
    });
}

std::string Bool::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<bool>";
    result += (val) ? "true" : "false";
    return result;
}

// ---------------------------------

Tuple::Tuple(Scope* parent) {
    scope = Scope(parent);
}

std::shared_ptr<Tuple> Tuple::from(Scope& scope) {
    auto t = std::make_shared<Tuple>(nullptr);
    t->scope = scope;
    return t;
}

void Tuple::add(const std::string& name, std::any val) {
    auto vptr = cast(val);
    vptr->scope.parent = &scope;
    scope.set(name, vptr, true);
}

std::string Tuple::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<tup>";
    result += "(";
    for (auto var : scope.vars) {
        if (debug)
            result += "[" + std::get<1>(var) + "]";
        result += std::get<2>(var)->tos(debug);
        result += ", ";
    }
    result.pop_back();
    result.pop_back();
    result += ")";
    return result;
}

// ---------------------------------

Block::Block(ExprPtr expr, Scope* parent, Evaluator* evaler) : evaler(evaler) {
    scope = Scope(parent);
    val = expr;

    // set parameters
    std::string param = "";
    for (char c : expr->val) {
        if (c == ' ') {
            params.push_back(param);
            param = "";
        } else
            param += c;
    }
    if (param != "")
        params.push_back(param);
}

std::string Block::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<block>";
    const void* address = static_cast<const void*>(&*val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

ValuePtr Block::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block) {
    // get argument count
    uint argc = 0;
    if (arg->ist())
        argc = static_cast<Tuple&>(*arg).count;
    else if (!arg->isNil())
        argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0)
        throw Error(NO_ARGUMENT);
    if (argc < params.size())
        throw Error(
            SMALL_TUPLE, "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");

    // set super and yield in scope
    Scope temp(&scope);
    temp.set("yield", block, true);
    temp.set("super", caller, true);

    // set parameters
    if (params.size() == 1)
        temp.set(params[0], arg, true);
    else {
        uint counter = ARRAY_BEGIN_INDEX;
        for (auto& param : params) {
            ValuePtr item = arg->scope.get(std::to_string(counter), false);
            if (item->isNil())
                throw Error(CANNOT_SPLIT);
            ++counter;

            temp.set(param, item, true);
        }
    }

    // evaluate the block's value
    ValuePtr result = Nil::in(&scope);
    ExprPtr expr = val;
    while (expr) {
        if (expr->left->type == Expression::RETURN)
            return evaler->eval(expr->left->right, temp);
        if (expr->left->type == Expression::BREAK)
            return result;
        result = evaler->eval(expr->left, temp);
        if (evaler->returning) {
            evaler->returning = false;
            return result;
        }
        expr = expr->right;
    }
    return result;
}

// ---------------------------------

Func::Func(CPPFunc func, const std::string& params, Scope* parent) : val(func), params(params) {
    scope = Scope(parent);
}

std::string Func::tos(bool debug) {
    std::string result = "";
    if (debug)
        result += "<cppfunc>";
    const void* address = static_cast<const void*>(&val);
    std::stringstream ss;
    ss << address;
    result += ss.str();
    return result;
}

ValuePtr Func::operator()(ValuePtr caller, ValuePtr arg, ValuePtr block) {
    // get argument count
    uint argc = 0;
    if (arg->ist())
        argc = static_cast<Tuple&>(*arg).count;
    else if (!arg->isNil())
        argc = 1;
    if (arg->ist() && argc == 0)
        argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0)
        throw Error(NO_ARGUMENT);
    if (argc < params.size())
        throw Error(
            SMALL_TUPLE, "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");

    // check argument types
    for (uint i = 0; i < params.size(); ++i) {
        ValuePtr v = arg;
        if (argc > 1 && params.size() > 1)
            v = arg->scope.get(std::to_string(i), false);
        switch (params[i]) {
        case 'i':
            if (!v->isi())
                throw Error(TYPE_MISMATCH, v->tos(true) + " wanted int.");
            break;
        case 'r':
            if (!v->isr())
                throw Error(TYPE_MISMATCH, v->tos(true) + " wanted real.");
            break;
        case 'n':
            if (!v->isi() && !v->isr())
                throw Error(TYPE_MISMATCH, v->tos(true) + " wanted int/real.");
            break;
        case 'b':
            if (!v->isb())
                throw Error(TYPE_MISMATCH, v->tos(true) + " wanted bool.");
            break;
        case 's':
            if (!v->iss())
                throw Error(TYPE_MISMATCH, v->tos(true) + " wanted str.");
            break;
        case 't':
            if (!v->ist())
                throw Error(TYPE_MISMATCH, v->tos(true) + " wanted tuple.");
            break;
        }
    }

    return val({caller, arg, block});
}

// ---------------------------------

std::shared_ptr<Nil> Nil::in(Scope* parent) {
    auto n = std::make_shared<Nil>();
    n->scope.parent = parent;
    return n;
}

std::string Nil::tos(bool debug) {
    return "nil";
}

bool Nil::isNil() {
    return true;
}

OCA_END
