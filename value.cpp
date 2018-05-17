/* ollieberzs 2018
** value.cpp
** all oca types
*/

#include <iostream>
#include <sstream>
#include <cmath>
#include <cctype>
#include <regex>
#include <algorithm>
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

    bind("ascii", "", [&] CPPFUNC {
        int num = arg.caller->toi();
        char c = static_cast<char>(num);
        std::string empty = "";
        return cast(empty + c);
    });

    bind("real", "", [&] CPPFUNC {
        int num = arg.caller->toi();
        return cast(static_cast<float>(num));
    });
}

ValuePtr Integer::copy() {
    return std::make_shared<Integer>(*this);
}

std::string Integer::tos() {
    return std::to_string(val);
}

std::string Integer::typestr() {
    return "int";
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

    bind("floor", "", [&] CPPFUNC {
        float num = arg.caller->tor();
        return cast(static_cast<int>(std::floor(num)));
    });

    bind("ceil", "", [&] CPPFUNC {
        float num = arg.caller->tor();
        return cast(static_cast<int>(std::ceil(num)));
    });

    bind("round", "", [&] CPPFUNC {
        float num = arg.caller->tor();
        return cast(static_cast<int>(std::round(num)));
    });
}

ValuePtr Real::copy() {
    return std::make_shared<Real>(*this);
}

std::string Real::tos() {
    std::string str = std::to_string(val);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (str.back() == '.')
        str += '0';

    return str;
}

std::string Real::typestr() {
    return "real";
}

// ---------------------------------

String::String(const std::string& val, Scope* parent) : val(val) {
    scope = Scope(parent);

    // functions
    bind("__add", "a", [&] CPPFUNC {
        std::string left = arg.caller->tos();
        std::string right = arg.value->tos();
        return cast(left + right);
    });

    bind("__mul", "i", [&] CPPFUNC {
        std::string left = arg.caller->tos();
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
        std::string left = arg.caller->tos();
        std::string right = arg.value->tos();
        return cast(left == right);
    });

    bind("__neq", "s", [&] CPPFUNC {
        std::string left = arg.caller->tos();
        std::string right = arg.value->tos();
        return cast(left != right);
    });

    bind("len", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        return cast(static_cast<int>(str.size()));
    });

    bind("upcase", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        std::string result;
        for (char c : str)
            result += std::toupper(c);
        return cast(result);
    });

    bind("lowcase", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        std::string result;
        for (char c : str)
            result += std::tolower(c);
        return cast(result);
    });

    bind("int", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        return cast(std::stoi(str));
    });

    bind("real", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        return cast(std::stof(str));
    });

    bind("ascii", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        if (str.size() != 1)
            throw Error(CUSTOM_ERROR, "String must be 1 character long.");
        return cast(static_cast<int>(str.at(0)));
    });

    bind("find", "s", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        std::string regexString = arg.value->tos();
        std::regex regex(regexString);
        std::smatch match;
        if (std::regex_search(str, match, regex))
            return cast(static_cast<int>(match.position()));
        else
            return cast(-1);
    });

    bind("replace", "ss", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        std::string regexString = arg[0]->tos();
        std::string replaceString = arg[1]->tos();
        std::regex regex(regexString);
        return cast(std::regex_replace(str, regex, replaceString));
    });

    bind("at", "i", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        int index = arg.value->toi();
        if (index < 0 || index >= static_cast<int>(str.size()))
            throw Error(CUSTOM_ERROR, "Index " + std::to_string(index) + " out of bounds.");
        return cast(std::string() + str.at(index));
    });

    bind("each", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        Block& yield = static_cast<Block&>(*arg.yield);
        for (uint i = 0; i < str.size(); ++i) {
            auto tuple = std::make_shared<Tuple>(nullptr);
            tuple->add("0", cast(i));
            tuple->add("1", cast(std::string() + str.at(i)));
            yield(Nil::in(arg.caller->scope.parent), tuple, NIL);
        }
        return arg.caller;
    });
}

ValuePtr String::copy() {
    return std::make_shared<String>(*this);
}

std::string String::tos() {
    return val;
}

