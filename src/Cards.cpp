#include "Cards.h"
#include "Player.h"
#include "Map.h"
#include "Orders.h"
#include "GameEngine.h"
#include <iostream>
#include <vector>

// Base Card
Card::Card() {}
Card::Card(const Card& other) {}
Card& Card::operator=(const Card& other) { return *this; }
Card::~Card() {}

std::ostream& operator<<(std::ostream& out, const Card& c) {
    out << "Card(" << c.getType() << ")";
    return out;
}

// BombCard
BombCard::BombCard() {}
BombCard::BombCard(const BombCard& other) {}
BombCard& BombCard::operator=(const BombCard& other) { return *this; }
BombCard::~BombCard() {}

std::string BombCard::getType() const { return CARD_TYPES::BOMB; }
void BombCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
    if (!player || !ordersList) return;

    std::cout << player->getName() << " plays Bomb Card -> create Bomb Order\n";

    // Choose target: must be an adjacent enemy territory
    std::vector<Territory*> attackable;
    for (Territory* t : player->getTerritories()) {
        for (Territory* adj : t->getAdjTerritories()) {
            if (!player->ownsTerritory(adj)) {
                attackable.push_back(adj);
            }
        }
    }

    Territory* target = chooseTerritory(attackable);
    if (target) {
        ordersList->add(new OrderBomb(target, player));
    }

    if (deck) deck->returnCard(new BombCard());
}

// ReinforcementCard
ReinforcementCard::ReinforcementCard() {}
ReinforcementCard::ReinforcementCard(const ReinforcementCard& other) {}
ReinforcementCard& ReinforcementCard::operator=(const ReinforcementCard& other) { return *this; }
ReinforcementCard::~ReinforcementCard() {}

std::string ReinforcementCard::getType() const { return CARD_TYPES::REINFORCEMENT; }
void ReinforcementCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
    if (!player) return;

    std::cout << player->getName() << " plays Reinforcement Card -> adds 5 reinforcements for next phase.\n";

    // Simply increase player's reinforcement pool for next reinforcement phase
    int bonus = 5; // example: could be any number
    player->setPendingReinforcements(bonus);

    if (deck) {
        deck->returnCard(new ReinforcementCard());
    }
}

// BlockadeCard
BlockadeCard::BlockadeCard() {}
BlockadeCard::BlockadeCard(const BlockadeCard& other) {}
BlockadeCard& BlockadeCard::operator=(const BlockadeCard& other) { return *this; }
BlockadeCard::~BlockadeCard() {}

std::string BlockadeCard::getType() const { return CARD_TYPES::BLOCKADE; }
void BlockadeCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
    if (!player || !ordersList) return;

    std::cout << player->getName() << " plays Blockade Card -> create Blockade Order\n";

    Territory* target = chooseTerritory(player->toDefend());
    if (target) {
        Player* neutralPlayer = GameEngine::getNeutralPlayer();
        ordersList->add(new OrderBlockade(neutralPlayer, player, target));
    }

    if (deck) deck->returnCard(new BlockadeCard());
}

// AirliftCard
AirliftCard::AirliftCard() {}
AirliftCard::AirliftCard(const AirliftCard& other) {}
AirliftCard& AirliftCard::operator=(const AirliftCard& other) { return *this; }
AirliftCard::~AirliftCard() {}

std::string AirliftCard::getType() const { return CARD_TYPES::AIRLIFT; }
void AirliftCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
    if (!player || !ordersList) return;

    std::cout << player->getName() << " plays Airlift Card -> create Airlift Order\n";

    std::cout << "Choose source and target territories for airlift:\n";
    Territory* source = chooseTerritory(player->toDefend());
    Territory* target = chooseTerritory(player->toDefend());

    if (source && target && source != target) {
        int armies = source->getArmies() / 2;
        if (armies > 0) {
            ordersList->add(new OrderAirlift(player, source, target, new int(armies)));
        }
    }

    if (deck) deck->returnCard(new AirliftCard());
}

// DiplomacyCard
DiplomacyCard::DiplomacyCard() {}
DiplomacyCard::DiplomacyCard(const DiplomacyCard& other) {}
DiplomacyCard& DiplomacyCard::operator=(const DiplomacyCard& other) { return *this; }
DiplomacyCard::~DiplomacyCard() {}

