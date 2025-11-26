#include "Player.h"
#include "PlayerStrategies.h"
#include "Map.h"
#include "Cards.h"
#include "Orders.h"
#include <algorithm>

// ==================== Player Class Implementation ====================

/**
 * Default constructor
 */
Player::Player() :
  strategy(nullptr),
  conqueredThisTurn(new bool(false)),
  playerName(new std::string("Unknown Player")),
  territories(new std::vector<Territory*>()),
  hand(new Hand()),
  deck(nullptr),
  orders(new OrdersList()),
  reinforcementPool(new int(0)),
  pendingReinforcements(new int(0)),
  canIssueOrderFlag(new bool(true)),
  gotAttackedThisTurn(new bool(false)) {}

/**
 * Parameterized constructor
 * @param name The name of the player
 */
Player::Player(const std::string& name, Deck* deck) :
  strategy(nullptr),
  conqueredThisTurn(new bool(false)),
  playerName(new std::string(name)),
  territories(new std::vector<Territory*>()),
  hand(new Hand()),
  deck(deck),
  orders(new OrdersList()),
  reinforcementPool(new int(0)), 
  pendingReinforcements(new int(0)),
  canIssueOrderFlag(new bool(true)),
  gotAttackedThisTurn(new bool(false))
  {}

/**
 * Copy constructor
 * @param other The player to copy from
 */
Player::Player(const Player& other) :
  strategy(nullptr),
  conqueredThisTurn(new bool(other.conqueredThisTurn)),
  playerName(new std::string(*other.playerName)),
  territories(new std::vector<Territory*>(*other.territories)),
  hand(new Hand(*other.hand)),
  orders(new OrdersList(*other.orders)),
  reinforcementPool(new int(*other.reinforcementPool)), 
  pendingReinforcements(new int(*other.pendingReinforcements)),
  canIssueOrderFlag(new bool(*other.canIssueOrderFlag)),
  gotAttackedThisTurn(new bool(*other.gotAttackedThisTurn))
  {}

/**
 * Assignment operator
 * @param other The player to assign from
 * @return Reference to this player
 */
Player& Player::operator=(const Player& other) {
  if (this != &other) {
      // Clean up existing resources
      delete strategy;
      delete conqueredThisTurn;
      delete playerName;
      delete territories;
      delete hand;
      delete orders;
      delete reinforcementPool;
      delete pendingReinforcements;
      delete canIssueOrderFlag;
      delete gotAttackedThisTurn;

      // Copy from other player
      strategy = nullptr;
      conqueredThisTurn = new bool(other.conqueredThisTurn); 
      playerName = new std::string(*other.playerName);
      territories = new std::vector<Territory*>(*other.territories);
      hand = new Hand(*other.hand);
      orders = new OrdersList(*other.orders);
      reinforcementPool = new int(*other.reinforcementPool);
      pendingReinforcements = new int(*other.pendingReinforcements);
      canIssueOrderFlag = new bool(*other.canIssueOrderFlag);
      gotAttackedThisTurn = new bool(*other.gotAttackedThisTurn);
  }
  return *this;
}

/**
 * Destructor
 */
Player::~Player() {
  delete strategy;
  delete conqueredThisTurn;
  delete playerName;
  delete territories;
  delete hand;
  delete orders;
  delete reinforcementPool;
  delete pendingReinforcements;
  delete canIssueOrderFlag;
  delete gotAttackedThisTurn;
}

// ==================== Getters ====================

bool* Player::getCanIssueOrderFlag() const {
    return canIssueOrderFlag;
}

void Player::setCanIssueOrderFlag(bool value) {
    *canIssueOrderFlag = value;
}

bool* Player::getGotAttackedThisTurn() const {
    return gotAttackedThisTurn;
}

void Player::setGotAttackedThisTurn(bool attacked) {
    *gotAttackedThisTurn = attacked;
}

PlayerStrategy* Player::getPlayerStrategy() const {
    return strategy;
}

/**
 * Get whether the player conquered a territory this turn
 * @return True if conquered this turn
 */
bool Player::getConqueredThisTurn() const {
  return *conqueredThisTurn;
}

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
 * Get the player's deck
 * @return Pointer to the deck
 */
