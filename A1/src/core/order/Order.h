#pragma once

#include <vector>
#include <iostream>

/*
store description of behavior as string
assignement operator?
validate method?
*/

/*
Abstract base class for all order types.
*/
class Order {
public:

  // validates an order is valid based on the current gamestate
  virtual bool validate() = 0;

  // executes the order, assuming it has been validated
  virtual void execute() = 0;
  virtual Order* clone() const = 0;

  //Make destructor virtual to avoid memory leaks
  virtual ~Order() = default;
};

class OrderDeploy : public Order {
public:
  OrderDeploy();
  OrderDeploy(const OrderDeploy& other);
  bool validate() override;
  void execute() override;
  OrderDeploy& operator=(const OrderDeploy& other);
  friend std::ostream& operator<<(std::ostream& os, const OrderDeploy& order);
  Order* clone() const override;
};

class OrderAdvance : public Order {
public:
  OrderAdvance();
  OrderAdvance(const OrderAdvance& other);
  bool validate() override;
  void execute() override;
  OrderAdvance& operator=(const OrderAdvance& other);
  friend std::ostream& operator<<(std::ostream& os, const OrderAdvance& order);
  Order* clone() const override;
};

class OrderBomb : public Order {
public:
  OrderBomb();
  OrderBomb(const OrderBomb& other);
  bool validate() override;
  void execute() override;
  OrderBomb& operator=(const OrderBomb& other);
  friend std::ostream& operator<<(std::ostream& os, const OrderBomb& order);
  Order* clone() const override;
};

class OrderBlockade : public Order {
public:
  OrderBlockade();
  OrderBlockade(const OrderBlockade& other);
  bool validate() override;
  void execute() override;
  OrderBlockade& operator=(const OrderBlockade& other);
  friend std::ostream& operator<<(std::ostream& os, const OrderBlockade& order);
  Order* clone() const override;
};

class OrderAirlift : public Order {
public:
  OrderAirlift();
  OrderAirlift(const OrderAirlift& other);
  bool validate() override;
  void execute() override;
  OrderAirlift& operator=(const OrderAirlift& other);
  friend std::ostream& operator<<(std::ostream& os, const OrderAirlift& order);
  Order* clone() const override;
};

class OrderNegotiate : public Order {
public:
  OrderNegotiate();
  OrderNegotiate(const OrderNegotiate& other);
  bool validate() override;
  void execute() override;
  OrderNegotiate& operator=(const OrderNegotiate& other);
  friend std::ostream& operator<<(std::ostream& os, const OrderNegotiate& order);
  Order* clone() const override;
};

class OrdersList {
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
};