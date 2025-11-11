#include "GameEngine.h"
#include "Map.h"
#include "Player.h"
#include "Cards.h"
#include "Orders.h"

#include <algorithm>
#include <iostream>

// ========== GameEngine Implementation ==========

/**
 * Default constructor for GameEngine
 * Initializes the game engine with startup state
 */
GameEngine::GameEngine() {
    states = new std::map<std::string, State*>();
    stateHistory = new std::vector<std::string>();
    initializeStates();
    currentState = (*states)["startup"];
    stateHistory->push_back("startup");
    players = new std::vector<Player*>();
    map = nullptr;
    deck = nullptr;
}

/**
 * Copy constructor for GameEngine
 * @param other The GameEngine object to copy from
 */
GameEngine::GameEngine(const GameEngine& other) {
    states = new std::map<std::string, State*>();
    stateHistory = new std::vector<std::string>(*other.stateHistory);

    // Deep copy all states
    for (const auto& pair : *other.states) {
        (*states)[pair.first] = new State(*pair.second);
    }

    currentState = (*states)[other.getCurrentStateName()];

    //Deep Copy Map
    if (other.map != nullptr) {
        map = new Map(*other.map);
    } else {
        map = nullptr;
    }

    //Deep Copy Deck
    if (other.deck != nullptr) {
        deck = new Deck(*other.deck);
    } else {
        deck = nullptr;
    }

    //Deep Copy Players
    players->clear();
    for (Player* p : *other.players) {
        players->push_back(new Player(*p)); 
    }
}

/**
 * Destructor for GameEngine
 * Cleans up all allocated memory
 */
GameEngine::~GameEngine() {
    cleanupStates();
    delete states;
    delete stateHistory;
    delete map;
    delete deck;
    for (auto* player : *players) {
        delete player;
    }
    delete players;
}

/**
 * Assignment operator for GameEngine
 * @param other The GameEngine object to assign from
 * @return Reference to this GameEngine
 */
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        // Clean up current resources
        cleanupStates();
        delete states;
        delete stateHistory;

        // Copy from other
        states = new std::map<std::string, State*>();
        stateHistory = new std::vector<std::string>(*other.stateHistory);

        for (const auto& pair : *other.states) {
            (*states)[pair.first] = new State(*pair.second);
        }

        currentState = (*states)[other.getCurrentStateName()];
    }
    return *this;
}

/**
 * Stream insertion operator for GameEngine
 * @param os Output stream
 * @param engine GameEngine object to output
 * @return Reference to the output stream
 */
std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {
    os << "GameEngine - Current State: " << engine.getCurrentStateName();
    return os;
}

/**
 * Process a command and trigger appropriate state transition
 * @param command The command string to process
 */
