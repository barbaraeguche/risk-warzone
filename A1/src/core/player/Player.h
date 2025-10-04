#ifndef COMP_345_PLAYER_H
#define COMP_345_PLAYER_H

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

/**
 * Player class representing a Warzone player
 * 
 * A player owns a collection of territories, a hand of cards, and a list of orders.
 * The player can issue orders, defend territories, and attack other territories.
 */
class Player {
private:
  std::string* playerName;                    // Player's name
  std::vector<Territory*>* territories;      // Collection of owned territories
  Hand* hand;                                // Player's hand of cards
  OrdersList* orders;                        // List of orders to execute
  int* reinforcementPool;                    // Number of armies available for deployment

public:
  // Constructors
  Player();
  Player(const std::string& name);
  Player(const Player& other);               // Copy constructor
  Player& operator=(const Player& other);    // Assignment operator
  ~Player();                                 // Destructor

  // Getters
  std::string getName() const;
  const std::vector<Territory*>& getTerritories() const;
  Hand* getHand() const;
  OrdersList* getOrders() const;
  int getReinforcementPool() const;

  // Setters
  void setName(const std::string& name);
  void setReinforcementPool(int armies);

  // Territory management
  void addTerritory(Territory* territory);
  void removeTerritory(Territory* territory);
  bool ownsTerritory(Territory* territory) const;

  // Card management
  void addCard(Card* card);
  void playCard(int index, Deck* deck);

  // Order management
  void issueOrder(Order* order);
  void issueDeployOrder(Territory* target, int armies);
  void issueAdvanceOrder(Territory* source, Territory* target, int armies);
  void issueBombOrder(Territory* target);
  void issueBlockadeOrder(Territory* target);
  void issueAirliftOrder(Territory* source, Territory* target, int armies);
  void issueNegotiateOrder(Player* targetPlayer);

  // Strategy methods (arbitrary implementation for now)
  std::vector<Territory*> toDefend() const;
  std::vector<Territory*> toAttack() const;

  // Utility methods
  void displayInfo() const;
  int getTotalArmies() const;
  int getTerritoryCount() const;

  // Stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Player& player);
};

#endif //COMP_345_PLAYER_H