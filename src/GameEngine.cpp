#include "GameEngine.h"
#include "CommandProcessing.h"
#include <algorithm>
#include <iostream>
#include "Map.h"
#include <filesystem>
#include <sstream>
#include "Player.h"
#include "Cards.h"
#include <random>
#include <cctype>
#include <set>
#include <map>


// ========== GameEngine Implementation ==========

/**
 * Default constructor for GameEngine
 * Initializes the game engine with startup state (before start)
 */
GameEngine::GameEngine() : Subject(), currentState(nullptr), states(new std::map<std::string, State*>()), stateHistory(new std::vector<std::string>()), map_(nullptr), deck_(new Deck()) {
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
}

/**
 * Destructor for GameEngine
 * Cleans up all allocated memory
 */
GameEngine::~GameEngine() {
    cleanupStates();
    delete states;
    delete stateHistory;

    // Free dynamically created players
    for (auto* p : players_)
        delete p;

    players_.clear();
    delete deck_; // free deck
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
        if (cmd == "loadmap") {
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

            std::cout << "Success: map loaded: " << map_->getMapName() << "\n";
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
                std::cout << "Map Name: " << map_->getMapName() << "\n";
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

            auto* np = new Player(name);
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
