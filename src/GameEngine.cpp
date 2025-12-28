#include "GameEngine.h"
#include "Cards.h"
#include "CommandProcessing.h"
#include "Map.h"
#include "Orders.h"
#include "Player.h"
#include "PlayerStrategies.h"
#include "Utility.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <ranges>
#include <set>
#include <sstream>


// ==================== State Class Implementation ====================
State::State() :
  name(new std::string("unknown")),
  transitions(new std::vector<Transition*>()) {}

State::State(const std::string& newName) :
  name(new std::string(newName)),
  transitions(new std::vector<Transition*>()) {}

State::State(const State& other) :
  name(new std::string(*other.name)),
  transitions(new std::vector<Transition*>()) {
  // deep copy transitions
  for (Transition* tran : *other.transitions) {
    transitions->push_back(new Transition(*tran));
  }
}

State& State::operator=(const State& other) {
  if (this != &other) {
    delete name;
    for (Transition* tran : *transitions) { delete tran; }
    transitions->clear();

    // deep and shallow copy objects
    name = new std::string(*other.name);
    for (Transition* tran : *other.transitions) {
      transitions->push_back(new Transition(*tran));
    }
  }
  return *this;
}

State::~State() {
  delete name;
  for (Transition* tran : *transitions) { delete tran; }
  delete transitions;
}

// --- GETTERS ---
std::string State::getName() const {
  return *name;
}

Transition* State::getTransition(const std::string& cmd) const {
  for (Transition* tran : *transitions) {
    if (tran->getCommand() == cmd) return tran;
  }
  return nullptr;
}

// --- TRANSITION MANAGEMENT ---
void State::addTransition(Transition* transition) {
  transitions->push_back(transition);
}

// --- UTILITY ---
std::vector<std::string> State::getValidCommands() const {
  std::vector<std::string> commands;

  for (Transition* tran : *transitions) {
    commands.push_back(tran->getCommand());
  }
  return commands;
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const State& st) {
  os << "State[" << st.getName() << "]" << std::endl;
  return os;
}


// ==================== Transition Class Implementation ====================
Transition::Transition() :
  command(new std::string("unknown")),
  targetState(new std::string("unknown")) {}


Transition::Transition(const std::string& cmd, const std::string& targSts) :
  command(new std::string(cmd)),
  targetState(new std::string(targSts)) {}


Transition::Transition(const Transition& other) :
  command(new std::string(*other.command)),
  targetState(new std::string(*other.targetState)) {}

Transition& Transition::operator=(const Transition& other) {
  if (this != &other) {
    delete command;
    delete targetState;
    command = new std::string(*other.command);
    targetState = new std::string(*other.targetState);
  }
  return *this;
}

Transition::~Transition() {
  delete command;
  delete targetState;
}

// --- GETTERS ---
std::string Transition::getCommand() const {
  return *command;
}

std::string Transition::getTargetState() const {
  return *targetState;
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Transition& tran) {
  os << "Transition[" << tran.getCommand() << " -> " << tran.getTargetState() << "]" << std::endl;
  return os;
}


// ==================== GameEngine Class Implementation ====================
Player* GameEngine::neutralPlayer = nullptr;
bool GameEngine::automaticMode = false;

GameEngine::GameEngine() :
  Subject(),
  currentState(nullptr),
  states(new std::map<std::string, State*>()),
  stateHistory(new std::vector<std::string>()),
  gameMap(nullptr),
  players(new std::vector<Player*>()),
  deck(new Deck()) {
  initializeStates();
  currentState = (*states)[GAME_PHASES::STARTUP];
  stateHistory->push_back(GAME_PHASES::STARTUP);
}

GameEngine::GameEngine(const GameEngine& other) :
  Subject(other),
  states(new std::map<std::string, State*>()),
  stateHistory(new std::vector<std::string>(*other.stateHistory)),
  gameMap(std::make_unique<Map>(*other.gameMap)),
  players(new std::vector<Player*>()),
  deck(new Deck(*other.deck)) {
  // deep copy states
  for (const auto& [fst, snd] : *other.states) {
    (*states)[fst] = new State(*snd);
  }
  currentState = (*states)[other.getCurrentStateName()];

  // deep copy players
  for (auto& play : *other.players) {
    players->push_back(new Player(*play));
  }
}

GameEngine& GameEngine::operator=(const GameEngine& other) {
  if (this != &other) {
    Subject::operator=(other);
    cleanupStates();
    delete states;
    delete stateHistory;

    states = new std::map<std::string, State*>();
    stateHistory = new std::vector<std::string>(*other.stateHistory);

    for (const auto& [fst, snd] : *other.states) {
      (*states)[fst] = new State(*snd);
    }
    currentState = (*states)[other.getCurrentStateName()];

    // smart pointer map
    if (other.gameMap) gameMap = std::make_unique<Map>(*other.gameMap);
    else gameMap.reset();

    for (auto& play : *players) { delete play; }
    players->clear();

    for (auto& play : *other.players) {
      players->push_back(new Player(*play));
    }

    deck = new Deck(*other.deck);
  }
  return *this;
}

