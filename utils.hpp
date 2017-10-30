#pragma once

#include <fstream>
#include <string>
#include "errors.hpp"

std::string file(const std::string& path)
{
  std::ifstream file(path);
  if (!file.is_open()) ERR << "Could not open file " << path;

  std::string result((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  file.close();
  return result;
}
