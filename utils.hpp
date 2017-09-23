#pragma once

#include <fstream>
#include <string>
#include "errors.hpp"

std::string file(const std::string& path)
{
  std::ifstream file(path);
  if (file.is_open())
  {
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
  ERR << "Could not open file " << path;
  file.close();
  return "";
}
