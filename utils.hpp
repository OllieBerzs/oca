#pragma once

#include "errors.hpp"
#include <iostream>
#include <fstream>
#include <string>

std::string file(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) std::cout << "Could not open file " << path << "\n";

  std::string result((std::istreambuf_iterator<char>(file)),
                     (std::istreambuf_iterator<char>()));
  file.close();
  return result;
}
