#include <iostream>
#include "Order.h"

void testOrdersLists() {
	OrderList ordersList;
	ordersList.add(new OrderDeploy());
	ordersList.add(new OrderAdvance());
	ordersList.add(new OrderBomb());
	ordersList.add(new OrderBlockade());
	ordersList.add(new OrderAirlift());
	ordersList.add(new OrderNegotiate());

	std::cout << "OrderList contents:" << '\n';
	for (const auto& order : *ordersList.orders) {
		std::cout << order << '\n';
	}

    std::cout << "Remove 2nd Order:" << '\n';
	ordersList.remove(1);
	for (const auto& order : *ordersList.orders) {
		std::cout << order << '\n';
	}

    std::cout << "Move 1st Order to 4th Index:" << '\n';
	ordersList.move(0, 3);
	for (const auto& order : *ordersList.orders) {
		std::cout << order << '\n';
	}

	std::cout << "Cloned OrderList:" << '\n';
	OrderList clonedList = ordersList;
	for (const auto& order : *clonedList.orders) {
		std::cout << order << '\n';
	}
}
