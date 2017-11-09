#pragma once

#include <string>
#include <vector>
#include "token.hpp"

namespace oca::internal
{

void lex(const std::string& script, std::vector<Token>& tokens);

} // namespace oca::internal
