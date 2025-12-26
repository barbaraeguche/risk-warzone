#pragma once
#include <iostream>
#include <string>
#include <vector>

class Territory;
class Player;
class OrdersList;
class Deck;

/**
 * Abstract base class for all card types.
 */
class Card {
public:
  Card(const Card& other); // copy constructor
  Card& operator=(const Card& other); // assignment operator
  virtual ~Card(); // destructor

  // getters
  virtual std::string getType() const = 0;

  // utility
  virtual void play(Player* player, OrdersList* ordersList, Deck* deck) = 0;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Card& crd);

protected:
  Card(); // protected constructor since this is abstract
};


class BombCard : public Card {
public:
  BombCard();
  BombCard(const BombCard& other); // copy constructor
  BombCard& operator=(const BombCard& other); // assignment operator
  ~BombCard() override; // destructor

  // getters
  std::string getType() const override;

  // utility
  void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};


class ReinforcementCard : public Card {
public:
  ReinforcementCard();
  ReinforcementCard(const ReinforcementCard& other); // copy constructor
  ReinforcementCard& operator=(const ReinforcementCard& other); // assignment operator
  ~ReinforcementCard() override; // destructor

  // getters
  std::string getType() const override;

  // utility
  void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};


class BlockadeCard : public Card {
public:
  BlockadeCard();
  BlockadeCard(const BlockadeCard& other); // copy constructor
  BlockadeCard& operator=(const BlockadeCard& other); // assignment operator
  ~BlockadeCard() override; // destructor

  // getters
  std::string getType() const override;

  // utility
  void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};


class AirliftCard : public Card {
public:
  AirliftCard();
  AirliftCard(const AirliftCard& other); // copy constructor
  AirliftCard& operator=(const AirliftCard& other); // assignment operator
  ~AirliftCard() override; // destructor

  // getters
  std::string getType() const override;

  // utility
  void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};


class DiplomacyCard : public Card {
public:
  DiplomacyCard();
  DiplomacyCard(const DiplomacyCard& other); // copy constructor
  DiplomacyCard& operator=(const DiplomacyCard& other); // assignment operator
  ~DiplomacyCard() override; // destructor

  // getters
  std::string getType() const override;

  // utility
  void play(Player* player, OrdersList* ordersList, Deck* deck) override;
};


class Deck {
private:
  std::vector<Card*>* deckCards;

public:
  Deck();
  Deck(const Deck& other); // copy constructor
  Deck& operator=(const Deck& other); // assignment operator
  ~Deck(); // destructor

  // card management
  void addCard(Card* card);

  // utility
  Card* draw();
  void displayInfo() const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Deck& dec);

private:
  // utility
  void clearCards();
  void copyCardsFrom(const std::vector<Card*>* sourceCards);
};


/**
 * Represents a player's hand of cards.
 */
class Hand {
private:
  std::vector<Card*>* handCards;

public:
  Hand();
  Hand(const Hand& other); // copy constructor
  Hand& operator=(const Hand& other); // assignment operator
  ~Hand(); // destructor

  // getters
  const std::vector<Card*>* getCards() const;

  // card management
  void addCard(Card* card);
  void playCard(int index, Player* player, OrdersList* ordersList, Deck* deck);
  void playAll(Player* player, OrdersList* ordersList, Deck* deck);

  // utility
  int size() const;
  void displayInfo() const;
  bool empty() const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Hand& hnd);

private:
  // utility
  void clearCards();
  void copyCardsFrom(const std::vector<Card*>* sourceCards);
};

// namespaces
namespace CARD_TYPES {
  constexpr auto BOMB = "bomb";
  constexpr auto REINFORCEMENT = "reinforcement";
  constexpr auto BLOCKADE = "blockade";
  constexpr auto AIRLIFT = "airlift";
  constexpr auto DIPLOMACY = "diplomacy";
}

// helpers
Card* cloneCard(const Card* card);

// utility
Territory* chooseTerritory(const std::vector<Territory*>& territories);