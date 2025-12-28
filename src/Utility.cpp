#include "Utility.h"
#include <algorithm>
#include <iostream>

/**
 * Recursively find all .map files in a directory and its subdirectories
 */
std::vector<std::string> Utility::findMapFiles(const std::string& dir) {
  std::vector<std::string> dotMapFiles;

  try {
    if (!fs::exists(dir)) {
      std::cerr << "Directory does not exist: " << dir << std::endl;
      return dotMapFiles;
    }

    // recursively check all items in the directory
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
      if (entry.is_regular_file() && entry.path().extension() == ".map") {
        dotMapFiles.push_back(entry.path().string());
      }
    }
  } catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
  }

  std::ranges::sort(dotMapFiles);
  return dotMapFiles;
}
