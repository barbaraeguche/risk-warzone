#pragma once
#include "Map.h"

#include <string>
#include <iostream>
#include <vector>
#include "Player.h"

class PlayerStrategy {
protected:
    Player* player;
    std::string* strategyDescription;
public:
    PlayerStrategy(Player* p, std::string* desc);
    PlayerStrategy(const PlayerStrategy& other);
    PlayerStrategy& operator=(const PlayerStrategy& other);
    virtual ~PlayerStrategy();

    virtual void issueOrder() = 0;
    virtual std::vector<Territory*> toDefend() = 0;
    virtual std::vector<Territory*> toAttack() = 0;

    std::string* getStrategyDescription() const;
    friend std::ostream& operator<<(std::ostream& os, const PlayerStrategy& playerStrategy);
};

class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy(Player* p);
    void issueOrder() override;
    std::vector<Territory*> toDefend() override;
    std::vector<Territory*> toAttack() override;
};

class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy(Player* p);
    void issueOrder() override;
    std::vector<Territory*> toDefend() override;
    std::vector<Territory*> toAttack() override;
};

class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy(Player* p);
    void issueOrder() override;
    std::vector<Territory*> toDefend() override;
    std::vector<Territory*> toAttack() override;
};

class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy(Player* p);
    void issueOrder() override;
    std::vector<Territory*> toDefend() override;
    std::vector<Territory*> toAttack() override;
};

class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy(Player* p);
    void issueOrder() override;
    std::vector<Territory*> toDefend() override;
    std::vector<Territory*> toAttack() override;
};
