#include "CommandProcessing.h"
#include "GameEngine.h"

#include <iostream>
#include <vector>


// ==================== Command Class Implementation ====================
Command::Command() :
  Subject(),
  command(new std::string("")),
  effect(new std::string("")) {}

Command::Command(const std::string& cmd) :
  Subject(),
  command(new std::string(cmd)),
  effect(new std::string("")) {}

Command::Command(const Command& other) :
  Subject(other),
  command(new std::string(*other.command)),
  effect(new std::string(*other.effect)) {}

Command& Command::operator=(const Command& other) {
  if (this != &other) {
    // clear previous memory
    Subject::operator=(other);
    delete command;
    delete effect;

    command = new std::string(*other.command);
    effect = new std::string(*other.effect);
  }
  return *this;
}

Command::~Command() {
  delete command;
  delete effect;
  command = nullptr;
  effect = nullptr;
}

// --- GETTERS ---
std::string Command::getCommand() const {
  return *command;
}

std::string Command::getEffect() const {
  return *effect;
}

// --- UTILITY ---
void Command::saveEffect(const std::string& eff) {
  *effect = eff;
  notify();
}

std::string Command::stringToLog() const {
  return "Command: " + (command ? *command : "<Unknown>") +
         " Effect: " + (effect ? *effect : "<None>");
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const Command& cmd) {
  os << "Command[" << (cmd.command ? *cmd.command : "<Unknown>")
     << ", Effect:" << (cmd.effect ? *cmd.effect : "<Nil>") << "]";
  return os;
}


// ==================== CommandProcessor Class Implementation ====================
CommandProcessor::CommandProcessor() :
  Subject(),
  commands(new std::vector<Command*>()),
  lastSavedCommand(nullptr) {}

CommandProcessor::CommandProcessor(const CommandProcessor& other) :
  Subject(other),
  commands(new std::vector<Command*>()),
  lastSavedCommand(nullptr) {
  // deep copy commands
  for (const Command* cmd : *other.commands) {
    commands->push_back(new Command(*cmd));
  }
  if (!commands->empty()) {
    lastSavedCommand = commands->back();
  }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
  if (this != &other) {
    Subject::operator=(other);
    // delete old objects
    for (const Command* cmd : *commands) {
      delete cmd;
    }
    commands->clear();

    // deep copy commands
    for (const Command* cmd : *other.commands) {
      commands->push_back(new Command(*cmd));
    }
    lastSavedCommand = commands->empty() ? nullptr : commands->back();
  }
  return *this;
}

CommandProcessor::~CommandProcessor() {
  for (const Command* cmd : *commands) {
    delete cmd;
  }
  delete commands;
  commands = nullptr;
  lastSavedCommand = nullptr;
}

// --- GETTERS ---
Command* CommandProcessor::getCommand() {
  const std::string cmdStr = readCommand();
  Command* cmd = new Command(cmdStr);

  saveCommand(cmd);
  return cmd;
}

std::vector<Command*> CommandProcessor::getCommands() const {
  return *commands;
}

// --- UTILITY ---
std::string CommandProcessor::readCommand() const {
  std::string cmdStr;
  std::cout << "Enter command: ";
  std::getline(std::cin, cmdStr);
  return cmdStr;
}

void CommandProcessor::saveCommand(Command* cmd) {
  commands->push_back(cmd);
  lastSavedCommand = cmd;

  // Propagate observers to newly saved command
  if (cmd) {
    for (Observer* observer : *observers) {
      cmd->attach(observer);
    }
  }

  notify();
}

