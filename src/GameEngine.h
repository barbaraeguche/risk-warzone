#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

class State;
class Transition;
class Map;
class Player;
class Deck;
class Territory;
class Order;


class GameEngine {
private:
    State* currentState;
    std::map<std::string, State*>* states;
    std::vector<std::string>* stateHistory;
    std::vector<Player*>* players;
    Map* map;
    Deck* deck;

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

    void mainGameLoop();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();

    void addPlayer(Player* player);
    void setMap(Map* map);
    void setDeck(Deck* deck);

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
