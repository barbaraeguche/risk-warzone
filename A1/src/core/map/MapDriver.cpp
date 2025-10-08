#include "Map.h"
#include "Player.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

/**
 * Recursively find all .map files in a directory and its subdirectories
 */
std::vector<std::string> findMapFiles(const std::string& directory) {
  std::vector<std::string> dotMapFiles;

  try {
    if (!fs::exists(directory)) {
      std::cerr << "Directory does not exist: " << directory << std::endl;
      return dotMapFiles;
    }

    // Iterate through all items in the directory recursively
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
      if (entry.is_regular_file() && entry.path().extension() == ".map") {
        dotMapFiles.push_back(entry.path().string());
      }
    }
  } catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
  }

  return dotMapFiles;
}

/**
 * Test function that loads map files from A1/mapFiles directory structure
 */
void testLoadMaps() {
  std::cout << "========================================" << std::endl;
  std::cout << "   WARZONE MAP LOADER TEST SUITE" << std::endl;
  std::cout << "   Loading from A1/mapFiles/" << std::endl;
  std::cout << "========================================\n" << std::endl;

  MapLoader mapLoader;

  // Find all .map files in mapFiles directory
  const std::filesystem::path cwd = std::filesystem::current_path();
  const std::filesystem::path mapDirectoryPath = cwd / "A1" / "mapFiles";
  const std::string mapDirectory = mapDirectoryPath.string();

  std::cout << "Searching for .map files in: " << mapDirectory << std::endl;
  const std::vector<std::string> mapFiles = findMapFiles(mapDirectory);

  if (mapFiles.empty()) {
    std::cout << "No .map files found in " << mapDirectory << std::endl;
    std::cout << "Please ensure the directory exists and contains .map files." << std::endl;
    return;
  }

  std::cout << "Found " << mapFiles.size() << " .map file(s)\n" << std::endl;

  int testNumber = 1;
  int passedTests = 0;
  const int totalTests = static_cast<int>(mapFiles.size());

  for (const std::string& filepath : mapFiles) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST " << testNumber++ << ": " << filepath << std::endl;
    std::cout << "========================================" << std::endl;

    // Check if the file can be read
    if (!mapLoader.canReadFile(filepath)) {
      std::cout << "RESULT: Cannot read file" << std::endl;
      continue;
    }

    // Attempt to load the map
    std::unique_ptr<Map> map = mapLoader.loadMap(filepath);

    if (!map) {
      std::cout << "RESULT: Failed to load map - Invalid map format" << std::endl;
      continue;
    }

    std::cout << "Map loaded successfully" << std::endl;
    std::cout << "\n--- MAP INFORMATION ---" << std::endl;
    std::cout << "Map Name: " << map->getMapName() << std::endl;
    std::cout << "Number of Territories: " << map->getNumberOfTerritories() << std::endl;
    std::cout << "Number of Continents: " << map->getNumberOfContinents() << std::endl;

    // Perform validation tests
    std::cout << "\n--- VALIDATION TESTS ---" << std::endl;

    const bool isConnected = map->isConnectedGraph();
    std::cout << "1. Map is a connected graph: " << (isConnected ? "PASS" : "FAIL") << std::endl;

    const bool continentsConnected = map->areContinentsConnected();
    std::cout << "2. Continents are connected subgraphs: " << (continentsConnected ? "PASS" : "FAIL") << std::endl;

    const bool territoriesInContinents = map->eachTerritoryBelongsToOneContinent();
    std::cout << "3. Each territory belongs to one continent: " << (territoriesInContinents ? "PASS" : "FAIL") <<
        std::endl;

    const bool overallValid = map->validate();
    std::cout << "\n--- OVERALL VALIDATION ---" << std::endl;
    std::cout << "Overall Result: " << (overallValid ? "VALID MAP" : "INVALID MAP") << std::endl;

    if (overallValid) {
      passedTests++;
      // optionally display full map details for valid maps
      // map->displayMap();
    }
  }

  // Summary
  std::cout << "\n========================================" << std::endl;
  std::cout << "TEST SUMMARY" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << "Total maps tested: " << totalTests << std::endl;
  std::cout << "Valid maps: " << passedTests << std::endl;
  std::cout << "Invalid maps: " << (totalTests - passedTests) << std::endl;
  std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << std::endl;

  std::cout << "\n========================================" << std::endl;
  std::cout << "   ALL TESTS COMPLETED" << std::endl;
  std::cout << "========================================\n" << std::endl;
}
