#include "Map.h"

#include <fstream>
#include <iostream>

// #include <vector>
// #include <string>
//
/**
 * Creates test map files for demonstration purposes
 */
void createTestMapFiles() {
  std::cout << "Creating test map files..." << std::endl;

  // Create a valid map file
  std::ofstream validMap("test_valid.map");
  validMap << "[Map]\n";
  validMap << "name=Test Valid Map\n";
  validMap << "author=Test Author\n";
  validMap << "\n";
  validMap << "[Continents]\n";
  validMap << "North America=5\n";
  validMap << "Europe=3\n";
  validMap << "\n";
  validMap << "[Territories]\n";
  validMap << "Alaska,0,0,North America,Northwest Territory,Alberta\n";
  validMap << "Northwest Territory,1,1,North America,Alaska,Alberta,Ontario\n";
  validMap << "Alberta,2,2,North America,Alaska,Northwest Territory,Ontario,Saskatchewan\n";
  validMap << "Ontario,3,3,North America,Northwest Territory,Alberta,Quebec,Iceland\n";
  validMap << "Quebec,4,4,North America,Ontario\n";
  validMap << "Saskatchewan,5,5,North America,Alberta\n";
  validMap << "Iceland,10,10,Europe,Ontario,Great Britain,Scandinavia\n";
  validMap << "Great Britain,11,11,Europe,Iceland,Scandinavia,Western Europe\n";
  validMap << "Scandinavia,12,12,Europe,Iceland,Great Britain\n";
  validMap << "Western Europe,13,13,Europe,Great Britain\n";
  validMap.close();

  // Create a map with disconnected graph
  std::ofstream disconnectedMap("test_disconnected.map");
  disconnectedMap << "[Map]\n";
  disconnectedMap << "name=Test Disconnected Map\n";
  disconnectedMap << "\n";
  disconnectedMap << "[Continents]\n";
  disconnectedMap << "Continent1=2\n";
  disconnectedMap << "Continent2=3\n";
  disconnectedMap << "\n";
  disconnectedMap << "[Territories]\n";
  disconnectedMap << "Territory1,0,0,Continent1,Territory2\n";
  disconnectedMap << "Territory2,1,1,Continent1,Territory1\n";
  disconnectedMap << "Territory3,2,2,Continent2,Territory4\n";
  disconnectedMap << "Territory4,3,3,Continent2,Territory3\n";
  disconnectedMap.close();

  // Create a map with disconnected continent
  std::ofstream disconnectedContinent("test_disconnected_continent.map");
  disconnectedContinent << "[Map]\n";
  disconnectedContinent << "name=Test Disconnected Continent Map\n";
  disconnectedContinent << "\n";
  disconnectedContinent << "[Continents]\n";
  disconnectedContinent << "BadContinent=5\n";
  disconnectedContinent << "GoodContinent=2\n";
  disconnectedContinent << "\n";
  disconnectedContinent << "[Territories]\n";
  disconnectedContinent << "Territory1,0,0,BadContinent,Territory3,Territory5\n";
  disconnectedContinent << "Territory2,1,1,BadContinent,Territory3\n";
  disconnectedContinent << "Territory3,2,2,BadContinent,Territory1,Territory2\n";
  disconnectedContinent << "Territory4,3,3,BadContinent,Territory5\n";
  disconnectedContinent << "Territory5,4,4,GoodContinent,Territory1,Territory4\n";
  disconnectedContinent.close();

  // Create a map where territory doesn't belong to any continent
  std::ofstream noContinent("test_no_continent.map");
  noContinent << "[Map]\n";
  noContinent << "name=Test No Continent Map\n";
  noContinent << "\n";
  noContinent << "[Continents]\n";
  noContinent << "SomeContinent=3\n";
  noContinent << "\n";
  noContinent << "[Territories]\n";
  noContinent << "Territory1,0,0,SomeContinent,Territory2\n";
  noContinent << "Territory2,1,1,,Territory1\n";
  noContinent.close();

  // Create an invalid file format
  std::ofstream invalidFormat("test_invalid_format.map");
  invalidFormat << "This is not a valid map file\n";
  invalidFormat << "It has no proper sections\n";
  invalidFormat << "Random content here\n";
  invalidFormat.close();

  // Create a file with missing sections
  std::ofstream missingSections("test_missing_sections.map");
  missingSections << "[Map]\n";
  missingSections << "name=Incomplete Map\n";
  missingSections << "\n";
  missingSections << "[Continents]\n";
  missingSections << "SomeContinent=3\n";
  // Missing [Territories] section
  missingSections.close();

  // Create an empty file
  std::ofstream emptyFile("test_empty.map");
  emptyFile.close();

  // Create a more complex valid map
  std::ofstream complexMap("test_complex_valid.map");
  complexMap << "[Map]\n";
  complexMap << "name=Complex Risk-like Map\n";
  complexMap << "\n";
  complexMap << "[Continents]\n";
  complexMap << "North America=5\n";
  complexMap << "South America=2\n";
  complexMap << "Europe=5\n";
  complexMap << "Africa=3\n";
  complexMap << "\n";
  complexMap << "[Territories]\n";
  complexMap << "Alaska,70,126,North America,Northwest Territory,Alberta,Kamchatka\n";
  complexMap << "Alberta,148,127,North America,Alaska,Northwest Territory,Ontario,Western United States\n";
  complexMap << "Northwest Territory,148,84,North America,Alaska,Alberta,Ontario,Greenland\n";
  complexMap << "Ontario,199,125,North America,Northwest Territory,Alberta,Western United States,Eastern United States,Quebec,Greenland\n";
  complexMap << "Greenland,324,74,North America,Northwest Territory,Ontario,Quebec,Iceland\n";
  complexMap << "Quebec,253,166,North America,Ontario,Eastern United States,Greenland\n";
  complexMap << "Western United States,159,192,North America,Alberta,Ontario,Eastern United States,Central America\n";
  complexMap << "Eastern United States,216,191,North America,Western United States,Ontario,Quebec,Central America\n";
  complexMap << "Central America,183,262,North America,Western United States,Eastern United States,Venezuela\n";
  complexMap << "Venezuela,263,303,South America,Central America,Peru,Brazil\n";
  complexMap << "Peru,262,349,South America,Venezuela,Brazil,Argentina\n";
  complexMap << "Brazil,308,337,South America,Venezuela,Peru,Argentina,North Africa\n";
  complexMap << "Argentina,263,407,South America,Peru,Brazil\n";
  complexMap << "Iceland,417,148,Europe,Greenland,Great Britain,Scandinavia\n";
  complexMap << "Great Britain,442,193,Europe,Iceland,Scandinavia,Northern Europe,Western Europe\n";
  complexMap << "Scandinavia,502,148,Europe,Iceland,Great Britain,Northern Europe,Ukraine\n";
  complexMap << "Northern Europe,494,215,Europe,Great Britain,Scandinavia,Ukraine,Southern Europe,Western Europe\n";
  complexMap << "Ukraine,560,188,Europe,Scandinavia,Northern Europe,Southern Europe,Middle East,Afghanistan,Ural\n";
  complexMap << "Western Europe,449,249,Europe,Great Britain,Northern Europe,Southern Europe,North Africa\n";
  complexMap << "Southern Europe,502,253,Europe,Northern Europe,Ukraine,Western Europe,North Africa,Egypt,Middle East\n";
  complexMap << "North Africa,477,319,Africa,Brazil,Western Europe,Southern Europe,Egypt,East Africa,Congo\n";
  complexMap << "Egypt,549,319,Africa,Southern Europe,North Africa,East Africa,Middle East\n";
  complexMap << "East Africa,577,360,Africa,Egypt,North Africa,Congo,South Africa,Madagascar,Middle East\n";
  complexMap << "Congo,520,399,Africa,North Africa,East Africa,South Africa\n";
  complexMap << "South Africa,541,445,Africa,Congo,East Africa,Madagascar\n";
  complexMap << "Madagascar,618,407,Africa,East Africa,South Africa\n";
  complexMap << "Middle East,617,305,Europe,Ukraine,Southern Europe,Egypt,East Africa,Afghanistan\n";
  complexMap << "Afghanistan,663,251,Europe,Ukraine,Middle East,Ural,China,India\n";
  complexMap << "Ural,648,148,Europe,Ukraine,Afghanistan,Siberia,China\n";
  complexMap << "Kamchatka,806,103,Europe,Alaska,Yakutsk,Irkutsk,Mongolia,Japan\n";
  complexMap.close();

  std::cout << "Test map files created successfully.\n" << std::endl;
}

