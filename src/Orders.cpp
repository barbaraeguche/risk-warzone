#include "Orders.h"

std::unique_ptr<std::vector<NegotiationRecord>> Order::negotiationRecords = std::make_unique<std::vector<NegotiationRecord>>();

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

void Order::clearNegotiationRecords() {
  negotiationRecords->clear();
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
  std::cout << "Validating Deploy Order: Deploying " << *soldiers << " armies to " << target->getName() << ".\n";
  std::cout << "Player Reinforcement Pool: " << player->getReinforcementPool() << " armies.\n";
  if (player == nullptr || target == nullptr || soldiers == nullptr) {
    return false;
  }else if (!player->ownsTerritory(target) || *soldiers <= 0) {
    return false;
  } else {
    return true;
  }
}

void OrderDeploy::execute() {
  if (this->validate())
  {
    std::cout << "Executing Deploy Order: Deploying " << *soldiers << " armies to " << target->getName() << ".\n";
    target->setArmies(target->getArmies() + *soldiers);
    player->setReinforcementPool(player->getReinforcementPool() - *soldiers);
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
  std::cout << source->isAdjacentTo(target) << player->ownsTerritory(source) << (source->getArmies() - 1 >= *soldiers) << ".\n";
  if (source && target && source->isAdjacentTo(target) && player->ownsTerritory(source) && *soldiers > 0 && (source->getArmies() - 1 >= *soldiers) ) {
    Player* targetPlayer = target->getOwner();
    // Check for negotiation
    for (const auto& record : *(this->negotiationRecords)) {
      if ((record.player1 == player && record.player2 == targetPlayer) || (record.player1 == targetPlayer && record.player2 == player)) {
        std::cout << "Advance Order Validation Failed: Negotiation exists between " << player->getName() << " and " << targetPlayer->getName() << ". Cannot attack.\n";
        return false; // Negotiation exists, cannot attack
      }
    }
    return true;
  }else{
    return false;
  }
}

void OrderAdvance::execute() {
  if(this->validate()){
    std::cout << "Executing Advance Order: Moving " << *soldiers << " armies from " << source->getName() << " to " << target->getName() << ".\n";
    if (player->ownsTerritory(target)){
      source->setArmies(source->getArmies() - *soldiers);
      target->setArmies(target->getArmies() + *soldiers);
    } else {
      // Remove soldiers from source territory
      source->setArmies(source->getArmies() - *soldiers);

      int attackers = *soldiers;
      int defenders = target->getArmies();

      int attackSuccesses = 0;
      int defenseSuccesses = 0;

      for (int i = 0; i < *soldiers; i++) {
        if((rand() % 100) < 60) {
          attackSuccesses++;
        }
      }

      for (int i = 0; i < target->getArmies(); i++) {
        if((rand() % 100) < 70) {
          defenseSuccesses++;
        }
      }

      attackers -= defenseSuccesses;
      defenders -= attackSuccesses;

      if(defenders <= 0){
          player->setConqueredThisTurn(true);
          player->addTerritory(target);
          target->setOwner(player);
          if(attackers < 0){
            attackers = 1; // At least one army must occupy the conquered territory
            // Rare edge case where all attackers die but defenders also die
          }
          target->setArmies(attackers);
      } else {
        target->setArmies(defenders);
        if(attackers >= 0){
          source->setArmies(source->getArmies() + attackers); // Return surviving attackers back to source
        }
      }
    }
  }else{
    std::cout << "Advance Order validation failed. Order not executed.\n";
  }

}

Order* OrderAdvance::clone() const { 
  return new OrderAdvance(*this); 
}


//  OrderBomb 
OrderBomb::OrderBomb(Territory* target, Player* player) {
  this->target = target;
  this->player = player;
  type = new std::string("bomb");
  description = new std::string("Destroys half the armies on an enemy territory, can only be used once per game.");
}

OrderBomb::OrderBomb(const OrderBomb& other) {
  this->target = other.target;
  this->player = other.player;
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
    this->player = other.player;
  }
  return *this;
}

bool OrderBomb::validate() { 
  if (target && !player->ownsTerritory(target)) {
    for (auto& t : player->getTerritories()) {
      if (t->isAdjacentTo(target)) {
        return true;
      }
    }
    return false;
  }else {
    return false;
  }
}

void OrderBomb::execute() {
  if (this->validate())
  {
    target->setArmies(target->getArmies() / 2);
  }
}

Order* OrderBomb::clone() const { 
  return new OrderBomb(*this); 
}


//  OrderBlockade 
OrderBlockade::OrderBlockade(Player* nPlayer, Player* player, Territory* target) {
  this->nPlayer = nPlayer;
  this->player = player;
  this->target = target;
  type = new std::string("blockade");
  description = new std::string("Triples the armies on one of your territories and makes it neutral, can only be used once per game.");
}

OrderBlockade::OrderBlockade(const OrderBlockade& other) {
  this->nPlayer = other.nPlayer;
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
    this->nPlayer = other.nPlayer;
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

void OrderBlockade::execute() {
  if (this->validate())
  {
    target->setArmies(target->getArmies() * 2);
    player->removeTerritory(target);
    nPlayer->addTerritory(target);
    target->setOwner(nPlayer);
  }
}

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
  if (source && target && player->ownsTerritory(source) && player->ownsTerritory(target) && *soldiers > 0 && source->getArmies() >= *soldiers ) {
    return true;
  }else{
    return false;
  }
}

void OrderAirlift::execute() {
  if(this->validate()){
    source->setArmies(source->getArmies() - *soldiers);
    target->setArmies(target->getArmies() + *soldiers);
  }
}

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

void OrderNegotiate::execute() {
  if (this->validate())
  {
    this->negotiationRecords->push_back(NegotiationRecord{player, targetPlayer});
  }
}

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
  os << "A Orders List:\n";
  for (const auto& order : *orderList.orders) {
    os << *order << '\n';
  }
  return os;
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

