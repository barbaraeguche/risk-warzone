#include "Cards.h"
#include "GameEngine.h"
#include "Map.h"
#include "Orders.h"
#include "Player.h"

#include <iostream>
#include <random>
#include <vector>


// ==================== Card Class Implementation ====================
Card::Card() {}

Card::Card(const Card& other) {}

Card& Card::operator=(const Card& other) {
  return *this;
}

Card::~Card() {}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Card& crd) {
  os << "Card[" << crd.getType() << "]";
  return os;
}


// ==================== BombCard Class Implementation ====================
BombCard::BombCard() {}

BombCard::BombCard(const BombCard& other) {}

BombCard& BombCard::operator=(const BombCard& other) {
  return *this;
}

BombCard::~BombCard() {}

// --- GETTERS ---
std::string BombCard::getType() const {
  return CARD_TYPES::BOMB;
}

// --- UTILITY ---
void BombCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
  if (!player || !ordersList) return;

  std::cout << player->getName() << " plays Bomb Card -> create Bomb Order\n";
  std::vector<Territory*> attackable;

  // choose target: must be an adjacent enemy territory
  for (Territory* terr : player->getTerritories()) {
    for (Territory* adj : terr->getAdjTerritories()) {
      if (!player->ownsTerritory(adj)) {
        attackable.push_back(adj);
      }
    }
  }

  Territory* target = chooseTerritory(attackable);
  if (target) ordersList->add(new OrderBomb(target, player));
  if (deck) deck->addCard(new BombCard());
}


// ==================== ReinforcementCard Class Implementation ====================
ReinforcementCard::ReinforcementCard() {}

ReinforcementCard::ReinforcementCard(const ReinforcementCard& other) {}

ReinforcementCard& ReinforcementCard::operator=(const ReinforcementCard& other) {
  return *this;
}

ReinforcementCard::~ReinforcementCard() {}

// --- GETTERS ---
std::string ReinforcementCard::getType() const {
  return CARD_TYPES::REINFORCEMENT;
}

// --- UTILITY ---
void ReinforcementCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
  if (!player) return;

  std::cout << player->getName() << " plays Reinforcement Card -> adds 5 reinforcements for next phase.\n";
  player->setPendingReinforcements(5); // could be any number

  if (deck) deck->addCard(new ReinforcementCard());
}


// ==================== BlockadeCard Class Implementation ====================
BlockadeCard::BlockadeCard() {}

BlockadeCard::BlockadeCard(const BlockadeCard& other) {}

BlockadeCard& BlockadeCard::operator=(const BlockadeCard& other) {
  return *this;
}

BlockadeCard::~BlockadeCard() {}

// --- GETTERS ---
std::string BlockadeCard::getType() const {
  return CARD_TYPES::BLOCKADE;
}

// --- UTILITY ---
void BlockadeCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
  if (!player || !ordersList) return;

  std::cout << player->getName() << " plays Blockade Card -> create Blockade Order\n";
  Territory* target = chooseTerritory(player->toDefend());

  if (target) {
    Player* neutralPlayer = GameEngine::getNeutralPlayer();
    ordersList->add(new OrderBlockade(neutralPlayer, player, target));
  }

  if (deck) deck->addCard(new BlockadeCard());
}


// ==================== AirliftCard Class Implementation ====================
AirliftCard::AirliftCard() {}

AirliftCard::AirliftCard(const AirliftCard& other) {}

AirliftCard& AirliftCard::operator=(const AirliftCard& other) {
  return *this;
}

AirliftCard::~AirliftCard() {}

// --- GETTERS ---
std::string AirliftCard::getType() const {
  return CARD_TYPES::AIRLIFT;
}

// --- UTILITY ---
void AirliftCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
  if (!player || !ordersList) return;

  std::cout << player->getName() << " plays Airlift Card -> create Airlift Order\n";
  std::cout << "Choose source and target territories for airlift:\n";

  Territory* source = chooseTerritory(player->toDefend());
  Territory* target = chooseTerritory(player->toDefend());

  if (source && target && source != target) {
    const int armies = source->getArmies() / 2;
    if (armies > 0) ordersList->add(new OrderAirlift(player, source, target, new int(armies)));
  }

  if (deck) deck->addCard(new AirliftCard());
}


// ==================== DiplomacyCard Class Implementation ====================
DiplomacyCard::DiplomacyCard() {}

DiplomacyCard::DiplomacyCard(const DiplomacyCard& other) {}

DiplomacyCard& DiplomacyCard::operator=(const DiplomacyCard& other) {
  return *this;
}

DiplomacyCard::~DiplomacyCard() {}

// --- GETTERS ---
std::string DiplomacyCard::getType() const {
  return CARD_TYPES::DIPLOMACY;
}

// --- UTILITY ---
void DiplomacyCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
  if (!player || !ordersList) return;
  std::cout << player->getName() << " plays Diplomacy Card -> create Negotiate Order\n";

  // choose another player
  std::vector<Player*> otherPlayers;
  GameEngine engine;
  for (Player* p : engine.getPlayers()) {
    if (p != player) otherPlayers.push_back(p);
  }

  Player* targetPlayer = player->choosePlayer(otherPlayers);
  if (targetPlayer) ordersList->add(new OrderNegotiate(player, targetPlayer));
  if (deck) deck->addCard(new DiplomacyCard());
}


