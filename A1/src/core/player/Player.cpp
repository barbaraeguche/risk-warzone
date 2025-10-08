#include "Player.h"
#include "../map/Map.h"
#include "../card-deck-hand/Cards.h"
#include "../orders/Orders.h"
#include <algorithm>
#include <random>

// ==================== Player Class Implementation ====================

/**
 * Default constructor
 */
Player::Player() :
  playerName(new std::string("Unknown Player")),
  territories(new std::vector<Territory*>()),
  hand(new Hand()),
  orders(new OrdersList()),
  reinforcementPool(new int(0)) {}

/**
 * Parameterized constructor
 * @param name The name of the player
 */
Player::Player(const std::string& name) :
  playerName(new std::string(name)),
  territories(new std::vector<Territory*>()),
  hand(new Hand()),
  orders(new OrdersList()),
  reinforcementPool(new int(0)) {}

/**
 * Copy constructor
 * @param other The player to copy from
 */
Player::Player(const Player& other) :
  playerName(new std::string(*other.playerName)),
  territories(new std::vector<Territory*>(*other.territories)),
  hand(new Hand(*other.hand)),
  orders(new OrdersList(*other.orders)),
  reinforcementPool(new int(*other.reinforcementPool)) {}

/**
 * Assignment operator
 * @param other The player to assign from
 * @return Reference to this player
 */
Player& Player::operator=(const Player& other) {
  if (this != &other) {
      // Clean up existing resources
      delete playerName;
      delete territories;
      delete hand;
      delete orders;
      delete reinforcementPool;

      // Copy from other player
      playerName = new std::string(*other.playerName);
      territories = new std::vector<Territory*>(*other.territories);
      hand = new Hand(*other.hand);
      orders = new OrdersList(*other.orders);
      reinforcementPool = new int(*other.reinforcementPool);
  }
  return *this;
}

/**
 * Destructor
 */
Player::~Player() {
  delete playerName;
  delete territories;
  delete hand;
  delete orders;
  delete reinforcementPool;
}

// ==================== Getters ====================

/**
 * Get the player's name
 * @return The player's name
 */
std::string Player::getName() const {
  return *playerName;
}

/**
 * Get the player's territories
 * @return Reference to the vector of territories
 */
const std::vector<Territory*>& Player::getTerritories() const {
  return *territories;
}

/**
 * Get the player's hand
 * @return Pointer to the hand
 */
Hand* Player::getHand() const {
  return hand;
}

/**
 * Get the player's orders
 * @return Pointer to the order list
 */
OrdersList* Player::getOrders() const {
  return orders;
}

/**
 * Get the reinforcement pool
 * @return Number of armies in reinforcement pool
 */
int Player::getReinforcementPool() const {
  return *reinforcementPool;
}

// ==================== Setters ====================

/**
 * Set the player's name
 * @param name The new name
 */
void Player::setName(const std::string& name) {
  *playerName = name;
}

/**
 * Set the reinforcement pool
 * @param armies Number of armies
 */
void Player::setReinforcementPool(int armies) {
  *reinforcementPool = armies;
}

// ==================== Territory Management ====================

/**
 * Add a territory to the player's collection
 * @param territory Pointer to the territory to add
 */
void Player::addTerritory(Territory* territory) {
  if (territory && !ownsTerritory(territory)) {
      territories->push_back(territory);
      territory->setOwner(this);
  }
}

/**
 * Remove a territory from the player's collection
 * @param territory Pointer to the territory to remove
 */
void Player::removeTerritory(Territory* territory) {
  if (territory) {
      auto it = std::find(territories->begin(), territories->end(), territory);
      if (it != territories->end()) {
          territories->erase(it);
          territory->setOwner(nullptr);
      }
  }
}

/**
 * Check if the player owns a territory
 * @param territory Pointer to the territory to check
 * @return True if the player owns the territory
 */
bool Player::ownsTerritory(Territory* territory) const {
  if (!territory) return false;
  return std::find(territories->begin(), territories->end(), territory) != territories->end();
}

// ==================== Card Management ====================

/**
 * Add a card to the player's hand
 * @param card Pointer to the card to add
 */
void Player::addCard(Card* card) {
  if (card) {
      hand->addCard(card);
  }
}

/**
 * Play a card from the hand
 * @param index Index of the card to play
 * @param deck Pointer to the deck to return the card to
 */
void Player::playCard(int index, Deck* deck) {
  if (hand && deck) {
      hand->playCard(index, this, orders, deck);
  }
}

// ==================== Order Management ====================

