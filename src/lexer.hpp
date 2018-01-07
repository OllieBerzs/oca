#pragma once

#include "token.hpp"
#include <string>
#include <vector>

namespace oca::internal
{

  void lex(const std::string& script, std::vector<Token>& tokens);

  bool isIn(char c, const std::string& str);
  bool match(const std::string& to, const std::string& script, unsigned int& index);

  bool whitespace(const std::string& script, unsigned int& index);

  bool punctuation(const std::string& script, unsigned int& index, std::vector<Token>& tokens);
  bool number(const std::string& script, unsigned int& index, std::vector<Token>& tokens);
  bool string(const std::string& script, unsigned int& index, std::vector<Token>& tokens);
  bool boolean(const std::string& script, unsigned int& index, std::vector<Token>& tokens);
  bool keyword(const std::string& script, unsigned int& index, std::vector<Token>& tokens);
  bool name(const std::string& script, unsigned int& index, std::vector<Token>& tokens);

} // namespace oca::internal