Deck* Player::getDeck() const {
  return deck;
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

int Player::getPendingReinforcements() const {
  return *pendingReinforcements;
}

// ==================== Setters ====================

/**
 * Set whether the player conquered a territory this turn
 * @param conquered True if conquered this turn
 */
void Player::setConqueredThisTurn(bool conquered) {
  *conqueredThisTurn = conquered;
}

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

void Player::setPendingReinforcements(int armies) {
  *pendingReinforcements = armies;
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
  return std::any_of(territories->begin(), territories->end(),
                   [&](Territory* t){ return t->getName() == territory->getName(); });
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
void Player::issueOrder(bool deployPhase, bool& advanceIssued, Deck* deck_) {
    if (deployPhase) {
        //Deploy phase
        std::vector<Territory*> defendList = toDefend();
        if (*reinforcementPool > 0 && !defendList.empty()) {
            std::cout << "\nPlayer " << getName() << " - Reinforcement Pool: " << *reinforcementPool << " armies\n";
            std::cout << "Territories to defend:\n";
            for (size_t i = 0; i < defendList.size(); i++) {
                std::cout << i << ": " << defendList[i]->getName() 
                          << " (Current armies: " << defendList[i]->getArmies() << ")\n";
            }

            int choice = -1;
            int armies = -1;
            while (choice < 0 || choice >= defendList.size()) {
                std::cout << "Choose territory to deploy to (index): ";
                std::cin >> choice;
            }

            while (armies <= 0 || armies > *reinforcementPool) {
              std::cout << "Enter number of armies to deploy (max " << *reinforcementPool << "): ";
              std::cin >> armies;
            }

            this->issueDeployOrder(defendList[choice], armies);
            *reinforcementPool -= armies;
            std::cout << armies << " armies deployed to " << defendList[choice]->getName() << ".\n";
        }
    } else {
        //Advance phase
        if (advanceIssued) return; // Only one advance order per player

        std::vector<Territory*> attackList = toAttack();
        std::vector<Territory*> defendList = toDefend();

        bool canAdvance = false;
        for (Territory* t : *territories) {
            if (t->getArmies() > 1) {
                canAdvance = true;
                break;
            }
        }
        if (!canAdvance){
            if (!hand->empty()) {
              std::cout << "Player " << getName() << "'s cards:\n";
              hand->printHand();

              int cardIndex = -1;
              std::cout << "Choose a card to play: (-1 for no cards) ";
              std::cin >> cardIndex;
              if(cardIndex == -1) return;
              else if(cardIndex < 0 || cardIndex >= hand->size()) std::cout << "Invalid card index.\n";

              playCard(cardIndex, deck_);
            }  
            return;
        };

        std::cout << "\nPlayer " << getName() << " - Advance Phase\n";

        std::cout << "Your own territories:\n";
        for (size_t i = 0; i < territories->size(); i++) {
            std::cout << i << ": " << territories->at(i)->getName() 
                      << " (Armies: " << territories->at(i)->getArmies() << ")\n";
        }

        std::vector<Territory*> territoriesWithEnemies;

        for (Territory* t : *territories) { // all territories you own
            if (!t) continue;
            const std::vector<Territory*>& adj = t->getAdjTerritories();
            for (Territory* a : adj) {
                if (a && !ownsTerritory(a)) { // if adjacent to enemy
                    territoriesWithEnemies.push_back(t);
                    break; // only need one enemy to qualify
                }
            }
        }

        // Display
        if (territoriesWithEnemies.empty()) {
            std::cout << "No territories adjacent to enemies.\n";
        } else {
            std::cout << "Your territories adjacent to enemies:\n";
            for (size_t i = 0; i < territoriesWithEnemies.size(); ++i) {
                std::cout << i << ": " << territoriesWithEnemies[i]->getName()
                          << " (Armies: " << territoriesWithEnemies[i]->getArmies() << ")\n";
            }
        }

        int sourceIndex = -1;
        Territory* source = nullptr;
        Territory* target = nullptr;

        while (true) { // loop until a valid source + target is chosen
            // Choose source territory
            sourceIndex = -1;
            while (sourceIndex < 0 || sourceIndex >= territories->size() || territories->at(sourceIndex)->getArmies() <= 1) {
                std::cout << "Choose source territory (index, must have >1 army): ";
                std::cin >> sourceIndex;
            }
            source = territories->at(sourceIndex);
          
            // Choose target type
            int targetType = -1;
            while (targetType != 0 && targetType != 1) {
                std::cout << "Target type: 0 = defend (own), 1 = attack (enemy): ";
                std::cin >> targetType;
            }
          
            // Filter adjacent territories based on target type
            const std::vector<Territory*>& adjTerritories = source->getAdjTerritories();
            std::vector<Territory*> validTargets;
            for (Territory* adj : adjTerritories) {
                if (!adj) continue;
                if (targetType == 1 && !ownsTerritory(adj)) { // attack
                    validTargets.push_back(adj);
                } else if (targetType == 0 && ownsTerritory(adj)) { // defend/reinforce
                    validTargets.push_back(adj);
                }
            }
          
            // If no valid targets, notify and restart loop
            if (validTargets.empty()) {
                std::cout << "No valid adjacent territories for this source. Choose another source.\n";
                continue; // back to source selection
            }
          
            // Display valid targets
            for (size_t i = 0; i < validTargets.size(); ++i) {
                std::cout << i << ": " << validTargets[i]->getName()
                          << " (Armies: " << validTargets[i]->getArmies() << ")\n";
            }
          
            // Choose target
            int targetIndex = -1;
            while (targetIndex < 0 || targetIndex >= validTargets.size()) {
                std::cout << "Choose target territory (index): -1 to reselect source: ";
                std::cin >> targetIndex;
                if (targetIndex == -1) break; // reselect source
            }
            if (targetIndex == -1) continue; // reselect source
            target = validTargets[targetIndex];
          
            // Choose number of armies to move
            int armies = -1;
            while (armies <= 0 || armies > source->getArmies() - 1) {
                std::cout << "Enter number of armies to move (max " << source->getArmies() - 1 << "): ";
                std::cin >> armies;
            }
          
            // Issue order and exit loop
            issueAdvanceOrder(source, target, armies);
            std::cout << "Advance order issued from " << source->getName() 
                      << " to " << target->getName() << " with " << armies << " armies.\n";
          
            advanceIssued = true;
            std::cout << "Advance order issued.\n";
            break; // exit the while(true) loop
        }

        if (!hand->empty()) {
            std::cout << "Your cards:\n";
            hand->printHand();

            int cardIndex = -1;
    
            std::cout << "Choose a card to play: (-1 for no cards) ";
            std::cin >> cardIndex;

            if(cardIndex == -1) return;
            else if(cardIndex < 0 || cardIndex >= hand->size()) std::cout << "Invalid card index.\n";

            playCard(cardIndex, deck_);
        }
    }
}

// bool Player::issueOrder() {
//   // Priority 1: Deploy armies if we have any in reinforcement pool
//   if (*reinforcementPool > 0) {
//       std::vector<Territory*> defendList = toDefend();
      
//       if (!defendList.empty()) {
//           // Deploy to first territory in defend list
//           Territory* target = defendList[0];
//           int armiesToDeploy = std::min(*reinforcementPool, 3); // Deploy up to 3 at a time
          
//           std::cout << "  Issuing Deploy order: " << armiesToDeploy << " armies to " << target->getName() << "\n";
//           issueDeployOrder(target, armiesToDeploy);
//           return true;
//       }
//   }
  
//   // Priority 2: Issue advance orders (defend or attack)
//   std::vector<Territory*> defendList = toDefend();
//   std::vector<Territory*> attackList = toAttack();
  
//   // Try to move armies to defend territories
//   if (!defendList.empty() && territories->size() > 1) {
//       // Find a territory with armies to move
//       for (Territory* source : *territories) {
//           if (source->getArmies() > 1) {
//               // Try to find adjacent friendly territory to reinforce
//               for (Territory* adjacent : source->getAdjTerritories()) {
//                   if (ownsTerritory(adjacent)) {
//                       int armiesToMove = source->getArmies() / 2;
//                       if (armiesToMove > 0) {
//                           std::cout << "  Issuing Advance order (defense): " << armiesToMove 
//                                     << " armies from " << source->getName() 
//                                     << " to " << adjacent->getName() << "\n";
//                           issueAdvanceOrder(source, adjacent, armiesToMove);
//                           return true;
//                       }
//                   }
//               }
//           }
//       }
//   }
  
//   // Try to attack enemy territories
//   if (!attackList.empty()) {
//       for (Territory* target : attackList) {
//           // Find a friendly territory adjacent to this enemy territory
//           for (Territory* source : *territories) {
//               if (source->isAdjacentTo(target) && source->getArmies() > 1) {
//                   int armiesToAttack = source->getArmies() - 1; // Leave 1 army behind
//                   std::cout << "  Issuing Advance order (attack): " << armiesToAttack 
//                             << " armies from " << source->getName() 
//                             << " to " << target->getName() << "\n";
//                   issueAdvanceOrder(source, target, armiesToAttack);
//                   return true;
//               }
//           }
//       }
//   }
  
//   // Priority 3: Play cards if we have any
//   if (hand->size() > 0) {
//       std::cout << "  Playing a card from hand\n";
//       // For now, just note that card is played but don't actually play
//       // In a full implementation, this would create orders from cards
//       // For simplicity in this demo, we'll skip actual card playing
//   }
  
//   // No more orders to issue
//   return false;
// }

void Player::issueOrder() {
  if (strategy) this->strategy->issueOrder();
}

void Player::setStrategy(PlayerStrategy* strategy) {
  this->strategy = strategy;
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
  orders->add(new OrderBomb(target, this));
}

/**
 * Issue a blockade order
 * @param target Territory to blockade
 */
void Player::issueBlockadeOrder(Player* nPlayer, Territory* target) {
  orders->add(new OrderBlockade(nPlayer, this, target));
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

void Player::issueCheatOrder() {
    orders->add(new OrderCheat(this));
}

// ==================== Strategy Methods ====================

/**
 * Get territories to defend (arbitrary implementation)
 * @return Vector of territories to defend
 */
std::vector<Territory*> Player::toDefend() const {
  if (strategy) return this->strategy->toDefend();
  else
    return std::vector<Territory*>();
}

/**
 * Get territories to attack (arbitrary implementation)
 * @return Vector of territories to attack
 */
std::vector<Territory*> Player::toAttack() const {
  if (strategy) return this->strategy->toAttack();
  else
    return std::vector<Territory*>();
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

Player* Player::choosePlayer(const std::vector<Player*>& players) {
    if (players.empty()) return nullptr;

    std::cout << "Choose a player:\n";
    for (size_t i = 0; i < players.size(); i++) {
        std::cout << i << ": " << players[i]->getName() << "\n";
    }

    int choice = -1;
    while (choice < 0 || choice >= players.size()) {
        std::cout << "Enter the number of your choice: ";
        std::cin >> choice;
    }

    return players[choice];
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