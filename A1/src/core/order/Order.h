#pragma once

#include <vector>
#include <iostream>

/*
assignement operator?
validate method?
*/

/*
Abstract base class for all order types.
*/
class Order {
public:
  Order();

  // validates an order is valid based on the current gamestate
  virtual bool validate() = 0;

  // executes the order, assuming it has been validated
  virtual void execute() = 0;
  virtual Order* clone() const = 0;

  //Make destructor virtual to avoid memory leaks
  virtual ~Order() = default;

  //stream insertion operator for printing orders
  friend std::ostream& operator<<(std::ostream& os, const Order& order);

protected:
  //order type and description
  std::string* type;
  std::string* description;

};

class OrderDeploy : public Order {
public:
  OrderDeploy();
  OrderDeploy(const OrderDeploy& other);
  ~OrderDeploy();
  bool validate() override;
  void execute() override;
  OrderDeploy& operator=(const OrderDeploy& other);
  Order* clone() const override;
};

class OrderAdvance : public Order {
public:
  OrderAdvance();
  OrderAdvance(const OrderAdvance& other);
  ~OrderAdvance();
  bool validate() override;
  void execute() override;
  OrderAdvance& operator=(const OrderAdvance& other);
  Order* clone() const override;
};

class OrderBomb : public Order {
public:
  OrderBomb();
  OrderBomb(const OrderBomb& other);
  ~OrderBomb();
  bool validate() override;
  void execute() override;
  OrderBomb& operator=(const OrderBomb& other);
  Order* clone() const override;
};

class OrderBlockade : public Order {
public:
  OrderBlockade();
  OrderBlockade(const OrderBlockade& other);
  ~OrderBlockade();
  bool validate() override;
  void execute() override;
  OrderBlockade& operator=(const OrderBlockade& other);
  Order* clone() const override;
};

class OrderAirlift : public Order {
public:
  OrderAirlift();
  OrderAirlift(const OrderAirlift& other);
  ~OrderAirlift();
  bool validate() override;
  void execute() override;
  OrderAirlift& operator=(const OrderAirlift& other);
  Order* clone() const override;
};

class OrderNegotiate : public Order {
public:
  OrderNegotiate();
  OrderNegotiate(const OrderNegotiate& other);
  ~OrderNegotiate();
  bool validate() override;
  void execute() override;
  OrderNegotiate& operator=(const OrderNegotiate& other);
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