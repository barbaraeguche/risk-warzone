#include <iostream>

// assignment 1
void testLoadMaps();
void testPlayers();
void testOrdersLists();
void testCards();
void testGameStates();

// assignment 2
void testCommandProcessor();
void testStartupPhase();
void testMainGameLoop();
void testOrderExecution();
void testLoggingObserver();

// assignment 3
void testPlayerStrategies();
void testTournament();


/**
 * The main driver calls the test functions for each part which
 * demonstrates the execution of all parts of the assignment.
 */
int main() {
  std::cout << "=== COMP 345 - Risk Warzone, A Team Project ===" << std::endl;
  std::cout << "Advanced Program Design, C++" << std::endl;
  std::cout << "\nRunning the test functions for each part..." << std::endl;

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


    // Part 6: Command Processor Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 6: COMMAND PROCESSOR TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testCommandProcessor();

    // Part 7: Game Startup Phase Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 7: STARTUP PHASE TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testStartupPhase();

    // Part 8: Main Game Loop Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 8: MAIN GAME LOOP TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testMainGameLoop();

    // Part 9: Testing order execution
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 9: ORDER EXECUTION TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testOrderExecution();

    // Part 10: logging observer Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 10: LOGGING OBSERVER TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testLoggingObserver();


    // Part 11: Player Strategy Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 11: PLAYER STRATEGY TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testPlayerStrategies();

    // Part 12: Tournament Mode Testing
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PART 12: TOURNAMENT MODE TESTING" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    testTournament();

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "ALL WARZONE TESTS COMPLETED SUCCESSFULLY" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error during testing: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
