#include <iostream>
#include "Orders.h"

void testOrdersLists() {

  // Testing order functions
  Order* testOrder = new OrderDeploy(nullptr, nullptr, nullptr);

  // Testing validate() and execute()
  std::cout << "Testing validate() and execute() on a Deploy Order:" << '\n';
  std::cout << "Is the order valid? " << (testOrder->validate() ? "Yes" : "No") << '\n';
  testOrder->execute(); // Should do nothing as validate() returns false

  delete testOrder;

  // Testing OrdersList functions
  OrdersList ordersList;
  ordersList.add(new OrderDeploy(nullptr, nullptr, nullptr));
  ordersList.add(new OrderAdvance(nullptr, nullptr, nullptr, nullptr));
  ordersList.add(new OrderBomb(nullptr, nullptr));
  ordersList.add(new OrderBlockade(nullptr, nullptr, nullptr));
  ordersList.add(new OrderAirlift(nullptr, nullptr, nullptr, nullptr));
  ordersList.add(new OrderNegotiate(nullptr, nullptr));

  // Testing add()
  std::cout << "OrderList contents:" << '\n';
  for (const auto& order : *ordersList.orders) {
    std::cout << *order << '\n';
  }

  // Testing remove()
  std::cout << "Remove 2nd Order:" << '\n';
  ordersList.remove(1);
  for (const auto& order : *ordersList.orders) {
    std::cout << *order << '\n';
  }

  // Testing move()
  std::cout << "Move 1st Order to 4th Index:" << '\n';
  ordersList.move(0, 3);
  for (const auto& order : *ordersList.orders) {
    std::cout << *order << '\n';
  }

  std::cout << "Cloned OrderList:" << '\n';
  OrdersList clonedList = ordersList;
  for (const auto& order : *clonedList.orders) {
    std::cout << *order << '\n';
  }
}

void testOrderExecution() {
  Player* player1 = new Player("Player1");
  Player* player2 = new Player("Player2");
  Player* nPlayer = new Player("Neutral player");

  Territory* territoryA = new Territory("TerritoryA", 1);
  territoryA->setArmies(20);

  Territory* territoryB = new Territory("TerritoryB", 2);
  territoryB->setArmies(4);

  Territory* territoryC = new Territory("TerritoryC", 3);
  territoryC->setArmies(8);

  Territory* territoryD = new Territory("TerritoryD", 4);
  territoryD->setArmies(12);

  player1->addTerritory(territoryA);
  territoryA->setOwner(player1);

  player2->addTerritory(territoryB);
  territoryB->setOwner(player2);

  territoryA->addAdjacentTerritory(territoryB);
  territoryB->addAdjacentTerritory(territoryA);

  player1->addTerritory(territoryC);
  territoryC->setOwner(player1);

  player2->addTerritory(territoryD);
  territoryD->setOwner(player2);

  territoryC->addAdjacentTerritory(territoryD);
  territoryD->addAdjacentTerritory(territoryC);


  // (1) each order is validated before being executed according to the above descriptions; 
  // (2) ownership of a territory is transferred to the attacking player if a territory is conquered as a result of an advance order;
  // (3) one card is given to a player if they conquer at least one territory in a turn (not more than one card per turn); 
  std::cout << "Before Advance Order Execution: " << territoryB->getArmies() << " armies in TerritoryA, belongs to " << territoryB->getOwner()->getName() << '\n';
  std::cout << "Before attack Player1->conqueredThisTurn = "<< player1->getConqueredThisTurn() << '\n';

  Order* advanceOrder = new OrderAdvance(player1, territoryA, territoryB, new int(19));
  advanceOrder->execute();
  std::cout << "After Advance Order Execution: " << territoryB->getArmies() << " armies in TerritoryA, belongs to " << territoryB->getOwner()->getName() << '\n';
  std::cout << "After attack Player1->conqueredThisTurn = "<< player1->getConqueredThisTurn() << '\n';
  std::cout << "\n";

  // (4) the negotiate order prevents attacks between the two players involved; 
  Order* negotiateOrder = new OrderNegotiate(player1, player2);
  negotiateOrder->execute();
  std::cout << "Before Advance Order Execution with Negotiation: " << territoryD->getArmies() << " armies in TerritoryD, belongs to " << territoryD->getOwner()->getName() << '\n';
  Order* advanceOrderWithNegotiation = new OrderAdvance(player1, territoryC, territoryD, new int(5));
  advanceOrderWithNegotiation->execute();
  std::cout << "After Advance Order Execution with Negotiation: " << territoryD->getArmies() << " armies in TerritoryD, belongs to " << territoryD->getOwner()->getName() << '\n';
  std::cout << "\n";

  // (5) the blockade order transfers ownership to the Neutral player; 
  std::cout << "Before Blockade Order Execution: " << territoryC->getArmies() << " armies in TerritoryC, belongs to " << territoryC->getOwner()->getName() << '\n';
  Order* blockadeOrder = new OrderBlockade(nPlayer, player1, territoryC);
  blockadeOrder->execute();
  std::cout << "After Blockade Order Execution: " << territoryC->getArmies() << " armies in TerritoryC, belongs to " << territoryC->getOwner()->getName() << '\n';
  std::cout << "\n";

  //6) all the orders described above can be issued by a player and executed by the game engine. This driver function must be in the OrdersDriver.cpp file. 
  player1->issueAdvanceOrder(territoryA, territoryB, 10);
  player1->issueNegotiateOrder(player2);
  player1->issueBlockadeOrder(nPlayer, territoryA);
  std::cout << "Player1's Orders List:" << '\n';
  std::cout << *(player1->getOrders()) << '\n';

  delete territoryA;
  delete territoryB;
  delete territoryC;
  delete territoryD;
  delete player1;
  delete player2;
  delete nPlayer;
}
