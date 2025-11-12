#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>   
#include "Player.h"
#include "Map.h"
#include "LoggingObserver.h"


class State;
class Transition;
class GameCommand;
class Map;         
class Player;      
class Deck;
class CommandProcessor;
class Territory;
class Order;

class GameEngine : public Subject, public ILoggable {
private:
    State* currentState;
    std::map<std::string, State*>* states;
    std::vector<std::string>* stateHistory;
    std::unique_ptr<Map> map_;         //  add: holds the loaded/validated map
    std::vector<Player*> players_;     // players created by addplayer
    Deck* deck_;
    static Player* neutralPlayer_;

    //A2: startup phase state
    std::unique_ptr<Map> map_;         //  add: holds the loaded/validated map using smart pointer
    std::vector<Player*> players_;     // players created by addplayer
    Deck* deck_;
public:
    GameEngine();
    GameEngine(const GameEngine& other);
    ~GameEngine();

    GameEngine& operator=(const GameEngine& other);

    friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine);

    void processCommand(const std::string& command);
    void transitionState(const std::string& stateName);
    std::string getCurrentStateName() const;
    void displayValidCommands() const;
    void displayStateHistory() const;
    std::string stringToLog() const override;

    void startupPhase();
    void startupPhase(CommandProcessor&);

    void mainGameLoop();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    void addPlayer(Player* player);
    std::vector<Player*> getPlayers() const;
    void setMap(Map* map);
    void loadMap(const std::string& filename);
    void setDeck(Deck* deck);

    static Player* getNeutralPlayer();

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


class GameCommand {
private:
    std::string* commandString;

public:
    GameCommand();
    GameCommand(const std::string& command);
    GameCommand(const GameCommand& other);
    ~GameCommand();

    GameCommand& operator=(const GameCommand& other);

    friend std::ostream& operator<<(std::ostream& os, const GameCommand& command);

    std::string getCommandString() const;
    bool isValid() const;
};

void testGameStates();
