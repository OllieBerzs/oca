#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace oca::internal
{

  std::string readFile(const std::string& path) 
  {
    std::ifstream file(path);
    if (!file.is_open()) std::cout << "Could not open file " << path << "\n";

    std::string result((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();
    return result;
  }

} // namespace oca::internal
