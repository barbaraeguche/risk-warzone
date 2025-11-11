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

    Map* map = new Map();

    // Create a few territories for demonstration
    Territory* t1 = new Territory("Alaska", 1);
    Territory* t2 = new Territory("Northwest Territory", 2);
    Territory* t3 = new Territory("Greenland", 3);
    Territory* t4 = new Territory("Alberta", 4);

    // Add territories to a continent
    Continent* c1 = map->addContinent("North America", 1, 5);
    Continent* c2 = map->addContinent("Europe", 2, 3);
    c1->addTerritory(t1);
    c1->addTerritory(t2);
    c1->addTerritory(t3);
    c1->addTerritory(t4);

    engine.setMap(map);

    // 2. Create players
    Player* p1 = new Player("Alice");
    Player* p2 = new Player("Bob");

    // Add players to the game
    engine.addPlayer(p1);
    engine.addPlayer(p2);

    // 3. Assign territories
    p1->addTerritory(t1);
    p1->addTerritory(t2);
    t1->setOwner(p1);
    t2->setOwner(p1);

    p2->addTerritory(t3);
    p2->addTerritory(t4);
    t3->setOwner(p2);
    t4->setOwner(p2);

    // 4. Set initial reinforcement pool
    p1->setReinforcementPool(50);
    p2->setReinforcementPool(50);

    // 5. Give initial cards
    Deck* deck = new Deck();
    p1->getHand()->addCard(deck->draw());
    p1->getHand()->addCard(deck->draw());

    p2->getHand()->addCard(deck->draw());
    p2->getHand()->addCard(deck->draw());

    // Optional: store deck in game if needed
    engine.setDeck(deck);

    std::cout << "Mock setup complete: 2 players, 4 territories, initial reinforcements and cards assigned.\n";

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