bool CommandProcessor::validate(const std::string& cmd, const GameEngine* engine) const {
  if (cmd.empty() || engine == nullptr) {
    return false;
  }

  // get the current state name
  std::string currState = engine->getCurrentStateName();

  // parse the base command
  std::string baseCmd = cmd;
  std::size_t spacePos = cmd.find(' ');
  if (spacePos != std::string::npos) {
    baseCmd = cmd.substr(0, spacePos);
  }

  if (baseCmd == "tournament") {
    if (currState != "start") {
      std::cout << "Tournament command can only be used in 'start' state.\n";
      return false;
    }

    std::size_t mPos = cmd.find("-M ");
    std::size_t pPos = cmd.find("-P ");
    std::size_t gPos = cmd.find("-G ");
    std::size_t dPos = cmd.find("-D ");

    if (mPos == std::string::npos || pPos == std::string::npos ||
      gPos == std::string::npos || dPos == std::string::npos) {
      std::cout << "Incorrect format. Must follow: tournament -M <listofmapfiles> -P "
                   "<listofplayerstrategies> -G <numberofgames> -D <maxnumberofturns>\n";
      return false;
    }

    // Ensure flags are in correct order
    if (!(mPos < pPos && pPos < gPos && gPos < dPos)) {
      std::cout << "Flags must be in order: -M, -P, -G, -D\n";
      return false;
    }

    // Extract parameter strings
    std::string mapsStr = cmd.substr(mPos + 3, pPos - mPos - 3);
    std::string playersStr = cmd.substr(pPos + 3, gPos - pPos - 3);
    std::string gamesStr = cmd.substr(gPos + 3, dPos - gPos - 3);
    std::string turnsStr = cmd.substr(dPos + 3);

    // Helper lambda to trim whitespace
    auto trim = [](std::string& s) {
      // Trim leading whitespace
      size_t start = s.find_first_not_of(" \t\n\r");
      if (start == std::string::npos) {
        s = "";
        return;
      }
      // Trim trailing whitespace
      size_t end = s.find_last_not_of(" \t\n\r");
      s = s.substr(start, end - start + 1);
    };

    trim(mapsStr);
    trim(playersStr);
    trim(gamesStr);
    trim(turnsStr);

    // Validate maps: count comma-separated items (1-5 maps)
    if (mapsStr.empty()) {
      std::cout << "No map files specified.\n";
      return false;
    }
    int mapCount = 1;
    for (char c : mapsStr) {
      if (c == ',') mapCount++;
    }
    if (mapCount < 1 || mapCount > 5) {
      std::cout << "Invalid number of maps. Must be between 1 and 5.\n";
      return false;
    }

    // Validate players: count comma-separated items (2-4 strategies)
    if (playersStr.empty()) {
      std::cout << "No player strategies specified.\n";
      return false;
    }
    int playerCount = 1;
    for (char c : playersStr) {
      if (c == ',') playerCount++;
    }
    if (playerCount < 2 || playerCount > 4) {
      std::cout << "Invalid number of player strategies. Must be between 2 and 4.\n";
      return false;
    }

    // Validate number of games (1-5)
    if (gamesStr.empty()) {
      std::cout << "No number of games specified.\n";
      return false;
    }
    int numGames;
    try {
      numGames = std::stoi(gamesStr);
    } catch (...) {
      std::cout << "Invalid number of games. Must be an integer.\n";
      return false;
    }
    if (numGames < 1 || numGames > 5) {
      std::cout << "Invalid number of games. Must be between 1 and 5.\n";
      return false;
    }

    // Validate max turns (10-50)
    if (turnsStr.empty()) {
      std::cout << "No max number of turns specified.\n";
      return false;
    }
    int maxTurns;
    try {
      maxTurns = std::stoi(turnsStr);
    } catch (...) {
      std::cout << "Invalid max number of turns. Must be an integer.\n";
      return false;
    }
    if (maxTurns < 10 || maxTurns > 50) {
      std::cout << "Invalid max number of turns. Must be between 10 and 50.\n";
      return false;
    }

    // all validations passed
    return true;
  }

  // validate based on current state according to assignment state diagram
  if (currState == "startup" && baseCmd == "start") return true;
  if (currState == "start" && baseCmd == "loadmap") return true;
  if (currState == "map loaded" && (baseCmd == "loadmap" || baseCmd == "validatemap")) return true;
  if (currState == "map validated" && baseCmd == "addplayer") return true;
  if (currState == "players added" && (baseCmd == "addplayer" || baseCmd == "gamestart")) return true;
  if (currState == "assign reinforcement" && baseCmd == "issueorder") return true;
  if (currState == "issue orders" && (baseCmd == "issueorder" || baseCmd == "endissueorders")) return true;
  if (currState == "execute orders" && (baseCmd == "execorder" || baseCmd == "endexecorders" || baseCmd == "win")) return true;
  if (currState == "win" && (baseCmd == "replay" || baseCmd == "quit")) return true;
  if (baseCmd == "help") return true;

  return false;
}

