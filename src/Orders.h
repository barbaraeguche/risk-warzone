#pragma once
#include "Player.h"
#include "Map.h"

#include <vector>
#include <iostream>
#include <memory>

#include "LoggingObserver.h"

struct NegotiationRecord {
    Player* player1;
    Player* player2;
};

/*
Abstract base class for all order types.
*/
class Order : public Subject, public ILoggable {
public:
  Order();
  Order(const Order& other);
  Order& operator=(const Order& other);

  // validates an order is valid based on the current gamestate
  virtual bool validate() = 0;

  // executes the order, assuming it has been validated
  virtual void execute() = 0;
  virtual Order* clone() const = 0;

  //Make destructor virtual to avoid memory leaks
  virtual ~Order();

  //stream insertion operator for printing orders
  friend std::ostream& operator<<(std::ostream& os, const Order& order);

  //static method to access negotiation records
  static void clearNegotiationRecords();

  void saveEffect(const std::string& effectDescription);
  std::string getEffect() const;
  std::string stringToLog() const override;

protected:
  // Needs to be cleared every turn in the game engine
  static std::unique_ptr<std::vector<NegotiationRecord>> negotiationRecords;

  //order type and description
  std::string* type;
  std::string* description;
  std::string* effect;

};

class OrderDeploy : public Order {
private:
  Player* player;
  Territory* target;
  int* soldiers;
public:
  OrderDeploy(Player* player, Territory* target, int* soldiers);
  OrderDeploy(const OrderDeploy& other);
  ~OrderDeploy() override;
  bool validate() override;
  void execute() override;
  OrderDeploy& operator=(const OrderDeploy& other);
  Order* clone() const override;
};

class OrderAdvance : public Order {
private:
  Player* player;
  Territory* source;
  Territory* target;
  int* soldiers;
public:
  OrderAdvance(Player* player, Territory* source, Territory* target, int* soldiers);
  OrderAdvance(const OrderAdvance& other);
  ~OrderAdvance() override;
  bool validate() override;
  void execute() override;
  OrderAdvance& operator=(const OrderAdvance& other);
  Order* clone() const override;
};

class OrderBomb : public Order {
private:
  Player* player;
  Territory* target;
public:
  OrderBomb(Territory* target, Player* player);
  OrderBomb(const OrderBomb& other);
  ~OrderBomb() override;
  bool validate() override;
  void execute() override;
  OrderBomb& operator=(const OrderBomb& other);
  Order* clone() const override;
};

class OrderBlockade : public Order {
private:
  Player* nPlayer; // Neutral player
  Player* player;
  Territory* target;
public:
  OrderBlockade(Player* nPlayer, Player* player, Territory* target);
  OrderBlockade(const OrderBlockade& other);
  ~OrderBlockade() override;
  bool validate() override;
  void execute() override;
  OrderBlockade& operator=(const OrderBlockade& other);
  Order* clone() const override;
};

class OrderAirlift : public Order {
private:
  Player* player;
  Territory* source;
  Territory* target;
  int* soldiers;
public:
  OrderAirlift(Player* player, Territory* source, Territory* target, int* soldiers);
  OrderAirlift(const OrderAirlift& other);
  ~OrderAirlift() override;
  bool validate() override;
  void execute() override;
  OrderAirlift& operator=(const OrderAirlift& other);
  Order* clone() const override;
};

class OrderNegotiate : public Order {
private:
  Player* player;
  Player* targetPlayer;
public:
  OrderNegotiate(Player* player, Player* targetPlayer);
  OrderNegotiate(const OrderNegotiate& other);
  ~OrderNegotiate() override;
  bool validate() override;
  void execute() override;
  OrderNegotiate& operator=(const OrderNegotiate& other);
  Order* clone() const override;
};

class OrderCheat : public Order {
private:
  Player* player;
public:
  OrderCheat(Player* player);
  OrderCheat(const OrderCheat& other);
  ~OrderCheat() override;
  bool validate() override;
  void execute() override;
  OrderCheat& operator=(const OrderCheat& other);
  Order* clone() const override;
};

class OrdersList : public Subject, public ILoggable {
public:
  std::vector<Order*>* orders;

  OrdersList();
  OrdersList(const OrdersList& other);
  OrdersList& operator=(const OrdersList& other);
  friend std::ostream& operator<<(std::ostream& os, const OrdersList& orderlist);

  //add an order to the list
  void add(Order* order);

  //remove an order from the list at a given index
  void remove(int index);

  //move an order from one index to another
  void move(int fromIndex, int toIndex);

  ~OrdersList();

private:

  //helper functions for validating indices
  bool validateIndex(int index);
  Order* lastAddedOrder;
public:
  std::string stringToLog() const override;
};