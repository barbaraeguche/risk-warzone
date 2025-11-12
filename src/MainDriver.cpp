#include <iostream>

void testLoadMaps(); 
void testPlayers(); 
void testOrdersLists(); 
void testOrderExecution(); 
void testCards(); 
void testGameStates();
void testCommandProcessor();
void testStartupPhase();
void testLoggingObserver();
void testMainGameLoop();
void automatictestMainGameLoop();


/**
 * The Main driver function that calls all test functions for each part
 * This demonstrates the execution of all parts of the assignment
 */
int main() {
  std::cout << "=== COMP 345 - Assignment 1 - Team Project ===" << std::endl;
  std::cout << "Advanced Program Design with C++" << std::endl;
  std::cout << "Warzone Game Implementation" << std::endl;
  std::cout << "\nRunning all test functions for each part..." << std::endl;

  try {
    /** 
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
    testOrderExecution();

    // Part 4: Cards Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 4: CARDS TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testCards();

    // Part 5: Game Engine Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 5: GAME ENGINE TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testGameStates();
    */

    // Part 6: Command Processor Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 6: COMMAND PROCESSOR TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testCommandProcessor();

    // Part 7: startup phase Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 7: STARTUP PHASE TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testStartupPhase();
    
    // Part 8: Main Game Loop Testing with GameEngine
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 8: MAIN GAME LOOP TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    automatictestMainGameLoop();
    
    //Part 9: Testing order execution
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 9: ORDER EXECUTION TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testOrderExecution();

    // Part 10: logging observer Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 10: LOGGING OBSERVER TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testLoggingObserver();

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "ALL TESTS COMPLETED SUCCESSFULLY" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error during testing: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
