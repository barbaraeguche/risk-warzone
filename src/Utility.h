#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class Utility {
public:
  static std::vector<std::string> findMapFiles(const std::string& dir);
};
