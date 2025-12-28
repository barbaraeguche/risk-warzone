#include "Orders.h"
#include "Map.h"


// initialise static negotiation records
std::unique_ptr<std::vector<NegotiationRecord>> Order::negotiationRecords = std::make_unique<std::vector<NegotiationRecord>>();

// ==================== Order Class Implementation ====================
Order::Order():
  Subject(),
  type(nullptr),
  description(nullptr),
  effect(new std::string("")) {}

Order::Order(const Order& other) :
  Subject(other),
  type(new std::string(*other.type)),
  description(new std::string(*other.description)),
  effect(new std::string(*other.effect)) {}

Order& Order::operator=(const Order& other) {
  if (this != &other) {
    Subject::operator=(other);

    delete type;
    delete description;
    delete effect;

    // deep copy primitives
    type = new std::string(*other.type);
    description = new std::string(*other.description);
    effect = new std::string(*other.effect);
  }
  return *this;
}

Order::~Order() {
  delete type;
  delete description;
  delete effect;
}

// --- GETTERS ---
std::string Order::getEffect() const {
  return *effect;
}

// --- UTILITY ---
void Order::saveEffect(const std::string& eff) {
  *effect = eff;
  notify();
}

std::string Order::stringToLog() const {
  return "Order: " + (type ? *type : "<Unknown>") + ", Effect: " + (effect ? *effect : "<None>");
}

void Order::clearNegotiationRecords() {
  negotiationRecords->clear();
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Order& ord) {
  os << "Order[" << (ord.type ? *ord.type : "<Unknown>")
     << ", Description:" << (ord.description ? *ord.description : "<Nil>") << "]";
  return os;
}


// ==================== OrderDeploy Class Implementation ====================
OrderDeploy::OrderDeploy(Player* play, Territory* targ, int* sold) :
  Order(),
  player(play),
  target(targ),
  soldiers(sold) {
  type = new std::string(ORDER_TYPES::DEPLOY);
  description = new std::string("Deploys armies to an owned specified territory.");
}

OrderDeploy::OrderDeploy(const OrderDeploy& other) :
  Order(other),
  player(other.player),
  target(other.target),
  soldiers(other.soldiers) {}

OrderDeploy& OrderDeploy::operator=(const OrderDeploy& other) {
  if (this != &other) {
    Order::operator=(other);
    player = other.player;
    target = other.target;
    soldiers = other.soldiers;
  }
  return *this;
}

OrderDeploy::~OrderDeploy() = default;

// --- VALIDATION ---
bool OrderDeploy::validate() {
  if (player == nullptr || target == nullptr || soldiers == nullptr) return false;
  if (!player->ownsTerritory(target) || *soldiers <= 0) return false;
  return true;
}

// --- MANAGEMENT ---
void OrderDeploy::execute() {
  if (!validate()) {
    saveEffect("Invalid deploy order. Not executed.");
    return;
  }

  std::cout << "Executing Deploy Order: Deploying " << *soldiers << " armies to " << target->getName() << std::endl;
  target->setArmies(target->getArmies() + *soldiers);
  saveEffect("Deployed " + std::to_string(*soldiers) + " armies to " + target->getName() + ".");
}

// --- HELPERS ---
Order* OrderDeploy::clone() const {
  return new OrderDeploy(*this);
}


// ==================== OrderAdvance Class Implementation ====================
OrderAdvance::OrderAdvance(Player* play, Territory* src, Territory* targ, int* sold) :
  Order(),
  player(play),
  source(src),
  target(targ),
  soldiers(sold) {
  type = new std::string(ORDER_TYPES::ADVANCE);
  description = new std::string("Moves armies between territories. It can also be used to attack enemy territories.");
}

OrderAdvance::OrderAdvance(const OrderAdvance& other) :
  Order(other),
  player(other.player),
  source(other.source),
  target(other.target),
  soldiers(other.soldiers) {}

OrderAdvance& OrderAdvance::operator=(const OrderAdvance& other) {
  if (this != &other) {
    Order::operator=(other);
    player = other.player;
    source = other.source;
    target = other.target;
    soldiers = other.soldiers;
  }
  return *this;
}

OrderAdvance::~OrderAdvance() = default;

