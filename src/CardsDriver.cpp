#include "Cards.h"
#include "Orders.h"
#include "Player.h"

void testCards() {
  std::cout << "\n=== Testing Cards Deck/Hand ===" << std::endl;
  Deck deck, tempDeck;
  Hand hand;

  // create a deck of cards of all different kinds
  std::cout << "\n1. Creating deck with cards of all types:" << std::endl;
  deck.displayInfo();
  std::cout << "Deck stream operator: " << deck << std::endl;

  // create a hand object that is filled by drawing cards from the deck
  std::cout << "\n2. Creating hand and drawing cards from deck:" << std::endl;
  std::cout << "Initial hand: " << hand << std::endl;

  // draw 5 cards from deck by repeatedly calling draw() method
  std::cout << "\nDrawing 5 cards..." << std::endl;
  for (int i = 0; i < 5; i++) {
    Card* drawnCard = deck.draw();
    if (drawnCard) {
      hand.addCard(drawnCard);
      std::cout << " Drew: " << *drawnCard << std::endl;
    }
  }

  std::cout << "\nHand after drawing:" << std::endl;
  hand.displayInfo();
  std::cout << "Hand stream operator: " << hand << std::endl;
  std::cout << "\nDeck after drawing:" << std::endl;
  deck.displayInfo();

  // call play() method of all cards in hand
  std::cout << "\n3. Playing all cards in hand (creates orders and returns cards to deck):" << std::endl;
  Player* dummyPlayer = new Player();
  OrdersList* dummyOrders = new OrdersList();

  // store initial deck size to verify cards are returned
  int initialDeckSize = 0;
  while (tempDeck.draw() != nullptr) initialDeckSize++;

  (void) initialDeckSize;
  hand.playAll(dummyPlayer, dummyOrders, &deck);

  // show that cards are back in the deck after playing
  std::cout << "\n4. Verifying cards returned to deck:" << std::endl;
  std::cout << "Hand after playing all cards:" << std::endl;
  hand.displayInfo();
  std::cout << "\nDeck after playing cards (should have more cards):" << std::endl;
  deck.displayInfo();

  // test individual card playing
  std::cout << "\n5. Testing individual card play:" << std::endl;
  Card* testCard = deck.draw();
  if (testCard) {
    hand.addCard(testCard);
    std::cout << "Added card to hand: " << *testCard << std::endl;
    hand.displayInfo();

    // play the single card
    hand.playCard(0, dummyPlayer, dummyOrders, &deck);
    std::cout << "After playing single card:" << std::endl;
    hand.displayInfo();
  }

  // test copy constructor and assignment operator
  std::cout << "\n6. Testing copy constructor and assignment operator:" << std::endl;
  // add cards to hand before testing copy
  std::cout << "Drawing 3 cards for copy testing..." << std::endl;
  for (int i = 0; i < 3; i++) {
    Card* card = deck.draw();
    if (card) hand.addCard(card);
  }

  std::cout << "Original hand before copying: " << hand << std::endl;
  hand.displayInfo();

  Hand hand2(hand);
  std::cout << "Copied hand: " << hand2 << std::endl;
  hand2.displayInfo();

  Hand hand3 = hand;
  std::cout << "Assigned hand: " << hand3 << std::endl;
  hand3.displayInfo();

  Deck deck2(deck);
  std::cout << "Copied deck: " << deck2 << std::endl;
  Deck deck3 = deck;
  std::cout << "Assigned deck: " << deck3 << std::endl;

  // cleanup
  delete dummyPlayer;
  delete dummyOrders;

  std::cout << "\nCards testing completed " << std::endl;
}
