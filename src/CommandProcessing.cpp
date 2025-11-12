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

  // validate based on current state
  if (currState == "startup" && baseCmd == "start") return true;
  if (currState == "start" && baseCmd == "loadmap") return true;
  if (currState == "map loaded" && (baseCmd == "loadmap" || baseCmd == "validatemap")) return true;
  if (currState == "map validated" && baseCmd == "addplayer") return true;
  if (currState == "players added" && (baseCmd == "addplayer" || baseCmd == "gamestart")) return true;
  if (currState == "assigncountries" && baseCmd == "play") return true;
  if (currState == "play" && baseCmd == "play") return true;
  if (currState == "assign reinforcement" && baseCmd == "issueorder") return true;
  if (currState == "issue orders" && (baseCmd == "issueorder" || baseCmd == "endissueorders")) return true;
  if (currState == "execute orders" && (baseCmd == "execorder" || baseCmd == "endexecorders" || baseCmd == "win")) return true;
  if (currState == "win" && (baseCmd == "end" || baseCmd == "play")) return true;
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