// --- VALIDATION ---
bool OrderAdvance::validate() {
  if (
    source && target && source->isAdjacentTo(target) &&
    player->ownsTerritory(source) && *soldiers > 0 && source->getArmies() - 1 >= *soldiers
  ) {
    Player* targetPlayer = target->getOwner();

    // check for negotiation
    for (const auto& record : *(this->negotiationRecords)) {
      if (
        (record.player1 == player && record.player2 == targetPlayer) ||
        (record.player1 == targetPlayer && record.player2 == player)
      ) {
        std::cout << "Advance Order Validation Failed: Negotiation exists between " << player->getName()
                  << " and " << targetPlayer->getName() << ". Cannot attack." << std::endl;
        return false; // negotiation exists, cannot attack
      }
    }

    saveEffect("Invalid advance order. Not executed.");
    return true;
  }

  return false;
}

// --- MANAGEMENT ---
void OrderAdvance::execute() {
  if (!validate()) {
    saveEffect("Invalid advance order. Not executed.");
    return;
  }
  std::cout << "Executing Advance Order: Moving " << *soldiers << " armies from " 
            << source->getName() << " to " << target->getName() << ".\n";
  
  if (player->ownsTerritory(target)){
    source->setArmies(source->getArmies() - *soldiers);
    target->setArmies(target->getArmies() + *soldiers);
    saveEffect("Advanced armies to friendly territory " + target->getName() + ".");
  } else {
    target->getOwner()->setGotAttackedThisTurn(true);
    // remove soldiers from source territory
    source->setArmies(source->getArmies() - *soldiers);

    int attackers = *soldiers;
    int defenders = target->getArmies();
    int attackSuccesses = 0, defenseSuccesses = 0;

    for (int i = 0; i < *soldiers; i++) {
      if ((rand() % 100) < 60) attackSuccesses++;
    }

    for (int i = 0; i < target->getArmies(); i++) {
      if (rand() % 100 < 70) defenseSuccesses++;
    }

    attackers -= defenseSuccesses;
    defenders -= attackSuccesses;

    if (defenders <= 0) {
      player->setConqueredThisTurn(true);
      target->getOwner()->removeTerritory(target);
      player->addTerritory(target);

      // at least one army must occupy the conquered territory
      // rare edge case where all attackers die but defenders also die
      if (attackers < 0) attackers = 1;
      target->setArmies(attackers);
      saveEffect("Advance order succeeded and conquered " + target->getName() + ".");
    } else {
      target->setArmies(defenders);
      // return surviving attackers back to source
      if (attackers >= 0) source->setArmies(source->getArmies() + attackers);
      saveEffect("Advance order failed to conquer " + target->getName() + ".");
    }
  }
}

// --- HELPERS ---
Order* OrderAdvance::clone() const { 
  return new OrderAdvance(*this); 
}


// ==================== OrderAirlift Class Implementation ====================
OrderAirlift::OrderAirlift(Player* play, Territory* src, Territory* targ, int* sold) :
  Order(),
  player(play),
  source(src),
  target(targ),
  soldiers(sold) {
  type = new std::string(ORDER_TYPES::AIRLIFT);
  description = new std::string("Moves armies between owned territories. It can only be used once per game.");
}

OrderAirlift::OrderAirlift(const OrderAirlift& other) :
  Order(other),
  player(other.player),
  source(other.source),
  target(other.target),
  soldiers(other.soldiers) {}

OrderAirlift& OrderAirlift::operator=(const OrderAirlift& other) {
  if (this != &other) {
    Order::operator=(other);
    player = other.player;
    source = other.source;
    target = other.target;
    soldiers = other.soldiers;
  }
  return *this;
}

OrderAirlift::~OrderAirlift() = default;

// --- VALIDATION ---
bool OrderAirlift::validate() {
  if (
    source && target && player->ownsTerritory(source) &&
    player->ownsTerritory(target) && *soldiers > 0 && source->getArmies() >= *soldiers
  ) return true;

  saveEffect("Invalid airlift order. Not executed.");
  return false;
}

void OrderAirlift::execute() {
  if (!validate()){
    saveEffect("Invalid airlift order. Not executed.");
    return;
  }

  source->setArmies(source->getArmies() - *soldiers);
  target->setArmies(target->getArmies() + *soldiers);
  saveEffect("Airlifted " + std::to_string(*soldiers) + " armies to " + target->getName() + ".");
}

