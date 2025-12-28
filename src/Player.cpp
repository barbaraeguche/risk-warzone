#include "Player.h"
#include "Cards.h"
#include "Map.h"
#include "Orders.h"
#include "PlayerStrategies.h"

#include <algorithm>
#include <numeric>


// ==================== Player Class Implementation ====================
Player::Player() :
  name(new std::string("Default")),
  territories(new std::vector<Territory*>()),
  hand(new Hand()),
  ordersList(new OrdersList()),
  reinforcementPool(new int(0)),
  conqueredThisTurn(new bool(false)),
  deck(nullptr),
  pendingReinforcements(new int(0)),
  canIssueOrder(new bool(true)),
  gotAttackedThisTurn(new bool(false)),
  strategy(nullptr) {}

Player::Player(const std::string& newName, Deck* newDeck) :
  name(new std::string(newName)),
  territories(new std::vector<Territory*>()),
  hand(new Hand()),
  ordersList(new OrdersList()),
  reinforcementPool(new int(0)),
  conqueredThisTurn(new bool(false)),
  deck(newDeck),
  pendingReinforcements(new int(0)),
  canIssueOrder(new bool(true)),
  gotAttackedThisTurn(new bool(false)),
  strategy(nullptr) {}

Player::Player(const Player& other) :
  name(new std::string(*other.name)),
  territories(new std::vector(*other.territories)),
  hand(new Hand(*other.hand)),
  ordersList(new OrdersList(*other.ordersList)),
  reinforcementPool(new int(*other.reinforcementPool)),
  conqueredThisTurn(new bool(*other.conqueredThisTurn)),
  deck(other.deck),
  pendingReinforcements(new int(*other.pendingReinforcements)),
  canIssueOrder(new bool(*other.canIssueOrder)),
  gotAttackedThisTurn(new bool(*other.gotAttackedThisTurn)),
  strategy(nullptr) {}

Player& Player::operator=(const Player& other) {
  if (this != &other) {
    delete name;
    delete territories;
    delete hand;
    delete ordersList;
    delete reinforcementPool;
    delete conqueredThisTurn;
    delete pendingReinforcements;
    delete canIssueOrder;
    delete gotAttackedThisTurn;
    delete strategy;

    // deep copy primitives and container
    name = new std::string(*other.name);
    territories = new std::vector(*other.territories);
    hand = new Hand(*other.hand);
    ordersList = new OrdersList(*other.ordersList);
    reinforcementPool = new int(*other.reinforcementPool);
    conqueredThisTurn = new bool(*other.conqueredThisTurn);
    pendingReinforcements = new int(*other.pendingReinforcements);
    canIssueOrder = new bool(*other.canIssueOrder);
    gotAttackedThisTurn = new bool(*other.gotAttackedThisTurn);

    // shallow copy shared references
    deck = other.deck;
    // do not copy strategy
    strategy = nullptr;
  }
  return *this;
}

Player::~Player() {
  delete name;
  delete territories;
  delete hand;
  delete ordersList;
  delete reinforcementPool;
  delete conqueredThisTurn;
  delete pendingReinforcements;
  delete canIssueOrder;
  delete gotAttackedThisTurn;
  delete strategy;
}

// --- GETTERS ---
std::string Player::getName() const {
  return *name;
}

const std::vector<Territory*>& Player::getTerritories() const {
  return *territories;
}

Hand* Player::getHand() const {
  return hand;
}

OrdersList* Player::getOrders() const {
  return ordersList;
}

int Player::getReinforcementPool() const {
  return *reinforcementPool;
}

bool Player::getConqueredThisTurn() const {
  return *conqueredThisTurn;
}

Deck* Player::getDeck() const {
  return deck;
}

int Player::getPendingReinforcements() const {
  return *pendingReinforcements;
}

bool* Player::getCanIssueOrder() const {
  return canIssueOrder;
}

bool* Player::getGotAttackedThisTurn() const {
  return gotAttackedThisTurn;
}

PlayerStrategy* Player::getPlayerStrategy() const {
  return strategy;
}

// --- SETTERS ---
void Player::setName(const std::string& newName) {
  *name = newName;
}

void Player::setReinforcementPool(int armies) {
  *reinforcementPool = armies;
}

void Player::setConqueredThisTurn(bool conquered) {
  *conqueredThisTurn = conquered;
}

void Player::setPendingReinforcements(int armies) {
  *pendingReinforcements = armies;
}

void Player::setCanIssueOrder(bool value) {
  *canIssueOrder = value;
}

void Player::setGotAttackedThisTurn(bool attacked) {
  *gotAttackedThisTurn = attacked;
}

void Player::setStrategy(PlayerStrategy* newStrategy) {
  strategy = newStrategy;
}

// --- TERRITORY MANAGEMENT ---
void Player::addTerritory(Territory* terr) {
  if (terr && !ownsTerritory(terr)) {
    territories->push_back(terr);
    terr->setOwner(this);
  }
}

void Player::removeTerritory(Territory* terr) {
  if (terr) terr->setOwner(nullptr);
  std::erase(*territories, terr);
}

