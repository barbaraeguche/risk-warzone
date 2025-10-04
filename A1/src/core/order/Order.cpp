#include "Order.h"

//  OrderDeploy
OrderDeploy::OrderDeploy() {}
OrderDeploy::OrderDeploy(const OrderDeploy& other) {}
OrderDeploy& OrderDeploy::operator=(const OrderDeploy& other) { return *this; }
bool OrderDeploy::validate() { return false; }
void OrderDeploy::execute() {}

Order* OrderDeploy::clone() const { 
  return new OrderDeploy(*this); 
}

std::ostream& operator<<(std::ostream& os, const OrderDeploy& order) { 
  os << "A Deploy Order."; return os; 
}

//  OrderAdvance 
OrderAdvance::OrderAdvance() {}
OrderAdvance::OrderAdvance(const OrderAdvance& other) {}
OrderAdvance& OrderAdvance::operator=(const OrderAdvance& other) { return *this; }
bool OrderAdvance::validate() { return false; }
void OrderAdvance::execute() {}

Order* OrderAdvance::clone() const { 
  return new OrderAdvance(*this); 
}

std::ostream& operator<<(std::ostream& os, const OrderAdvance& order) { 
  os << "A Advance Order."; return os; 
}

//  OrderBomb 
OrderBomb::OrderBomb() {}
OrderBomb::OrderBomb(const OrderBomb& other) {}
OrderBomb& OrderBomb::operator=(const OrderBomb& other) { return *this; }
bool OrderBomb::validate() { return false; }
void OrderBomb::execute() {}

Order* OrderBomb::clone() const { 
  return new OrderBomb(*this); 
}

std::ostream& operator<<(std::ostream& os, const OrderBomb& order) { 
  os << "A Bomb Order."; return os; 
}

//  OrderBlockade 
OrderBlockade::OrderBlockade() {}
OrderBlockade::OrderBlockade(const OrderBlockade& other) {}
OrderBlockade& OrderBlockade::operator=(const OrderBlockade& other) { return *this; }
bool OrderBlockade::validate() { return false; }
void OrderBlockade::execute() {}

Order* OrderBlockade::clone() const { 
  return new OrderBlockade(*this); 
}

std::ostream& operator<<(std::ostream& os, const OrderBlockade& order) { 
  os << "A Blockade Order."; return os; 
}

//  OrderAirlift 
OrderAirlift::OrderAirlift() {}
OrderAirlift::OrderAirlift(const OrderAirlift& other) {}
OrderAirlift& OrderAirlift::operator=(const OrderAirlift& other) { return *this; }
bool OrderAirlift::validate() { return false; }
void OrderAirlift::execute() {}

Order* OrderAirlift::clone() const { 
  return new OrderAirlift(*this); 
}

std::ostream& operator<<(std::ostream& os, const OrderAirlift& order) { 
  os << "A Airlift Order."; return os; 
}

//  OrderNegotiate 
OrderNegotiate::OrderNegotiate() {}
OrderNegotiate::OrderNegotiate(const OrderNegotiate& other) {}
OrderNegotiate& OrderNegotiate::operator=(const OrderNegotiate& other) { return *this; }
bool OrderNegotiate::validate() { return false; }
void OrderNegotiate::execute() {}

Order* OrderNegotiate::clone() const { 
  return new OrderNegotiate(*this); 
}

std::ostream& operator<<(std::ostream& os, const OrderNegotiate& order) { 
  os << "A Negotiate Order."; return os; 
}

//  OrdersList 
OrdersList::OrdersList() {
  orders = new std::vector<Order*>();
}

OrdersList::OrdersList(const OrdersList& other) {
  orders = new std::vector<Order*>();
  for (const auto& order : *other.orders) {
    orders->push_back(order->clone());
  }
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
  if (this != &other) {
    delete orders;
    orders = new std::vector<Order*>();
    for (const auto& order : *other.orders) {
      orders->push_back(order->clone());
    }
  }
  return *this;
}

std::ostream& operator<<(std::ostream& os, const OrdersList& orderList) {
  os << "A Orders List"; return os;
}

bool OrdersList::validateIndex(int index) {
  return index >= 0 && index < orders->size();
}

void OrdersList::add(Order* order) {
  orders->push_back(order);
}

void OrdersList::remove(int index) {
  if (!validateIndex(index)) {
    return;
  }
  orders->erase(orders->begin() + index);
}

void OrdersList::move(int fromIndex, int toIndex) {
  if (!validateIndex(fromIndex) || !validateIndex(toIndex)) {
    return;
  }
    
    //Remove order from the list and reinsert it at the new index
    Order* order = orders->at(fromIndex);
  orders->erase(orders->begin() + fromIndex);
  orders->insert(orders->begin() + toIndex, order);
}

OrdersList::~OrdersList() {
  //assume that OrdersList owns the orders and is responsible for deleting them
  for (const auto& order : *orders) {
    delete order;
  }

  delete orders;
}

