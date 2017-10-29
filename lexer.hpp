#pragma once

#include <string>
#include <vector>
#include "token.hpp"

void lex(const std::string& script, std::vector<Token>& tokens);