bool Player::ownsTerritory(Territory* terr) const {
  if (!terr) return false;
  return std::ranges::any_of(*territories, [terr](const Territory* t) {
    return t->getName() == terr->getName() && t->getId() == terr->getId();
  });
}

// --- CARD MANAGEMENT ---
void Player::addCard(Card* card) {
  if (card) hand->addCard(card);
}

void Player::playCard(int index, Deck* gameDeck) {
  if (hand && gameDeck) hand->playCard(index, this, ordersList, gameDeck);
}

// --- ORDER MANAGEMENT ---
void Player::issueOrder() {
  if (strategy) this->strategy->issueOrder();
}

void Player::issueOrder(bool deployPhase, bool& advanceIssued, Deck* gameDeck) {
  if (deployPhase) {
    const std::vector<Territory*> defendList = toDefend();
    const int reinforcePool = getReinforcementPool();

    if (reinforcePool > 0 && !defendList.empty()) {
      std::cout << "\nPlayer " << getName() << " - Reinforcement Pool: " << reinforcePool << " armies\n";
      std::cout << "Territories to defend:\n";
      for (size_t i = 0; i < defendList.size(); i++) {
        std::cout << i << ": " << defendList[i]->getName()
                  << " (Current armies: " << defendList[i]->getArmies() << ")\n";
      }

      int choice = -1;
      while (choice < 0 || choice >= defendList.size()) {
        std::cout << "Choose territory to deploy to (index): ";
        std::cin >> choice;
      }

      int armies = -1;
      while (armies <= 0 || armies > reinforcePool) {
        std::cout << "Enter number of armies to deploy (max " << reinforcePool << "): ";
        std::cin >> armies;
      }

      this->issueDeployOrder(defendList[choice], armies);
      *reinforcementPool -= armies;
      std::cout << armies << " armies deployed to " << defendList[choice]->getName() << ".\n";
    }
  } else {
    // advance phase
    if (advanceIssued) return; // only one advance order per player

    std::vector<Territory*> attackList = toAttack();
    std::vector<Territory*> defendList = toDefend();

    bool canAdvance = false;
    for (const Territory* t : *territories) {
      if (t->getArmies() > 1) {
        canAdvance = true;
        break;
      }
    }

    if (!canAdvance) {
      if (!hand->empty()) {
        std::cout << "Player " << getName() << "'s cards:\n";
        hand->displayInfo();

        int cardIndex = -2;
        while (cardIndex < -1 || cardIndex >= hand->size()) {
          std::cout << "Choose a card to play: (-1 for no cards) ";
          std::cin >> cardIndex;
        }

        if (cardIndex == -1) return;
        playCard(cardIndex, gameDeck);
      }
      return;
    };

    std::cout << "\nPlayer " << getName() << " - Advance Phase\n";
    std::cout << "Your territories:\n";
    for (size_t i = 0; i < territories->size(); i++) {
      std::cout << i << ": " << territories->at(i)->getName()
                << " (Armies: " << territories->at(i)->getArmies() << ")\n";
    }

    std::vector<Territory*> territoriesWithEnemies;
    for (Territory* t : *territories) {
      if (!t) continue;

      const std::vector<Territory*>& adj = t->getAdjTerritories();
      for (Territory* a : adj) {
        if (a && !ownsTerritory(a)) { // if adjacent to enemy
          territoriesWithEnemies.push_back(t);
          break; // only need one enemy to qualify
        }
      }
    }

    // display enemy territories
    if (territoriesWithEnemies.empty()) {
      std::cout << "No territories adjacent to enemies.\n";
    } else {
      std::cout << "Your territories adjacent to enemies:\n";
      for (size_t i = 0; i < territoriesWithEnemies.size(); ++i) {
        std::cout << i << ": " << territoriesWithEnemies[i]->getName()
                  << " (Armies: " << territoriesWithEnemies[i]->getArmies() << ")\n";
      }
    }

    Territory *source = nullptr, *target = nullptr;
    while (true) { // loop until a valid source + target is chosen
      // pick source territory
      int sourceIndex = -1;
      while (sourceIndex < 0 || sourceIndex >= territories->size() || territories->at(sourceIndex)->getArmies() <= 1) {
        std::cout << "Choose source territory (index, must have >1 army): ";
        std::cin >> sourceIndex;
      }
      source = territories->at(sourceIndex);

      // choose target type
      int targetType = -1;
      while (targetType != 0 && targetType != 1) {
        std::cout << "Target type: 0 = defend (own), 1 = attack (enemy): ";
        std::cin >> targetType;
      }

      // filter adjacent territories based on target type
      std::vector<Territory*> validTargets;
      const std::vector<Territory*>& adjTerritories = source->getAdjTerritories();

      for (Territory* adj : adjTerritories) {
        if (!adj) continue;

        if (targetType == 1 && !ownsTerritory(adj)) { // attack
          validTargets.push_back(adj);
        } else if (targetType == 0 && ownsTerritory(adj)) { // defend/reinforce
          validTargets.push_back(adj);
        }
      }

      // if no valid targets, notify and restart loop
      if (validTargets.empty()) {
        std::cout << "No valid adjacent territories for this source. Choose another source.\n";
        continue; // back to source selection
      }

      // display valid targets
      for (size_t i = 0; i < validTargets.size(); ++i) {
        std::cout << i << ": " << validTargets[i]->getName()
                  << " (Armies: " << validTargets[i]->getArmies() << ")\n";
      }

      // pick target territory
      int targetIndex = -1;
      while (targetIndex < 0 || targetIndex >= validTargets.size()) {
        std::cout << "Choose target territory (index): -1 to reselect source: ";
        std::cin >> targetIndex;

        if (targetIndex == -1) break; // reselect source
      }
      if (targetIndex == -1) continue; // reselect source
      target = validTargets[targetIndex];

      // choose number of armies to move
      int armies = -1;
      while (armies <= 0 || armies > source->getArmies() - 1) {
        std::cout << "Enter number of armies to move (max " << source->getArmies() - 1 << "): ";
        std::cin >> armies;
      }

      // issue order and exit loop
      issueAdvanceOrder(source, target, armies);
      std::cout << "Advance order issued from " << source->getName() << " to " << target->getName()
                << " with " << armies << " armies.\n";

      advanceIssued = true;
      std::cout << "Advance order issued.\n";
      break; // exit the while(true) loop
    }

    if (!hand->empty()) {
      std::cout << "Your cards:\n";
      hand->displayInfo();

      int cardIndex = -2;
      while (cardIndex < -1 || cardIndex >= hand->size()) {
        std::cout << "Choose a card to play: (-1 for no cards) ";
        std::cin >> cardIndex;
      }

      if(cardIndex == -1) return;
      playCard(cardIndex, gameDeck);
    }
  }
}

