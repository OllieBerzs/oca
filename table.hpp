#pragma once

#include <map>
#include "errors.hpp"
#include "value.hpp"

class Table
{
  std::map<std::string, Value*> items;
  Table* parent;

public:
  Table(Table* parent) : parent(parent) {}

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
      else ERR << "Unknown symbol '" << name << "'";
    }
    else
    {
      return val->second;
    }
    return val->second;
  }
};
