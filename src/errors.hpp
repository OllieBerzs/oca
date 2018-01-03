#pragma once

#include <iostream>
#include <string>
#include "token.hpp"

namespace oca::internal::errors
{
  struct Script { static std::string data; };

  void parseError(const Token& token, const std::string& message);
}