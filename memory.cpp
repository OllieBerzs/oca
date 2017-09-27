#include "memory.hpp"

int Memory::_size = 0;

void Memory::add()
{
  _size++;
}

void Memory::rem()
{
  _size--;
}

int Memory::get()
{
  return _size;
}
