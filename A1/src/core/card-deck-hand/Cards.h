//
// Part 4: Cards deck/hand
//Cards.h
//

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Forward declarations (real classes implemented in Part 2 and 3)
class Player;
class OrdersList;

class Card {
private:
    std::string* type;  // pointer type (assignment rule)

public:
    Card(const std::string& t);
    Card(const Card& other); // copy constructor
    Card& operator=(const Card& other); // assignment operator
    ~Card();

    std::string getType() const;
    void play(Player* player, OrdersList* ordersList); // creates order, returns card to deck

    friend std::ostream& operator<<(std::ostream& out, const Card& c);
};

class Deck {
private:
    std::vector<Card*>* cards;  // collection of cards

public:
    Deck();
    Deck(const Deck& other);
    Deck& operator=(const Deck& other);
    ~Deck();

    Card* draw();                  // remove random card, return it
    void returnCard(Card* card);   // put card back into deck
    void printDeck() const;

    // ADD: Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Deck& deck);
};

class Hand {
private:
    std::vector<Card*>* handCards;  // player's current cards

public:
    Hand();
    Hand(const Hand& other);
    Hand& operator=(const Hand& other);
    ~Hand();

    void addCard(Card* card);
    //deck parameter to properly return cards
    void playAll(Player* player, OrdersList* ordersList, Deck* deck);
    void printHand() const;

    // ADD: Stream insertion operator
    friend std::ostream& operator<<(std::ostream& out, const Hand& hand);
};