GameEngine::~GameEngine() {
  cleanupStates();
  delete states;
  delete stateHistory;
  delete deck;
  for (auto& play : *players) { delete play; }
  players->clear();
}

// --- GETTERS --
std::string GameEngine::getCurrentStateName() const {
  return currentState->getName();
}

const std::vector<Player*>& GameEngine::getPlayers() const {
  return *players;
}

Player* GameEngine::getNeutralPlayer() {
  if (!neutralPlayer) neutralPlayer = new Player("Neutral", nullptr);
  return neutralPlayer;
}

bool GameEngine::getAutomaticMode() {
  return automaticMode;
}

// --- SETTERS --
void GameEngine::setMap(Map* map) {
  gameMap = std::make_unique<Map>(std::move(*map));
}

void GameEngine::setDeck(Deck* newDeck) {
  deck = newDeck;
}

void GameEngine::setAutomaticMode(bool mode) {
  automaticMode = mode;
}

// --- PLAYER MANAGEMENT --
void GameEngine::addPlayer(Player* player) {
  players->push_back(player);
}

// --- STARTUP PHASE MANAGEMENT --
void GameEngine::startupPhase() {
  CommandProcessor consoleCp;
  startupPhase(consoleCp);
}

void GameEngine::startupPhase(CommandProcessor& cmdSrc) {
  std::cout << "\n=== STARTUP PHASE ===\n";
  std::cout << "Commands:\n"
            << "  loadmap <path-or-name>\n"
            << "  validatemap\n"
            << "  addplayer <playername>\n"
            << "  gamestart\n";

  // show available map files under /maps
  const fs::path cwd = fs::current_path();
  const fs::path mapDirPath = cwd / "maps";
  const std::string mapDir = mapDirPath.string();

  std::cout << "Searching for .map files in: " << mapDir << std::endl;
  const std::vector<std::string> mapFiles = Utility::findMapFiles(mapDir);

  if (mapFiles.empty()) {
    std::cout << "No .map files found under: " << mapDirPath << "\nEnsure /maps exists and contains .map files.\n";
  } else {
    std::cout << "\nAvailable maps:\n";
    for (const auto& mapfl : mapFiles) {
      std::cout << "  - " << mapfl << "\n";
    }
  }

  gameMap.reset();
  MapLoader loader;
  bool hasLoaded = false, isValid = false;
  std::cout << "\nYour first command should be 'start'\n";

  while (true) {
    // read one command via CommandProcessor
    auto* cmd = cmdSrc.getCommand(); // <- avoids type ambiguity
    if (!cmd) break;

    std::string line = trimCopy(cmd->getCommand());
    if (line.empty()) continue;

    // validate command against current state
    if (!cmdSrc.validate(line, this)) {
      std::istringstream tmp(line);
      std::string baseCmd;
      tmp >> baseCmd;

      std::cout << " Command '" << baseCmd << "' is invalid in state '" << getCurrentStateName() << "'.\n";
      cmd->saveEffect("Invalid for current state");
      continue;
    }

    // parse base command
    std::istringstream iss(line);
    std::string baseCmd;
    iss >> baseCmd;

    if (baseCmd == GAME_STATES::START) {
      if (getCurrentStateName() == GAME_PHASES::STARTUP) {
        transitionState(GAME_STATES::START);
        std::cout << "Transitioned to 'start'. Now you can use 'loadmap <file>'.\n\n";
        cmd->saveEffect("state -> start");
      } else {
        std::cout << "Already past 'startup' phase.\n";
        cmd->saveEffect("no-op");
      }
    } else if (baseCmd == GAME_TRANSITIONS::LOADMAP) {
      std::string arg;
      std::getline(iss, arg);
      arg = trimCopy(arg);

      if (arg.empty()) {
        std::cout << "Usage: loadmap <path-or-name>\n";
        cmd->saveEffect("Missing path");
        continue;
      }

      fs::path candidate(arg);
      if (!fs::exists(candidate)) { candidate = mapDirPath / arg; } // allow bare filename

      std::cout << "Loading: " << candidate.string() << " ...\n";
      auto loaded = loader.loadMap(candidate.string());
      if (!loaded) {
        std::cout << "Error: could not load .map file.\n";
        cmd->saveEffect("Map loadig failed");
        continue;
      }

      gameMap = std::move(loaded);
      hasLoaded = true;
      isValid = false;

      std::cout << "Success: map loaded: " << gameMap->getName() << "\n";
      transitionState(GAME_STATES::MAP_LOADED);
      std::cout << "\nNext: type 'validatemap'\n";
      cmd->saveEffect("Map loaded; state -> map loaded");
    } else if (baseCmd == GAME_TRANSITIONS::VALIDATEMAP) {
      if (!hasLoaded || !gameMap) {
        std::cout << "Load a map first.\n";
        cmd->saveEffect("No map loaded");
        continue;
      }

      // perform validation tests
      std::cout << "\n--- VALIDATION TESTS ---" << std::endl;
      const bool isConnectedGraph = gameMap->isConnectedGraph();
      std::cout << "1. Map is a connected graph: " << (isConnectedGraph ? "PASS" : "FAIL") << std::endl;

      const bool continentsAreConnected = gameMap->areContinentsConnected();
      std::cout << "2. Continents are connected subgraphs: " << (continentsAreConnected ? "PASS" : "FAIL") << std::endl;

      const bool territoryBelongsToOneContinent = gameMap->eachTerritoryBelongsToOneContinent();
      std::cout << "3. Each territory belongs to one continent: " << (territoryBelongsToOneContinent ? "PASS" : "FAIL")
                << std::endl;

      const bool overallValid = gameMap->validate();
      std::cout << "\n--- OVERALL VALIDATION ---" << std::endl;
      std::cout << "Overall Result: " << (overallValid ? "VALID MAP" : "INVALID MAP") << std::endl;

      if (overallValid) {
        transitionState(GAME_STATES::MAP_VALIDATED);
        isValid = true;

        // summary
        std::cout << "\nMap Summary:\n";
        std::cout << "  - Map name: " << gameMap->getName() << "\n";
        std::cout << "  - Number of Territories: " << gameMap->getNumberOfTerritories() << "\n";
        std::cout << "  - Number of Continents: " << gameMap->getNumberOfContinents() << "\n";
        std::cout << "Map validated successfully. Transitioned to 'map validated'.\n\n";
        cmd->saveEffect("Map validated; state -> map validated");
      } else {
        std::cout << "Invalid map. Load a different one.\n";
        cmd->saveEffect("Invalid map after validation");
      }
    } else if (baseCmd == GAME_TRANSITIONS::ADDPLAYER) {
      std::string name;
      std::getline(iss, name);
      name = trimCopy(name);

      if (name.empty()) {
        std::cout << "Usage: addplayer <playername>\n";
        cmd->saveEffect("Missing player name");
        continue;
      }
      if (
        !name.empty() &&
        ((name.front() == '"' && name.back() == '"') || (name.front() == '\'' && name.back() == '\''))
      ) {
        name = name.substr(1, name.size() - 2);
      }

      // must be 2-6 players
      if (players->size() >= 6) {
        std::cout << "Maximum of 6 players reached.\n";
        cmd->saveEffect("Too many players");
        continue;
      }

      bool duplicate = false;
      for (auto* play : *players) {
        if (play && play->getName() == name) {
          duplicate = true;
          break;
        }
      }
      if (duplicate) {
        std::cout << "A player named '" << name << "' already exists. Pick another name.\n";
        cmd->saveEffect("Duplicate player name");
        continue;
      }

      auto* np = new Player(name, nullptr);

      // assign default strategy based on automatic mode
      // in tournament/automatic mode, strategies will be set separately
      // in regular mode, assign strategies in round-robin fashion for testing
      if (automaticMode) {
        np->setStrategy(new NeutralPlayerStrategy(np));
      } else {
        const size_t playerIndex = players->size();
        switch (playerIndex % 4) { // this ensures each player has a valid strategy to avoid infinite loops
          case 0: np->setStrategy(new AggressivePlayerStrategy(np)); break;
          case 1: np->setStrategy(new BenevolentPlayerStrategy(np)); break;
          case 2: np->setStrategy(new NeutralPlayerStrategy(np)); break;
          case 3: np->setStrategy(new CheaterPlayerStrategy(np)); break;
          default: np->setStrategy(new AggressivePlayerStrategy(np)); break;
        }
        std::cout << "Assigned " << *np->getPlayerStrategy()->getStrategy() << " strategy to " << name << "\n";
      }

      players->push_back(np);
      transitionState(GAME_STATES::PLAYERS_ADDED);

      std::cout << "Added player: " << name << "\n";
      std::cout << "Current players (" << players->size() << "): ";
      for (size_t i = 0; i < players->size(); ++i) {
        std::cout << (*players)[i]->getName();
        if (i < players->size() - 1) { std::cout << ", "; }
      }

      std::cout << "\nHint: add between 2 and 6 players total, then type 'gamestart'.\n\n";
      cmd->saveEffect("Player added; state -> players added");
    } else if (baseCmd == GAME_TRANSITIONS::GAMESTART) {
      if (!gameMap) {
        std::cout << "No map loaded. Use 'loadmap <file>' first.\n";
        cmd->saveEffect("Map not loaded");
        continue;
      }
      if (!isValid) {
        std::cout << "Map is not validated yet. Use 'validatemap' first.\n";
        cmd->saveEffect("Map not validated");
        continue;
      }
      if (players->size() < 2 || players->size() > 6) {
        std::cout << "'gamestart' requires between 2 and 6 players -> Current: " << players->size() << "\n";
        cmd->saveEffect("Invalid player count");
        continue;
      }

      std::cout << "Preconditions satisfied for 'gamestart'. Proceeding to game setup...\n";

      // distribute all territories evenly
      std::vector<Territory*> allTerritories;
      const auto& territories = gameMap->getTerritories();

      allTerritories.reserve(territories.size());
      for (const auto& terr : territories) {
        allTerritories.push_back(terr.get());
      }

      std::mt19937 rng(std::random_device{}());
      std::ranges::shuffle(allTerritories, rng);
      for (size_t i = 0; i < allTerritories.size(); ++i) {
        Territory* terr = allTerritories[i];
        Player* oldOwner = terr->getOwner();
        Player* newOwner = (*players)[i % players->size()];

        if (oldOwner && oldOwner != newOwner) oldOwner->removeTerritory(terr);
        if (!newOwner->ownsTerritory(terr)) newOwner->addTerritory(terr);
        if (terr->getOwner() != newOwner) terr->setOwner(newOwner);
      }

      // randomize play order
      std::ranges::shuffle(*players, rng);
      // 50 armies each
      for (auto* play : *players) play->setReinforcementPool(50);

      // draw 2 initial cards each
      if (deck) {
        for (auto* play : *players) {
          for (int j = 0; j < 2; ++j) {
            Card* card = deck->draw();
            if (card) play->addCard(card);
          }
        }
      }

      // output
      std::cout << "\n=== TERRITORY DISTRIBUTION RESULTS ===\n";
      std::cout << "Total territories on map: " << allTerritories.size() << "\n";
      std::cout << "Number of players: " << players->size() << "\n";
      for (auto* play : *players) {
        std::cout << " - " << play->getName() << " received " << play->getTerritoryCount() << " territories.\n";
      }

      std::cout << "\n=== PLAYER ORDER ===\nPlay order: ";
      for (size_t i = 0; i < players->size(); ++i) {
        std::cout << (*players)[i]->getName();
        if (i < players->size() - 1) { std::cout << " -> "; }
      }
      std::cout << "\n";

      std::cout << "\n=== REINFORCEMENT POOLS ===\n";
      for (auto* play : *players) {
        std::cout << " - " << play->getName() << " starts with " << play->getReinforcementPool() << " armies.\n";
      }

      std::cout << "\n=== INITIAL CARDS DRAWN ===\n";
      for (auto* play : *players) {
        std::cout << play->getName() << " drew 2 cards and now has\n ";
        play->getHand()->displayInfo();
        std::cout << "\n";
      }

      transitionState(GAME_PHASES::PLAY);
      std::cout << "'gamestart' complete. Transitioned to 'play' phase.\n";
      std::cout << "Current Game State: " << getCurrentStateName() << "\n";
      cmd->saveEffect("Game started; state -> play");
      break; // leave startup loop
    } else if (baseCmd == "help") {
      std::cout << "Commands:\n"
                << "  loadmap <path-or-name>\n"
                << "  validatemap\n"
                << "  addplayer <name>\n"
                << "  gamestart\n";
      cmd->saveEffect("Help shown");
    } else {
      std::cout << "Unknown command. Try 'help'.\n";
      cmd->saveEffect("Unknown command");
    }
  }

  if (!hasLoaded || !isValid) std::cout << "Startup phase aborted (no valid map).\n";
}