Order* OrderAirlift::clone() const {
  return new OrderAirlift(*this);
}


// ==================== OrderAirlift Class Implementation ====================
OrderBomb::OrderBomb(Player* play, Territory* targ) :
  Order(),
  player(play),
  target(targ) {
  type = new std::string(ORDER_TYPES::BOMB);
  description = new std::string("Destroys half the armies on an enemy territory. It can only be used once per game.");
}

OrderBomb::OrderBomb(const OrderBomb& other) :
  Order(other),
  player(other.player),
  target(other.target) {}

OrderBomb& OrderBomb::operator=(const OrderBomb& other) {
  if (this != &other) {
    Order::operator=(other);
    player = other.player;
    target = other.target;
  }
  return *this;
}

OrderBomb::~OrderBomb() = default;

// --- VALIDATION ---
bool OrderBomb::validate() {
  if (target && !player->ownsTerritory(target)) {
    for (auto& terr : player->getTerritories()) {
      if (terr->isAdjacentTo(target)) return true;
    }
    return false;
  }

  saveEffect("Invalid bomb order. Not executed.");
  return false;
}

// --- MANAGEMENT ---
void OrderBomb::execute() {
  if (!validate()) {
    saveEffect("Invalid bomb order. Not executed.");
    return;
  }

  target->setArmies(target->getArmies() / 2);
  saveEffect("Bombed " + target->getName() + "; armies halved.");
}

// --- HELPERS ---
Order* OrderBomb::clone() const { 
  return new OrderBomb(*this); 
}


// ==================== OrderBlockade Class Implementation ====================
OrderBlockade::OrderBlockade(Player* nPlay, Player* play, Territory* targ) :
  Order(),
  nPlayer(nPlay),
  player(play),
  target(targ) {
  type = new std::string(ORDER_TYPES::BLOCKADE);
  description = new std::string("Triples the armies in an owned territory and makes it neutral. It can only be used once per game.");
}

OrderBlockade::OrderBlockade(const OrderBlockade& other) :
  Order(other),
  nPlayer(other.nPlayer),
  player(other.player),
  target(other.target) {}

OrderBlockade& OrderBlockade::operator=(const OrderBlockade& other) { 
  if (this != &other) {
    Order::operator=(other);
    nPlayer = other.nPlayer;
    player = other.player;
    target = other.target;
  }
  return *this;
}

OrderBlockade::~OrderBlockade() = default;

// --- VALIDATION ---
bool OrderBlockade::validate() { 
  if (target && player->ownsTerritory(target)) return true;
  saveEffect("Invalid blockade order. Not executed.");
  return false;
}

// --- MANAGEMENT ---
void OrderBlockade::execute() {
  if (!validate()) {
    saveEffect("Invalid blockade order. Not executed.");
    return;
  }

  target->setArmies(target->getArmies() * 2);
  player->removeTerritory(target);
  nPlayer->addTerritory(target);
  saveEffect("Blockade executed on " + target->getName() + "; territory becomes neutral.");
}

// --- HELPERS ---
Order* OrderBlockade::clone() const { 
  return new OrderBlockade(*this); 
}


// ==================== OrderNegotiate Class Implementation ====================
OrderNegotiate::OrderNegotiate(Player* tPlay, Player* play) :
  Order(),
  tPlayer(tPlay),
  player(play) {
  type = new std::string(ORDER_TYPES::NEGOTIATE);
  description = new std::string("Prevents attacks between you and another player until your next turn. It can only be used once per game.");
}

OrderNegotiate::OrderNegotiate(const OrderNegotiate& other) :
  Order(other),
  tPlayer(other.tPlayer),
  player(other.player) {}

OrderNegotiate& OrderNegotiate::operator=(const OrderNegotiate& other) { 
  if (this != &other) {
    Order::operator=(other);
    tPlayer = other.tPlayer;
    player = other.player;
  }
  return *this;
}

OrderNegotiate::~OrderNegotiate() = default;

// --- VALIDATION ---
bool OrderNegotiate::validate() { 
  if (tPlayer && tPlayer != player) return true;
  saveEffect("Invalid negotiate order. Not executed.");
  return false;
}

