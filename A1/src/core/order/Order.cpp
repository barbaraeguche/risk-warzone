#include "Order.h"

//  OrderDeploy
OrderDeploy::OrderDeploy() {}
OrderDeploy::OrderDeploy(const OrderDeploy& other) {}
OrderDeploy& OrderDeploy::operator=(const OrderDeploy& other) { return *this; }
bool OrderDeploy::validate() { return false; }
void OrderDeploy::execute() {}
std::ostream& OrderDeploy::print(std::ostream& os) const { return os; }

//  OrderAdvance 
OrderAdvance::OrderAdvance() {}
OrderAdvance::OrderAdvance(const OrderAdvance& other) {}
OrderAdvance& OrderAdvance::operator=(const OrderAdvance& other) { return *this; }
bool OrderAdvance::validate() { return false; }
void OrderAdvance::execute() {}
std::ostream& OrderAdvance::print(std::ostream& os) const { return os; }

//  OrderBomb 
OrderBomb::OrderBomb() {}
OrderBomb::OrderBomb(const OrderBomb& other) {}
OrderBomb& OrderBomb::operator=(const OrderBomb& other) { return *this; }
bool OrderBomb::validate() { return false; }
void OrderBomb::execute() {}
std::ostream& OrderBomb::print(std::ostream& os) const { return os; }

//  OrderBlockade 
OrderBlockade::OrderBlockade() {}
OrderBlockade::OrderBlockade(const OrderBlockade& other) {}
OrderBlockade& OrderBlockade::operator=(const OrderBlockade& other) { return *this; }
bool OrderBlockade::validate() { return false; }
void OrderBlockade::execute() {}
std::ostream& OrderBlockade::print(std::ostream& os) const { return os; }

//  OrderAirlift 
OrderAirlift::OrderAirlift() {}
OrderAirlift::OrderAirlift(const OrderAirlift& other) {}
OrderAirlift& OrderAirlift::operator=(const OrderAirlift& other) { return *this; }
bool OrderAirlift::validate() { return false; }
void OrderAirlift::execute() {}
std::ostream& OrderAirlift::print(std::ostream& os) const { return os; }

//  OrderNegotiate 
OrderNegotiate::OrderNegotiate() {}
OrderNegotiate::OrderNegotiate(const OrderNegotiate& other) {}
OrderNegotiate& OrderNegotiate::operator=(const OrderNegotiate& other) { return *this; }
bool OrderNegotiate::validate() { return false; }
void OrderNegotiate::execute() {}
std::ostream& OrderNegotiate::print(std::ostream& os) const { return os; }

//  OrderList 
OrderList::OrderList() {}
OrderList::OrderList(const OrderList& other) {}
OrderList& OrderList::operator=(const OrderList& other) { return *this; }
std::ostream& OrderList::print(std::ostream& os) const { return os; }
void OrderList::add(Order* order) {}
void OrderList::remove(int index) {}
void OrderList::move(int fromIndex, int toIndex) {}
OrderList::~OrderList() {}

