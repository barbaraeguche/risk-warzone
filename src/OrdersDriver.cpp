#include "Cards.h"
#include "Map.h"
#include "Orders.h"

#include <iostream>

void testOrdersLists() {
  // testing order functions
  Order* testOrder = new OrderDeploy(nullptr, nullptr, nullptr);

  // testing validate() and execute()
  std::cout << "Testing validate() and execute() on a Deploy Order:" << '\n';
  std::cout << "Is the order valid? " << (testOrder->validate() ? "YES" : "NO") << "\n\n";
  testOrder->execute(); // should do nothing as validate() returns false
  delete testOrder;

  // testing OrdersList functions
  OrdersList ordersList;
  ordersList.addOrder(new OrderDeploy(nullptr, nullptr, nullptr));
  ordersList.addOrder(new OrderAdvance(nullptr, nullptr, nullptr, nullptr));
  ordersList.addOrder(new OrderBomb(nullptr, nullptr));
  ordersList.addOrder(new OrderBlockade(nullptr, nullptr, nullptr));
  ordersList.addOrder(new OrderAirlift(nullptr, nullptr, nullptr, nullptr));
  ordersList.addOrder(new OrderNegotiate(nullptr, nullptr));

  // testing addOrder()
  std::cout << ordersList << std::endl;

  // testing removeOrder()
  std::cout << "\nRemove 2nd Order:" << '\n';
  ordersList.removeOrder(1);
  std::cout << ordersList << std::endl;

  // testing moveOrder()
  std::cout << "\nMove 1st Order to 4th Index:" << '\n';
  ordersList.moveOrder(0, 3);
  std::cout << ordersList << std::endl;

  std::cout << "\nCloned OrderList:" << '\n';
  OrdersList clonedList = ordersList;
  std::cout << clonedList << std::endl;
}

void testOrderExecution() {
  Deck* deck = new Deck();
  Player* player1 = new Player("Player1", deck);
  Player* player2 = new Player("Player2", deck);
  Player* nPlayer = new Player("Neutral player", deck);

  Territory* territoryA = new Territory("TerritoryA", 1);
  territoryA->setArmies(20);

  Territory* territoryB = new Territory("TerritoryB", 2);
  territoryB->setArmies(4);

  Territory* territoryC = new Territory("TerritoryC", 3);
  territoryC->setArmies(8);

  Territory* territoryD = new Territory("TerritoryD", 4);
  territoryD->setArmies(12);

  // assign territories to players
  player1->addTerritory(territoryA);
  player2->addTerritory(territoryB);
  player1->addTerritory(territoryC);
  player2->addTerritory(territoryD);

  // add adjacency territories
  territoryA->addAdjTerritory(territoryB);
  territoryB->addAdjTerritory(territoryA);
  territoryC->addAdjTerritory(territoryD);
  territoryD->addAdjTerritory(territoryC);


  // (1) each order is validated before being executed according to the above descriptions; 
  // (2) ownership of a territory is transferred to the attacking player if a territory is conquered as a result of an advance order;
  // (3) one card is given to a player if they conquer at least one territory in a turn (not more than one card per turn); 
  std::cout << "Before Advance Order Execution: " << territoryB->getArmies() << " armies in TerritoryA, belongs to "
            << territoryB->getOwner()->getName() << '\n';
  std::cout << "Before attack Player1->conqueredThisTurn = "<< player1->getConqueredThisTurn() << '\n';

  Order* advanceOrder = new OrderAdvance(player1, territoryA, territoryB, new int(19));
  advanceOrder->execute();
  std::cout << "After Advance Order Execution: " << territoryB->getArmies() << " armies in TerritoryA, belongs to "
            << territoryB->getOwner()->getName() << '\n';
  std::cout << "After attack Player1->conqueredThisTurn = "<< player1->getConqueredThisTurn() << '\n';
  std::cout << "\n";

  // (4) the negotiate order prevents attacks between the two players involved; 
  Order* negotiateOrder = new OrderNegotiate(player2, player1);
  negotiateOrder->execute();
  std::cout << "Before Advance Order Execution with Negotiation: " << territoryD->getArmies()
            << " armies in TerritoryD, belongs to " << territoryD->getOwner()->getName() << '\n';

  Order* advanceOrderWithNegotiation = new OrderAdvance(player1, territoryC, territoryD, new int(5));
  advanceOrderWithNegotiation->execute();
  std::cout << "After Advance Order Execution with Negotiation: " << territoryD->getArmies()
            << " armies in TerritoryD, belongs to " << territoryD->getOwner()->getName() << '\n';
  std::cout << "\n";

  // (5) the blockade order transfers ownership to the Neutral player; 
  std::cout << "Before Blockade Order Execution: " << territoryC->getArmies() << " armies in TerritoryC, belongs to "
            << territoryC->getOwner()->getName() << '\n';
  Order* blockadeOrder = new OrderBlockade(nPlayer, player1, territoryC);
  blockadeOrder->execute();
  std::cout << "After Blockade Order Execution: " << territoryC->getArmies() << " armies in TerritoryC, belongs to "
            << territoryC->getOwner()->getName() << '\n';
  std::cout << "\n";

  // (6) all the orders described above can be issued by a player and executed by the game engine.
  player1->issueAdvanceOrder(territoryA, territoryB, 10);
  player1->issueNegotiateOrder(player2);
  player1->issueBlockadeOrder(nPlayer, territoryA);
  std::cout << "Player1's Orders List:" << '\n';
  std::cout << *(player1->getOrders()) << '\n';

  // cleanup
  delete player1;
  delete player2;
  delete nPlayer;
  delete territoryA;
  delete territoryB;
  delete territoryC;
  delete territoryD;
}