// --- ORDERS MANAGEMENT --
void GameEngine::reinforcementPhase() {
  std::cout << "\n--- REINFORCEMENT PHASE ---\n";
  transitionState(GAME_STATES::ASSIGN_REINFORCEMENT);

  for (Player* player : *players) {
    int reinforcement = std::max(3, static_cast<int>(player->getTerritories().size()) / 3);

    // add continent bonuses
    std::map<Continent*, bool> continentOwnership;
    const auto& continents = gameMap->getContinents();

    // initialize all continents as potentially owned
    for (const auto& continent : continents) {
      continentOwnership[continent.get()] = true;
    }

    // check each continent to see if the player owns all territories
    for (const auto& continent : continents) {
      for (const auto& territory : continent->getTerritories()) {
        if (territory->getOwner() != player) {
          continentOwnership[continent.get()] = false;
          break;
        }
      }
    }

    // add bonuses for owned continents
    int continentBonus = 0;
    for (const auto& [fst, snd] : continentOwnership) {
      if (snd) {
        continentBonus += fst->getBonus();
        std::cout << "  " << player->getName() << " controls all of "
                  << fst->getName() << " (+" << fst->getBonus() << " bonus)\n";
      }
    }

    reinforcement += continentBonus;
    int totalReinforcement = reinforcement + player->getPendingReinforcements();

    player->setReinforcementPool(totalReinforcement);
    player->setPendingReinforcements(0);
    std::cout << "Player " << player->getName() << " receives " << totalReinforcement
              << " reinforcement armies." << std::endl;
  }
}

