#pragma once
#include "LoggingObserver.h"
#include "Player.h"

#include <memory>
#include <vector>

struct NegotiationRecord {
  Player* player1;
  Player* player2;
};


/*
Abstract base class for all order types.
*/
class Order : public Subject, public ILoggable {
protected:
  std::string* type;
  std::string* description;
  std::string* effect;

  // needs to be cleared every turn in the game engine
  static std::unique_ptr<std::vector<NegotiationRecord>> negotiationRecords;

public:
  Order();
  Order(const Order& other); // copy constructor
  Order& operator=(const Order& other); // assignment operator
  virtual ~Order(); // destructor

  // getters
  std::string getEffect() const;

  // validation
  virtual bool validate() = 0; // validates an order is valid based on the current game state

  // management
  virtual void execute() = 0; // executes the order, assuming it has been validated

  // helper
  virtual Order* clone() const = 0;

  // utility
  void saveEffect(const std::string& eff);
  std::string stringToLog() const override;
  static void clearNegotiationRecords();

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Order& ord);
};


class OrderDeploy : public Order {
private:
  Player* player;
  Territory* target;
  int* soldiers;

public:
  OrderDeploy(Player* play, Territory* targ, int* sold);
  OrderDeploy(const OrderDeploy& other); // copy constructor
  OrderDeploy& operator=(const OrderDeploy& other); // assignment operator
  ~OrderDeploy() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrderAdvance : public Order {
private:
  Player* player;
  Territory* source;
  Territory* target;
  int* soldiers;

public:
  OrderAdvance(Player* play, Territory* src, Territory* targ, int* sold);
  OrderAdvance(const OrderAdvance& other); // copy constructor
  OrderAdvance& operator=(const OrderAdvance& other); // assignment operator
  ~OrderAdvance() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrderAirlift : public Order {
private:
  Player* player;
  Territory* source;
  Territory* target;
  int* soldiers;

public:
  OrderAirlift(Player* play, Territory* src, Territory* targ, int* sold);
  OrderAirlift(const OrderAirlift& other); // copy constructor
  OrderAirlift& operator=(const OrderAirlift& other); // assignment operator
  ~OrderAirlift() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrderBomb : public Order {
private:
  Player* player;
  Territory* target;

public:
  OrderBomb(Player* play, Territory* targ);
  OrderBomb(const OrderBomb& other); // copy constructor
  OrderBomb& operator=(const OrderBomb& other); // assignment operator
  ~OrderBomb() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrderBlockade : public Order {
private:
  Player* nPlayer;   // neutral player
  Player* player;
  Territory* target;

public:
  OrderBlockade(Player* nPlay, Player* play, Territory* targ);
  OrderBlockade(const OrderBlockade& other); // copy constructor
  OrderBlockade& operator=(const OrderBlockade& other); // assignment operator
  ~OrderBlockade() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrderNegotiate : public Order {
private:
  Player* tPlayer; // target player
  Player* player;

public:
  OrderNegotiate(Player* tPlay, Player* play);
  OrderNegotiate(const OrderNegotiate& other); // copy constructor
  OrderNegotiate& operator=(const OrderNegotiate& other); // assignment operator
  ~OrderNegotiate() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrderCheat : public Order {
private:
  Player* player;

public:
  OrderCheat(Player* play);
  OrderCheat(const OrderCheat& other); // copy constructor
  OrderCheat& operator=(const OrderCheat& other); // assignment operator
  ~OrderCheat() override; // destructor

  // validation
  bool validate() override;

  // management
  void execute() override;

  // helper
  Order* clone() const override;
};


class OrdersList : public Subject, public ILoggable {
public:
  std::vector<Order*>* orders;

  OrdersList();
  OrdersList(const OrdersList& other); // copy constructor
  OrdersList& operator=(const OrdersList& other); // assignment operator
  ~OrdersList() override; // destructor

  // management
  void addOrder(Order* order);
  void removeOrder(int index);
  void moveOrder(int oldIdx, int newIdx); // move an order from one index to another

  // utility
  std::string stringToLog() const override;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const OrdersList& ordLst);

private:
  Order* lastAddedOrder;

  // utility
  bool validateIndex(int index);
};

// namespaces
namespace ORDER_TYPES {
  constexpr auto DEPLOY = "deploy";
  constexpr auto ADVANCE = "advance";
  constexpr auto AIRLIFT = "airlift";
  constexpr auto BOMB = "bomb";
  constexpr auto BLOCKADE = "blockade";
  constexpr auto NEGOTIATE = "negotiate";
  constexpr auto CHEAT = "cheat";
}

// free function
void testOrderExecution();
void testOrdersLists();