void GameEngine::processCommand(const std::string& command) {
    Transition* transition = currentState->getTransition(command);

    if (transition != nullptr) {
        std::cout << "Executing command: " << command << std::endl;
        transitionTo(transition->getTargetState());
    } else {
        std::cout << "Invalid command: " << command << std::endl;
        std::cout << "Valid commands for state '" << currentState->getName() << "': ";
        std::vector<std::string> validCommands = currentState->getValidCommands();
        for (size_t i = 0; i < validCommands.size(); ++i) {
            std::cout << validCommands[i];
            if (i < validCommands.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

/**
 * Transition to a new state
 * @param stateName Name of the state to transition to
 */
void GameEngine::transitionTo(const std::string& stateName) {
    auto stateIt = states->find(stateName);
    if (stateIt != states->end()) {
        currentState = stateIt->second;
        stateHistory->push_back(stateName);
        std::cout << "Transitioned to state: " << stateName << std::endl;
    } else {
        std::cout << "Error: State '" << stateName << "' does not exist!" << std::endl;
    }
}

/**
 * Get the name of the current state
 * @return Current state name
 */
std::string GameEngine::getCurrentStateName() const {
    return currentState->getName();
}

/**
 * Display valid commands for the current state
 */
void GameEngine::displayValidCommands() const {
    std::cout << "Valid commands for state '" << currentState->getName() << "': ";
    std::vector<std::string> validCommands = currentState->getValidCommands();
    for (size_t i = 0; i < validCommands.size(); ++i) {
        std::cout << validCommands[i];
        if (i < validCommands.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

/**
 * Display the history of state transitions
 */
void GameEngine::displayStateHistory() const {
    std::cout << "State transition history: ";
    for (size_t i = 0; i < stateHistory->size(); ++i) {
        std::cout << (*stateHistory)[i];
        if (i < stateHistory->size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
}

/**
 * Initialize all game states and their transitions
 */
void GameEngine::initializeStates() {
    // Create all states
    (*states)["startup"] = new State("startup");
    (*states)["start"] = new State("start");
    (*states)["map loaded"] = new State("map loaded");
    (*states)["map validated"] = new State("map validated");
    (*states)["players added"] = new State("players added");
    (*states)["assigncountries"] = new State("assigncountries");
    (*states)["play"] = new State("play");
    (*states)["assign reinforcement"] = new State("assign reinforcement");
    (*states)["issue orders"] = new State("issue orders");
    (*states)["execute orders"] = new State("execute orders");
    (*states)["win"] = new State("win");
    (*states)["end"] = new State("end");

    // Set up transitions based on the state diagram
    (*states)["startup"]->addTransition(new Transition("start", "start"));

    (*states)["start"]->addTransition(new Transition("loadmap", "map loaded"));

    (*states)["map loaded"]->addTransition(new Transition("loadmap", "map loaded"));
    (*states)["map loaded"]->addTransition(new Transition("validatemap", "map validated"));

    (*states)["map validated"]->addTransition(new Transition("addplayer", "players added"));

    (*states)["players added"]->addTransition(new Transition("addplayer", "players added"));
    (*states)["players added"]->addTransition(new Transition("assigncountries", "assigncountries"));

    (*states)["assigncountries"]->addTransition(new Transition("play", "play"));

    (*states)["play"]->addTransition(new Transition("play", "assign reinforcement"));

    (*states)["assign reinforcement"]->addTransition(new Transition("issueorder", "issue orders"));

    (*states)["issue orders"]->addTransition(new Transition("issueorder", "issue orders"));
    (*states)["issue orders"]->addTransition(new Transition("endissueorders", "execute orders"));

    (*states)["execute orders"]->addTransition(new Transition("execorder", "execute orders"));
    (*states)["execute orders"]->addTransition(new Transition("endexecorders", "assign reinforcement"));
    (*states)["execute orders"]->addTransition(new Transition("win", "win"));

    (*states)["win"]->addTransition(new Transition("end", "end"));
    (*states)["win"]->addTransition(new Transition("play", "play"));
}

/**
 * Clean up all allocated state objects
 */
void GameEngine::cleanupStates() {
    for (auto& pair : *states) {
        delete pair.second;
    }
    states->clear();
}

// ========== State Implementation ==========

/**
 * Default constructor for State
 */
State::State() {
    name = new std::string("unknown");
    transitions = new std::vector<Transition*>();
}

/**
 * Constructor for State with name
 * @param name The name of the state
 */
State::State(const std::string& name) {
    this->name = new std::string(name);
    transitions = new std::vector<Transition*>();
}

/**
 * Copy constructor for State
 * @param other The State object to copy from
 */
State::State(const State& other) {
    name = new std::string(*other.name);
    transitions = new std::vector<Transition*>();

    // Deep copy all transitions
    for (Transition* transition : *other.transitions) {
        transitions->push_back(new Transition(*transition));
    }
}

/**
 * Destructor for State
 * Cleans up all allocated memory
 */
State::~State() {
    delete name;
    for (Transition* transition : *transitions) {
        delete transition;
    }
    delete transitions;
}

/**
 * Assignment operator for State
 * @param other The State object to assign from
 * @return Reference to this State
 */
State& State::operator=(const State& other) {
    if (this != &other) {
        // Clean up current resources
        delete name;
        for (Transition* transition : *transitions) {
            delete transition;
        }
        delete transitions;

        // Copy from other
        name = new std::string(*other.name);
        transitions = new std::vector<Transition*>();
        for (Transition* transition : *other.transitions) {
            transitions->push_back(new Transition(*transition));
        }
    }
    return *this;
}

/**
 * Stream insertion operator for State
 * @param os Output stream
 * @param state State object to output
 * @return Reference to the output stream
 */
std::ostream& operator<<(std::ostream& os, const State& state) {
    os << "State: " << state.getName();
    return os;
}

/**
 * Get the name of the state
 * @return State name
 */
std::string State::getName() const {
    return *name;
}

/**
 * Add a transition to this state
 * @param transition Pointer to the transition to add
 */
void State::addTransition(Transition* transition) {
    transitions->push_back(transition);
}

/**
 * Get a transition for a given command
 * @param command The command to find transition for
 * @return Pointer to the transition, or nullptr if not found
 */
Transition* State::getTransition(const std::string& command) const {
    for (Transition* transition : *transitions) {
        if (transition->getCommand() == command) {
            return transition;
        }
    }
    return nullptr;
}

/**
 * Get all valid commands for this state
 * @return Vector of valid command strings
 */
std::vector<std::string> State::getValidCommands() const {
    std::vector<std::string> commands;
    for (Transition* transition : *transitions) {
        commands.push_back(transition->getCommand());
    }
    return commands;
}

void GameEngine::addPlayer(Player* player) {
    players->push_back(player);
}
void GameEngine::setMap(Map* map) {
    this->map = map;
}
void GameEngine::setDeck(Deck* deck) {
    this->deck = deck;
}

void GameEngine::mainGameLoop() {
    bool gameOver = false;
    int turn = 1;

    std::cout << "\n=== Starting Main Game Loop ===" << std::endl;

    while (!gameOver) {
        std::cout << "\n=== Turn " << turn << " ===" << std::endl;
        
        // Reinforcement Phase
            transitionTo("assign reinforcement");
            reinforcementPhase();

        for (Player* player : *players) {
            std::cout << "\n--- Player: " << player->getName() << " ---" << std::endl;

            // Issue Orders Phase
            transitionTo("issue orders");
            issueOrdersPhase();

            // Execute Orders Phase
            transitionTo("execute orders");
            executeOrdersPhase();

            // Check win condition
            if (player->getTerritories().size() == map->getTerritories().size()) {
                transitionTo("win");
                std::cout << "Player " << player->getName() << " has won the game!" << std::endl;
                gameOver = true;
                break;
            }
        }
        for (auto it = players->begin(); it != players->end();) {
        Player* p = *it;
        if (p->getTerritories().empty()) {
            std::cout << "Player " << p->getName() << " has been eliminated!\n";
            delete p;
            it = players->erase(it);
        } else {
            ++it;
        }
    }
        turn++;
    }

    transitionTo("end");
    std::cout << "Game Over. Thanks for playing!" << std::endl;
}

void GameEngine::reinforcementPhase() {
    for (Player* player : *players) {
        int reinforcement = std::max(3, static_cast<int>(player->getTerritories().size()) / 3);
        for (auto& continent : map->getContinents()) {
        bool controlsAll = true;
        for (Territory* terr : continent->getTerritories()) {
            if (terr->getOwner() != player) {
                controlsAll = false;
                break;
            }
        }
        if (controlsAll) {
            reinforcement += continent->getControlValue();
        }
    }
        player->setReinforcementPool(reinforcement);
        std::cout << "Player " << player->getName() << " receives " << reinforcement << " reinforcement armies." << std::endl;
    }
}

void GameEngine::issueOrdersPhase() {
    bool anyOrdersLeft;
    do {
        anyOrdersLeft = false;
        for (Player* player : *players) {
            std::cout << "\nPlayer " << player->getName() << "'s turn to issue an order." << std::endl;
            if (player->getReinforcementPool() > 0 || !player->getHand()->getCards()->empty()) {
                std::cout << "Reinforcement Pool: " << player->getReinforcementPool() << std::endl;
                player->issueOrder(); // issue one order at a time
                anyOrdersLeft = true;
            }
        }
    } while (anyOrdersLeft);
}

void GameEngine::executeOrdersPhase() {
    bool ordersLeft;
    do {
        ordersLeft = false;
        for (Player* player : *players) {
            auto* orderList = player->getOrders()->orders;
            if (!orderList->empty()) {
                Order* order = orderList->front();
                order->execute();
                delete order;
                orderList->erase(orderList->begin());
                ordersLeft = true;
            }
        }
    } while (ordersLeft);
}

// ========== Transition Implementation ==========

/**
 * Default constructor for Transition
 */
Transition::Transition() {
    command = new std::string("unknown");
    targetState = new std::string("unknown");
}

/**
 * Constructor for Transition with command and target state
 * @param command The command that triggers the transition
 * @param targetState The target state name
 */
Transition::Transition(const std::string& command, const std::string& targetState) {
    this->command = new std::string(command);
    this->targetState = new std::string(targetState);
}

/**
 * Copy constructor for Transition
 * @param other The Transition object to copy from
 */
Transition::Transition(const Transition& other) {
    command = new std::string(*other.command);
    targetState = new std::string(*other.targetState);
}

/**
 * Destructor for Transition
 * Cleans up all allocated memory
 */
Transition::~Transition() {
    delete command;
    delete targetState;
}

/**
 * Assignment operator for Transition
 * @param other The Transition object to assign from
 * @return Reference to this Transition
 */
Transition& Transition::operator=(const Transition& other) {
    if (this != &other) {
        delete command;
        delete targetState;
        command = new std::string(*other.command);
        targetState = new std::string(*other.targetState);
    }
    return *this;
}

/**
 * Stream insertion operator for Transition
 * @param os Output stream
 * @param transition Transition object to output
 * @return Reference to the output stream
 */
std::ostream& operator<<(std::ostream& os, const Transition& transition) {
    os << "Transition: " << transition.getCommand() << " -> " << transition.getTargetState();
    return os;
}

/**
 * Get the command that triggers this transition
 * @return Command string
 */
std::string Transition::getCommand() const {
    return *command;
}

/**
 * Get the target state of this transition
 * @return Target state name
 */
std::string Transition::getTargetState() const {
    return *targetState;
}