void GameEngine::issueOrdersPhase() {
  std::cout << "\n--- ISSUING ORDERS PHASE ---\n";
  transitionState(GAME_STATES::ISSUE_ORDERS);

  bool allDone = false;
  while (!allDone) {
    allDone = true;
    for (Player* play : *players) {
      if (*(play->getCanIssueOrder())) {
        play->issueOrder();
        allDone = false;
      }
    }
  }

  for (Player* play : *players) {
    play->setCanIssueOrder(true);
    if (play->getGotAttackedThisTurn() && *(play->getPlayerStrategy()->getStrategy()) == "Neutral Player Strategy") {
      play->setStrategy(new AggressivePlayerStrategy(play));
    }
  }
  std::cout << "\nAll players have finished issuing orders.\n";
}

void GameEngine::executeOrdersPhase() {
  std::cout << "\n--- ORDERS EXECUTION PHASE ---\n";
  transitionState(GAME_STATES::EXECUTE_ORDERS);

  // first: execute all deploy orders in round-robin fashion
  bool deploysLeft = true;
  while (deploysLeft) {
    deploysLeft = false;

    for (Player* play : *players) {
      auto* orders = play->getOrders()->orders;

      if (!orders->empty()) {
        Order* order = orders->front();
        if (dynamic_cast<OrderDeploy*>(order)) {
          order->execute();
          std::cout << "Executed deploy order for " << play->getName() << "\n";

          orders->erase(orders->begin());
          delete order;
          deploysLeft = true;
        }
      }
    }
  }

  // then: execute other orders in round-robin fashion
  bool ordersLeft = true;
  while (ordersLeft) {
    ordersLeft = false;
    for (Player* play : *players) {
      auto* orders = play->getOrders()->orders;

      if (!orders->empty()) {
        Order* order = orders->front();
        if (!dynamic_cast<OrderDeploy*>(order)) { order->execute(); }

        delete order;
        orders->erase(orders->begin());
        ordersLeft = true;
      }
    }
  }

  std::cout << "All Orders Executed\n";
}