std::string String::typestr() {
    return "str";
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

ValuePtr Bool::copy() {
    return std::make_shared<Bool>(*this);
}

std::string Bool::tos() {
    return (val) ? "true" : "false";
}

std::string Bool::typestr() {
    return "bool";
}

// ---------------------------------

Tuple::Tuple(Scope* parent) {
    scope = Scope(parent);

    bind("size", "", [&] CPPFUNC {
        auto tuple = arg.caller;
        return cast(static_cast<int>(static_cast<Tuple&>(*tuple).size));
    });

    bind("insert", "ka", [&] CPPFUNC {
        auto& tuple = static_cast<Tuple&>(*arg.caller);
        std::string name = arg[0]->tos();
        if (std::isdigit(name[0])) {
            int index = std::stoi(name);
            if (index < 0 || index > tuple.count)
                throw Error(CUSTOM_ERROR, "Index " + name + " out of range(+1).");

            std::vector<ValuePtr> array(tuple.count);
            for (int i = 0; i < tuple.count; ++i)
                array[i] = tuple.scope.get(std::to_string(i), false);

            array.insert(array.begin() + index, arg[1]);
            tuple.add(std::to_string(tuple.count), cast(0));

            for (uint i = 0; i < array.size(); ++i)
                tuple.scope.set(std::to_string(i), array[i], true);
        } else
            tuple.add(name, arg[1]);
        return arg.caller;
    });

    bind("remove", "k", [&] CPPFUNC {
        auto& tuple = static_cast<Tuple&>(*arg.caller);
        std::string name = arg.value->tos();
        if (std::isdigit(name[0])) {
            int index = std::stoi(name);
            if (index < 0 || index >= tuple.count)
                throw Error(CUSTOM_ERROR, "Index " + name + " out of range.");

            std::vector<ValuePtr> array(tuple.count);
            for (int i = 0; i < tuple.count; ++i)
                array[i] = tuple.scope.get(std::to_string(i), false);

            array.erase(array.begin() + index);
            tuple.remove(std::to_string(tuple.count - 1));

            for (uint i = 0; i < array.size(); ++i)
                tuple.scope.set(std::to_string(i), array[i], true);

        } else if (!tuple.remove(name))
            throw Error(CUSTOM_ERROR, "Key '" + name + "' does not exist.");
        return arg.caller;
    });

    bind("at", "k", [&] CPPFUNC {
        auto tuple = arg.caller;
        std::string name = arg.value->tos();
        return tuple->scope.get(name, false);
    });

    bind("each", "", [&] CPPFUNC {
        auto& tuple = static_cast<Tuple&>(*arg.caller);
        Block& yield = static_cast<Block&>(*arg.yield);
        for (auto& var : tuple.scope.vars) {
            auto& vref = *var.value;
            if (TYPE_EQ(vref, Func))
                continue;
            auto param = std::make_shared<Tuple>(nullptr);
            param->add("0", cast(var.name));
            param->add("1", var.value);
            yield(Nil::in(arg.caller->scope.parent), param, NIL);
        }
        return arg.caller;
    });

    bind("sort", "", [&] CPPFUNC {
        auto& tuple = static_cast<Tuple&>(*arg.caller);
        Block& yield = static_cast<Block&>(*arg.yield);
        int count = tuple.count;

        std::vector<ValuePtr> array(count);
        for (int i = 0; i < count; ++i)
            array[i] = tuple.scope.get(std::to_string(i), false);

        std::sort(array.begin(), array.end(), [&](ValuePtr& a, ValuePtr& b) -> bool {
            auto param = std::make_shared<Tuple>(nullptr);
            param->add("0", a);
            param->add("1", b);
            return yield(Nil::in(arg.caller->scope.parent), param, NIL)->tob();
        });

        for (int i = 0; i < count; ++i)
            tuple.scope.set(std::to_string(i), array[i], true);

        return arg.caller;
    });
}

ValuePtr Tuple::copy() {
    // return std::make_shared<Tuple>(*this);
    return nullptr;
}

std::shared_ptr<Tuple> Tuple::from(Scope& scope) {
    auto t = std::make_shared<Tuple>(nullptr);
    t->scope = scope;
    return t;
}

void Tuple::add(const std::string& name, ValuePtr value) {
    scope.set(name, value, true);
    if (std::isdigit(name.at(0)))
        ++count;
    ++size;
}

bool Tuple::remove(const std::string& name) {
    if (scope.remove(name)) {
        if (std::isdigit(name.at(0)))
            --count;
        --size;
        return true;
    }
    return false;
}

std::string Tuple::tos() {
    std::string result = "(";
    for (int i = 0; i < count; ++i) {
        result += scope.get(std::to_string(i), true)->tos();
        result += ", ";
    }
    for (auto& var : scope.vars) {
        auto& vref = *var.value;
        if (TYPE_EQ(vref, Func))
            continue;
        if (std::isdigit(var.name[0]))
            continue;
        result += var.name + ": ";
        result += var.value->tos();
        result += ", ";
    }
    result.pop_back();
    result.pop_back();
    result += ")";
    return result;
}

std::string Tuple::typestr() {
    std::string result = "(";
    for (auto& var : scope.vars) {
        auto& vref = *var.value;
        if (TYPE_EQ(vref, Func))
            continue;
        result += var.value->typestr();
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

ValuePtr Block::copy() {
    return std::make_shared<Block>(*this);
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

std::string Block::tos() {
    const void* address = static_cast<const void*>(&*val);
    std::stringstream ss;
    ss << address;
    return ss.str();
}

std::string Block::typestr() {
    return "block";
}

// ---------------------------------

Func::Func(CPPFunc func, const std::string& params, Scope* parent) : val(func), params(params) {
    scope = Scope(parent);
}

ValuePtr Func::copy() {
    return std::make_shared<Func>(*this);
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
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted int.");
            break;
        case 'r':
            if (!v->isr())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted real.");
            break;
        case 'n':
            if (!v->isi() && !v->isr())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted int/real.");
            break;
        case 'b':
            if (!v->isb())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted bool.");
            break;
        case 's':
            if (!v->iss())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted str.");
            break;
        case 't':
            if (!v->ist())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted tuple.");
            break;
        case 'k':
            if (!v->isi() && !v->iss())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted int/str.");
            break;
        }
    }

    return val({caller, arg, block});
}

std::string Func::tos() {
    const void* address = static_cast<const void*>(&val);
    std::stringstream ss;
    ss << address;
    return ss.str();
}

std::string Func::typestr() {
    return "func";
}

// ---------------------------------

std::shared_ptr<Nil> Nil::in(Scope* parent) {
    auto n = std::make_shared<Nil>();
    n->scope.parent = parent;
    return n;
}

ValuePtr Nil::copy() {
    return std::make_shared<Nil>(*this);
}

bool Nil::isNil() {
    return true;
}

std::string Nil::tos() {
    return "nil";
}

std::string Nil::typestr() {
    return "nil";
}

OCA_END
