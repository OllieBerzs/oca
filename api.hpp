#pragma once

#include "scope.hpp"

namespace oca
{
    typedef internal::Scope State;
    #define NIL new internal::Value("nil", nullptr)
    #define DLLEXPORT __declspec(dllexport) void

    void def(State& state, const std::string& name, internal::NativeMethod native)
    {
        internal::Value* val = new internal::Value("native", nullptr);
        val->refCount++;
        val->native = native;
        state.set(name, val);
    }
}
