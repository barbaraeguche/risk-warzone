//
//Part 4: Cards deck/hand
//Cards.cpp
//

#include "Cards.h"

//Card Implementation 
Card::Card(const std::string& t) {
    type = new std::string(t);
}

Card::Card(const Card& other) {
    type = new std::string(*other.type);
}

Card& Card::operator=(const Card& other) {
    if (this != &other) {
        delete type;
        type = new std::string(*other.type);
    }
    return *this;
}

Card::~Card() {
    delete type;
}

std::string Card::getType() const {
    return *type;
}

void Card::play(Player* player, OrdersList* ordersList) {
    // Creates an order based on card type and adds to ordersList
    std::cout << "Playing card: " << *type << " -> Creating corresponding order and adding to OrdersList" << std::endl;

}

std::ostream& operator<<(std::ostream& out, const Card& c) {
    out << "Card(" << *c.type << ")";
    return out;
}

//Deck Implementation
Deck::Deck() {
    cards = new std::vector<Card*>();

    // initialize with some cards of each type
    std::vector<std::string> types = {"bomb", "reinforcement", "blockade", "airlift", "diplomacy"};
    for (int i = 0; i < 3; i++) {  // 3 of each
        for (auto& t : types) {
            cards->push_back(new Card(t));
        }
    }

    std::srand(std::time(nullptr)); // seed RNG
}

Deck::Deck(const Deck& other) {
    cards = new std::vector<Card*>();
    for (auto* c : *other.cards) {
        cards->push_back(new Card(*c));
    }
}

Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        for (auto* c : *cards) delete c;
        delete cards;
        cards = new std::vector<Card*>();
        for (auto* c : *other.cards) {
            cards->push_back(new Card(*c));
        }
    }
    return *this;
}

Deck::~Deck() {
    for (auto* c : *cards) delete c;
    delete cards;
}

Card* Deck::draw() {
    if (cards->empty()) return nullptr;

    int idx = rand() % cards->size();
    Card* drawn = cards->at(idx);
    cards->erase(cards->begin() + idx);
    return drawn;
}

void Deck::returnCard(Card* card) {
    if (card) {
        cards->push_back(card);
    }
}

void Deck::printDeck() const {
    std::cout << "Deck contains: " << cards->size() << " cards" << std::endl;
    for (auto* c : *cards) {
        std::cout << " - " << *c << std::endl;
    }
}

//Stream insertion operator for Deck
std::ostream& operator<<(std::ostream& out, const Deck& deck) {
    out << "Deck[" << deck.cards->size() << " cards]";
    return out;
}

//Hand Implementation
Hand::Hand() {
    handCards = new std::vector<Card*>();
}

Hand::Hand(const Hand& other) {
    handCards = new std::vector<Card*>();
    for (auto* c : *other.handCards) {
        handCards->push_back(new Card(*c));
    }
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        for (auto* c : *handCards) delete c;
        delete handCards;
        handCards = new std::vector<Card*>();
        for (auto* c : *other.handCards) {
            handCards->push_back(new Card(*c));
        }
    }
    return *this;
}

Hand::~Hand() {
    for (auto* c : *handCards) delete c;
    delete handCards;
}

void Hand::addCard(Card* card) {
    if (card) {
        handCards->push_back(card);
    }
}

//Now properly returns cards to deck
void Hand::playAll(Player* player, OrdersList* ordersList, Deck* deck) {
    for (auto* c : *handCards) {
        c->play(player, ordersList);
        //Return card to deck instead of deleting
        if (deck) {
            deck->returnCard(c);
        }
    }

    std::cout << "All cards have been played and returned to deck." << std::endl;
    // Clear the hand without deleting cards (they're now back in deck)
    handCards->clear();
}

void Hand::printHand() const {
    std::cout << "Hand contains: " << handCards->size() << " cards" << std::endl;
    for (auto* c : *handCards) {
        std::cout << " - " << *c << std::endl;
    }
}

//Stream insertion operator for Hand
std::ostream& operator<<(std::ostream& out, const Hand& hand) {
    out << "Hand[" << hand.handCards->size() << " cards]";
    return out;
}
