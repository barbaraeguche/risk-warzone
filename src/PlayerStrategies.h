#pragma once
#include "Map.h"

#include <iostream>
#include <string>
#include <vector>

class PlayerStrategy {
protected:
  Player* player;
  std::string* strategy;

public:
  PlayerStrategy(Player* play, std::string* strat);
  PlayerStrategy(const PlayerStrategy& other); // copy constructor
  PlayerStrategy& operator=(const PlayerStrategy& other); // assignment operator
  virtual ~PlayerStrategy(); // destructor

  // getters
  std::string* getStrategy() const;

  // strategy management
  virtual void issueOrder() = 0;
  virtual std::vector<Territory*> toDefend() = 0;
  virtual std::vector<Territory*> toAttack() = 0;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const PlayerStrategy& strat);
};


class HumanPlayerStrategy : public PlayerStrategy {
public:
  HumanPlayerStrategy(Player* play);
  HumanPlayerStrategy(const HumanPlayerStrategy& other); // copy constructor
  HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other); // assignment operator

  // strategy management
  void issueOrder() override;
  std::vector<Territory*> toDefend() override;
  std::vector<Territory*> toAttack() override;
};


class AggressivePlayerStrategy : public PlayerStrategy {
public:
  AggressivePlayerStrategy(Player* play);
  AggressivePlayerStrategy(const AggressivePlayerStrategy& other); // copy constructor
  AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other); // assignment operator

  // strategy management
  void issueOrder() override;
  std::vector<Territory*> toDefend() override;
  std::vector<Territory*> toAttack() override;

private:
  // utility
  Territory* strongestTerritory();
};


class BenevolentPlayerStrategy : public PlayerStrategy {
public:
  BenevolentPlayerStrategy(Player* play);
  BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other); // copy constructor
  BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other); // assignment operator

  // strategy management
  void issueOrder() override;
  std::vector<Territory*> toDefend() override;
  std::vector<Territory*> toAttack() override;
};


class NeutralPlayerStrategy : public PlayerStrategy {
public:
  NeutralPlayerStrategy(Player* play);
  NeutralPlayerStrategy(const NeutralPlayerStrategy& other); // copy constructor
  NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other); // assignment operator

  // strategy management
  void issueOrder() override;
  std::vector<Territory*> toDefend() override;
  std::vector<Territory*> toAttack() override;
};


class CheaterPlayerStrategy : public PlayerStrategy {
public:
  CheaterPlayerStrategy(Player* play);
  CheaterPlayerStrategy(const CheaterPlayerStrategy& other); // copy constructor
  CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other); // assignment operator

  // strategy management
  void issueOrder() override;
  std::vector<Territory*> toDefend() override;
  std::vector<Territory*> toAttack() override;
};

// namespaces
namespace PLAYER_STRATEGIES {
  constexpr auto HUMAN = "human";
  constexpr auto AGGRESSIVE = "aggressive";
  constexpr auto BENEVOLENT = "benevolent";
  constexpr auto NEUTRAL = "neutral";
  constexpr auto CHEATER = "cheater";
}

// free function
void testPlayerStrategies();
