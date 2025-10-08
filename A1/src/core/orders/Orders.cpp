#include "Orders.h"

// Base Order
Order::Order():
  type{nullptr},
  description{nullptr}
  {}

std::ostream& operator<<(std::ostream& os, const Order& order) { 
  if (!order.type || !order.description) {
    os << "This order is not properly initialized.";
    return os;
  }

  os << "An order of type: " << *(order.type) << ". \n" << "Description: " << *(order.description) << ". \n";
  return os;  
}

//  OrderDeploy
OrderDeploy::OrderDeploy(Player* player, Territory* target, int* soldiers) {
  this->player = player;
  this->target = target;
  this->soldiers = soldiers;
  type = new std::string("deploy");
  description = new std::string("Deploys armies to a specified territory you own.");
}

OrderDeploy::OrderDeploy(const OrderDeploy& other) {
  this->player = other.player;
  this->target = other.target;
  this->soldiers = other.soldiers;
  type = new std::string(*(other.type));
  description = new std::string(*(other.description));
}

OrderDeploy::~OrderDeploy() {
  delete type;
  delete description;
}

OrderDeploy& OrderDeploy::operator=(const OrderDeploy& other) { 
  if (this != &other) {
    delete type;
    delete description;
    type = new std::string(*(other.type));
    description = new std::string(*(other.description));
    this->player = other.player;
    this->target = other.target;
    this->soldiers = other.soldiers;
  }
  return *this;
}

bool OrderDeploy::validate() { 
  if (player == nullptr || target == nullptr || soldiers == nullptr) {
    return false;
  }else if (!player->ownsTerritory(target) || *soldiers <= 0 || *soldiers > player->getReinforcementPool()) {
    return false;
  } else {
    return true;
  }
}

void OrderDeploy::execute() {
  if (this->validate())
  {

  }
}

Order* OrderDeploy::clone() const { 
  return new OrderDeploy(*this); 
}


//  OrderAdvance 
OrderAdvance::OrderAdvance(Player* player, Territory* source, Territory* target, int* soldiers) {
  this->player = player;
  this->source = source;
  this->target = target;
  this->soldiers = soldiers;
  type = new std::string("advance");
  description = new std::string("Moves armies from one territory to another, can also be used to attack enemy territories.");
}

OrderAdvance::OrderAdvance(const OrderAdvance& other) {
  this->player = other.player;
  this->source = other.source;
  this->target = other.target;
  this->soldiers = other.soldiers;
  type = new std::string(*(other.type));
  description = new std::string(*(other.description));
}

OrderAdvance::~OrderAdvance() {
  delete type;
  delete description;
}

OrderAdvance& OrderAdvance::operator=(const OrderAdvance& other) { 
  if (this != &other) {
    delete type;
    delete description;
    type = new std::string(*(other.type));
    description = new std::string(*(other.description));
    this->player = other.player;
    this->source = other.source;
    this->target = other.target;
    this->soldiers = other.soldiers;
  }
  return *this;
}

bool OrderAdvance::validate() { 
  if (source && target && player->ownsTerritory(source) && *soldiers > 0) {
    return true;
  }else{
    return false;
  }
}
void OrderAdvance::execute() {}

Order* OrderAdvance::clone() const { 
  return new OrderAdvance(*this); 
}


//  OrderBomb 
OrderBomb::OrderBomb(Territory* target) {
  this->target = target;
  type = new std::string("bomb");
  description = new std::string("Destroys half the armies on an enemy territory, can only be used once per game.");
}

OrderBomb::OrderBomb(const OrderBomb& other) {
  this->target = other.target;
  type = new std::string(*(other.type));
  description = new std::string(*(other.description));
}

OrderBomb::~OrderBomb() {
  delete type;
  delete description;
}

OrderBomb& OrderBomb::operator=(const OrderBomb& other) { 
  if (this != &other) {
    delete type;
    delete description;
    type = new std::string(*(other.type));
    description = new std::string(*(other.description));
    this->target = other.target;
  }
  return *this;
}

