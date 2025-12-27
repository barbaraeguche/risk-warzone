#pragma once
#include "LoggingObserver.h"
#include "Map.h"
#include "Player.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class Transition;
class CommandProcessor;
class Deck;

class State {
private:
  std::string* name;
  std::vector<Transition*>* transitions;

public:
  State();
  State(const std::string& newName);
  State(const State& other); // copy constructor
  State& operator=(const State& other); // assignment operator
  ~State(); // destructor

  // getters
  std::string getName() const;
  Transition* getTransition(const std::string& cmd) const;

  // transition management
  void addTransition(Transition* transition);

  // utility
  std::vector<std::string> getValidCommands() const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const State& st);
};


class Transition {
private:
  std::string* command;
  std::string* targetState;

public:
  Transition();
  Transition(const std::string& cmd, const std::string& targSts);
  Transition(const Transition& other); // copy constructor
  Transition& operator=(const Transition& other); // assignment operator
  ~Transition(); // destructor

  // getters
  std::string getCommand() const;
  std::string getTargetState() const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Transition& tran);
};


class GameEngine : public Subject, public ILoggable {
private:
  State* currentState;
  std::map<std::string, State*>* states;
  std::vector<std::string>* stateHistory;
  std::unique_ptr<Map> gameMap;           //  add: holds the loaded/validated map
  std::vector<Player*> players;           // players created by addplayer
  Deck* deck;
  static Player* neutralPlayer;
  static bool automaticMode;              // flag to indicate if game is in automatic/tournament mode

public:
  GameEngine();
  GameEngine(const GameEngine& other); // copy constructor
  GameEngine& operator=(const GameEngine& other); // assignment operator
  ~GameEngine() override; // destructor

  // getters
  std::string getCurrentStateName() const;
  std::vector<Player*> getPlayers() const;
  static Player* getNeutralPlayer();
  static bool getAutomaticMode();

  // setters
  void setMap(Map* map);
  void setDeck(Deck* d);
  static void setAutomaticMode(bool mode);

  // player management
  void addPlayer(Player* player);

  // startup phase management
  void startupPhase();
  void startupPhase(CommandProcessor& cp);

  // orders management
  void reinforcementPhase();
  void issueOrdersPhase();
  void executeOrdersPhase();

  // main game loop management
  std::string mainGameLoop(int maxTurns);

  // tournament mode management
  void executeTournament(const std::string& cmd);
  std::string playTournamentGame(const std::string& mapPath, const std::vector<std::string>& strategies, int maxTurns);
  void logTournamentResults(
    const std::vector<std::string>& maps, const std::vector<std::string>& strategies,
    int numGames, int maxTurns, const std::vector<std::vector<std::string>>& results
  );

  // utility
  std::string stringToLog() const override;
  void displayValidCommands() const;
  void displayStateHistory() const;

  // helpers
  void processCommand(const std::string& cmd);
  void transitionState(const std::string& stateName);

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine);

private:
  // helpers
  void initializeStates();
  void cleanupStates();
};

// namespaces
namespace GAME_PHASES {
  constexpr auto STARTUP = "startup";
  constexpr auto PLAY = "play";
}

namespace GAME_STATES {
  constexpr auto START = "start";
  constexpr auto MAP_LOADED = "map loaded";
  constexpr auto MAP_VALIDATED = "map validated";
  constexpr auto PLAYERS_ADDED = "players added";
  constexpr auto ASSIGN_REINFORCEMENT = "assign reinforcement";
  constexpr auto ISSUE_ORDERS = "issue orders";
  constexpr auto EXECUTE_ORDERS = "execute orders";
  constexpr auto WIN = "win";
}

namespace GAME_TRANSITIONS {
  constexpr auto LOADMAP = "loadmap";
  constexpr auto VALIDATEMAP = "validatemap";
  constexpr auto ADDPLAYER = "addplayer";
  constexpr auto ISSUEORDER = "issueorder";
  constexpr auto ENDISSUEORDERS = "endissueorders";
  constexpr auto EXECORDER = "execorder";
  constexpr auto ENDEXECORDERS = "endexecorders";
  constexpr auto REPLAY = "replay";
  constexpr auto QUIT = "quit";
}

// free function
void testGameStates();
void testStartupPhase();
void testMainGameLoop();
