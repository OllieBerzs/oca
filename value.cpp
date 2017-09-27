#include "table.hpp"

Value::~Value()
{
  Memory::rem();
  delete env;
}