// --- MANAGEMENT ---
void OrderNegotiate::execute() {
  if (!validate()) {
    saveEffect("Invalid negotiate order. Not executed.");
    return;
  }

  this->negotiationRecords->push_back(NegotiationRecord{player, tPlayer});
  saveEffect("Negotiation established between " + player->getName() + " and " + tPlayer->getName() + ".");
}

// --- HELPERS ---
Order* OrderNegotiate::clone() const { 
  return new OrderNegotiate(*this); 
}


// ==================== OrderCheat Class Implementation ====================
OrderCheat::OrderCheat(Player* play) :
  Order(),
  player(play) {
  type = new std::string(ORDER_TYPES::CHEAT);
  description = new std::string("Conquers all adjacent territories of the player once per turn.");
}

OrderCheat::OrderCheat(const OrderCheat& other) :
  Order(other),
  player(other.player) {}

OrderCheat& OrderCheat::operator=(const OrderCheat& other) { 
  if (this != &other) {
    Order::operator=(other);
    player = other.player;
  }
  return *this;
}

OrderCheat::~OrderCheat() = default;

// --- VALIDATION ---
bool OrderCheat::validate() { 
  if (player) return true;
  saveEffect("Invalid cheat order. Not executed.");
  return false;
}

// --- MANAGEMENT ---
void OrderCheat::execute() {
  if (!validate()) {
    saveEffect("Invalid cheat order. Not executed.");
    return;
  }

  for (auto& terr : player->toAttack()) {
    Player* originalOwner = terr->getOwner();
    originalOwner->removeTerritory(terr);
    player->addTerritory(terr);
  }
  saveEffect("Cheat executed; all armies on player's territories doubled.");
}

// --- HELPERS ---
Order* OrderCheat::clone() const { 
  return new OrderCheat(*this); 
}


// ==================== OrdersList Class Implementation ====================
OrdersList::OrdersList() :
  Subject(),
  orders(new std::vector<Order*>()),
  lastAddedOrder(nullptr) {}

OrdersList::OrdersList(const OrdersList& other) :
  Subject(other),
  orders(new std::vector<Order*>()),
  lastAddedOrder(nullptr) {
  // deep copy orders
  for (const auto& ord : *other.orders) {
    orders->push_back(ord->clone());
  }
  if (!orders->empty()) lastAddedOrder = orders->back();
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
  if (this != &other) {
    Subject::operator=(other);
    // delete old objects
    for (const Order* ord : *orders) { delete ord; }
    orders->clear();

    // deep copy orders
    for (const Order* ord : *other.orders) {
      orders->push_back(ord->clone());
    }
    lastAddedOrder = orders->empty() ? nullptr : orders->back();
  }
  return *this;
}

OrdersList::~OrdersList() {
  for (const Order* ord : *orders) { delete ord; }
  delete orders;

  orders = nullptr;
  lastAddedOrder = nullptr;
}

// --- MANAGEMENT ---
void OrdersList::addOrder(Order* order) {
  if (!order) return;

  orders->push_back(order);
  lastAddedOrder = order;

  for (Observer* obs : *observers) order->attach(obs);
  notify();
}

void OrdersList::removeOrder(int index) {
  if (!validateIndex(index)) return;
  orders->erase(orders->begin() + index);
}

void OrdersList::moveOrder(int oldIdx, int newIdx) {
  // validate indices
  if (!validateIndex(oldIdx) || !validateIndex(newIdx)) return;

  // remove order from the list and reinsert it at the new index
  Order* order = orders->at(oldIdx);
  orders->erase(orders->begin() + oldIdx);
  orders->insert(orders->begin() + newIdx, order);
}

// --- UTILITY ---
std::string OrdersList::stringToLog() const {
  if (lastAddedOrder) return "OrdersList added: " + lastAddedOrder->stringToLog();
  return "OrdersList has no orders saved.";
}

bool OrdersList::validateIndex(int index) {
  return index >= 0 && index < orders->size();
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const OrdersList& ordLst) {
  os << "Orders List[" << ordLst.orders->size() << " orders:\n";
  for (size_t i = 0; i < ordLst.orders->size(); i++) {
    os << "  - " << (i + 1) << ". " << *(*ordLst.orders)[i] << "\n";
  }
  os << "]";
  return os;
}
