#include "GameEngine.h"
#include "Map.h"
#include "Player.h"
#include "Cards.h"
#include "Orders.h"
#include "PlayerStrategies.h"
#include "CommandProcessing.h"

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <random>
#include <cctype>
#include <set>
#include <map>
#include <fstream>



// ========== GameEngine Implementation ==========

/**
 * Default constructor for GameEngine
 * Initializes the game engine with startup state (before start)
 */
GameEngine::GameEngine() :
    Subject(),
    currentState(nullptr),
    states(new std::map<std::string, State*>()),
    stateHistory(new std::vector<std::string>()),
    map_(nullptr),
    deck_(new Deck()) {

    initializeStates();
    currentState = (*states)["startup"];
    stateHistory->push_back("startup");
}

/**
 * Copy constructor for GameEngine
 * @param other The GameEngine object to copy from
 */
GameEngine::GameEngine(const GameEngine& other) : Subject(other) {
    states = new std::map<std::string, State*>();
    stateHistory = new std::vector<std::string>(*other.stateHistory);

    // Deep copy all states
    for (const auto& pair : *other.states) {
        (*states)[pair.first] = new State(*pair.second);
    }

    currentState = (*states)[other.getCurrentStateName()];

    //Deep Copy Map
    if (other.map_) {
        map_ = std::make_unique<Map>(*other.map_);
    } else {
        map_ = nullptr;
    }

    //Deep Copy Deck
    if (other.deck_ != nullptr) {
        deck_ = new Deck(*other.deck_);
    } else {
        deck_ = nullptr;
    }

    //Deep Copy Players
    players_.clear();
    for (auto p : other.players_) {
        players_.push_back(new Player(*p)); 
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
    delete deck_;
    for (auto p : players_) {
        delete p;
    }
    players_.clear();
}

/**
 * Assignment operator for GameEngine
 * @param other The GameEngine object to assign from
 * @return Reference to this GameEngine
 */
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        Subject::operator=(other);
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

        if (other.map_)
            map_ = std::make_unique<Map>(*other.map_);
        else
            map_.reset();

        for (auto p : players_) {
            delete p;
        }
        players_.clear();
        for (auto p : other.players_) {
            players_.push_back(new Player(*p)); 
        }

        if (other.deck_ != nullptr) {
            deck_ = new Deck(*other.deck_);
        } else {
            deck_ = nullptr;
        }
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

Player* GameEngine::neutralPlayer_ = nullptr;
bool GameEngine::isAutomaticMode_ = false;

Player* GameEngine::getNeutralPlayer() {
    if (!neutralPlayer_) {
        neutralPlayer_ = new Player("Neutral", nullptr);
    }
    return neutralPlayer_;
}

void GameEngine::setAutomaticMode(bool mode) {
    isAutomaticMode_ = mode;
}

bool GameEngine::isAutomaticMode() {
    return isAutomaticMode_;
}


/**
 * Process a command and trigger appropriate state transition
 * @param command The command string to process
 */
void GameEngine::processCommand(const std::string& command) {
    // Extract base command (before any parameters)
    std::string baseCommand = command;
    std::size_t spacePos = command.find(' ');
    if (spacePos != std::string::npos) {
        baseCommand = command.substr(0, spacePos);
    }

    // Look for transition using base command
    Transition* transition = currentState->getTransition(baseCommand);

    if (transition != nullptr) {
        std::cout << "Executing command: " << command << std::endl;
        transitionState(transition->getTargetState());
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
void GameEngine::transitionState(const std::string& stateName) {
    auto stateIt = states->find(stateName);
    if (stateIt != states->end()) {
        currentState = stateIt->second;
        stateHistory->push_back(stateName);
        std::cout << "Transitioned to state: " << stateName << std::endl;
        notify();
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

std::string GameEngine::stringToLog() const {
    return "GameEngine state changed to: " + getCurrentStateName();
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
    // Create all states based on the assignment state diagram
    (*states)["startup"] = new State("startup");
    (*states)["start"] = new State("start");
    (*states)["map loaded"] = new State("map loaded");
    (*states)["map validated"] = new State("map validated");
    (*states)["players added"] = new State("players added");
    (*states)["play"] = new State("play");
    (*states)["assign reinforcement"] = new State("assign reinforcement");
    (*states)["issue orders"] = new State("issue orders");
    (*states)["execute orders"] = new State("execute orders");
    (*states)["win"] = new State("win");

    // Set up transitions based on the assignment state diagram
    // From startup state (the initial state before start)
    (*states)["startup"]->addTransition(new Transition("start", "start"));

    // From start state
    (*states)["start"]->addTransition(new Transition("loadmap", "map loaded"));

    // From map loaded state
    (*states)["map loaded"]->addTransition(new Transition("loadmap", "map loaded"));
    (*states)["map loaded"]->addTransition(new Transition("validatemap", "map validated"));

    // From map validated state
    (*states)["map validated"]->addTransition(new Transition("addplayer", "players added"));

    // From players added state
    (*states)["players added"]->addTransition(new Transition("addplayer", "players added"));
    (*states)["players added"]->addTransition(new Transition("gamestart", "assign reinforcement"));

    (*states)["play"]->addTransition(new Transition("play", "assign reinforcement"));
    // From assign reinforcement state
    (*states)["assign reinforcement"]->addTransition(new Transition("issueorder", "issue orders"));

    // From issue orders state
    (*states)["issue orders"]->addTransition(new Transition("issueorder", "issue orders"));
    (*states)["issue orders"]->addTransition(new Transition("endissueorders", "execute orders"));

    // From execute orders state
    (*states)["execute orders"]->addTransition(new Transition("execorder", "execute orders"));
    (*states)["execute orders"]->addTransition(new Transition("endexecorders", "assign reinforcement"));
    (*states)["execute orders"]->addTransition(new Transition("win", "win"));

    // From win state
    (*states)["win"]->addTransition(new Transition("replay", "start"));
    (*states)["win"]->addTransition(new Transition("quit", "win")); // stays in win, but signals exit
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
    players_.push_back(player);
}
void GameEngine::setMap(Map* map) {
    map_ = std::make_unique<Map>(*map);
}
void GameEngine::setDeck(Deck* deck) {
    delete deck_;
    deck_ = deck;
}

std::string GameEngine::mainGameLoop(int maxTurns) {
    bool gameOver = false;
    int turn = 1;
    std::string winner = "Draw";

    std::cout << "\n=== Starting Main Game Loop ===" << std::endl;

    if (players_.empty() || !map_) {
        std::cout << "Error: Cannot start main game loop without players and map.\n";
        return "";
    }

    while (!gameOver && (maxTurns == -1 ? true : turn <= maxTurns)) {
        std::cout << "\n=== Turn " << turn << " ===" << std::endl;

         // Clear negotiation records at the start of each turn
        Order::clearNegotiationRecords();

        // Reset conquered flag for all players
        for (Player* player : players_) {
            player->setConqueredThisTurn(false);
        }

        // Reinforcement Phase
        reinforcementPhase();

        // Issue Orders Phase
        issueOrdersPhase();
        
        // Execute Orders Phase
        executeOrdersPhase();

        // Give cards to players who conquered territories
        for (Player* player : players_) {
            if (player->getConqueredThisTurn() && deck_) {
                Card* drawnCard = deck_->draw();
                if (drawnCard) {
                    player->addCard(drawnCard);
                    std::cout << player->getName() << " conquered a territory this turn and receives a card!\n";
                }
            }
        }

        // --- Remove eliminated players ---
        for (auto it = players_.begin(); it != players_.end();) {
            Player* player = *it;
            if (player->getTerritories().empty()) {
                std::cout << "Player " << player->getName() << " has been eliminated!\n";
                delete player;
                it = players_.erase(it);
            } else {
                ++it;
            }
        }
        
        // --- Check win condition ---
        if (players_.size() == 1) {
            winner = players_.front()->getName();
            transitionState("win");
            gameOver = true;
        } else if (players_.empty()) {
            winner = "Draw";
            gameOver = true;
        }

        turn++;
    }

    // If we hit max turns without a winner, it's a draw
    if (maxTurns > 0 && turn > maxTurns && !gameOver) {
        winner = "Draw";
    }

    std::cout << "Game Over. Thanks for playing!" << std::endl;
    return winner;
}

void GameEngine::reinforcementPhase() {
    std::cout << "\n--- REINFORCEMENT PHASE ---\n";
    transitionState("assign reinforcement");

    for (Player* player : players_) {
        int reinforcement = std::max(3, static_cast<int>(player->getTerritories().size()) / 3);

        // Add continent bonuses
        std::map<Continent*, bool> continentOwnership;
        const auto& continents = map_->getContinents();
        
        // Initialize all continents as potentially owned
        for (const auto& continent : continents) {
            continentOwnership[continent.get()] = true;
        }

        // Check each continent to see if the player owns all territories
        for (const auto& continent : continents) {
            for (const auto& territory : continent->getTerritories()) {
                if (territory->getOwner() != player) {
                    continentOwnership[continent.get()] = false;
                    break;
                }
            }
        }

        // Add bonuses for owned continents
        int continentBonus = 0;
        for (const auto& pair : continentOwnership) {
            if (pair.second) {
                continentBonus += pair.first->getBonus();
                std::cout << "  " << player->getName() << " controls all of " 
                          << pair.first->getName() << " (+" << pair.first->getBonus() << " bonus)\n";
            }
        }

        reinforcement += continentBonus;

        int totalreinforcement = reinforcement + player->getPendingReinforcements();
        player->setReinforcementPool(totalreinforcement);
        player->setPendingReinforcements(0);
        std::cout << "Player " << player->getName() << " receives " << totalreinforcement << " reinforcement armies." << std::endl;
    }
}


// TODO : Refactor completely
void GameEngine::issueOrdersPhase() {
    std::cout << "\n--- ISSUING ORDERS PHASE ---\n";
    transitionState("issue orders");


    bool allDone = false;
    while (!allDone) {
        allDone = true;
        for (Player* player : players_) {
            if (*(player->getCanIssueOrderFlag())) {
                player->issueOrder();
                allDone = false;
            }
        }
    }
    for (Player* player : players_) {
        player->setCanIssueOrderFlag(true);
        if (player->getGotAttackedThisTurn() && *(player->getPlayerStrategy()->getStrategyDescription()) == "Neutral Player Strategy") {
            player->setStrategy(new AggressivePlayerStrategy(player));

        }
    }

    std::cout << "\nAll players have finished issuing orders.\n";
}

void GameEngine::executeOrdersPhase() {
    std::cout << "\n--- ORDERS EXECUTION PHASE ---\n";
    transitionState("execute orders");

    // First: Execute all deploy orders in round-robin fashion
    bool deploysLeft = true;
    while (deploysLeft) {
        deploysLeft = false;
        
        for (Player* player : players_) {
            auto* orders = player->getOrders()->orders;
            if (!orders->empty()) {
                Order* order = orders->front();
                if (dynamic_cast<OrderDeploy*>(order)) {
                    order->execute();
                    std::cout << "Executed deploy order for " << player->getName() << "\n";
                    orders->erase(orders->begin());
                    delete order;
                    deploysLeft = true;
                }
            }
        }
    }

    // Then: Execute other orders in round-robin fashion
    bool ordersLeft = true;
    while (ordersLeft) {
        ordersLeft = false;
        for (Player* player : players_) {
            auto* orders = player->getOrders()->orders;
            if (!orders->empty()) {
                Order* order = orders->front();
                if (!dynamic_cast<OrderDeploy*>(order)) {
                    order->execute();
                }
                delete order;
                orders->erase(orders->begin());
                ordersLeft = true;
            }
        }
    }

    std::cout << "All Orders Executed\n";
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

std::vector<Player*> GameEngine::getPlayers() const {
    return players_;
}

/**
 * Get the target state of this transition
 * @return Target state name
 */
std::string Transition::getTargetState() const {
    return *targetState;
}

// ========== GameCommand Implementation ==========

/**
 * Default constructor for GameCommand
 */
GameCommand::GameCommand() {
    commandString = new std::string("");
}

/**
 * Constructor for GameCommand with command string
 * @param command The command string
 */
GameCommand::GameCommand(const std::string& command) {
    commandString = new std::string(command);
}

/**
 * Copy constructor for GameCommand
 * @param other The Command object to copy from
 */
GameCommand::GameCommand(const GameCommand& other) {
    commandString = new std::string(*other.commandString);
}

/**
 * Destructor for GameCommand
 * Cleans up all allocated memory
 */
GameCommand::~GameCommand() {
    delete commandString;
}

/**
 * Assignment operator for GameCommand
 * @param other The Command object to assign from
 * @return Reference to this Command
 */
GameCommand& GameCommand::operator=(const GameCommand& other) {
    if (this != &other) {
        delete commandString;
        commandString = new std::string(*other.commandString);
    }
    return *this;
}

/**
 * Stream insertion operator for GameCommand
 * @param os Output stream
 * @param command Command object to output
 * @return Reference to the output stream
 */
std::ostream& operator<<(std::ostream& os, const GameCommand& command) {
    os << "GameCommand: " << command.getCommandString();
    return os;
}

/**
 * Get the command string
 * @return Command string
 */
std::string GameCommand::getCommandString() const {
    return *commandString;
}

/**
 * Check if the command is valid (not empty and not whitespace only)
 * @return True if valid, false otherwise
 */
bool GameCommand::isValid() const {
    if (commandString->empty()) return false;

    // Check if string contains only whitespace
    for (char c : *commandString) {
        if (!std::isspace(c)) {
            return true;
        }
    }
    return false;
}

//==========A2 implementation==========
namespace fs = std::filesystem;

// Helper: list .map files under a directory
static std::vector<std::string> listMapFiles(const std::string& root) {
    std::vector<std::string> files;
    try {
        if (fs::exists(root)) {
            for (const auto& p : fs::recursive_directory_iterator(root)) {
                if (p.is_regular_file() && p.path().extension() == ".map") {
                    files.push_back(p.path().string());
                }
            }
        }
    } catch (...) {}
    std::sort(files.begin(), files.end());
    return files;
}

// Helper to trim copy
static std::string trimCopy(const std::string& s) {
    auto b = std::find_if_not(s.begin(), s.end(), [](int c){ return std::isspace(c); });
    auto e = std::find_if_not(s.rbegin(), s.rend(), [](int c){ return std::isspace(c); }).base();
    return (e <= b) ? std::string() : std::string(b, e);
}

// Startup phase that reads via CommandProcessor (console or file)
void GameEngine::startupPhase(CommandProcessor& cmdSrc) {
    std::cout << "\n=== STARTUP PHASE ===\n";
    std::cout << "Commands:\n"
              << "  loadmap <path-or-name>\n"
              << "  validatemap\n"
              << "  addplayer <playername>\n"
              << "  gamestart\n";

    // Show available map files under A1/mapFiles
    const fs::path base = fs::current_path()  / "maps";
    const auto maps = listMapFiles(base.string());
    if (maps.empty()) {
        std::cout << "No .map files found under: " << base << "\n"
                  << "Ensure A1/mapFiles exists and contains .map files.\n";
    } else {
        std::cout << "\nAvailable maps:\n";
        for (const auto& m : maps) std::cout << "  - " << m << "\n";
    }

    MapLoader loader;
    map_.reset();
    bool hasLoaded = false;
    bool isValid   = false;

    while (true) {
        // Read one command via CommandProcessor
        auto* c = cmdSrc.getCommand();     // <- avoids type ambiguity
        if (!c) break;

        std::string line = trimCopy(c->getCommand());
        if (line.empty()) continue;

        // Validate command against current state
        if (!cmdSrc.validate(line, this)) {
            std::istringstream tmp(line);
            std::string baseCmd;
            tmp >> baseCmd;
            std::cout << " Command '" << baseCmd << "' is invalid in state '"
                      << getCurrentStateName() << "'.\n";
            c->saveEffect("Invalid for current state");
            continue;
        }

        // Parse base command
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "start") {
            if (getCurrentStateName() == "startup") {
                transitionState("start");
                std::cout << "Transitioned to 'start'. Now you can use 'loadmap <file>'.\n";
                c->saveEffect("state -> start");
            } else {
                std::cout << "Already past 'startup'.\n";
                c->saveEffect("no-op");
            }
        }
        else if (cmd == "loadmap") {
            std::string arg; std::getline(iss, arg); arg = trimCopy(arg);
            if (arg.empty()) {
                std::cout << "Usage: loadmap <path-or-name>\n";
                c->saveEffect("Missing path");
                continue;
            }

            fs::path candidate(arg);
            if (!fs::exists(candidate)) candidate = base / arg; // allow bare filename

            std::cout << "Loading: " << candidate.string() << " ...\n";
            auto loaded = loader.loadMap(candidate.string());
            if (!loaded) {
                std::cout << "Error: could not load file.\n";
                c->saveEffect("Load failed");
                continue;
            }

            map_ = std::move(loaded);
            hasLoaded = true;
            isValid = false;

            std::cout << "Success: map loaded: " << map_->getName() << "\n";
            transitionState("map loaded");
            std::cout << "Next: type 'validatemap'\n";
            c->saveEffect("Map loaded; state -> map loaded");
        }
        else if (cmd == "validatemap") {
            if (!hasLoaded || !map_) {
                std::cout << "Load a map first.\n";
                c->saveEffect("No map loaded");
                continue;
            }

            std::cout << "\n--- VALIDATION TESTS ---\n";
            const bool isConnected = map_->isConnectedGraph();
            std::cout << "1. Map is a connected graph: " << (isConnected ? "PASS" : "FAIL") << "\n";

            const bool continentsConnected = map_->areContinentsConnected();
            std::cout << "2. Continents are connected subgraphs: " << (continentsConnected ? "PASS" : "FAIL") << "\n";

            const bool territoriesInContinents = map_->eachTerritoryBelongsToOneContinent();
            std::cout << "3. Each territory belongs to one continent: "
                      << (territoriesInContinents ? "PASS" : "FAIL") << "\n";

            const bool overallValid = map_->validate();
            std::cout << "\n--- OVERALL VALIDATION ---\n"
                      << "Overall Result: " << (overallValid ? "VALID MAP" : "INVALID MAP") << "\n";

            if (overallValid) {
                transitionState("map validated");
                isValid = true;

                // summary
                std::cout << "\nMap Summary:\n";
                std::cout << "Map Name: " << map_->getName() << "\n";
                std::cout << "Number of Territories: " << map_->getNumberOfTerritories() << "\n";
                std::cout << "Number of Continents: " << map_->getNumberOfContinents() << "\n";

                std::cout << "\nMap validated successfully. Transitioned to 'map validated'.\n";
                c->saveEffect("Map validated; state -> map validated");
            } else {
                std::cout << "Invalid map. Load a different one.\n";
                c->saveEffect("Invalid map");
            }
        }
        else if (cmd == "addplayer") {
            std::string name; std::getline(iss, name); name = trimCopy(name);
            if (!name.empty() && ((name.front()=='"' && name.back()=='"') || (name.front()=='\'' && name.back()=='\''))) {
                name = name.substr(1, name.size()-2);
            }
            if (name.empty()) {
                std::cout << "Usage: addplayer <playername>\n";
                c->saveEffect("Missing player name");
                continue;
            }
            if (players_.size() >= 6) {
                std::cout << "Maximum of 6 players reached.\n";
                c->saveEffect("Too many players");
                continue;
            }
            bool duplicate = false;
            for (auto* p : players_) if (p && p->getName() == name) { duplicate = true; break; }
            if (duplicate) {
                std::cout << "A player named '" << name << "' already exists. Pick another name.\n";
                c->saveEffect("Duplicate name");
                continue;
            }

            auto* np = new Player(name, nullptr);
            players_.push_back(np);
            transitionState("players added");

            std::cout << "Added player: " << name << "\n";
            std::cout << "Current players (" << players_.size() << "): ";
            for (size_t i = 0; i < players_.size(); ++i) {
                std::cout << players_[i]->getName();
                if (i + 1 < players_.size()) std::cout << ", ";
            }
            std::cout << "\nHint: add between 2 and 6 players total, then type 'gamestart'.\n";
            c->saveEffect("Player added; state -> players added");
        }
        else if (cmd == "gamestart") {
            if (!map_) {
                std::cout << "No map loaded. Use 'loadmap <file>' first.\n";
                c->saveEffect("No map");
                continue;
            }
            if (!isValid) {
                std::cout << "Map is not validated yet. Use 'validatemap' first.\n";
                c->saveEffect("Map not validated");
                continue;
            }
            if (players_.size() < 2 || players_.size() > 6) {
                std::cout << "'gamestart' requires between 2 and 6 players. Current: " << players_.size() << "\n";
                c->saveEffect("Invalid player count");
                continue;
            }

            std::cout << "Preconditions satisfied for 'gamestart'. Proceeding to game setup...\n";

            // Distribute all territories evenly
            std::vector<Territory*> all;
            const auto& territories = map_->getTerritories();
            all.reserve(territories.size());
            for (const auto& t : territories) all.push_back(t.get());

            std::mt19937 rng(std::random_device{}());
            std::shuffle(all.begin(), all.end(), rng);
            for (size_t i = 0; i < all.size(); ++i) {
                Territory* terr = all[i];
                Player* newOwner = players_[i % players_.size()];
                if (Player* oldOwner = terr->getOwner(); oldOwner && oldOwner != newOwner) {
                    oldOwner->removeTerritory(terr);
                }
                if (!newOwner->ownsTerritory(terr)) newOwner->addTerritory(terr);
                if (terr->getOwner() != newOwner) terr->setOwner(newOwner);
            }

            // Randomize play order
            std::shuffle(players_.begin(), players_.end(), rng);

            // 50 armies each
            for (auto* p : players_) p->setReinforcementPool(50);

            // Draw 2 initial cards each
            if (deck_) {
                for (auto* p : players_) {
                    for (int j = 0; j < 2; ++j) {
                        if (Card* card = deck_->draw()) { p->addCard(card); }
                    }
                }
            }

            // Output
            std::cout << "\n=== TERRITORY DISTRIBUTION RESULTS ===\n";
            std::cout << "Total territories on map: " << all.size() << "\n";
            std::cout << "Number of players: " << players_.size() << "\n";
            for (auto* p : players_) {
                std::cout << " - " << p->getName()
                          << " received " << p->getTerritoryCount() << " territories.\n";
            }

            std::cout << "\n=== PLAYER ORDER ===\nPlay order: ";
            for (size_t i = 0; i < players_.size(); ++i) {
                std::cout << players_[i]->getName();
                if (i + 1 < players_.size()) std::cout << " -> ";
            }
            std::cout << "\n";

            std::cout << "\n=== REINFORCEMENT POOLS ===\n";
            for (auto* p : players_) {
                std::cout << " - " << p->getName()
                          << " starts with " << p->getReinforcementPool() << " armies.\n";
            }

            std::cout << "\n=== INITIAL CARDS DRAWN ===\n";
            for (auto* p : players_) {
                std::cout << p->getName() << " drew 2 cards and now has\n ";
                p->getHand()->printHand();
                std::cout << "\n";
            }

            transitionState("play");
            std::cout << "gamestart complete. Transitioned to 'play' state.\n";
            std::cout << "Current Game State: " << getCurrentStateName() << "\n";
            c->saveEffect("Game started; state -> play");
            break; // leave startup loop
        }
        else if (cmd == "help") {
            std::cout << "Commands:\n"
                      << "  loadmap <path-or-name>\n"
                      << "  validatemap\n"
                      << "  addplayer <name>\n"
                      << "  gamestart\n";
            c->saveEffect("Help shown");
        }
        else {
            std::cout << "Unknown command. Try 'help'.\n";
            c->saveEffect("Unknown command");
        }
    }

    if (!hasLoaded || !isValid) {
        std::cout << "StartupPhase aborted (no valid map).\n";
    }
}

// === Keep signature working console by default
void GameEngine::startupPhase() {
    CommandProcessor consoleCP;       // reads from stdin
    startupPhase(consoleCP);
}

// ==================== Tournament Mode Implementation ====================

/**
 * Parse and execute a tournament command
 * @param command The full tournament command string
 */
void GameEngine::executeTournament(const std::string& command) {
    std::cout << "\n=== TOURNAMENT MODE ===\n";
    std::cout << "Parsing tournament command...\n";

    // Find flag positions
    std::size_t mPos = command.find("-M ");
    std::size_t pPos = command.find("-P ");
    std::size_t gPos = command.find("-G ");
    std::size_t dPos = command.find("-D ");

    // Extract parameter strings
    std::string mapsStr = command.substr(mPos + 3, pPos - mPos - 3);
    std::string playersStr = command.substr(pPos + 3, gPos - pPos - 3);
    std::string gamesStr = command.substr(gPos + 3, dPos - gPos - 3);
    std::string turnsStr = command.substr(dPos + 3);

    // Helper lambda to trim whitespace
    auto trim = [](std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            s = "";
            return;
        }
        size_t end = s.find_last_not_of(" \t\n\r");
        s = s.substr(start, end - start + 1);
    };

    trim(mapsStr);
    trim(playersStr);
    trim(gamesStr);
    trim(turnsStr);

    // Parse maps (comma-separated)
    std::vector<std::string> maps;
    std::istringstream mapsStream(mapsStr);
    std::string mapToken;
    while (std::getline(mapsStream, mapToken, ',')) {
        trim(mapToken);
        if (!mapToken.empty()) {
            maps.push_back(mapToken);
        }
    }

    // Parse player strategies (comma-separated)
    std::vector<std::string> strategies;
    std::istringstream strategiesStream(playersStr);
    std::string strategyToken;
    while (std::getline(strategiesStream, strategyToken, ',')) {
        trim(strategyToken);
        if (!strategyToken.empty()) {
            // Convert to lowercase for case-insensitive comparison
            std::string lower = strategyToken;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            strategies.push_back(lower);
        }
    }

    // Parse the number of games and max turns
    int numGames = std::stoi(gamesStr);
    int maxTurns = std::stoi(turnsStr);

    // Display tournament configuration
    std::cout << "\nTournament Configuration:\n";
    std::cout << "Maps (" << maps.size() << "): ";
    for (size_t i = 0; i < maps.size(); ++i) {
        std::cout << maps[i];
        if (i < maps.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";

    std::cout << "Player Strategies (" << strategies.size() << "): ";
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::cout << strategies[i];
        if (i < strategies.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";

    std::cout << "Number of Games per Map: " << numGames << "\n";
    std::cout << "Max Turns per Game: " << maxTurns << "\n\n";

    // Execute tournament: play G games on each of M maps
    std::vector<std::vector<std::string>> results(maps.size(), std::vector<std::string>(numGames));

    for (size_t mapIdx = 0; mapIdx < maps.size(); ++mapIdx) {
        std::cout << "\n--- Playing games on map: " << maps[mapIdx] << " ---\n";

        for (int gameIdx = 0; gameIdx < numGames; ++gameIdx) {
            std::cout << "\nGame " << (gameIdx + 1) << " on " << maps[mapIdx] << "...\n";

            std::string winner = playTournamentGame(maps[mapIdx], strategies, maxTurns);
            results[mapIdx][gameIdx] = winner;

            std::cout << "Result: " << winner << "\n";
        }
    }

    // Log results
    logTournamentResults(maps, strategies, numGames, maxTurns, results);

    std::cout << "\n=== TOURNAMENT COMPLETE ===\n";
}

/**
 * Play a single tournament game
 * @param mapPath Path to the map file
 * @param strategies List of player strategy names
 * @param maxTurns Maximum number of turns before declaring a draw
 * @return Winner's strategy name or "Draw"
 */
std::string GameEngine::playTournamentGame(const std::string& mapPath, const std::vector<std::string>& strategies, int maxTurns) {
    // Enable automatic mode for tournament
    setAutomaticMode(true);

    // Load map
    MapLoader loader;

    // Try to find the map file
    const fs::path base = fs::current_path() / "maps";
    fs::path mapFile(mapPath);
    if (!fs::exists(mapFile)) {
        mapFile = base / mapPath;
    }

    auto loadedMap = loader.loadMap(mapFile.string());
    if (!loadedMap) {
        std::cout << "Error: Failed to load map " << mapPath << "\n";
        return "Error";
    }

    if (!loadedMap->validate()) {
        std::cout << "Error: Map " << mapPath << " is invalid\n";
        return "Error";
    }

    map_ = std::move(loadedMap);

    // Clear existing players
    for (auto* p : players_) {
        delete p;
    }
    players_.clear();

    // Create players with specified strategies
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::string strategyName = strategies[i];

        // Capitalize first letter for display
        std::string displayName = strategyName;
        if (!displayName.empty()) {
            displayName[0] = std::toupper(displayName[0]);
        }

        Player* player = new Player(displayName, nullptr);

        // Assign strategy based on name
        if (strategyName == "aggressive") {
            player->setStrategy(new AggressivePlayerStrategy(player));
        } else if (strategyName == "benevolent") {
            player->setStrategy(new BenevolentPlayerStrategy(player));
        } else if (strategyName == "neutral") {
            player->setStrategy(new NeutralPlayerStrategy(player));
        } else if (strategyName == "cheater") {
            player->setStrategy(new CheaterPlayerStrategy(player));
        } else {
            std::cout << "Warning: Unknown strategy " << strategyName << ", using Neutral\n";
            player->setStrategy(new NeutralPlayerStrategy(player));
        }

        players_.push_back(player);
    }

    // Distribute territories
    std::vector<Territory*> allTerritories;
    const auto& territories = map_->getTerritories();
    allTerritories.reserve(territories.size());
    for (const auto& t : territories) {
        allTerritories.push_back(t.get());
    }

    std::mt19937 rng(std::random_device{}());
    std::shuffle(allTerritories.begin(), allTerritories.end(), rng);

    for (size_t i = 0; i < allTerritories.size(); ++i) {
        Territory* terr = allTerritories[i];
        Player* owner = players_[i % players_.size()];

        if (Player* oldOwner = terr->getOwner(); oldOwner && oldOwner != owner) {
            oldOwner->removeTerritory(terr);
        }
        if (!owner->ownsTerritory(terr)) {
            owner->addTerritory(terr);
        }
        if (terr->getOwner() != owner) {
            terr->setOwner(owner);
        }

        // Initialize with some armies
        terr->setArmies(3);
    }

    // Initialize deck
    if (deck_) {
        delete deck_;
    }
    deck_ = new Deck();

    // Give each player initial armies
    for (auto* p : players_) {
        p->setReinforcementPool(30);
    }

    // Play the game using mainGameLoop with turn limit
    std::string winner = mainGameLoop(maxTurns);

    // Disable automatic mode after tournament game
    setAutomaticMode(false);

    return winner;
}

/**
 * Log tournament results to console and file
 * @param maps List of map names
 * @param strategies List of player strategy names
 * @param numGames Number of games played per map
 * @param maxTurns Maximum turns per game
 * @param results 2D vector of results [mapIndex][gameIndex] -> winner name or "Draw"
 */
void GameEngine::logTournamentResults(const std::vector<std::string>& maps, const std::vector<std::string>& strategies, int numGames, int maxTurns, const std::vector<std::vector<std::string>>& results) {
    std::cout << "\n========================================\n";
    std::cout << "TOURNAMENT RESULTS\n";
    std::cout << "========================================\n\n";

    std::cout << "Tournament mode:\n";

    // display maps
    std::cout << "M: ";
    for (size_t i = 0; i < maps.size(); ++i) {
        std::cout << maps[i];
        if (i < maps.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";

    // display player strategies
    std::cout << "P: ";
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::string strategyDisplay = strategies[i];
        if (!strategyDisplay.empty()) {
            strategyDisplay[0] = std::toupper(strategyDisplay[0]);
        }
        std::cout << strategyDisplay;
        if (i < strategies.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";

    std::cout << "G: " << numGames << "\n";
    std::cout << "D: " << maxTurns << "\n\n";

    // display results table
    std::cout << "Results:\n";

    // header row
    std::cout << "        ";
    for (int g = 0; g < numGames; ++g) {
        std::cout << "Game " << (g + 1) << "    ";
    }
    std::cout << "\n";

    // results for each map
    for (size_t m = 0; m < maps.size(); ++m) {
        std::cout << maps[m] << "    ";
        for (int g = 0; g < numGames; ++g) {
            std::cout << results[m][g] << "    ";
        }
        std::cout << "\n";
    }

    std::cout << "\n========================================\n";

    // also write to log file (gamelog.txt)
    std::ofstream logFile("gamelog.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "\n========================================\n";
        logFile << "TOURNAMENT RESULTS\n";
        logFile << "========================================\n\n";

        logFile << "Tournament mode:\n";
        logFile << "M: ";
        for (size_t i = 0; i < maps.size(); ++i) {
            logFile << maps[i];
            if (i < maps.size() - 1) logFile << ", ";
        }
        logFile << "\n";

        logFile << "P: ";
        for (size_t i = 0; i < strategies.size(); ++i) {
            std::string strategyDisplay = strategies[i];
            if (!strategyDisplay.empty()) {
                strategyDisplay[0] = std::toupper(strategyDisplay[0]);
            }
            logFile << strategyDisplay;
            if (i < strategies.size() - 1) logFile << ", ";
        }
        logFile << "\n";

        logFile << "G: " << numGames << "\n";
        logFile << "D: " << maxTurns << "\n\n";

        logFile << "Results:\n";
        logFile << "        ";
        for (int g = 0; g < numGames; ++g) {
            logFile << "Game " << (g + 1) << "    ";
        }
        logFile << "\n";

        for (size_t m = 0; m < maps.size(); ++m) {
            logFile << maps[m] << "    ";
            for (int g = 0; g < numGames; ++g) {
                logFile << results[m][g] << "    ";
            }
            logFile << "\n";
        }

        logFile << "\n========================================\n";
        logFile.close();
    }
}