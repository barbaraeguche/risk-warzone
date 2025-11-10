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

  if (choice.find("-file") == std::string::npos) {
    // (1) commands can be read from the console using the CommandProcessor class
    processor = new CommandProcessor();
    std::cout << "Reading commands from console" << std::endl;
  } else {
    // extract the file name
    size_t pos = choice.find("-file");
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






  /*
   * (3) commands that are invalid in the current game state are rejected,
   * and valid commands result in the correct effect and state change
   */

  // display commands and their effects
  std::cout << "\n=== Command History ===" << std::endl;
  std::cout << *processor << std::endl;

  std::cout << "\n=== Final Game State ===" << std::endl;
  std::cout << *engine << std::endl;

  // free memory
  delete processor;
  delete engine;
};
