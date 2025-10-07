#include <iostream>
#include "Orders.h"

void testOrdersLists() {

  // Testing order functions
  Orders* testOrder = new OrderDeploy();

  // Testing validate() and execute()
  std::cout << "Testing validate() and execute() on a Deploy Order:" << '\n';
  std::cout << "Is the order valid? " << (testOrder->validate() ? "Yes" : "No") << '\n';
  testOrder->execute(); // Should do nothing as validate() returns false

  delete testOrder;

  // Testing OrdersList functions
  OrdersList ordersList;
  ordersList.add(new OrderDeploy());
  ordersList.add(new OrderAdvance());
  ordersList.add(new OrderBomb());
  ordersList.add(new OrderBlockade());
  ordersList.add(new OrderAirlift());
  ordersList.add(new OrderNegotiate());

  // Testing add()
  std::cout << "OrderList contents:" << '\n';
  for (const auto& order : *ordersList.orders) {
    std::cout << order << '\n';
  }

  // Testing remove()
  std::cout << "Remove 2nd Order:" << '\n';
  ordersList.remove(1);
  for (const auto& order : *ordersList.orders) {
    std::cout << order << '\n';
  }

  // Testing move()
  std::cout << "Move 1st Order to 4th Index:" << '\n';
  ordersList.move(0, 3);
  for (const auto& order : *ordersList.orders) {
    std::cout << order << '\n';
  }

  std::cout << "Cloned OrderList:" << '\n';
  OrdersList clonedList = ordersList;
  for (const auto& order : *clonedList.orders) {
    std::cout << order << '\n';
  }
}
