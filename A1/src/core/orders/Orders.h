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
class Orders {
public:
  Orders();

  // validates an order is valid based on the current gamestate
  virtual bool validate() = 0;

  // executes the order, assuming it has been validated
  virtual void execute() = 0;
  virtual Orders* clone() const = 0;

  //Make destructor virtual to avoid memory leaks
  virtual ~Orders() = default;

  //stream insertion operator for printing orders
  friend std::ostream& operator<<(std::ostream& os, const Orders& order);

protected:
  //order type and description
  std::string* type;
  std::string* description;

};

class OrderDeploy : public Orders {
public:
  OrderDeploy();
  OrderDeploy(const OrderDeploy& other);
  ~OrderDeploy() override;
  bool validate() override;
  void execute() override;
  OrderDeploy& operator=(const OrderDeploy& other);
  Orders* clone() const override;
};

class OrderAdvance : public Orders {
public:
  OrderAdvance();
  OrderAdvance(const OrderAdvance& other);
  ~OrderAdvance() override;
  bool validate() override;
  void execute() override;
  OrderAdvance& operator=(const OrderAdvance& other);
  Orders* clone() const override;
};

class OrderBomb : public Orders {
public:
  OrderBomb();
  OrderBomb(const OrderBomb& other);
  ~OrderBomb() override;
  bool validate() override;
  void execute() override;
  OrderBomb& operator=(const OrderBomb& other);
  Orders* clone() const override;
};

class OrderBlockade : public Orders {
public:
  OrderBlockade();
  OrderBlockade(const OrderBlockade& other);
  ~OrderBlockade() override;
  bool validate() override;
  void execute() override;
  OrderBlockade& operator=(const OrderBlockade& other);
  Orders* clone() const override;
};

class OrderAirlift : public Orders {
public:
  OrderAirlift();
  OrderAirlift(const OrderAirlift& other);
  ~OrderAirlift() override;
  bool validate() override;
  void execute() override;
  OrderAirlift& operator=(const OrderAirlift& other);
  Orders* clone() const override;
};

class OrderNegotiate : public Orders {
public:
  OrderNegotiate();
  OrderNegotiate(const OrderNegotiate& other);
  ~OrderNegotiate() override;
  bool validate() override;
  void execute() override;
  OrderNegotiate& operator=(const OrderNegotiate& other);
  Orders* clone() const override;
};

class OrdersList {
public:
  std::vector<Orders*>* orders;

  OrdersList();
  OrdersList(const OrdersList& other);
  OrdersList& operator=(const OrdersList& other);
  friend std::ostream& operator<<(std::ostream& os, const OrdersList& orderlist);

  //add an order to the list
  void add(Orders* order);

  //remove an order from the list at a given index
  void remove(int index);

  //move an order from one index to another
  void move(int fromIndex, int toIndex);

  ~OrdersList();

private:

  //helper functions for validating indices
  bool validateIndex(int index);
};