#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "LoggingObserver.h"

// forward declarations
class GameEngine;

/**
 * The Command class represents a game command with its associated effect.
 * It stores the command string and the effect of executing that command.
 */
class Command : public Subject, public ILoggable {
private:
  std::string* command; // the command string
  std::string* effect;  // the effect/result of the command

public:
  // constructors
  Command();
  Command(const std::string& cmd);
  Command(const Command& other); // copy constructor
  Command& operator=(const Command& other); // assignment operator
  ~Command(); // destructor

  // getters
  std::string getCommand() const;
  std::string getEffect() const;

  // utility
  void saveEffect(const std::string& eff);
  std::string stringToLog() const override;

  // stream insertion
  friend std::ostream& operator<<(std::ostream& os, const Command& cmd);
};


/**
 * The CommandProcessor class handles reading and storing commands.
 * The commands can be read from the console or overridden by subclasses.
 * Game components must get their commands from this processor.
 */
class CommandProcessor : public Subject, public ILoggable {
private:
  std::vector<Command*>* commands; // a collection of command objects
  Command* lastSavedCommand;

public:
  CommandProcessor();
  CommandProcessor(const CommandProcessor& other); // copy constructor
  CommandProcessor& operator=(const CommandProcessor& other); // assignment operator
  virtual ~CommandProcessor(); // destructor

  // getters
  Command* getCommand();
  std::vector<Command*> getCommands() const;

  // utility
  void saveCommand(Command* cmd);
  bool validate(const std::string& cmd, const GameEngine* engine) const;
  std::string stringToLog() const override;

  // stream insertion
  friend std::ostream& operator<<(std::ostream& os, const CommandProcessor& cp);

private:
  // utility
  virtual std::string readCommand() const;
};


/**
 * The FileLineReader class reads lines from a text file.
 */
class FileLineReader {
private:
  std::ifstream* file;   // file stream pointer
  std::string* fileName; // name of the file

public:
  FileLineReader();
  FileLineReader(const std::string& name);
  FileLineReader(const FileLineReader& other); // copy constructor
  FileLineReader& operator=(const FileLineReader& other); // assignment operator
  ~FileLineReader(); // destructor

  // utility
  std::string readLineFromFile() const;
  bool hasMoreLines() const;

  // stream insertion
  friend std::ostream& operator<<(std::ostream& os, const FileLineReader& flr);
};

/**
 * The FileCommandProcessorAdapter class adapts CommandProcessor to read from files.
 * It implements the Adapter design pattern.
 */
class FileCommandProcessorAdapter : public CommandProcessor {
private:
  FileLineReader* fileReader;

public:
  FileCommandProcessorAdapter();
  FileCommandProcessorAdapter(const std::string& fileName);
  FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other); // copy constructor
  FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other); // assignment operator
  ~FileCommandProcessorAdapter(); // destructor

  // stream insertion
  friend std::ostream& operator<<(std::ostream& os, const FileCommandProcessorAdapter& fcpa);

private:
  // utility
  std::string readCommand() const override;
};

// free function
void testCommandProcessor();
