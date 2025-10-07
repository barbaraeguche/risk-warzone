#include "Map.h"
#include "../player/Player.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * Recursively find all .map files in a directory and its subdirectories
 */
std::vector<std::string> findMapFiles(const std::string& directory) {
    std::vector<std::string> mapFiles;

    try {
        if (!fs::exists(directory)) {
            std::cerr << "Directory does not exist: " << directory << std::endl;
            return mapFiles;
        }

        // Iterate through all items in the directory recursively
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".map") {
                mapFiles.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    return mapFiles;
}

/**
 * Test function that loads map files from A1/mapFiles directory structure
 */
void testLoadMapsFromDirectory() {
    std::cout << "========================================" << std::endl;
    std::cout << "   WARZONE MAP LOADER TEST SUITE" << std::endl;
    std::cout << "   Loading from A1/mapFiles/" << std::endl;
    std::cout << "========================================\n" << std::endl;

    MapLoader loader;

    // Find all .map files in mapFiles directory
    // Adjust path based on where the executable is run from
    std::string mapDirectory = "../mapFiles";  // From src directory
    std::cout << "Searching for .map files in: " << mapDirectory << std::endl;

    // Try to resolve absolute path for debugging
    try {
        std::cout << "Absolute path: " << fs::absolute(mapDirectory) << std::endl;
    } catch (...) {
        // Ignore if path doesn't exist yet
    }

    std::vector<std::string> mapFiles = findMapFiles(mapDirectory);

    if (mapFiles.empty()) {
        std::cout << "❌ No .map files found in " << mapDirectory << std::endl;
        std::cout << "Please ensure the directory exists and contains .map files." << std::endl;
        return;
    }

    std::cout << "Found " << mapFiles.size() << " .map file(s)\n" << std::endl;

    int testNumber = 1;
    int passedTests = 0;
    int totalTests = mapFiles.size();

    for (const std::string& filepath : mapFiles) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "TEST " << testNumber++ << ": " << filepath << std::endl;
        std::cout << "========================================" << std::endl;

        // Check if the file can be read
        if (!loader.canReadFile(filepath)) {
            std::cout << "❌ RESULT: Cannot read file" << std::endl;
            continue;
        }

        // Attempt to load the map
        std::unique_ptr<Map> map = loader.loadMap(filepath);

        if (!map) {
            std::cout << "❌ RESULT: Failed to load map - Invalid map format" << std::endl;
            continue;
        }

        std::cout << "✅ Map loaded successfully" << std::endl;
        std::cout << "\n--- MAP INFORMATION ---" << std::endl;
        std::cout << "Map Name: " << map->getMapName() << std::endl;
        std::cout << "Number of Territories: " << map->getNumberOfTerritories() << std::endl;
        std::cout << "Number of Continents: " << map->getNumberOfContinents() << std::endl;

        // Perform validation tests
        std::cout << "\n--- VALIDATION TESTS ---" << std::endl;

        bool isConnected = map->isConnectedGraph();
        std::cout << "1. Map is a connected graph: " << (isConnected ? "✅ PASS" : "❌ FAIL") << std::endl;

        bool continentsConnected = map->areContinentsConnected();
        std::cout << "2. Continents are connected subgraphs: " << (continentsConnected ? "✅ PASS" : "❌ FAIL") << std::endl;

        bool territoriesInContinents = map->eachTerritoryBelongsToOneContinent();
        std::cout << "3. Each territory belongs to one continent: " << (territoriesInContinents ? "✅ PASS" : "❌ FAIL") << std::endl;

        bool overallValid = map->validate();
        std::cout << "\n--- OVERALL VALIDATION ---" << std::endl;
        std::cout << "Overall Result: " << (overallValid ? "✅ VALID MAP" : "❌ INVALID MAP") << std::endl;

        if (overallValid) {
            passedTests++;
            // Optionally display full map details for valid maps
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