// --- MAIN GAME LOOP MANAGEMENT --
std::string GameEngine::mainGameLoop(int maxTurns) {
  int turn = 1;
  bool gameOver = false;
  std::string winner = "Draw";

  std::cout << "\n=== Starting Main Game Loop ===" << std::endl;
  if (players->empty() || !gameMap) {
    std::cout << "Error: Cannot start main game loop without players and map.\n";
    return "";
  }

  while (!gameOver && (maxTurns == -1 ? true : turn <= maxTurns)) {
    std::cout << "\n=== Turn " << turn << " ===" << std::endl;

    // clear negotiation records at the start of each turn
    Order::clearNegotiationRecords();

    // reset conquered flag for all players
    for (Player* play : *players) {
      play->setConqueredThisTurn(false);
    }

    // reinforcement Phase
    reinforcementPhase();
    // issue Orders Phase
    issueOrdersPhase();
    // execute Orders Phase
    executeOrdersPhase();

    // give cards to players who conquered territories
    for (Player* play : *players) {
      if (play->getConqueredThisTurn() && deck) {
        Card* drawnCard = deck->draw();
        if (drawnCard) {
          play->addCard(drawnCard);
          std::cout << play->getName() << " conquered a territory this turn and receives a card!\n";
        }
      }
    }

    // --- remove eliminated players ---
    for (auto it = players->begin(); it != players->end();) {
      Player* play = *it;

      if (play->getTerritories().empty()) {
        std::cout << "Player " << play->getName() << " has been eliminated!\n";
        delete play;
        it = players->erase(it);
      } else { ++it; }
    }

    // --- check win condition ---
    if (players->size() == 1) {
      winner = players->front()->getName();
      transitionState(GAME_STATES::WIN);
      gameOver = true;
    } else if (players->empty()) {
      winner = "Draw";
      gameOver = true;
    }

    turn++;
  }

  // if we hit max turns without a winner, it's a draw
  if (maxTurns > 0 && turn > maxTurns && !gameOver) {
    winner = "Draw";
  }

  std::cout << "Game Over. Thanks for playing!" << std::endl;
  return winner;
}

// --- TOURNAMENT MODE MANAGEMENT --
/**
 * parse and execute a tournament command
 * @param cmd the full tournament command string
 */
