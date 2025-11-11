#include "CommandProcessing.h"
#include "GameEngine.h"

void testCommandProcessor() {
  std::cout << "=== Command Processor Test ===" << std::endl;
  std::cout << "Select input mode: " << std::endl;
  std::cout << "1. Console input (-console)" << std::endl;
  std::cout << "2. File input (-file <filename>)" << std::endl;
  std::cout << "Enter choice: ";

  std::string choice;
  std::getline(std::cin, choice);

  CommandProcessor* processor = nullptr;

  // determine the input mode
  if (choice.find("-file") == std::string::npos) {
    // (1) commands can be read from the console using the CommandProcessor class
    processor = new CommandProcessor();
    std::cout << "Reading commands from console" << std::endl;
  } else {
    // extract the file name
    const size_t pos = choice.find("-file");
    std::string filename = choice.substr(pos + 6);

    // trim any whitespace
    filename.erase(0, filename.find_first_not_of(" \t"));
    filename.erase(filename.find_last_not_of(" \t") + 1);

    // (2) commands can be read from a saved text file using the FileCommandProcessorAdapter
    processor = new FileCommandProcessorAdapter(filename);
    std::cout << "Reading commands from file: " << filename << std::endl;
  }

  // create a game engine to test
  GameEngine* engine = new GameEngine();

  std::cout << "\n" << *engine << std::endl;
  std::cout << "\nEnter commands to navigate through states." << std::endl;
  std::cout << "Enter 'quit' or empty command to exit.\n" << std::endl;

  // display valid commands for the current state
  engine->displayValidCommands();
  std::cout << std::endl;

  // process commands until end state or empty command
  while (engine->getCurrentStateName() != "end") {
    // get the command from the processor
    Command* cmd = processor->getCommand();

    // check for empty command or quit
    if (cmd->getCommand().empty() || cmd->getCommand() == "quit") {
      std::cout << "Exiting command processor." << std::endl;
      break;
    }

    // validate the command using the CommandProcessor's validate method
    bool isValid = processor->validate(cmd->getCommand(), engine);

    /*
    * (3) commands that are invalid in the current game state are rejected,
    * and valid commands result in the correct effect and state change
    */
    if (isValid) {
      // process the command to get the effect
      std::string oldState = engine->getCurrentStateName();
      engine->processCommand(cmd->getCommand());
      std::string newState = engine->getCurrentStateName();

      // save the effect of the transition
      std::string effect = "Transitioned from '" + oldState + "' to '" + newState + "'";
      cmd->saveEffect(effect);
    } else {
      std::string errorMsg = "Invalid command '" + cmd->getCommand() +
                             "' in state '" + engine->getCurrentStateName() + "'";
      cmd->saveEffect(errorMsg);
      std::cout << errorMsg << std::endl;
    }

    std::cout << "\n" << *engine << std::endl;
    engine->displayValidCommands();
    std::cout << std::endl;
  }

  // display commands and their effects
  std::cout << "\n=== Command History ===" << std::endl;
  std::cout << *processor << std::endl;

  std::cout << "\n=== State Transition History ===" << std::endl;
  engine->displayStateHistory();

  // free memory
  delete engine;
  delete processor;
};
