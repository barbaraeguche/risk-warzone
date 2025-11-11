#include "GameEngine.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <iostream>
#include <string>
#include <vector>


    /**
     * Main game loop integrated with GameEngine states
     * Demonstrates turn-based gameplay using states:
     * "play", "assign reinforcement", "issue orders", "execute orders", "win"
     */
    void testMainGameLoop() {
    GameEngine engine;
    
    engine.startupPhase();
    engine.mainGameLoop();
    }

/**
 * Driver function to test the GameEngine functionality
 * Creates a console-driven interface that allows the user to navigate through all states
 * by typing commands as specified in the state transition diagram
 */
void testGameStates() {
    std::cout << "\n=== Testing Game States ===" << std::endl;
    std::cout << "Type 'quit' to exit the test." << std::endl;
    std::cout << "Type 'help' to see valid commands for the current state." << std::endl;
    std::cout << "Type 'history' to see the state transition history." << std::endl;

    GameEngine engine;
    std::string input;

    std::cout << "\nStarting Game Engine..." << std::endl;
    std::cout << "Current state: " << engine.getCurrentStateName() << std::endl;
    engine.displayValidCommands();

    while (true) {
        std::cout << "\nEnter command: ";
        std::getline(std::cin, input);

        // Handle special commands
        if (input == "quit") {
            std::cout << "Exiting Game Engine test." << std::endl;
            break;
        } else if (input == "help") {
            engine.displayValidCommands();
            continue;
        } else if (input == "history") {
            engine.displayStateHistory();
            continue;
        }

        // Process regular game commands
        engine.processCommand(input);

        // Show current state after command processing
        std::cout << "Current state: " << engine.getCurrentStateName() << std::endl;

        // End the test if we reach the "end" state
        if (engine.getCurrentStateName() == "end") {
            std::cout << "Game has ended. Final state reached." << std::endl;
            engine.displayStateHistory();
            break;
        }
    }

    std::cout << "\n=== End of Game States Test ===" << std::endl;
}

/**
 * Demonstration function that shows automated testing of the game states
 * This function demonstrates various state transitions automatically
 */
void demonstrateGameStates() {
    std::cout << "\n=== Automated Game States Demonstration ===" << std::endl;

    GameEngine engine;

    // Demonstrate a typical game flow
    std::string commands[] = {
        "start",           // startup -> start
        "loadmap",         // start -> map loaded
        "validatemap",     // map loaded -> map validated
        "addplayer",       // map validated -> players added
        "addplayer",       // players added -> players added (add another player)
        "assigncountries", // players added -> assigncountries
        "play",            // assigncountries -> play
        "play",            // play -> assign reinforcement
        "issueorder",      // assign reinforcement -> issue orders
        "issueorder",      // issue orders -> issue orders (issue another order)
        "endissueorders",  // issue orders -> execute orders
        "execorder",       // execute orders -> execute orders
        "endexecorders",   // execute orders -> assign reinforcement
        "issueorder",      // assign reinforcement -> issue orders (next turn)
        "endissueorders",  // issue orders -> execute orders
        "win",             // execute orders -> win
        "end"              // win -> end
    };

    std::cout << "Current state: " << engine.getCurrentStateName() << std::endl;

    for (const std::string& command : commands) {
        std::cout << "\nExecuting command: " << command << std::endl;
        engine.processCommand(command);
        std::cout << "Current state: " << engine.getCurrentStateName() << std::endl;

        if (engine.getCurrentStateName() == "end") {
            std::cout << "Reached end state. Demonstration complete." << std::endl;
            break;
        }
    }

    engine.displayStateHistory();
    std::cout << "\n=== End of Automated Demonstration ===" << std::endl;
}

/**
 * Function to test invalid commands and error handling
 */
void testInvalidCommands() {
    std::cout << "\n=== Testing Invalid Commands ===" << std::endl;

    GameEngine engine;

    // Test invalid commands from startup state
    std::cout << "Testing invalid commands from startup state:" << std::endl;
    engine.processCommand("loadmap");        // Invalid from startup
    engine.processCommand("validatemap");    // Invalid from startup
    engine.processCommand("invalidcommand"); // Completely invalid

    // Move to a different state and test invalid commands
    engine.processCommand("start");          // Valid transition
    std::cout << "\nTesting invalid commands from start state:" << std::endl;
    engine.processCommand("start");          // Invalid from start
    engine.processCommand("validatemap");    // Invalid from start
    engine.processCommand("addplayer");      // Invalid from start

    std::cout << "\n=== End of Invalid Commands Test ===" << std::endl;
}

//function to test startupPhase() in gameEngine
void testStartupPhase() {
    std::cout << "\n=== testStartupPhase: loadmap + validatemap ===\n";
    GameEngine engine;
    std::cout << "Current state: " << engine.getCurrentStateName() << "\n";
    engine.startupPhase(); 
}