void GameEngine::executeTournament(const std::string& cmd) {
  std::cout << "\n=== TOURNAMENT MODE ===\n";
  std::cout << "Parsing tournament command...\n";

  // find flag positions
  std::size_t mPos = cmd.find("-M ");
  std::size_t pPos = cmd.find("-P ");
  std::size_t gPos = cmd.find("-G ");
  std::size_t dPos = cmd.find("-D ");

  // extract parameter strings
  std::string mapsStr = cmd.substr(mPos + 3, pPos - mPos - 3);
  std::string playersStr = cmd.substr(pPos + 3, gPos - pPos - 3);
  std::string gamesStr = cmd.substr(gPos + 3, dPos - gPos - 3);
  std::string turnsStr = cmd.substr(dPos + 3);

  // helper lambda to trim whitespace
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

  // parse maps (comma-separated)
  std::vector<std::string> maps;
  std::istringstream mapsStream(mapsStr);
  std::string mapToken;
  while (std::getline(mapsStream, mapToken, ',')) {
    trim(mapToken);
    if (!mapToken.empty()) {
      maps.push_back(mapToken);
    }
  }

  // parse player strategies (comma-separated)
  std::vector<std::string> strategies;
  std::istringstream strategiesStream(playersStr);
  std::string strategyToken;
  while (std::getline(strategiesStream, strategyToken, ',')) {
    trim(strategyToken);
    if (!strategyToken.empty()) {
      // convert to lowercase for case-insensitive comparison
      std::string lower = strategyToken;
      std::ranges::transform(lower, lower.begin(), ::tolower);
      strategies.push_back(lower);
    }
  }

  // parse the number of games and max turns
  int numGames = std::stoi(gamesStr);
  int maxTurns = std::stoi(turnsStr);

  // display tournament configuration
  std::cout << "\nTournament Configuration:\n";
  std::cout << "Maps (" << maps.size() << "): ";
  for (size_t i = 0; i < maps.size(); ++i) {
    std::cout << maps[i];
    if (i < maps.size() - 1) { std::cout << ", "; }
  }
  std::cout << "\n";

  std::cout << "Player Strategies (" << strategies.size() << "): ";
  for (size_t i = 0; i < strategies.size(); ++i) {
    std::cout << strategies[i];
    if (i < strategies.size() - 1) { std::cout << ", "; }
  }
  std::cout << "\n";

  std::cout << "Number of Games per Map: " << numGames << "\n";
  std::cout << "Max Turns per Game: " << maxTurns << "\n\n";

  // execute tournament: play G games on each of M maps
  std::vector results(maps.size(), std::vector<std::string>(numGames));

  for (size_t mapIdx = 0; mapIdx < maps.size(); ++mapIdx) {
    std::cout << "\n--- Playing games on map: " << maps[mapIdx] << " ---\n";

    for (int gameIdx = 0; gameIdx < numGames; ++gameIdx) {
      std::cout << "\nGame " << (gameIdx + 1) << " on " << maps[mapIdx] << "...\n";
      std::string winner = playTournamentGame(maps[mapIdx], strategies, maxTurns);
      results[mapIdx][gameIdx] = winner;
      std::cout << "Result: " << winner << "\n";
    }
  }

  // log results
  logTournamentResults(maps, strategies, numGames, maxTurns, results);
  std::cout << "\n=== TOURNAMENT COMPLETE ===\n";
}

/**
 * play a single tournament game
 * @param mapPath path to the map file
 * @param strategies list of player strategy names
 * @param maxTurns maximum number of turns before declaring a draw
 * @return winner's strategy name or "Draw"
 */
std::string GameEngine::playTournamentGame(
  const std::string& mapPath, const std::vector<std::string>& strategies, int maxTurns
) {
  // enable automatic mode for tournament
  setAutomaticMode(true);
  // load map
  MapLoader loader;

  // try to find the map file
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

  gameMap = std::move(loadedMap);

  // clear existing players
  for (auto* play : *players) { delete play; }
  players->clear();

  // create players with specified strategies
  for (size_t i = 0; i < strategies.size(); ++i) {
    std::string strategyName = strategies[i];

    // capitalize first letter for display
    std::string displayName = strategyName;
    if (!displayName.empty()) {
      displayName[0] = std::toupper(displayName[0]);
    }

    Player* player = new Player(displayName, nullptr);

    // assign strategy based on name
    if (strategyName == PLAYER_STRATEGIES::HUMAN) {
      player->setStrategy(new HumanPlayerStrategy(player));
    } else if (strategyName == PLAYER_STRATEGIES::AGGRESSIVE) {
      player->setStrategy(new AggressivePlayerStrategy(player));
    } else if (strategyName == PLAYER_STRATEGIES::BENEVOLENT) {
      player->setStrategy(new BenevolentPlayerStrategy(player));
    } else if (strategyName == PLAYER_STRATEGIES::NEUTRAL) {
      player->setStrategy(new NeutralPlayerStrategy(player));
    } else if (strategyName == PLAYER_STRATEGIES::CHEATER) {
      player->setStrategy(new CheaterPlayerStrategy(player));
    } else {
      std::cout << "Warning: Unknown strategy " << strategyName << ", using Neutral\n";
      player->setStrategy(new NeutralPlayerStrategy(player));
    }

    players->push_back(player);
  }

  // distribute territories
  std::vector<Territory*> allTerritories;
  const auto& territories = gameMap->getTerritories();
  allTerritories.reserve(territories.size());
  for (const auto& t : territories) {
    allTerritories.push_back(t.get());
  }

  std::mt19937 rng(std::random_device{}());
  std::ranges::shuffle(allTerritories, rng);

  for (size_t i = 0; i < allTerritories.size(); ++i) {
    Territory* terr = allTerritories[i];
    Player* oldOwner = terr->getOwner();
    Player* newOwner = (*players)[i % players->size()];

    if (oldOwner && oldOwner != newOwner) oldOwner->removeTerritory(terr);
    if (!newOwner->ownsTerritory(terr)) newOwner->addTerritory(terr);
    if (terr->getOwner() != newOwner) terr->setOwner(newOwner);

    // initialize with some armies
    terr->setArmies(3);
  }

  // initialize deck
  if (deck) { delete deck; }
  deck = new Deck();

  // give each player initial armies
  for (auto* play : *players) { play->setReinforcementPool(30); }

  // play the game using mainGameLoop with turn limit
  std::string winner = mainGameLoop(maxTurns);
  // disable automatic mode after tournament game
  setAutomaticMode(false);
  return winner;
}

