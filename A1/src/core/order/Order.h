#pragma once

#include <vector>
#include <iostream>

class Order {
public:
  virtual bool validate() = 0;
  virtual void execute() = 0;
  virtual Order& operator=(const Order& other) = 0;
  virtual std::ostream& print(std::ostream& os) const = 0;
  virtual ~Order() = default;
};

class OrderDeploy : public Order {
public:
  OrderDeploy();
  OrderDeploy(const OrderDeploy& other);
  bool validate() override;
  void execute() override;
  OrderDeploy& operator=(const OrderDeploy& other);
  std::ostream& print(std::ostream& os) const override;
};

class OrderAdvance : public Order {
public:
  OrderAdvance();
  OrderAdvance(const OrderAdvance& other);
  bool validate() override;
  void execute() override;
  OrderAdvance& operator=(const OrderAdvance& other);
  std::ostream& print(std::ostream& os) const override;
};

class OrderBomb : public Order {
public:
  OrderBomb();
  OrderBomb(const OrderBomb& other);
  bool validate() override;
  void execute() override;
  OrderBomb& operator=(const OrderBomb& other);
  std::ostream& print(std::ostream& os) const override;
};

class OrderBlockade : public Order {
public:
  OrderBlockade();
  OrderBlockade(const OrderBlockade& other);
  bool validate() override;
  void execute() override;
  OrderBlockade& operator=(const OrderBlockade& other);
  std::ostream& print(std::ostream& os) const override;
};

class OrderAirlift : public Order {
public:
  OrderAirlift();
  OrderAirlift(const OrderAirlift& other);
  bool validate() override;
  void execute() override;
  OrderAirlift& operator=(const OrderAirlift& other);
  std::ostream& print(std::ostream& os) const override;
};

class OrderNegotiate : public Order {
public:
  OrderNegotiate();
  OrderNegotiate(const OrderNegotiate& other);
  bool validate() override;
  void execute() override;
  OrderNegotiate& operator=(const OrderNegotiate& other);
  std::ostream& print(std::ostream& os) const override;
};

class OrderList {
public:
  std::vector<Order*>* orders;

  OrderList();
  OrderList(const OrderList& other);
  OrderList& operator=(const OrderList& other);
  std::ostream& print(std::ostream& os) const;
  void add(Order* order);
  void remove(int index);
  void move(int fromIndex, int toIndex);
  ~OrderList();
};