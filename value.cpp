/* ollieberzs 2018
** value.cpp
** all oca types
*/

#include <iostream>
#include <sstream>
#include <cmath>
#include <cctype>
#include <regex>
#include <iomanip>
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

oca_int Value::toi() {
    if (TYPE_EQ(*this, Integer))
        return static_cast<Integer&>(*this).val;
    else
        return 0;
}

oca_real Value::tor() {
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
    if (TYPE_EQ(*this, Table))
        return true;
    else
        return false;
}

// ---------------------------------

Integer::Integer(oca_int val, Scope* parent) : val(val) {
    scope = Scope(parent);

    // functions
    bind("__add", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left + arg.value->toi());
        if (arg.value->isr())
            return cast(left + arg.value->tor());
        return NIL;
    });

    bind("__sub", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left - arg.value->toi());
        if (arg.value->isr())
            return cast(left - arg.value->tor());
        return NIL;
    });

    bind("__mul", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left * arg.value->toi());
        if (arg.value->isr())
            return cast(left * arg.value->tor());
        return NIL;
    });

    bind("__div", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left / arg.value->toi());
        if (arg.value->isr())
            return cast(left / arg.value->tor());
        return NIL;
    });

    bind("__mod", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left % right);
    });

    bind("__pow", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        ValuePtr right = arg.value;
        if (right->isi())
            return cast(static_cast<oca_int>(std::pow(left, right->toi())));
        if (right->isr())
            return cast(static_cast<oca_real>(std::pow(left, right->tor())));
        return NIL;
    });

    bind("__eq", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left == right);
    });

    bind("__neq", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left != right);
    });

    bind("__gr", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left > arg.value->toi());
        if (arg.value->isr())
            return cast(left > arg.value->tor());
        return NIL;
    });

    bind("__ls", "n", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        if (arg.value->isi())
            return cast(left < arg.value->toi());
        if (arg.value->isr())
            return cast(left < arg.value->tor());
        return NIL;
    });

    bind("__geq", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left >= right);
    });

    bind("__leq", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left <= right);
    });

    bind("__ran", "i", [&] CPPFUNC {
        oca_int begin = arg.caller->toi();
        oca_int end = arg.value->toi();
        std::vector<oca_int> vec(end - begin + 1);
        oca_int counter = 0;
        for (oca_int i = begin; i <= end; ++i) {
            vec[counter] = i;
            ++counter;
        }
        return cast(vec);
    });

    bind("__and", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left & right);
    });

    bind("__or", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left | right);
    });

    bind("__xor", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left ^ right);
    });

    bind("__lsh", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left << right);
    });

    bind("__rsh", "i", [&] CPPFUNC {
        oca_int left = arg.caller->toi();
        oca_int right = arg.value->toi();
        return cast(left >> right);
    });

    bind("times", "", [&] CPPFUNC {
        oca_int times = arg.caller->toi();
        Block& yield = static_cast<Block&>(*arg.yield);
        for (oca_int i = 0; i < times; ++i) {
            yield(Table::from(*arg.caller->scope.parent), cast(i), NIL);
        }
        return NIL;
    });

    bind("ascii", "", [&] CPPFUNC {
        oca_int num = arg.caller->toi();
        char c = static_cast<char>(num);
        std::string empty = "";
        return cast(empty + c);
    });

    bind("real", "", [&] CPPFUNC {
        oca_int num = arg.caller->toi();
        return cast(static_cast<oca_real>(num));
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

Real::Real(oca_real val, Scope* parent) : val(val) {
    scope = Scope(nullptr);
    scope.parent = parent;

    // functions
    bind("__add", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left + arg.value->toi());
        if (arg.value->isr())
            return cast(left + arg.value->tor());
        return NIL;
    });

    bind("__sub", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left - arg.value->toi());
        if (arg.value->isr())
            return cast(left - arg.value->tor());
        return NIL;
    });

    bind("__mul", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left * arg.value->toi());
        if (arg.value->isr())
            return cast(left * arg.value->tor());
        return NIL;
    });

    bind("__div", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left / arg.value->toi());
        if (arg.value->isr())
            return cast(left / arg.value->tor());
        return NIL;
    });

    bind("__pow", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        ValuePtr right = arg.value;
        if (right->isi())
            return cast(static_cast<oca_real>(std::pow(left, right->toi())));
        if (right->isr())
            return cast(static_cast<oca_real>(std::pow(left, right->tor())));
        return NIL;
    });

    bind("__gr", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left > arg.value->toi());
        if (arg.value->isr())
            return cast(left > arg.value->tor());
        return NIL;
    });

    bind("__ls", "n", [&] CPPFUNC {
        oca_real left = arg.caller->tor();
        if (arg.value->isi())
            return cast(left < arg.value->toi());
        if (arg.value->isr())
            return cast(left < arg.value->tor());
        return NIL;
    });

    bind("floor", "", [&] CPPFUNC {
        oca_real num = arg.caller->tor();
        return cast(static_cast<oca_int>(std::floor(num)));
    });

    bind("ceil", "", [&] CPPFUNC {
        oca_real num = arg.caller->tor();
        return cast(static_cast<oca_int>(std::ceil(num)));
    });

    bind("round", "", [&] CPPFUNC {
        oca_real num = arg.caller->tor();
        return cast(static_cast<oca_int>(std::round(num)));
    });
}

