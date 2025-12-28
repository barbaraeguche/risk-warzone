#include "PlayerStrategies.h"
#include "Cards.h"
#include "Map.h"
#include "Player.h"

#include <algorithm>
#include <vector>


// ==================== PlayerStrategy Class Implementation ====================
PlayerStrategy::PlayerStrategy(Player* play, std::string* strat) :
  player(play),
  strategy(strat) {}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other) :
  player(other.player),
  strategy(new std::string(*other.strategy)) {}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
  if (this != &other) {
    delete strategy;
    player = other.player;
    strategy = new std::string(*(other.strategy));
  }
  return *this;
}

PlayerStrategy::~PlayerStrategy() {
  delete strategy;
}

// --- GETTERS ---
std::string* PlayerStrategy::getStrategy() const {
  return strategy;
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const PlayerStrategy& strat) {
  os << "Player Strategy[" << (strat.strategy ? *strat.strategy : "Neutral Player") << "]" << std::endl;
  return os;
}


// ==================== HumanPlayerStrategy Class Implementation ====================
HumanPlayerStrategy::HumanPlayerStrategy(Player* play) :
  PlayerStrategy(play, new std::string("Human Player Strategy")) {}

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other) :
  PlayerStrategy(other) {}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
  PlayerStrategy::operator=(other);
  return *this;
}

// --- STRATEGY MANAGEMENT ---
void HumanPlayerStrategy::issueOrder() {
  std::cout << "Human player issuing order...\n";
  if (player->getReinforcementPool() > 0) {
    std::cout << "You have " << player->getReinforcementPool() << " armies in your reinforcement pool.\n";
    std::cout << "Your territories:\n";
    for (const auto terr : player->getTerritories()) {
      std::cout << " - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
    }

    std::string territoryName;
    Territory* target = nullptr;

    std::cout << "Enter the name of the territory to deploy armies to: ";
    std::cin >> territoryName;

    for (const auto terr : player->getTerritories()) {
      if (terr->getName() == territoryName) {
        target = terr;
        break;
      }
    }

    if (target) {
      int armies;
      std::cout << "Enter number of armies to deploy: ";
      std::cin >> armies;

      if (armies > 0 && armies <= player->getReinforcementPool()) {
        player->issueDeployOrder(target, armies);
        player->setReinforcementPool(player->getReinforcementPool() - armies);
        std::cout << "Deployed " << armies << " armies to " << target->getName() << ".\n";
      } else {
        std::cout << "Invalid number of armies.\n";
      }
    } else {
      std::cout << "Territory not found.\n";
    }
  }

  // advance orders
  std::cout << "Your territories:\n";
  for (const auto terr : player->getTerritories()) {
    std::cout << " - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
  }

  std::cout << "Enemy territories:\n";
  for (const auto terr : this->toAttack()) {
    std::cout << " - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
  }

  std::string sourceName;
  std::cout << "Enter the name of the source territory for advance order (or 'skip' to skip): ";
  std::cin >> sourceName;

  if (sourceName != "skip") {
    Territory* source = nullptr;
    for (const auto terr : player->getTerritories()) {
      if (terr->getName() == sourceName) {
        source = terr;
        break;
      }
    }

    if (source) {
      std::string targetName;
      Territory* target = nullptr;

      std::cout << "Enter the name of the target territory for advance order: ";
      std::cin >> targetName;
      for (const auto& terr : this->toAttack()) {
        if (terr->getName() == targetName) {
          target = terr;
          break;
        }
      }

      if (target) {
        int armies;
        std::cout << "Enter number of armies to advance: ";
        std::cin >> armies;

        if (armies > 0 && armies < source->getArmies()) {
          player->issueAdvanceOrder(source, target, armies);
          std::cout << "Issued advance order from " << source->getName()
                    << " to " << target->getName() << " with " << armies << " armies.\n";
        } else {
          std::cout << "Invalid number of armies.\n";
        }
      } else {
        std::cout << "Target territory not found.\n";
      }
    } else {
      std::cout << "Source territory not found.\n";
    }
  }

  // playing cards
  const Hand* hand = player->getHand();
  if (hand->size() > 0) {
    std::cout << "You have the following cards in your hand:\n";
    for (size_t i = 0; i < hand->size(); i++) {
      std::cout << i << ": " << hand->getCards()->at(i)->getType() << "\n";
    }

    int cardIndex = -2;
    std::cout << "Enter the index of the card to play (-1 to skip): ";
    while (cardIndex < -1 || cardIndex >= hand->size()) {
      std::cin >> cardIndex;
    }

    if (cardIndex != -1) {
      std::cout << "Played card: " << hand->getCards()->at(cardIndex)->getType() << "\n";
      player->playCard(cardIndex, player->getDeck());
    }
  }

  player->setCanIssueOrder(false); // end turn after issuing orders
}

