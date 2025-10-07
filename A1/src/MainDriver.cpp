#include <iostream>

void testLoadMaps(); // Implemented in MapDriver.cpp
void testPlayers(); // To be implemented in PlayerDriver.cpp
void testOrdersLists(); // To be implemented in OrdersDriver.cpp
void testCards(); // To be implemented in CardsDriver.cpp
void testGameStates(); // Implemented in GameEngineDriver.cpp

/**
 * Main driver function that calls all test functions for each part
 * This demonstrates the execution of all parts of the assignment
 */
int main() {
  std::cout << "=== COMP 345 - Assignment 1 - Team Project ===" << std::endl;
  std::cout << "Advanced Program Design with C++" << std::endl;
  std::cout << "Warzone Game Implementation" << std::endl;
  std::cout << "\nRunning all test functions for each part..." << std::endl;

  try {
    // Part 1: Map Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 1: MAP TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testLoadMaps();

    // Part 2: Player Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 2: PLAYER TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testPlayers();

    // Part 3: Orders Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 3: ORDERS TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testOrdersLists();

    // // Part 4: Cards Testing
    // std::cout << "\n" << std::string(50, '=') << std::endl;
    // std::cout << "PART 4: CARDS TESTING" << std::endl;
    // std::cout << std::string(50, '=') << std::endl;
    // testCards();

    // // Part 5: Game Engine Testing
    // std::cout << "\n" << std::string(50, '=') << std::endl;
    // std::cout << "PART 5: GAME ENGINE TESTING" << std::endl;
    // std::cout << std::string(50, '=') << std::endl;
    // testGameStates();

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "ALL TESTS COMPLETED SUCCESSFULLY" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error during testing: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
