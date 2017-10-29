#pragma once

#include <fstream>
#include <string>
#include "errors.hpp"

std::string file(const std::string& path)
{
  std::ifstream file(path);
  if (!file.is_open()) ERR << "Could not open file " << path;

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return buffer.str();
}