std::vector<Territory*> HumanPlayerStrategy::toDefend() {
  return player->getTerritories();
}

std::vector<Territory*> HumanPlayerStrategy::toAttack() {
  std::vector<Territory*> attackList;

  for (const auto terr : player->getTerritories()) {
    std::vector<Territory*> adjacent = terr->getAdjTerritories();

    for (auto adj : adjacent) {
      if (adj->getOwner() != player) {
        attackList.push_back(adj);
      }
    }
  }
  return attackList;
}


// ==================== AggressivePlayerStrategy Class Implementation ====================
AggressivePlayerStrategy::AggressivePlayerStrategy(Player* play) :
  PlayerStrategy(play, new std::string("Aggressive Player Strategy")) {}

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other) :
  PlayerStrategy(other) {}

AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
  PlayerStrategy::operator=(other);
  return *this;
}

// --- STRATEGY MANAGEMENT ---
void AggressivePlayerStrategy::issueOrder() {
  if (player->getReinforcementPool() > 0) {
    std::vector<Territory*> defendList = this->toDefend();

    if (!defendList.empty()) {
      // deploy to first territory in defend list
      Territory* target = defendList[0];
      const int armiesToDeploy = player->getReinforcementPool(); // deploy all available armies

      std::cout << player->getName() << "  issuing deploy order: " << armiesToDeploy
                << " armies to " << target->getName() << "\n";
      player->issueDeployOrder(target, armiesToDeploy);
    }

    player->setReinforcementPool(0);
    return;
  }

  Territory* strongest = player->toDefend().front();
  const std::vector<Territory*> adjacent = strongest->getAdjTerritories();

  for (const auto adj : adjacent) {
    if (player->ownsTerritory(adj) && adj->getArmies() > 1) {
      const int armiesToMove = adj->getArmies() - 1;
      if (armiesToMove > 0) player->issueAdvanceOrder(adj, strongest, armiesToMove);
    }
  }

  for (const auto adj : player->toAttack()) {
    const int armiesToAttack = strongest->getArmies() - 1; // leave 1 army behind
    if (armiesToAttack > 0) {
      std::cout << player->getName() << " issuing advance order (attack): " << armiesToAttack
                << " armies from " << strongest->getName() << " to " << adj->getName() << "\n";
      player->issueAdvanceOrder(strongest, adj, armiesToAttack);
    }
  }

  // priority 3: play cards if we have any
  const Hand* hand = player->getHand();
  if (hand->size() > 0) {
    // play aggressive card if available
    for (const auto card : *hand->getCards()) {
      if (card->getType() == CARD_TYPES::BOMB) {
        card->play(player, player->getOrders(), player->getDeck());
        std::cout << "  Playing aggressive card: " << card->getType() << "\n";
        break; // play only one card per turn
      }
    }
  }

  player->setCanIssueOrder(false); // end turn after issuing
}

std::vector<Territory*> AggressivePlayerStrategy::toDefend() {
  std::vector<Territory*> defendList;

  Territory* strongest = strongestTerritory();
  if (strongest) defendList.push_back(strongest);

  return defendList;
}