/**
 * log tournament results to console and file
 * @param maps list of map names
 * @param strategies list of player strategy names
 * @param numGames number of games played per map
 * @param maxTurns maximum turns per game
 * @param results 2d vector of results [mapIndex][gameIndex] -> winner name or "Draw"
 */
void GameEngine::logTournamentResults(
  const std::vector<std::string>& maps, const std::vector<std::string>& strategies,
  int numGames, int maxTurns, const std::vector<std::vector<std::string> >& results
) {
  std::cout << "\n========================================\n";
  std::cout << "TOURNAMENT RESULTS\n";
  std::cout << "========================================\n\n";

  std::cout << "Tournament mode:\n";

  // display maps
  std::cout << "M: ";
  for (size_t i = 0; i < maps.size(); ++i) {
    std::cout << maps[i];
    if (i < maps.size() - 1) { std::cout << ", "; }
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
    if (i < strategies.size() - 1) { std::cout << ", "; }
  }
  std::cout << "\n";

  std::cout << "G: " << numGames << "\n";
  std::cout << "D: " << maxTurns << "\n\n";

  // display results table
  std::cout << "Results:\n                    ";

  // header row
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
  std::ofstream logFile(GAMELOG_FILE, std::ios::app);
  if (logFile.is_open()) {
    logFile << "\n========================================\n";
    logFile << "TOURNAMENT RESULTS\n";
    logFile << "========================================\n\n";

    logFile << "Tournament mode:\n";
    logFile << "M: ";
    for (size_t i = 0; i < maps.size(); ++i) {
      logFile << maps[i];
      if (i < maps.size() - 1) { logFile << ", "; }
    }
    logFile << "\n";

    logFile << "P: ";
    for (size_t i = 0; i < strategies.size(); ++i) {
      std::string strategyDisplay = strategies[i];
      if (!strategyDisplay.empty()) {
        strategyDisplay[0] = std::toupper(strategyDisplay[0]);
      }
      logFile << strategyDisplay;
      if (i < strategies.size() - 1) { logFile << ", "; }
    }
    logFile << "\n";

    logFile << "G: " << numGames << "\n";
    logFile << "D: " << maxTurns << "\n\n";

    logFile << "Results:\n                    ";
    for (int g = 0; g < numGames; ++g) { logFile << "Game " << (g + 1) << "    "; }
    logFile << "\n";

    for (size_t m = 0; m < maps.size(); ++m) {
      logFile << maps[m] << "    ";
      for (int g = 0; g < numGames; ++g) { logFile << results[m][g] << "    "; }
      logFile << "\n";
    }

    logFile << "\n========================================\n";
    logFile.close();
  }
}

// --- UTILITY --
std::string GameEngine::stringToLog() const {
  return "GameEngine state changed to: " + getCurrentStateName();
}

void GameEngine::displayValidCommands() const {
  std::cout << "Valid commands for state '" << currentState->getName() << "': ";
  std::vector<std::string> validCommands = currentState->getValidCommands();

  for (size_t i = 0; i < validCommands.size(); ++i) {
    std::cout << validCommands[i];
    if (i < validCommands.size() - 1) { std::cout << ", "; }
  }
  std::cout << std::endl;
}

void GameEngine::displayStateHistory() const {
  std::cout << "State transition history: ";
  for (size_t i = 0; i < stateHistory->size(); ++i) {
    std::cout << (*stateHistory)[i];
    if (i < stateHistory->size() - 1) { std::cout << " -> "; }
  }
  std::cout << std::endl;
}