void Player::issueDeployOrder(Territory* target, int armies) {
  ordersList->addOrder(new OrderDeploy(this, target, new int(armies)));
}

void Player::issueAdvanceOrder(Territory* source, Territory* target, int armies) {
  ordersList->addOrder(new OrderAdvance(this, source, target, new int(armies)));
}

void Player::issueAirliftOrder(Territory* source, Territory* target, int armies) {
  ordersList->addOrder(new OrderAirlift(this, source, target, new int(armies)));
}

void Player::issueBombOrder(Territory* target) {
  ordersList->addOrder(new OrderBomb(this, target));
}

void Player::issueBlockadeOrder(Player* player, Territory* target) {
  ordersList->addOrder(new OrderBlockade(player, this, target));
}

void Player::issueNegotiateOrder(Player* player) {
  ordersList->addOrder(new OrderNegotiate(player, this));
}

void Player::issueCheatOrder() {
  ordersList->addOrder(new OrderCheat(this));
}

// --- STRATEGY MANAGEMENT ---
std::vector<Territory*> Player::toDefend() const {
  if (strategy) return this->strategy->toDefend();
  return std::vector<Territory*>();
}

std::vector<Territory*> Player::toAttack() const {
  if (strategy) return this->strategy->toAttack();
  return std::vector<Territory*>();
}

// --- PLAYER MANAGEMENT ---
Player* Player::choosePlayer(const std::vector<Player*>& players) {
  if (players.empty()) return nullptr;

  std::cout << "Available players:\n";
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

// --- UTILITY ---
int Player::getTotalArmies() const {
  return std::accumulate(territories->begin(), territories->end(), 0,
    [](int currSum, const Territory* t) {
      return currSum + t->getArmies();
    });
}

int Player::getTerritoryCount() const {
  return territories->size();
}

void Player::displayInfo() const {
  std::cout << "Player: " << (name ? *name : "Unnamed") << std::endl;
  std::cout << "  Territories: " << (territories ? territories->size() : 0)  << std::endl;
  std::cout << "  Total Armies: " << getTotalArmies() << std::endl;
  std::cout << "  Reinforcement Pool: " << (reinforcementPool ? *reinforcementPool : 0) << std::endl;
  std::cout << "  Cards in Hand: " << (hand ? hand->size() : 0) << std::endl;
  std::cout << "  Orders: " << (ordersList->orders ? ordersList->orders->size() : 0) << std::endl;

  const int numOfTerritories = territories ? territories->size() : 0;
  std::cout << "  Owned Territories (" << numOfTerritories << "): ";
  if (numOfTerritories == 0) {
    std::cout << "None";
  } else {
    for (const Territory* terr : *territories) {
      if (terr) std::cout << "    - " << terr->getName() << " (Armies: " << terr->getArmies() << ")" << std::endl;
    }
  }
  std::cout << std::endl;
}

bool Player::operator==(const Player& other) const {
  return name == other.name && territories == other.territories;
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Player& play) {
  os << "Player[" << (play.name ? *play.name : "Unnamed")
     << ", Territories: " << (play.territories ? play.territories->size() : 0)
     << ", Armies: " << play.getTotalArmies()
     << ", Reinforcement Pool: " << (play.reinforcementPool ? *play.reinforcementPool : 0) << "]";
  return os;
}