std::vector<Territory*> AggressivePlayerStrategy::toAttack() {
  std::vector<Territory*> attackList;

  Territory* strongest = strongestTerritory();
  if (!strongest) return attackList;

  for (auto* adj : strongest->getAdjTerritories()) {
    if (adj->getOwner() != player) {
      attackList.push_back(adj);
    }
  }

  return attackList;
}

// --- UTILITY ---
Territory* AggressivePlayerStrategy::strongestTerritory() {
  const auto& territories = player->getTerritories();

  // sort by number of armies descending
  std::vector<Territory*> sorted = territories;
  std::ranges::sort(sorted, std::greater{}, &Territory::getArmies);

  // find first strongest territory that has at least one neighboring enemy
  for (auto* terr : sorted) {
    for (const auto* adj : terr->getAdjTerritories()) {
      if (adj->getOwner() != player) {
        return terr;
      }
    }
  }

  // if none found, fallback: return the absolute strongest
  return sorted.empty() ? nullptr : sorted.front();
}


// ==================== BenevolentPlayerStrategy Class Implementation ====================
BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player* play) :
  PlayerStrategy(play, new std::string("Benevolent Player Strategy")) {}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other) :
  PlayerStrategy(other) {}

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& other) {
  PlayerStrategy::operator=(other);
  return *this;
}

// --- STRATEGY MANAGEMENT ---
void BenevolentPlayerStrategy::issueOrder() {
  // deploy troops to the weakest territories
  const std::vector<Territory*> defendList = this->toDefend();
  const std::vector<Territory*> territories = player->getTerritories();

  if (player->getReinforcementPool() > 0) {
    if (!defendList.empty()) {
      // deploy to first territory in defend list
      Territory* target = defendList[0];
      const int armiesToDeploy = player->getReinforcementPool(); // deploy all available armies

      std::cout << player->getName() << " issuing deploy order: " << armiesToDeploy
                << " armies to " << target->getName() << "\n";
      player->issueDeployOrder(target, armiesToDeploy);
    }

    player->setReinforcementPool(0);
    return;
  }

  constexpr int MAX_ADVANCES = 5;
  int advancesIssued = 0;

  std::unordered_set<Territory*> usedSources;

  if (!defendList.empty() && territories.size() > 1) {
    for (Territory* target : defendList) {
      if (advancesIssued >= MAX_ADVANCES) break;
      Territory* bestSource = nullptr;

      for (Territory* source : territories) {
        if (usedSources.contains(source)) continue;
        if (source->getArmies() <= 1) continue;

        bool adjacent = false;
        for (const Territory* adj : source->getAdjTerritories()) {
          if (adj == target) {
            adjacent = true;
            break;
          }
        }

        if (!adjacent) continue;
        if (!bestSource || source->getArmies() > bestSource->getArmies()) {
          bestSource = source;
        }
      }

      if (bestSource) {
        const int armiesToMove = bestSource->getArmies() / 2;

        if (armiesToMove > 0) {
          std::cout << player->getName() << " issuing advance (controlled): moving " << armiesToMove << " from "
                    << bestSource->getName() << " to " << target->getName() << "\n";

          player->issueAdvanceOrder(bestSource, target, armiesToMove);
          usedSources.insert(bestSource);
          advancesIssued++;
        }
      }
    }
  }

  // benevolent player only plays cards that don't involve attacking
  // priority 3: play cards if we have any
  const Hand* hand = player->getHand();
  if (hand->size() > 0) {
    // play non-aggressive card if available
    for (const auto card : *hand->getCards()) {
      if (card->getType() != CARD_TYPES::BOMB) {
        card->play(player, player->getOrders(), player->getDeck());
        std::cout << "  Playing non-aggressive card: " << card->getType() << "\n";
        break; // play only one card per turn
      }
    }
  }

  player->setCanIssueOrder(false); // end turn after issuing
}