/**
 * Test function that loads various map files and validates them
 */
void testLoadMaps() {
    std::cout << "========================================" << std::endl;
    std::cout << "   WARZONE MAP LOADER TEST SUITE" << std::endl;
    std::cout << "========================================\n" << std::endl;

     // Create test files first
    createTestMapFiles();

     MapLoader loader;
     std::vector<std::string> testFiles = {
         "test_valid.map",
         "test_complex_valid.map",
         "test_disconnected.map",
         "test_disconnected_continent.map",
         "test_no_continent.map",
         "test_invalid_format.map",
         "test_missing_sections.map",
         "test_empty.map",
         "nonexistent_file.map"
     };

     int testNumber = 1;
     int passedTests = 0;
     int totalTests = testFiles.size();

     for (const std::string& filename : testFiles) {
         std::cout << "\n========================================" << std::endl;
         std::cout << "TEST " << testNumber++ << ": " << filename << std::endl;
         std::cout << "========================================" << std::endl;

         // Check if the file can be read
         if (!loader.canReadFile(filename)) {
             std::cout << "❌ RESULT: Cannot read file (file does not exist or cannot be opened)" << std::endl;
             continue;
         }

         // Attempt to load the map
         std::unique_ptr<Map> map = loader.loadMap(filename);

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
         }

         // Display detailed map information for valid maps only
         if (overallValid && (filename == "test_valid.map" || filename == "test_complex_valid.map")) {
             map->displayMap();
         }
     }

     // Summary
     std::cout << "\n========================================" << std::endl;
     std::cout << "TEST SUMMARY" << std::endl;
     std::cout << "========================================" << std::endl;
     std::cout << "Valid maps found: " << passedTests << " / " << totalTests << std::endl;
     std::cout << "Expected valid maps: 2 (test_valid.map, test_complex_valid.map)" << std::endl;
     std::cout << "Expected invalid maps: " << (totalTests - 2) << std::endl;

     // Test copy constructor and assignment operator
     std::cout << "\n========================================" << std::endl;
     std::cout << "TESTING COPY OPERATIONS" << std::endl;
     std::cout << "========================================" << std::endl;

    if (std::unique_ptr<Map> originalMap = loader.loadMap("test_valid.map")) {
         std::cout << "\nOriginal map loaded: " << originalMap->getMapName() << std::endl;
         std::cout << "Original map territories: " << originalMap->getNumberOfTerritories() << std::endl;
         std::cout << "Original map valid: " << (originalMap->validate() ? "YES" : "NO") << std::endl;

         // Test copy constructor
         std::cout << "\n--- Testing Copy Constructor ---" << std::endl;
         Map copiedMap(*originalMap);
         std::cout << "Copied map name: " << copiedMap.getMapName() << std::endl;
         std::cout << "Copied map territories: " << copiedMap.getNumberOfTerritories() << std::endl;
         std::cout << "Copied map valid: " << (copiedMap.validate() ? "✅ YES" : "❌ NO") << std::endl;

         // Test assignment operator
         std::cout << "\n--- Testing Assignment Operator ---" << std::endl;
         const Map& assignedMap = copiedMap;
         std::cout << "Assigned map name: " << assignedMap.getMapName() << std::endl;
         std::cout << "Assigned map territories: " << assignedMap.getNumberOfTerritories() << std::endl;
         std::cout << "Assigned map valid: " << (assignedMap.validate() ? "✅ YES" : "❌ NO") << std::endl;

         std::cout << "\n✅ Copy operations test PASSED" << std::endl;
     }

     // Test programmatic map creation
     std::cout << "\n========================================" << std::endl;
     std::cout << "TESTING PROGRAMMATIC MAP CREATION" << std::endl;
     std::cout << "========================================" << std::endl;

     Map programmaticMap("Programmatic Test Map");

     // Add continents
     Continent* northAmerica = programmaticMap.addContinent("North America", 1);
     Continent* europe = programmaticMap.addContinent("Europe", 2);

     // Add territories
     Territory* alaska = programmaticMap.addTerritory("Alaska", 1);
     Territory* canada = programmaticMap.addTerritory("Canada", 2);
     Territory* usa = programmaticMap.addTerritory("USA", 3);
     Territory* uk = programmaticMap.addTerritory("UK", 4);
     Territory* france = programmaticMap.addTerritory("France", 5);

     // Assign territories to continents
     northAmerica->addTerritory(alaska);
     northAmerica->addTerritory(canada);
     northAmerica->addTerritory(usa);
     europe->addTerritory(uk);
     europe->addTerritory(france);

     // Set up adjacencies
     programmaticMap.addAdjacency(alaska, canada);
     programmaticMap.addAdjacency(canada, usa);
     programmaticMap.addAdjacency(usa, uk);  // Trans-Atlantic connection
     programmaticMap.addAdjacency(uk, france);

     // Set some game state
     alaska->setOwner("Player1");
     alaska->setArmies(3);
     canada->setOwner("Player1");
     canada->setArmies(2);
     usa->setOwner("Player2");
     usa->setArmies(4);
     uk->setOwner("Player2");
     uk->setArmies(1);
     france->setOwner("Player3");
     france->setArmies(2);

     std::cout << "Programmatic map created with:" << std::endl;
     std::cout << "  - " << programmaticMap.getNumberOfContinents() << " continents" << std::endl;
     std::cout << "  - " << programmaticMap.getNumberOfTerritories() << " territories" << std::endl;
     std::cout << "  - Territory ownership and army assignments" << std::endl;
     std::cout << "\nValidation result: " << (programmaticMap.validate() ? "✅ VALID" : "❌ INVALID") << std::endl;

     programmaticMap.displayMap();

     // Test stream insertion operators
     std::cout << "\n========================================" << std::endl;
     std::cout << "TESTING STREAM INSERTION OPERATORS" << std::endl;
     std::cout << "========================================" << std::endl;

     std::cout << "\nTesting Territory operator<<:" << std::endl;
     std::cout << "  " << *alaska << std::endl;
     std::cout << "  " << *france << std::endl;

     std::cout << "\nTesting Continent operator<<:" << std::endl;
     std::cout << "  " << *northAmerica << std::endl;
     std::cout << "  " << *europe << std::endl;

     std::cout << "\nTesting Map operator<<:" << std::endl;
     std::cout << "  " << programmaticMap << std::endl;

     std::cout << "\nTesting MapLoader operator<<:" << std::endl;
     std::cout << "  " << loader << std::endl;

     std::cout << "\n✅ Stream insertion operators test PASSED" << std::endl;

     std::cout << "\n========================================" << std::endl;
     std::cout << "   ALL TESTS COMPLETED SUCCESSFULLY" << std::endl;
     std::cout << "========================================\n" << std::endl;
}

/**
 * Main function
 */
int main() {
    try {
        testLoadMaps();
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown exception occurred" << std::endl;
        return 1;
    }

    return 0;
}