// ==================== Deck Class Implementation ====================
Deck::Deck() :
  deckCards(new std::vector<Card*>()) {
  // create multiple cards of each type
  for (int i = 0; i < 3; i++) {
    deckCards->push_back(new BombCard());
    deckCards->push_back(new ReinforcementCard());
    deckCards->push_back(new BlockadeCard());
    deckCards->push_back(new AirliftCard());
    deckCards->push_back(new DiplomacyCard());
  }
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Deck::Deck(const Deck& other) {
  copyCardsFrom(other.deckCards);
}

Deck& Deck::operator=(const Deck& other) {
  if (this != &other) {
    clearCards();
    copyCardsFrom(other.deckCards);
  }
  return *this;
}

Deck::~Deck() {
  clearCards();
}

// --- CARD MANAGEMENT ---
void Deck::addCard(Card* card) {
  if (card) deckCards->push_back(card);
}

// --- UTILITY ---
Card* Deck::draw() {
  if (deckCards->empty()) return nullptr;

  const int idx = rand() % deckCards->size();
  Card* drawn = deckCards->at(idx);

  deckCards->erase(deckCards->begin() + idx);
  return drawn;
}

void Deck::displayInfo() const {
  std::cout << "Deck: (" << deckCards->size() << " cards)" << std::endl;
  for (auto* crd : *deckCards) {
    std::cout << "  - " << *crd << std::endl;
  }
}

void Deck::clearCards() {
  for (auto* crd : *deckCards) { delete crd; }
  delete deckCards;
}

void Deck::copyCardsFrom(const std::vector<Card*>* sourceCards) {
  deckCards = new std::vector<Card*>();
  for (auto* crd : *sourceCards) {
    deckCards->push_back(cloneCard(crd));
  }
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Deck& dec) {
  os << "Deck[" << dec.deckCards->size() << " cards]";
  return os;
}


// ==================== Hand Class Implementation ====================
Hand::Hand() :
 handCards(new std::vector<Card*>()) {}

Hand::Hand(const Hand& other) {
  copyCardsFrom(other.handCards);
}

Hand& Hand::operator=(const Hand& other) {
  if (this != &other) {
    clearCards();
    copyCardsFrom(other.handCards);
  }
  return *this;
}

Hand::~Hand() {
  clearCards();
}

// --- GETTERS ---
const std::vector<Card*>* Hand::getCards() const {
  return handCards;
}

// --- CARD MANAGEMENT ---
void Hand::addCard(Card* card) {
  if (card) handCards->push_back(card);
}

void Hand::playCard(int index, Player* player, OrdersList* ordersList, Deck* deck) {
  if (index >= 0 && index < handCards->size()) {
    Card* card = handCards->at(index);
    card->play(player, ordersList, deck);

    // remove from hand (card is returned to deck in play() method)
    handCards->erase(handCards->begin() + index);
    delete card; // delete original card instance
  }
}

void Hand::playAll(Player* player, OrdersList* ordersList, Deck* deck) {
  std::cout << "Playing all cards in hand..." << std::endl;
  while (!handCards->empty()) {
    playCard(0, player, ordersList, deck);
  }
  std::cout << "All cards have been played and returned to deck." << std::endl;
}

// --- UTILITY ---
int Hand::size() const {
  return handCards->size();
}

void Hand::displayInfo() const {
  std::cout << "Hand: (" << handCards->size() << " cards)" << std::endl;
  for (auto* crd : *handCards) {
    std::cout << "  - " << *crd << std::endl;
  }
}

bool Hand::empty() const {
  return !handCards || handCards->empty();
}

void Hand::clearCards() {
  for (auto* crd : *handCards) { delete crd; }
  delete handCards;
}

void Hand::copyCardsFrom(const std::vector<Card*>* sourceCards) {
  handCards = new std::vector<Card*>();
  for (auto* c : *sourceCards) {
    handCards->push_back(cloneCard(c));
  }
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Hand& hnd) {
  os << "Hand[" << hnd.handCards->size() << " cards]";
  return os;
}


// ==================== Class Helpers Implementation ====================
Card* cloneCard(const Card* card) {
  if (!card) return nullptr;
  const std::string type = card->getType();

  if (type == CARD_TYPES::BOMB) return new BombCard();
  if (type == CARD_TYPES::REINFORCEMENT) return new ReinforcementCard();
  if (type == CARD_TYPES::BLOCKADE) return new BlockadeCard();
  if (type == CARD_TYPES::AIRLIFT) return new AirliftCard();
  if (type == CARD_TYPES::DIPLOMACY) return new DiplomacyCard();
  return nullptr;
}


// ==================== Class Helpers Implementation ====================
Territory* chooseTerritory(const std::vector<Territory*>& territories) {
  if (territories.empty()) return nullptr;

  // in automatic/tournament mode, automatically select a territory
  if (GameEngine::isAutomaticMode()) {
    // randomly select a territory for variety
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, territories.size() - 1);

    const size_t choice = dist(rng);
    std::cout << "Auto-selected territory: " << territories[choice]->getName() << "\n";
    return territories[choice];
  }

  // manual mode: prompt user for input
  std::cout << "Choose a territory:\n";
  for (size_t i = 0; i < territories.size(); i++) {
    std::cout << i << ": " << territories[i]->getName() << " (Armies: " << territories[i]->getArmies() << ")\n";
  }

  int choice = -1;
  while (choice < 0 || choice >= territories.size()) {
    std::cout << "Enter the number of your choice: ";
    std::cin >> choice;
  }

  return territories[choice];
}