ValuePtr Real::copy() {
    return std::make_shared<Real>(*this);
}

std::string Real::tos() {
    std::stringstream ss;
    ss << std::setprecision(16) << val;
    std::string str = ss.str();
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
        oca_int right = arg.value->toi();
        std::string result = "";
        if (right <= 0)
            return cast(result); // TODO: should error
        for (oca_int i = 0; i < right; ++i) {
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

    bind("size", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        return cast(static_cast<oca_int>(str.size()));
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
        return cast(std::stoll(str));
    });

    bind("real", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        return cast(std::stod(str));
    });

    bind("ascii", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        if (str.size() != 1)
            throw Error(CUSTOM_ERROR, "String must be 1 character long.");
        return cast(static_cast<oca_int>(str.at(0)));
    });

    bind("find", "s", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        std::string regexString = arg.value->tos();
        std::regex regex(regexString);
        std::smatch match;
        if (std::regex_search(str, match, regex))
            return cast(static_cast<oca_int>(match.position()));
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
        oca_int index = arg.value->toi();
        if (index < 0 || index >= static_cast<oca_int>(str.size()))
            throw Error(CUSTOM_ERROR, "Index " + std::to_string(index) + " out of bounds.");
        return cast(std::string() + str.at(index));
    });

    bind("each", "", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        Block& yield = static_cast<Block&>(*arg.yield);
        for (oca_int i = 0; i < static_cast<oca_int>(str.size()); ++i) {
            auto table = std::make_shared<Table>(nullptr);
            table->add("0", cast(i));
            table->add("1", cast(std::string() + str.at(i)));
            yield(Table::from(*arg.caller->scope.parent), table, NIL);
        }
        return arg.caller;
    });

    bind("split", "s", [&] CPPFUNC {
        std::string str = arg.caller->tos();
        std::string delim = arg.value->tos();
        std::string word = "";
        auto result = std::make_shared<Table>(nullptr);
        uint index = ARRAY_BEGIN_INDEX;
        for (char c : str) {
            if (c == delim[0]) {
                result->add(std::to_string(index), cast(word));
                word = "";
                ++index;
            } else
                word += c;
        }
        result->add(std::to_string(index), cast(word));
        return result;
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

Table::Table(Scope* parent) {
    scope = Scope(parent);

    bind("size", "", [&] CPPFUNC {
        auto table = arg.caller;
        return cast(static_cast<oca_int>(static_cast<Table&>(*table).size));
    });

    bind("insert", "ka", [&] CPPFUNC {
        auto& table = static_cast<Table&>(*arg.caller);
        std::string name = arg[0]->tos();
        if (std::isdigit(name[0])) {
            oca_int index = std::stoi(name);
            if (index < 0 || index > table.count)
                throw Error(CUSTOM_ERROR, "Index " + name + " out of range(+1).");

            std::vector<ValuePtr> array(table.count);
            for (oca_int i = 0; i < table.count; ++i)
                array[i] = table.scope.get(std::to_string(i), false);

            array.insert(array.begin() + index, arg[1]);
            table.add(std::to_string(table.count), cast(0));

            for (uint i = 0; i < array.size(); ++i)
                table.scope.set(std::to_string(i), array[i], true);
        } else
            table.add(name, arg[1]);
        return arg.caller;
    });

    bind("remove", "k", [&] CPPFUNC {
        auto& table = static_cast<Table&>(*arg.caller);
        std::string name = arg.value->tos();
        if (std::isdigit(name[0])) {
            oca_int index = std::stoi(name);
            if (index < 0 || index >= table.count)
                throw Error(CUSTOM_ERROR, "Index " + name + " out of range.");

            std::vector<ValuePtr> array(table.count);
            for (oca_int i = 0; i < table.count; ++i)
                array[i] = table.scope.get(std::to_string(i), false);

            array.erase(array.begin() + index);
            table.remove(std::to_string(table.count - 1));

            for (uint i = 0; i < array.size(); ++i)
                table.scope.set(std::to_string(i), array[i], true);

        } else if (!table.remove(name))
            throw Error(CUSTOM_ERROR, "Key '" + name + "' does not exist.");
        return arg.caller;
    });

    bind("at", "k", [&] CPPFUNC {
        auto table = arg.caller;
        std::string name = arg.value->tos();
        return table->scope.get(name, false);
    });

    bind("each", "", [&] CPPFUNC {
        auto& table = static_cast<Table&>(*arg.caller);
        Block& yield = static_cast<Block&>(*arg.yield);
        for (auto& var : table.scope.vars) {
            auto& vref = *var.value;
            if (TYPE_EQ(vref, Func))
                continue;
            auto param = std::make_shared<Table>(nullptr);
            param->add("0", cast(var.name));
            param->add("1", var.value);
            yield(Table::from(*arg.caller->scope.parent), param, NIL);
        }
        return arg.caller;
    });

    bind("sort", "", [&] CPPFUNC {
        auto& table = static_cast<Table&>(*arg.caller);
        Block& yield = static_cast<Block&>(*arg.yield);
        oca_int count = table.count;

        std::vector<ValuePtr> array(count);
        for (oca_int i = 0; i < count; ++i)
            array[i] = table.scope.get(std::to_string(i), false);

        std::sort(array.begin(), array.end(), [&](ValuePtr& a, ValuePtr& b) -> bool {
            auto param = std::make_shared<Table>(nullptr);
            param->add("0", a);
            param->add("1", b);
            return yield(Table::from(*arg.caller->scope.parent), param, NIL)->tob();
        });

        for (oca_int i = 0; i < count; ++i)
            table.scope.set(std::to_string(i), array[i], true);

        return arg.caller;
    });
}

ValuePtr Table::copy() {
    // return std::make_shared<Table>(*this);
    return nullptr;
}

std::shared_ptr<Table> Table::from(Scope& scope) {
    auto t = std::make_shared<Table>(nullptr);
    t->scope = scope;
    return t;
}

void Table::add(const std::string& name, ValuePtr value) {
    scope.set(name, value, true);
    if (std::isdigit(name.at(0)))
        ++count;
    ++size;
}

bool Table::remove(const std::string& name) {
    if (scope.remove(name)) {
        if (std::isdigit(name.at(0)))
            --count;
        --size;
        return true;
    }
    return false;
}

std::string Table::tos() {
    std::string result = "(";
    for (oca_int i = 0; i < count; ++i) {
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
    if (result.size() > 1) {
        result.pop_back();
        result.pop_back();
    }
    result += ")";
    return result;
}

std::string Table::typestr() {
    std::string result = "(";
    for (auto& var : scope.vars) {
        auto& vref = *var.value;
        if (TYPE_EQ(vref, Func))
            continue;
        result += var.value->typestr();
        result += ", ";
    }
    if (result.size() > 1) {
        result.pop_back();
        result.pop_back();
    }
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
        argc = static_cast<Table&>(*arg).count;
    else if (!arg->isNil())
        argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0)
        throw Error(NO_ARGUMENT);
    if (argc < params.size())
        throw Error(
            SMALL_TABLE, "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");

    // set super and yield in scope
    Scope temp(&scope);
    temp.set("yield", block, true);
    temp.set("self", caller, true);

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
        argc = static_cast<Table&>(*arg).count;
    else if (!arg->isNil())
        argc = 1;
    if (arg->ist() && argc == 0)
        argc = 1;

    // check argument count
    if (argc == 0 && params.size() > 0)
        throw Error(NO_ARGUMENT);
    if (argc < params.size())
        throw Error(
            SMALL_TABLE, "(" + std::to_string(argc) + " < " + std::to_string(params.size()) + ").");

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
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted table.");
            break;
        case 'k':
            if (!v->isi() && !v->iss())
                throw Error(TYPE_MISMATCH, v->typestr() + " wanted oca_int/str.");
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
