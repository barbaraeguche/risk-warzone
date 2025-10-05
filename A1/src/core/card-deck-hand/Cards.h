#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

class Player;
class OrdersList;
class Deck; // Forward declaration

/**
 * Abstract base class for all card types.
 */
class Card {
public:
    // Copy constructor
    Card(const Card& other);

    // Assignment operator
    Card& operator=(const Card& other);

    // Destructor
    virtual ~Card();

    // Returns card type
    virtual std::string getType() const = 0;

    // Play the card - creates order and returns card to deck
    virtual void play(Player* player, OrdersList* ordersList, Deck* deck) = 0;

    // Output operator
    friend std::ostream& operator<<(std::ostream& out, const Card& c);

protected:
    // Protected constructor since this is abstract
    Card();
};

// Derived Card Types
class BombCard : public Card {
public:
    BombCard();
    BombCard(const BombCard& other);
    BombCard& operator=(const BombCard& other);
    ~BombCard() override;

    std::string getType() const override;
    void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};

class ReinforcementCard : public Card {
public:
    ReinforcementCard();
    ReinforcementCard(const ReinforcementCard& other);
    ReinforcementCard& operator=(const ReinforcementCard& other);
    ~ReinforcementCard() override;

    std::string getType() const override;
    void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};

class BlockadeCard : public Card {
public:
    BlockadeCard();
    BlockadeCard(const BlockadeCard& other);
    BlockadeCard& operator=(const BlockadeCard& other);
    ~BlockadeCard() override;

    std::string getType() const override;
    void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};

class AirliftCard : public Card {
public:
    AirliftCard();
    AirliftCard(const AirliftCard& other);
    AirliftCard& operator=(const AirliftCard& other);
    ~AirliftCard() override;

    std::string getType() const override;
    void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};

class DiplomacyCard : public Card {
public:
    DiplomacyCard();
    DiplomacyCard(const DiplomacyCard& other);
    DiplomacyCard& operator=(const DiplomacyCard& other);
    ~DiplomacyCard() override;

    std::string getType() const override;
    void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};

/**
 * Represents the deck of cards.
 */
Card* cloneCard(const Card* card);

class Deck {
private:
    std::vector<Card*>* cards;
    void clearCards();
    void copyCardsFrom(const std::vector<Card*>* sourceCards);

public:
    Deck();
    Deck(const Deck& other);
    Deck& operator=(const Deck& other);
    ~Deck();

    Card* draw();
    void returnCard(Card* card);
    void printDeck() const;

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Deck& deck);
};

/**
 * Represents a player's hand of cards.
 */
class Hand {
private:
    std::vector<Card*>* handCards;
    void clearCards();
    void copyCardsFrom(const std::vector<Card*>* sourceCards);

public:
    Hand();
    Hand(const Hand& other);
    Hand& operator=(const Hand& other);
    ~Hand();

    void addCard(Card* card);
    void playCard(int index, Player* player, OrdersList* ordersList, Deck* deck);
    void playAll(Player* player, OrdersList* ordersList, Deck* deck);
    void printHand() const;
    int size() const;

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Hand& hand);
};
