#include "GameEngine.h"
#include "CommandProcessing.h"
#include <iostream>
#include <limits>

// tournament -M Canada/Canada.map, Alberta/Alberta.map, Palmerica/Palmerica.map -P Aggressive, Benevolent, Neutral, Cheater -G 4 -D 30
// tournament -M Canada/Canada.map, Alberta/Alberta.map, Palmerica/Palmerica.map -P Aggressive, Aggressive, Neutral -G 1 -D 180


/**
 * Test driver for Tournament Mode
 * Demonstrates that the tournament command can be:
 * 1. Processed and validated by the CommandProcessor
 * 2. Executed by the GameEngine
 * 3. Results in a tournament being played with proper results logging
 */
void testTournament() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "TOURNAMENT MODE TEST DRIVER\n";
    std::cout << std::string(60, '=') << "\n\n";

    // clear any leftover input buffer state from previous tests
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // select input mode
    std::cout << "Select input mode: " << std::endl;
    std::cout << "1. Console input (-console)" << std::endl;
    std::cout << "2. File input (-file <filename>)" << std::endl;
    std::cout << "Enter choice: ";

    std::string choice;
    std::getline(std::cin, choice);

    CommandProcessor* processor = nullptr;

    // determine the input mode
    if (choice.find("-file") == std::string::npos) {
        // commands can be read from the console using the CommandProcessor class
        processor = new CommandProcessor();
        std::cout << "Reading commands from console\n\n";
    } else {
        // extract the file name
        const size_t pos = choice.find("-file");
        std::string filename = choice.substr(pos + 6);

        // trim any whitespace
        filename.erase(0, filename.find_first_not_of(" \t"));
        filename.erase(filename.find_last_not_of(" \t") + 1);

        // commands can be read from a saved text file using the FileCommandProcessorAdapter
        processor = new FileCommandProcessorAdapter(filename);
        std::cout << "Reading commands from file: " << filename << "\n\n";
    }

    GameEngine engine;
    std::cout << "Current game state: " << engine.getCurrentStateName() << "\n\n";

    // transition to start state (required for tournament command)
    std::cout << "Step 1: Transitioning to 'start' state...\n";
    engine.transitionState("start");
    std::cout << "Current game state: " << engine.getCurrentStateName() << "\n\n";

    // get tournament command from processor
    std::cout << "Step 2: Reading tournament command...\n";
    Command* cmd = processor->getCommand();
    std::string tournamentCmd = cmd->getCommand();

    if (tournamentCmd.empty()) {
        std::cout << "No command entered. Exiting tournament test.\n";
        delete cmd;
        delete processor;
        return;
    }

    std::cout << "Command: " << tournamentCmd << "\n";
    std::cout << "\nStep 3: Validating tournament command...\n";

    // validate the tournament command
    bool isValid = processor->validate(tournamentCmd, &engine);

    if (isValid) {
        std::cout << "Validation: PASSED\n\n";
        std::cout << "Step 4: Executing tournament...\n";

        // execute the tournament
        engine.executeTournament(tournamentCmd);
        cmd->saveEffect("Tournament executed successfully");

        std::cout << "\nTournament test completed successfully!\n";
        std::cout << "Results have been displayed above and logged to gamelog.txt\n";
    } else {
        std::cout << "Validation: FAILED\n";
        std::cout << "The tournament command was rejected by the CommandProcessor.\n";
        cmd->saveEffect("Invalid tournament command");
    }

    // display command history
    std::cout << "\n=== Command History ===" << std::endl;
    std::cout << *processor << std::endl;

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "END OF TOURNAMENT MODE TEST\n";
    std::cout << std::string(60, '=') << "\n";

    // free memory
    delete processor;
}