bool OrderBomb::validate() { return target; }
void OrderBomb::execute() {
  if (this->validate())
  {

  }
}

Order* OrderBomb::clone() const { 
  return new OrderBomb(*this); 
}


//  OrderBlockade 
OrderBlockade::OrderBlockade(Player* player, Territory* target) {
  this->player = player;
  this->target = target;
  type = new std::string("blockade");
  description = new std::string("Triples the armies on one of your territories and makes it neutral, can only be used once per game.");
}

OrderBlockade::OrderBlockade(const OrderBlockade& other) {
  this->player = other.player;
  this->target = other.target;
  type = new std::string(*(other.type));
  description = new std::string(*(other.description));
}

OrderBlockade::~OrderBlockade() {
  delete type;
  delete description;
}

OrderBlockade& OrderBlockade::operator=(const OrderBlockade& other) { 
  if (this != &other) {
    delete type;
    delete description;
    type = new std::string(*(other.type));
    description = new std::string(*(other.description));
    this->player = other.player;
    this->target = other.target;
  }
  return *this;
}

bool OrderBlockade::validate() { 
  if (target && player->ownsTerritory(target)) {
    return true;
  }else {
    return false;
  }
}
void OrderBlockade::execute() {}

Order* OrderBlockade::clone() const { 
  return new OrderBlockade(*this); 
}


//  OrderAirlift 
OrderAirlift::OrderAirlift(Player* player, Territory* source, Territory* target, int* soldiers) {
  this->player = player;
  this->source = source;
  this->target = target;
  this->soldiers = soldiers;
  type = new std::string("airlift");
  description = new std::string("Moves any number of armies from one of your territories to another territory you own, can only be used once per game.");
}

OrderAirlift::OrderAirlift(const OrderAirlift& other) {
  this->player = other.player;
  this->source = other.source;
  this->target = other.target;
  this->soldiers = other.soldiers;
  type = new std::string(*(other.type));
  description = new std::string(*(other.description));
}

OrderAirlift::~OrderAirlift() {
  delete type;
  delete description;
}

OrderAirlift& OrderAirlift::operator=(const OrderAirlift& other) { 
  if (this != &other) {
    delete type;
    delete description;
    type = new std::string(*(other.type));
    description = new std::string(*(other.description));
    this->player = other.player;
    this->source = other.source;
    this->target = other.target;
    this->soldiers = other.soldiers;
  }
  return *this;
}

bool OrderAirlift::validate() { 
  if (source && target && player->ownsTerritory(source) && *soldiers > 0) {
    return true;
  }else{
    return false;
  }
}

void OrderAirlift::execute() {}

Order* OrderAirlift::clone() const { 
  return new OrderAirlift(*this); 
}


//  OrderNegotiate 
OrderNegotiate::OrderNegotiate(Player* player, Player* targetPlayer) {
  this->player = player;
  this->targetPlayer = targetPlayer;
  type = new std::string("negotiate");
  description = new std::string("Prevents attacks between you and another player until your next turn, can only be used once per game.");
}

OrderNegotiate::OrderNegotiate(const OrderNegotiate& other) {
  this->player = other.player;
  this->targetPlayer = other.targetPlayer;
  type = new std::string(*(other.type));
  description = new std::string(*(other.description));
}

OrderNegotiate::~OrderNegotiate() {
  delete type;
  delete description;
}

OrderNegotiate& OrderNegotiate::operator=(const OrderNegotiate& other) { 
  if (this != &other) {
    delete type;
    delete description;
    type = new std::string(*(other.type));
    description = new std::string(*(other.description));
    this->player = other.player;
    this->targetPlayer = other.targetPlayer;
  }
  return *this;
}

bool OrderNegotiate::validate() { 
  if (targetPlayer && targetPlayer != player) {
    return true;
  }else {
    return false;
  }
}

void OrderNegotiate::execute() {}

Order* OrderNegotiate::clone() const { 
  return new OrderNegotiate(*this); 
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

  //Validate indices
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

