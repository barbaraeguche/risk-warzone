#ifndef COMP_354_GAMEENGINE_H
#define COMP_354_GAMEENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

class State;
class Transition;
class Command;


class GameEngine {
private:
    State* currentState;
    std::map<std::string, State*>* states;
    std::vector<std::string>* stateHistory;

public:
    GameEngine();
    GameEngine(const GameEngine& other);
    ~GameEngine();

    GameEngine& operator=(const GameEngine& other);

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine);

    void processCommand(const std::string& command);
    void transitionTo(const std::string& stateName);
    std::string getCurrentStateName() const;
    void displayValidCommands() const;
    void displayStateHistory() const;

private:
    void initializeStates();
    void cleanupStates();
};


class State {
private:
    std::string* name;
    std::vector<Transition*>* transitions;

public:
    State();
    State(const std::string& name);
    State(const State& other);
    ~State();

    State& operator=(const State& other);

    friend std::ostream& operator<<(std::ostream& os, const State& state);

    std::string getName() const;
    void addTransition(Transition* transition);
    Transition* getTransition(const std::string& command) const;
    std::vector<std::string> getValidCommands() const;
};


class Transition {
private:
    std::string* command;
    std::string* targetState;

public:
    Transition();
    Transition(const std::string& command, const std::string& targetState);
    Transition(const Transition& other);
    ~Transition();

    Transition& operator=(const Transition& other);

    friend std::ostream& operator<<(std::ostream& os, const Transition& transition);

    std::string getCommand() const;
    std::string getTargetState() const;
};


class Command {
private:
    std::string* commandString;

public:
    Command();
    Command(const std::string& command);
    Command(const Command& other);
    ~Command();

    Command& operator=(const Command& other);

    friend std::ostream& operator<<(std::ostream& os, const Command& command);

    std::string getCommandString() const;
    bool isValid() const;
};

void testGameStates();

#endif //COMP_354_GAMEENGINE_H