// --- HELPERS --
void GameEngine::initializeStates() {
  // create all states based on the assignment state diagram
  (*states)[GAME_PHASES::STARTUP] = new State(GAME_PHASES::STARTUP);
  (*states)[GAME_STATES::START] = new State(GAME_STATES::START);
  (*states)[GAME_STATES::MAP_LOADED] = new State(GAME_STATES::MAP_LOADED);
  (*states)[GAME_STATES::MAP_VALIDATED] = new State(GAME_STATES::MAP_VALIDATED);
  (*states)[GAME_STATES::PLAYERS_ADDED] = new State(GAME_STATES::PLAYERS_ADDED);
  (*states)[GAME_PHASES::PLAY] = new State(GAME_PHASES::PLAY);
  (*states)[GAME_STATES::ASSIGN_REINFORCEMENT] = new State(GAME_STATES::ASSIGN_REINFORCEMENT);
  (*states)[GAME_STATES::ISSUE_ORDERS] = new State(GAME_STATES::ISSUE_ORDERS);
  (*states)[GAME_STATES::EXECUTE_ORDERS] = new State(GAME_STATES::EXECUTE_ORDERS);
  (*states)[GAME_STATES::WIN] = new State(GAME_STATES::WIN);

  // setup transitions based on the assignment state diagram
  // from startup state (the initial state before start)
  (*states)[GAME_PHASES::STARTUP]->addTransition(new Transition(GAME_STATES::START, GAME_STATES::START));

  // from start state
  (*states)[GAME_STATES::START]->addTransition(new Transition(GAME_TRANSITIONS::LOADMAP, GAME_STATES::MAP_LOADED));

  // from map loaded state
  (*states)[GAME_STATES::MAP_LOADED]->addTransition(new Transition(GAME_TRANSITIONS::LOADMAP, GAME_STATES::MAP_LOADED));
  (*states)[GAME_STATES::MAP_LOADED]->addTransition(new Transition(GAME_TRANSITIONS::VALIDATEMAP, GAME_STATES::MAP_VALIDATED));

  // from map validated state
  (*states)[GAME_STATES::MAP_VALIDATED]->addTransition(new Transition(GAME_TRANSITIONS::ADDPLAYER, GAME_STATES::PLAYERS_ADDED));

  // from players added state
  (*states)[GAME_STATES::PLAYERS_ADDED]->addTransition(new Transition(GAME_TRANSITIONS::ADDPLAYER, GAME_STATES::PLAYERS_ADDED));
  (*states)[GAME_STATES::PLAYERS_ADDED]->addTransition(new Transition(GAME_TRANSITIONS::GAMESTART, GAME_STATES::ASSIGN_REINFORCEMENT));

  (*states)[GAME_PHASES::PLAY]->addTransition(new Transition(GAME_PHASES::PLAY, GAME_STATES::ASSIGN_REINFORCEMENT));
  // from assign reinforcement state
  (*states)[GAME_STATES::ASSIGN_REINFORCEMENT]->addTransition(new Transition(GAME_TRANSITIONS::ISSUEORDER, GAME_STATES::ISSUE_ORDERS));

  // from issue orders state
  (*states)[GAME_STATES::ISSUE_ORDERS]->addTransition(new Transition(GAME_TRANSITIONS::ISSUEORDER, GAME_STATES::ISSUE_ORDERS));
  (*states)[GAME_STATES::ISSUE_ORDERS]->addTransition(new Transition(GAME_TRANSITIONS::ENDISSUEORDERS, GAME_STATES::EXECUTE_ORDERS));

  // from execute orders state
  (*states)[GAME_STATES::EXECUTE_ORDERS]->addTransition(new Transition(GAME_TRANSITIONS::EXECORDER, GAME_STATES::EXECUTE_ORDERS));
  (*states)[GAME_STATES::EXECUTE_ORDERS]->addTransition(new Transition(GAME_TRANSITIONS::ENDEXECORDERS, GAME_STATES::ASSIGN_REINFORCEMENT));
  (*states)[GAME_STATES::EXECUTE_ORDERS]->addTransition(new Transition(GAME_STATES::WIN, GAME_STATES::WIN));

  // from win state
  (*states)[GAME_STATES::WIN]->addTransition(new Transition(GAME_TRANSITIONS::REPLAY, GAME_STATES::START));
  (*states)[GAME_STATES::WIN]->addTransition(new Transition(GAME_TRANSITIONS::QUIT, GAME_STATES::WIN));
  // stays in win, but signals exit
}

void GameEngine::processCommand(const std::string& cmd) {
  // parse the base command
  std::string baseCmd = cmd;
  std::size_t spacePos = cmd.find(' ');
  if (spacePos != std::string::npos) {
    baseCmd = cmd.substr(0, spacePos);
  }

  // look for transition using base command
  Transition* transition = currentState->getTransition(baseCmd);

  if (transition == nullptr) displayValidCommands();
  else {
    std::cout << "Executing command: " << cmd << std::endl;
    transitionState(transition->getTargetState());
  }
}

void GameEngine::transitionState(const std::string& stateName) {
  auto it = states->find(stateName);
  if (it == states->end()) {
    std::cout << "Error: State '" << stateName << "' does not exist!" << std::endl;
    return;
  }

  currentState = it->second;
  stateHistory->push_back(stateName);
  std::cout << "Transitioned to state: " << stateName << std::endl;
  notify();
}

void GameEngine::cleanupStates() {
  for (auto& snd : *states | std::views::values) { delete snd; }
  states->clear();
}

std::string GameEngine::trimCopy(const std::string& inp) {
  auto begin = std::ranges::find_if_not(inp, [](int c) { return std::isspace(c); });
  auto end = std::find_if_not(inp.rbegin(), inp.rend(), [](int c) { return std::isspace(c); }).base();
  return (end <= begin) ? std::string() : std::string(begin, end);
}

// --- STREAM INSERTION OPERATOR --
std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {
  os << "GameEngine[Current State: " << engine.getCurrentStateName() << "]" << std::endl;
  return os;
}
