#pragma once
#include <iostream>
#include <string>
#include <vector>

class Territory;
class Hand;
class OrdersList;
class Card;
class Deck;
class PlayerStrategy;

/**
 * The Player class represents a Warzone player.
 * A player owns a collection of territories, a hand of cards, and a list of orders.
 * The player can issue orders, defend territories, and attack other territories.
 */
class Player {
private:
  std::string* name;
  std::vector<Territory*>* territories; // collection of owned territories
  Hand* hand;                           // player's hand of cards
  OrdersList* ordersList;               // list of orders to execute
  int* reinforcementPool;               // number of armies available for deployment
  bool* conqueredThisTurn;              // flag indicating if the player conquered a territory this turn
  Deck* deck;                           // pointer to the deck (for drawing/returning cards)
  int* pendingReinforcements;           // number of armies pending deployment
  bool* canIssueOrder;                  // flag indicating if the player can issue orders
  bool* gotAttackedThisTurn;            // flag indicating if the player got attacked this turn
  PlayerStrategy* strategy;             // strategy pattern for player behavior

public:
  Player();
  Player(const std::string& newName, Deck* newDeck);
  Player(const Player& other); // copy constructor
  Player& operator=(const Player& other); // assignment operator
  ~Player(); // destructor

  // getters
  std::string getName() const;
  const std::vector<Territory*>& getTerritories() const;
  Hand* getHand() const;
  OrdersList* getOrders() const;
  int getReinforcementPool() const;
  bool getConqueredThisTurn() const;
  Deck* getDeck() const;
  int getPendingReinforcements() const;
  bool* getCanIssueOrder() const;
  bool* getGotAttackedThisTurn() const;
  PlayerStrategy* getPlayerStrategy() const;

  // setters
  void setName(const std::string& newName);
  void setReinforcementPool(int armies);
  void setConqueredThisTurn(bool conquered);
  void setPendingReinforcements(int armies);
  void setCanIssueOrder(bool canIssue);
  void setGotAttackedThisTurn(bool attacked);
  void setStrategy(PlayerStrategy* newStrategy);

  // territory management
  void addTerritory(Territory* terr);
  void removeTerritory(Territory* terr);
  bool ownsTerritory(Territory* terr) const;

  // card management
  void addCard(Card* card);
  void playCard(int index, Deck* gameDeck);

  // order management
  void issueOrder();
  void issueOrder(bool deployPhase, bool& advanceIssued, Deck* gameDeck);
  void issueDeployOrder(Territory* target, int armies);
  void issueAdvanceOrder(Territory* source, Territory* target, int armies);
  void issueAirliftOrder(Territory* source, Territory* target, int armies);
  void issueBombOrder(Territory* target);
  void issueBlockadeOrder(Player* player, Territory* target);
  void issueNegotiateOrder(Player* player);
  void issueCheatOrder();

  // strategy management
  std::vector<Territory*> toDefend() const;
  std::vector<Territory*> toAttack() const;

  // player management
  static Player* choosePlayer(const std::vector<Player*>& players);

  // utility
  int getTotalArmies() const;
  int getTerritoryCount() const;
  void displayInfo() const;
  bool operator==(const Player& other) const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Player& play);
};

// free function
void testPlayers();
