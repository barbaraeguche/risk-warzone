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

  Territory* territoryA = new Territory("TerritoryA", 1);
  territoryA->setArmies(10);

  Territory* territoryB = new Territory("TerritoryB", 2);
  territoryB->setArmies(5);

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

  int* soldiersToDeploy = new int(5);
  Order* deployOrder = &OrderDeploy(player1, territoryA, soldiersToDeploy);

// (1) each order is validated before being executed according to the above descriptions; 
  std::cout << "Executing Deploy Order:" << '\n';
  int* soldiersToDeploy = new int(5);
  Order* deployOrder = new OrderDeploy(player1, territoryA, soldiersToDeploy);
  if (deployOrder->validate()) {
    deployOrder->execute();
  } else {
    std::cout << "Deploy Order is invalid." << '\n';
  }
  delete soldiersToDeploy;
  delete deployOrder;

  std::cout << "Executing Advance Order:" << '\n';
  int* soldiersToAdvance = new int(7);
  Order* advanceOrder = new OrderAdvance(player1, territoryA, territoryB, soldiersToAdvance);
  if (advanceOrder->validate()) {
    advanceOrder->execute();
  } else {
    std::cout << "Advance Order is invalid." << '\n';
  }
  delete soldiersToAdvance;
  delete advanceOrder;

  std::cout << "Executing Bomb Order:" << '\n';
  Order* bombOrder = new OrderBomb(territoryB, player1);
  if (bombOrder->validate()) {
    bombOrder->execute();
  } else {
    std::cout << "Bomb Order is invalid." << '\n';
  }
  delete bombOrder;

  // Clean up
  delete player1;
  delete player2;
  delete territoryA;
  delete territoryB;
  delete territoryC;
  delete territoryD;


// (2) ownership of a territory is transferred to the attacking player if a territory is conquered as a result of an advance order;
// (3) one card is given to a player if they conquer at least one territory in a turn (not more than one card per turn); 
// (4) the negotiate order prevents attacks between the two players involved; 
// (5) the blockade order transfers ownership to the Neutral player; 
}