/**
 * Issue a generic order
 * @param order Pointer to the order to add
 */
void Player::issueOrder(Order* order) {
  if (order) {
      orders->add(order);
  }
}

/**
 * Issue a deploy order
 * @param target Territory to deploy armies to
 * @param armies Number of armies to deploy
 */
void Player::issueDeployOrder(Territory* target, int armies) {
  orders->add(new OrderDeploy(this, target, new int(armies)));
}

/**
 * Issue an advance order
 * @param source Source territory
 * @param target Target territory
 * @param armies Number of armies to advance
 */
void Player::issueAdvanceOrder(Territory* source, Territory* target, int armies) {
  orders->add(new OrderAdvance(this, source, target, new int(armies)));
}

/**
 * Issue a bomb order
 * @param target Territory to bomb
 */
void Player::issueBombOrder(Territory* target) {
  orders->add(new OrderBomb(target));
}

/**
 * Issue a blockade order
 * @param target Territory to blockade
 */
void Player::issueBlockadeOrder(Territory* target) {
  orders->add(new OrderBlockade(this, target));
}

/**
 * Issue an airlift order
 * @param source Source territory
 * @param target Target territory
 * @param armies Number of armies to airlift
 */
void Player::issueAirliftOrder(Territory* source, Territory* target, int armies) {
  orders->add(new OrderAirlift(this, source, target, new int(armies)));
}

/**
 * Issue a negotiate order
 * @param targetPlayer Player to negotiate with
 */
void Player::issueNegotiateOrder(Player* targetPlayer) {
  orders->add(new OrderNegotiate(this, targetPlayer));
}

// ==================== Strategy Methods ====================

/**
 * Get territories to defend (arbitrary implementation)
 * @return Vector of territories to defend
 */
std::vector<Territory*> Player::toDefend() const {
  std::vector<Territory*> defendList;
  
  // For now, return all owned territories as arbitrary defense list
  for (Territory* territory : *territories) {
      if (territory) {
          defendList.push_back(territory);
      }
  }
  
  return defendList;
}

/**
 * Get territories to attack (arbitrary implementation)
 * @return Vector of territories to attack
 */
std::vector<Territory*> Player::toAttack() const {
  std::vector<Territory*> attackList;
  
  // For now, return adjacent territories of owned territories as arbitrary attack list
  for (Territory* territory : *territories) {
      if (territory) {
          const std::vector<Territory*>& adjTerritories = territory->getAdjTerritories();
          for (Territory* adjTerritory : adjTerritories) {
              if (adjTerritory && !ownsTerritory(adjTerritory)) {
                  // Check if already in attack list to avoid duplicates
                  if (std::find(attackList.begin(), attackList.end(), adjTerritory) == attackList.end()) {
                      attackList.push_back(adjTerritory);
                  }
              }
          }
      }
  }
  
  return attackList;
}

// ==================== Utility Methods ====================

/**
 * Display player information
 */
void Player::displayInfo() const {
  std::cout << "Player: " << *playerName << std::endl;
  std::cout << "  Territories: " << territories->size() << std::endl;
  std::cout << "  Total Armies: " << getTotalArmies() << std::endl;
  std::cout << "  Reinforcement Pool: " << *reinforcementPool << std::endl;
  std::cout << "  Cards in Hand: " << hand->size() << std::endl;
  std::cout << "  Orders: " << orders->orders->size() << std::endl;
  
  std::cout << "  Owned Territories:" << std::endl;
  for (Territory* territory : *territories) {
      if (territory) {
          std::cout << "    - " << territory->getName() << " (Armies: " << territory->getArmies() << ")" << std::endl;
      }
  }
}

/**
 * Get total armies across all territories
 * @return Total number of armies
 */
int Player::getTotalArmies() const {
  int total = 0;
  for (Territory* territory : *territories) {
      if (territory) {
          total += territory->getArmies();
      }
  }
  return total;
}

/**
 * Get the number of territories owned
 * @return Number of territories
 */
int Player::getTerritoryCount() const {
  return static_cast<int>(territories->size());
}

// ==================== Stream Insertion Operator ====================

/**
 * Stream insertion operator
 * @param os Output stream
 * @param player Player to output
 * @return Reference to output stream
 */
std::ostream& operator<<(std::ostream& os, const Player& player) {
  os << "Player(" << *player.playerName << ", Territories: " << player.territories->size() 
     << ", Armies: " << player.getTotalArmies() << ", Reinforcement Pool: " << *player.reinforcementPool << ")";
  return os;
}