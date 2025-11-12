#include "CommandProcessing.h"
#include "GameEngine.h"

#include <iostream>
#include <vector>


// ==================== Command Class Implementation ====================
Command::Command() :
  command(new std::string("")),
  effect(new std::string("")) {}

Command::Command(const std::string& cmd) :
  command(new std::string(cmd)),
  effect(new std::string("")) {}

Command::Command(const Command& other) :
  command(new std::string(*other.command)),
  effect(new std::string(*other.effect)) {}

Command& Command::operator=(const Command& other) {
  if (this != &other) {
    // clear previous memory
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
void Command::saveEffect(const std::string& eff) const {
  *effect = eff;
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const Command& cmd) {
  os << "Command[" << (cmd.command ? *cmd.command : "<Unknown>")
     << ", Effect:" << (cmd.effect ? *cmd.effect : "<Nil>") << "]";
  return os;
}


// ==================== CommandProcessor Class Implementation ====================
CommandProcessor::CommandProcessor() : commands(new std::vector<Command*>()) {}

CommandProcessor::CommandProcessor(const CommandProcessor& other) : commands(new std::vector<Command*>()) {
  // deep copy commands
  for (const Command* cmd : *other.commands) {
    commands->push_back(new Command(*cmd));
  }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
  if (this != &other) {
    // delete old objects
    for (const Command* cmd : *commands) {
      delete cmd;
    }
    commands->clear();

    // deep copy commands
    for (const Command* cmd : *other.commands) {
      commands->push_back(new Command(*cmd));
    }
  }
  return *this;
}

CommandProcessor::~CommandProcessor() {
  for (const Command* cmd : *commands) {
    delete cmd;
  }
  delete commands;
  commands = nullptr;
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