std::string DiplomacyCard::getType() const { return CARD_TYPES::DIPLOMACY; }
void DiplomacyCard::play(Player* player, OrdersList* ordersList, Deck* deck) {
    if (!player || !ordersList) return;

    std::cout << player->getName() << " plays Diplomacy Card -> create Negotiate Order\n";

    // Choose another player
    std::vector<Player*> others;
    GameEngine engine; 
    for (Player* p : engine.getPlayers()) {
        if (p != player) others.push_back(p);
    }

    Player* targetPlayer = player->choosePlayer(others);
    if (targetPlayer) {
        ordersList->add(new OrderNegotiate(player, targetPlayer));
    }

    if (deck) deck->returnCard(new DiplomacyCard());
}

Card* cloneCard(const Card* card) {
    if (card->getType() == "bomb") return new BombCard();
    else if (card->getType() == "reinforcement") return new ReinforcementCard();
    else if (card->getType() == "blockade") return new BlockadeCard();
    else if (card->getType() == "airlift") return new AirliftCard();
    else if (card->getType() == "diplomacy") return new DiplomacyCard();
    return nullptr;
}

// Deck
void Deck::clearCards() {
    for (auto* c : *cards) delete c;
    delete cards;
}

void Deck::copyCardsFrom(const std::vector<Card*>* sourceCards) {
    cards = new std::vector<Card*>();
    for (auto* c : *sourceCards) {
        cards->push_back(cloneCard(c));
    }
}

Deck::Deck() {
    cards = new std::vector<Card*>();
    // Create multiple cards of each type
    for (int i = 0; i < 3; i++) {
        cards->push_back(new BombCard());
        cards->push_back(new ReinforcementCard());
        cards->push_back(new BlockadeCard());
        cards->push_back(new AirliftCard());
        cards->push_back(new DiplomacyCard());
    }
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Deck::Deck(const Deck& other) {
    copyCardsFrom(other.cards);
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        clearCards();
        copyCardsFrom(other.cards);
    }
    return *this;
}

Deck::~Deck() {
    clearCards();
}

Card* Deck::draw() {
    if (cards->empty()) return nullptr;
    int idx = rand() % cards->size();
    Card* drawn = cards->at(idx);
    cards->erase(cards->begin() + idx);
    return drawn;
}

void Deck::returnCard(Card* card) {
    if (card) cards->push_back(card);
}

void Deck::printDeck() const {
    std::cout << "Deck contains " << cards->size() << " cards:" << std::endl;
    for (auto* c : *cards) std::cout << " - " << *c << std::endl;
}

std::ostream& operator<<(std::ostream& out, const Deck& deck) {
    out << "Deck with " << deck.cards->size() << " cards";
    return out;
}

// Hand
void Hand::clearCards() {
    for (auto* c : *handCards) delete c;
    delete handCards;
}

void Hand::copyCardsFrom(const std::vector<Card*>* sourceCards) {
    handCards = new std::vector<Card*>();
    for (auto* c : *sourceCards) {
        handCards->push_back(cloneCard(c));
    }
}

Hand::Hand() {
    handCards = new std::vector<Card*>();
}

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

void Hand::addCard(Card* card) {
    if (card) handCards->push_back(card);
}

void Hand::playCard(int index, Player* player, OrdersList* ordersList, Deck* deck) {
    if (index >= 0 && index < handCards->size()) {
        Card* card = handCards->at(index);
        card->play(player, ordersList, deck);
        // Remove from hand (card is returned to deck in play() method)
        handCards->erase(handCards->begin() + index);
        delete card; // Delete the original card instance
    }
}

void Hand::playAll(Player* player, OrdersList* ordersList, Deck* deck) {
    std::cout << "Playing all cards in hand..." << std::endl;
    while (!handCards->empty()) {
        playCard(0, player, ordersList, deck);
    }
    std::cout << "All cards have been played and returned to deck." << std::endl;
}

void Hand::printHand() const {
    std::cout << "Hand contains " << handCards->size() << " cards:" << std::endl;
    for (auto* c : *handCards) std::cout << " - " << *c << std::endl;
}

int Hand::size() const {
    return static_cast<int>(handCards->size());
}

std::ostream& operator<<(std::ostream& out, const Hand& hand) {
    out << "Hand with " << hand.handCards->size() << " cards";
    return out;
}
