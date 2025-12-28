#include "Map.h"
#include "Player.h"
#include "Utility.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

/**
 * Test function that loads map files from /maps directory structure
 */
void testLoadMaps() {
  std::cout << "========================================" << std::endl;
  std::cout << "   WARZONE MAP LOADER TEST" << std::endl;
  std::cout << "   Loading from /maps/" << std::endl;
  std::cout << "========================================\n" << std::endl;

  MapLoader mapLoader;

  // find all .map files in maps directory
  const fs::path cwd = fs::current_path();
  const fs::path mapDirPath = cwd / "maps";
  const std::string mapDir = mapDirPath.string();

  std::cout << "Searching for .map files in: " << mapDir << std::endl;
  const std::vector<std::string> mapFiles = Utility::findMapFiles(mapDir);

  if (mapFiles.empty()) {
    std::cout << "No .map files found in " << mapDir << std::endl;
    std::cout << "Please ensure the directory exists and contains .map files." << std::endl;
    return;
  }

  std::cout << "Found " << mapFiles.size() << " .map file(s)\n" << std::endl;

  int testNumber = 1, passedTests = 0;
  const int totalTests = mapFiles.size();

  for (const std::string& filepath : mapFiles) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST " << testNumber++ << ": " << filepath << std::endl;
    std::cout << "========================================" << std::endl;

    // check if the file can be read first
    if (!MapLoader::canReadFile(filepath)) {
      std::cout << "RESULT: Cannot read file" << std::endl;
      continue;
    }

    // attempt to load the map
    const std::unique_ptr<Map> map = mapLoader.loadMap(filepath);

    if (!map) {
      std::cout << "RESULT: Failed to load map - Invalid map format" << std::endl;
      continue;
    }

    std::cout << "Map loaded successfully" << std::endl;
    std::cout << "\n--- MAP INFORMATION ---" << std::endl;
    std::cout << "Map Name: " << map->getName() << std::endl;
    std::cout << "Number of Territories: " << map->getNumberOfTerritories() << std::endl;
    std::cout << "Number of Continents: " << map->getNumberOfContinents() << std::endl;

    // perform validation tests
    std::cout << "\n--- VALIDATION TESTS ---" << std::endl;
    const bool isConnectedGraph = map->isConnectedGraph();
    std::cout << "1. Map is a connected graph: " << (isConnectedGraph ? "PASS" : "FAIL") << std::endl;

    const bool continentsAreConnected = map->areContinentsConnected();
    std::cout << "2. Continents are connected subgraphs: " << (continentsAreConnected ? "PASS" : "FAIL") << std::endl;

    const bool territoryBelongsToOneContinent = map->eachTerritoryBelongsToOneContinent();
    std::cout << "3. Each territory belongs to one continent: " << (territoryBelongsToOneContinent ? "PASS" : "FAIL")
              << std::endl;

    const bool overallValid = map->validate();
    std::cout << "\n--- OVERALL VALIDATION ---" << std::endl;
    std::cout << "Overall Result: " << (overallValid ? "VALID MAP" : "INVALID MAP") << std::endl;

    if (overallValid) passedTests++;
  }

  std::cout << "\n========================================" << std::endl;
  std::cout << "TEST SUMMARY" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << "Total maps tested: " << totalTests << std::endl;
  std::cout << "Valid maps: " << passedTests << std::endl;
  std::cout << "Invalid maps: " << (totalTests - passedTests) << std::endl;
  std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << std::endl;

  std::cout << "\n========================================" << std::endl;
  std::cout << "ALL TESTS COMPLETED" << std::endl;
  std::cout << "========================================\n" << std::endl;
}
