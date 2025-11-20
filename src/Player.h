#pragma once

#include <string>
#include <vector>
#include <iostream>

// Forward declarations
class Territory;
class Card;
class Hand;
class Order;
class OrdersList;
class Deck;
class PlayerStrategy;

/**
 * Player class representing a Warzone player
 * 
 * A player owns a collection of territories, a hand of cards, and a list of orders.
 * The player can issue orders, defend territories, and attack other territories.
 */
class Player {
private:
  PlayerStrategy* strategy;              // Strategy pattern for player behavior
  bool* conqueredThisTurn;              // Flag indicating if the player conquered a territory this turn
  std::string* playerName;                    // Player's name
  std::vector<Territory*>* territories;      // Collection of owned territories
  Hand* hand;                                // Player's hand of cards
  Deck* deck;                                // Pointer to the deck (for drawing/returning cards)
  OrdersList* orders;                        // List of orders to execute
  int* reinforcementPool;                    // Number of armies available for deployment
  int* pendingReinforcements;               // Number of armies pending deployment
  bool* canIssueOrderFlag;               // Flag indicating if the player can issue orders
  bool* gotAttackedThisTurn;              // Flag indicating if the player got attacked this turn

public:
  // Constructors
  Player();
  Player(const std::string& name, Deck* deck);
  Player(const Player& other);               // Copy constructor
  Player& operator=(const Player& other);    // Assignment operator
  ~Player();                                 // Destructor

  // Getters
  bool getConqueredThisTurn() const;
  std::string getName() const;
  const std::vector<Territory*>& getTerritories() const;
  Hand* getHand() const;
  Deck* getDeck() const;
  OrdersList* getOrders() const;
  int getReinforcementPool() const;
  int getPendingReinforcements() const;
  bool* getGotAttackedThisTurn() const;
  PlayerStrategy* getPlayerStrategy() const;

  // Setters
  void setConqueredThisTurn(bool conquered);
  void setName(const std::string& name);
  void setReinforcementPool(int armies);
  void setPendingReinforcements(int armies);
  void setStrategy(PlayerStrategy* strategy);
  void setGotAttackedThisTurn(bool attacked);

  // Territory management
  void addTerritory(Territory* territory);
  void removeTerritory(Territory* territory);
  bool ownsTerritory(Territory* territory) const;

  // Card management
  void addCard(Card* card);
  void playCard(int index, Deck* deck);

  // Order management
  void issueOrder(bool deployPhase, bool& advanceIssued, Deck* deck_);
  //bool issueOrder();
  void issueOrder();
  bool* getCanIssueOrderFlag() const;
  void setCanIssueOrderFlag(bool canIssue);
  void issueDeployOrder(Territory* target, int armies);
  void issueAdvanceOrder(Territory* source, Territory* target, int armies);
  void issueBombOrder(Territory* target);
  void issueBlockadeOrder(Player* nPlayer, Territory* target);
  void issueAirliftOrder(Territory* source, Territory* target, int armies);
  void issueNegotiateOrder(Player* targetPlayer);
  void issueCheatOrder();

  // Strategy methods (arbitrary implementation for now)
  std::vector<Territory*> toDefend() const;
  std::vector<Territory*> toAttack() const;

  // Utility methods
  void displayInfo() const;
  int getTotalArmies() const;
  int getTerritoryCount() const;
  Player* choosePlayer(const std::vector<Player*>& players);

  // Stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Player& player);
};
