#pragma once

#include <map>
#include "errors.hpp"
#include "value.hpp"

class Scope
{
  std::map<std::string, Value*> items;
  Scope* parent;

public:
  Scope(Scope* parent) : parent(parent) {}

  void clean()
  {
    for (auto elem : items) delete elem.second;
  }

  ~Scope()
  {
    clean();
  }

  void set(const std::string& name, Value* value)
  {
    items.emplace(name, value);
  }

  Value* get(const std::string& name)
  {
    auto val = items.find(name);
    if (val == items.end())
    {
      if (parent) return parent->get(name);
      //else ERR << "Unknown symbol '" << name << "'";
      else return nullptr;
    }
    else
    {
      return val->second;
    }
    return val->second;
  }

  Value* operator[](const std::string& name)
  {
    Value* val = get(name);
    if (val)
    {
      return val;
    }
    else
    {
      items.emplace(name, val);
      return val;
    }
  }
};

inline Value::~Value()
{
  Memory::rem();
  if (env) delete env;
}