std::string CommandProcessor::stringToLog() const {
  if (lastSavedCommand) {
    return "CommandProcessor saved: " + lastSavedCommand->getCommand();
  }
  return "CommandProcessor has no commands saved.";
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp) {
  os << "CommandProcessor[" << cp.commands->size() << " commands:\n";
  for (size_t i = 0; i < cp.commands->size(); i++) {
    os << " " << (i + 1) << ". " << *(*cp.commands)[i] << "\n";
  }
  os << "]";
  return os;
}


// ==================== FileLineReader Class Implementation ====================
FileLineReader::FileLineReader() :
  file(new std::ifstream()),
  fileName(new std::string("")) {}

FileLineReader::FileLineReader(const std::string& name) :
  file(new std::ifstream()),
  fileName(new std::string(name)) {
  file->open(*fileName);
}

FileLineReader::FileLineReader(const FileLineReader& other) :
  file(new std::ifstream()),
  fileName(new std::string(*other.fileName)) {
  if (!other.fileName->empty()) {
    file->open(*fileName);
  }
}

FileLineReader& FileLineReader::operator=(const FileLineReader& other) {
  if (this != &other) {
    if (file->is_open()) {
      file->close();
    }
    delete file;
    delete fileName;

    file = new std::ifstream();
    fileName = new std::string(*other.fileName);
    if (!other.fileName->empty()) {
      file->open(*fileName);
    }
  }
  return *this;
}

FileLineReader::~FileLineReader() {
  if (file->is_open()) {
    file->close();
  }

  delete file;
  delete fileName;
  file = nullptr;
  fileName = nullptr;
}

// --- UTILITY ---
std::string FileLineReader::readLineFromFile() const {
  std::string line;
  if (file->is_open() && std::getline(*file, line)) {
    return line;
  }
  return "";
}

bool FileLineReader::hasMoreLines() const {
  return file->is_open() && !file->eof();
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const FileLineReader& flr) {
  os << "FileLineReader[reading from: " << (flr.fileName ? *flr.fileName : "<Nil>") << "]";
  return os;
}


// ==================== FileCommandProcessorAdapter Class Implementation ====================
FileCommandProcessorAdapter::FileCommandProcessorAdapter() :
  CommandProcessor(),
  fileReader(new FileLineReader()) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const std::string& fileName) :
 CommandProcessor(),
 fileReader(new FileLineReader(fileName)) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other) :
 CommandProcessor(other),
 fileReader(new FileLineReader(*other.fileReader)) {}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
  if (this != &other) {
    CommandProcessor::operator=(other);
    delete fileReader;
    fileReader = new FileLineReader(*other.fileReader);
  }
  return *this;
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
  delete fileReader;
  fileReader = nullptr;
}

// --- UTILITY ---
std::string FileCommandProcessorAdapter::readCommand() const {
  std::string line = fileReader->readLineFromFile();
  if (!line.empty()) {
    std::cout << "Reading from file: " << line << std::endl;
  }
  return line;
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const FileCommandProcessorAdapter& fcpa) {
  os << "FileCommandProcessorAdapter[";
  if (fcpa.fileReader) {
    os << *fcpa.fileReader;
  } else {
    os << "<None>";
  }
  os << "]";
  return os;
}