std::vector<Territory*> BenevolentPlayerStrategy::toDefend() {
  std::vector<Territory*> defendList = player->getTerritories();
  std::ranges::sort(defendList, std::less{});
  return defendList;
}

std::vector<Territory*> BenevolentPlayerStrategy::toAttack() {
  std::vector<Territory*> attackList;

  for (const auto terr : player->getTerritories()) {
    std::vector<Territory*> adjacent = terr->getAdjTerritories();

    for (auto adj : adjacent) {
      if (adj->getOwner() != player) {
        attackList.push_back(adj);
      }
    }
  }
  return attackList;
}


// ==================== NeutralPlayerStrategy Class Implementation ====================
NeutralPlayerStrategy::NeutralPlayerStrategy(Player* play) :
  PlayerStrategy(play, new std::string("Neutral Player Strategy")) {}

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other) :
  PlayerStrategy(other) {}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
  PlayerStrategy::operator=(other);
  return *this;
}

// --- STRATEGY MANAGEMENT ---
void NeutralPlayerStrategy::issueOrder() {
  if (player->getReinforcementPool() > 0) {
    const std::vector<Territory*> defendList = this->toDefend();

    if (!defendList.empty()) {
      // deploy to first territory in defend list
      Territory* target = defendList[0];
      const int armiesToDeploy = player->getReinforcementPool(); // deploy up to 3 at a time

      std::cout << player->getName() << " issuing deploy order: " << armiesToDeploy
                << " armies to " << target->getName() << "\n";
      player->issueDeployOrder(target, armiesToDeploy);
    }

    player->setReinforcementPool(0);
    return;
  }

  // neutral player does not attack or play any cards
  player->setCanIssueOrder(false); // end turn after issuing
}

std::vector<Territory*> NeutralPlayerStrategy::toDefend() {
  // neutral player only passively defends when attacked
  return player->getTerritories();
}

std::vector<Territory*> NeutralPlayerStrategy::toAttack() {
  // neutral player never attacks, it only acts as defensive buffer
  std::vector<Territory*> attackList;

  for (const auto terr : player->getTerritories()) {
    std::vector<Territory*> adjacent = terr->getAdjTerritories();

    for (auto adj : adjacent) {
      if (adj->getOwner() != player) {
        attackList.push_back(adj);
      }
    }
  }
  return attackList;
}


// ==================== CheaterPlayerStrategy Class Implementation ====================
CheaterPlayerStrategy::CheaterPlayerStrategy(Player* play) :
  PlayerStrategy(play, new std::string("Cheater Player Strategy")) {}

CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other) :
  PlayerStrategy(other) {}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
  PlayerStrategy::operator=(other);
  return *this;
}

// --- STRATEGY MANAGEMENT ---
void CheaterPlayerStrategy::issueOrder() {
  if (player->getReinforcementPool() > 0) {
    const std::vector<Territory*> defendList = this->toDefend();

    if (!defendList.empty()) {
      // deploy to first territory in defend list
      Territory* target = defendList[0];
      const int armiesToDeploy = player->getReinforcementPool(); // deploy up to 3 at a time

      std::cout << player->getName() << " issuing deploy order: " << armiesToDeploy
                << " armies to " << target->getName() << "\n";
      player->issueDeployOrder(target, armiesToDeploy);
    }

    player->setReinforcementPool(0);
    return;
  }

  // issue cheat order
  player->issueCheatOrder();
  player->setCanIssueOrder(false); // end turn after issuing
}

std::vector<Territory*> CheaterPlayerStrategy::toDefend() {
  return player->getTerritories();
}

std::vector<Territory*> CheaterPlayerStrategy::toAttack() {
  std::vector<Territory*> attackList;

  for (const auto terr : player->getTerritories()) {
    std::vector<Territory*> adjacent = terr->getAdjTerritories();

    for (auto adj : adjacent) {
      if (adj->getOwner() != player) {
        attackList.push_back(adj);
      }
    }
  }
  return attackList;
}
