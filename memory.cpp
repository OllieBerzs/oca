#include "memory.hpp"

namespace oca::internal
{

std::string Memory::_mem = "";

void Memory::add(char c)
{
    _mem += c;
}

void Memory::rem(char c)
{
    for (unsigned int i = 0; i < _mem.size(); i++)
    {
        if (_mem[i] == c)
        {
            _mem.erase(i, 1);
            break;
        }
    }
}

std::string Memory::get()
{
    return _mem;
}

} // namespace oca::internal
