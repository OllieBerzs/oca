#pragma once

#include "token.hpp"
#include <string>
#include <vector>

namespace oca::internal {

#define LEX_PARAMS

void lex(const std::string& script, std::vector<Token>& tokens);

bool integer(
  const std::string& script, unsigned int& index, std::vector<Token>& tokens);
bool floating(
  const std::string& script, unsigned int& index, std::vector<Token>& tokens);
bool string(
  const std::string& script, unsigned int& index, std::vector<Token>& tokens);
bool boolean(
  const std::string& script, unsigned int& index, std::vector<Token>& tokens);
bool name(
  const std::string& script, unsigned int& index, std::vector<Token>& tokens);
bool keyword(
  const std::string& script, unsigned int& index, std::vector<Token>& tokens);

} // namespace oca::internal
