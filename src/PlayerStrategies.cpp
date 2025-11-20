#include "PlayerStrategies.h"
#include "Cards.h"
#include "Map.h"
#include <algorithm>
#include <vector>

/*
    Need to think of mechanism for the game engine to know when to stop calling issueOrder
    Need to determine what are aggressive cards and non aggressive cards
    Need to implement advance orders
    Need to determine how to handle cards in general
*/

PlayerStrategy::PlayerStrategy(Player* p, std::string* desc) : player(p), strategyDescription(desc) {}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other) {
    player = other.player;
    strategyDescription = new std::string(*(other.strategyDescription));
}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
        player = other.player;
        delete strategyDescription;
        strategyDescription = new std::string(*(other.strategyDescription));
    }
    return *this;
}

PlayerStrategy::~PlayerStrategy() {
    delete strategyDescription;
}

std::string* PlayerStrategy::getStrategyDescription() const {
    return strategyDescription;
}

std::ostream& operator<<(std::ostream& os, const PlayerStrategy& playerStrategy) {
    os << "Player Strategy: " << *(playerStrategy.strategyDescription);
    return os;
}

HumanPlayerStrategy::HumanPlayerStrategy(Player* p) : PlayerStrategy(p, new std::string("Human Player Strategy")) {}

void HumanPlayerStrategy::issueOrder() {
    // Implementation for human player issuing orders
    std::cout << "Human player issuing order...\n";
    if(this->player->getReinforcementPool() > 0){
        std::cout << "You have " << this->player->getReinforcementPool() << " armies in your reinforcement pool.\n";
        std::cout << "Your territories:\n";
        for (auto t : this->player->getTerritories()) {
            std::cout << " - " << t->getName() << " (Armies: " << t->getArmies() << ")\n";
        }
        std::cout << "Enter the name of the territory to deploy armies to: ";
        std::string territoryName;
        std::cin >> territoryName;
        Territory* target = nullptr;
        for (auto t : this->player->getTerritories()) {
            if (t->getName() == territoryName) {
                target = t;
                break;
            }
        }
        if (target) {
            int armies;
            std::cout << "Enter number of armies to deploy: ";
            std::cin >> armies;
            if (armies > 0 && armies <= this->player->getReinforcementPool()) {
                this->player->issueDeployOrder(target, armies);
                this->player->setReinforcementPool(this->player->getReinforcementPool() - armies);
                std::cout << "Deployed " << armies << " armies to " << target->getName() << ".\n";
            } else {
                std::cout << "Invalid number of armies.\n";
            }
        } else {
            std::cout << "Territory not found.\n";
        }
    }

    // Advance orders
    std::cout << "Your territories:\n";
    for (auto t : this->player->getTerritories()) {
        std::cout << " - " << t->getName() << " (Armies: " << t->getArmies() << ")\n";
    }
    std::cout << "Enter the name of the source territory for advance order (or 'skip' to skip): ";
    std::string sourceName;
    std::cin >> sourceName;
    if (sourceName != "skip") {
        Territory* source = nullptr;
        for (auto t : this->player->getTerritories()) {
            if (t->getName() == sourceName) {
                source = t;
                break;
            }
        }
        if (source) {
            std::cout << "Enter the name of the target territory for advance order: ";
            std::string targetName;
            std::cin >> targetName;
            Territory* target = nullptr;
            for (auto& t : source->getAdjTerritories()) {
                if (t->getName() == targetName) {
                    target = t;
                    break;
                }
            }
            if (target) {
                int armies;
                std::cout << "Enter number of armies to advance: ";
                std::cin >> armies;
                if (armies > 0 && armies < source->getArmies()) {
                    this->player->issueAdvanceOrder(source, target, armies);
                    std::cout << "Issued advance order from " << source->getName() << " to " << target->getName() << " with " << armies << " armies.\n";
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

    // Playing cards
    Hand* hand = this->player->getHand();
    if (hand->size() > 0) {
        std::cout << "You have the following cards in your hand:\n";
        for (size_t i = 0; i < hand->size(); i++) {
            std::cout << i << ": " << hand->getCards()->at(i)->getType() << "\n";
        }
        std::cout << "Enter the index of the card to play (-1 to skip): ";
        int cardIndex;
        std::cin >> cardIndex;
        if (cardIndex >= 0 && cardIndex < hand->size()) {
            this->player->playCard(cardIndex, this->player->getDeck());
            std::cout << "Played card: " << hand->getCards()->at(cardIndex)->getType() << "\n";
        } else if (cardIndex != -1) {
            std::cout << "Invalid card index.\n";
        }
    }

}

std::vector<Territory*> HumanPlayerStrategy::toDefend() {
    // Implementation for human player determining territories to defend
    return this->player->getTerritories();
}

std::vector<Territory*> HumanPlayerStrategy::toAttack() {
    std::vector<Territory*> attackList;

    for(auto t : player->getTerritories()) {
        std::vector<Territory*> adjacent = t->getAdjTerritories();
        for(auto adj : adjacent) {
            if(adj->getOwner() != player) {
                attackList.push_back(adj);
            }
        }
    }

    return attackList;
}

AggressivePlayerStrategy::AggressivePlayerStrategy(Player* p) : PlayerStrategy(p, new std::string("Aggressive Player Strategy")) {}

void AggressivePlayerStrategy::issueOrder() {
    // Implementation for aggressive player issuing orders

    if (this->player->getReinforcementPool() > 0) {
        std::vector<Territory*> defendList = this->toDefend();
        
        if (!defendList.empty()) {
            Territory* target = defendList[0];
            int armiesToDeploy = this->player->getReinforcementPool(); // Deploy all available armies
            std::cout << "  Issuing Deploy order: " << armiesToDeploy << " armies to " << target->getName() << "\n";
            this->player->issueDeployOrder(target, armiesToDeploy);
            this->player->setReinforcementPool(0);
        } else {
            // No territories to defend, set reinforcement pool to 0
            this->player->setReinforcementPool(0);
        }
        return;
    }

    Territory* strongest = this->player->toDefend().front();
    std::vector<Territory*> adjacent = strongest->getAdjTerritories();
    for (auto adj: adjacent) {
        if (this->player->ownsTerritory(adj) && adj->getArmies() > 1) {
            int armiesToMove = adj->getArmies() - 1;
            if (armiesToMove > 0) {
                this->player->issueAdvanceOrder(adj, strongest, armiesToMove);
            }
        }
    }

    for (auto adj : this->player->toAttack()) {
            int armiesToAttack = strongest->getArmies() - 1; // Leave 1 army behind
            if (armiesToAttack > 0) {
                std::cout << "  Issuing Advance order (attack): " << armiesToAttack 
                          << " armies from " << strongest->getName() 
                          << " to " << adj->getName() << "\n";
                this->player->issueAdvanceOrder(strongest, adj, armiesToAttack);
            }
    }



  // Priority 3: Play cards if we have any
  Hand* hand = this->player->getHand();
  if (hand->size() > 0) {
    //Play aggressive card if available
    for(auto card : *hand->getCards()) {
        if(card->getType() == CARD_TYPES::BOMB) {
               card->play(this->player, this->player->getOrders(), this->player->getDeck());
               std::cout << "  Playing non-aggressive card: " << card->getType() << "\n";
               break; // Play only one card per turn
        }
    }
    }

}

std::vector<Territory*> AggressivePlayerStrategy::toDefend() {
    // Implementation for aggressive player determining territories to defend
    std::vector<Territory*> defendList;
    Territory* strongest = nullptr;
    for (auto t : player->getTerritories()) {
        if (strongest == nullptr || t->getArmies() > strongest->getArmies()) {
            strongest = t;
        }
    }
    if (strongest != nullptr) {
        defendList.push_back(strongest);
    }
    return defendList;
}

std::vector<Territory*> AggressivePlayerStrategy::toAttack() {
    // All adjacent territories of the territory with the most armies
    std::vector<Territory*> attackList;
    Territory* strongest = nullptr;
    for (auto t : player->getTerritories()) {
        if (strongest == nullptr || t->getArmies() > strongest->getArmies()) {
            strongest = t;
        }
    }
    if (strongest != nullptr) {
        std::vector<Territory*> adjacent = strongest->getAdjTerritories();
        for (auto adj : adjacent) {
            if (adj->getOwner() != player) {
                attackList.push_back(adj);
            }
        }
    }
    return attackList;
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player* p) : PlayerStrategy(p, new std::string("Benevolent Player Strategy")) {}

void BenevolentPlayerStrategy::issueOrder() {
    // Deploy troups to weakest territories
    // TODO: Adjust to deploy to multiple territories if reinforcement pool is large enough
    std::vector<Territory*> defendList = this->toDefend();
    std::vector<Territory*> territories = this->player->getTerritories();
    if (this->player->getReinforcementPool() > 0) {
        
        if (!defendList.empty()) {
            // Deploy to weakest territory
            Territory* target = defendList[0];
            int armiesToDeploy = this->player->getReinforcementPool(); // Deploy all available armies
            std::cout << "  Issuing Deploy order: " << armiesToDeploy << " armies to " << target->getName() << "\n";
            this->player->issueDeployOrder(target, armiesToDeploy);
            this->player->setReinforcementPool(0);
        } else {
            // No territories to defend, set reinforcement pool to 0
            this->player->setReinforcementPool(0);
        }
        return;
    }

      // Try to move armies to defend territories
  if (!defendList.empty() && territories.size() > 1) {
      // Find a territory with armies to move
      for (Territory* source : territories) {
          if (source->getArmies() > 1) {
              // Try to find adjacent friendly territory to reinforce
              for (Territory* adjacent : source->getAdjTerritories()) {
                  if (this->player->ownsTerritory(adjacent)) {
                      int armiesToMove = source->getArmies() / 2;
                      if (armiesToMove > 0) {
                          std::cout << "  Issuing Advance order (defense): " << armiesToMove 
                                    << " armies from " << source->getName() 
                                    << " to " << adjacent->getName() << "\n";
                          this->player->issueAdvanceOrder(source, adjacent, armiesToMove);
                      }
                  }
              }
          }
       }
    }

    // Benevolent player only plays cards that don't involve attacking
  // Priority 3: Play cards if we have any
  Hand* hand = this->player->getHand();
  if (hand->size() > 0) {
    //Play non aggressive card if available
    for(auto card : *hand->getCards()) {
        if(card->getType() != CARD_TYPES::BOMB) {
               card->play(this->player, this->player->getOrders(), this->player->getDeck());
               std::cout << "  Playing non-aggressive card: " << card->getType() << "\n";
               break; // Play only one card per turn
        }
    }
    }
}

std::vector<Territory*> BenevolentPlayerStrategy::toDefend() {
    //TODO: sort in order of weakest to strongest
    std::vector<Territory *> defendList = player->getTerritories();
    std::sort(defendList.begin(), defendList.end(), 
    [](const Territory* a, const Territory* b) {
              return a->getArmies() < b->getArmies();   // ascending order
          });
    return defendList;
}

std::vector<Territory*> BenevolentPlayerStrategy::toAttack() {
    std::vector<Territory*> attackList;

    for(auto t : player->getTerritories()) {
        std::vector<Territory*> adjacent = t->getAdjTerritories();
        for(auto adj : adjacent) {
            if(adj->getOwner() != player) {
                attackList.push_back(adj);
            }
        }
    }

    return attackList;
}

NeutralPlayerStrategy::NeutralPlayerStrategy(Player* p) : PlayerStrategy(p, new std::string("Neutral Player Strategy")) {}

void NeutralPlayerStrategy::issueOrder() {
  // Simply deploy if there are reinforcements
  if (this->player->getReinforcementPool() > 0) {
    std::vector<Territory*> defendList = this->toDefend();
    
    if (!defendList.empty()) {
        // Deploy to first territory in defend list
        Territory* target = defendList[0];
        int armiesToDeploy = this->player->getReinforcementPool(); // Deploy up to 3 at a time
        std::cout << "  Issuing Deploy order: " << armiesToDeploy << " armies to " << target->getName() << "\n";
        this->player->issueDeployOrder(target, armiesToDeploy);
        this->player->setReinforcementPool(0);
    }else {
        // No territories to defend, set reinforcement pool to 0
        this->player->setReinforcementPool(0);
    }
  }

  // Neutral player does not attack or play any cards
}

std::vector<Territory*> NeutralPlayerStrategy::toDefend() {
    return player->getTerritories();
}

std::vector<Territory*> NeutralPlayerStrategy::toAttack() {
    std::vector<Territory*> attackList;

    for(auto t : player->getTerritories()) {
        std::vector<Territory*> adjacent = t->getAdjTerritories();
        for(auto adj : adjacent) {
            if(adj->getOwner() != player) {
                attackList.push_back(adj);
            }
        }
    }

    return attackList;
}

CheaterPlayerStrategy::CheaterPlayerStrategy(Player* p) : PlayerStrategy(p, new std::string("Cheater Player Strategy")) {}

void CheaterPlayerStrategy::issueOrder() {
  if (this->player->getReinforcementPool() > 0) {
    std::vector<Territory*> defendList = this->toDefend();
    
    if (!defendList.empty()) {
        // Deploy to first territory in defend list
        Territory* target = defendList[0];
        int armiesToDeploy = this->player->getReinforcementPool(); // Deploy up to 3 at a time
        std::cout << "  Issuing Deploy order: " << armiesToDeploy << " armies to " << target->getName() << "\n";
        this->player->issueDeployOrder(target, armiesToDeploy);
        this->player->setReinforcementPool(0);
    }else {
        // No territories to defend, set reinforcement pool to 0
        this->player->setReinforcementPool(0);
    }
  }

  // Issue cheat order
    std::vector<Territory*> attackList = this->toAttack();
    this->player->issueCheatOrder();
}

std::vector<Territory*> CheaterPlayerStrategy::toDefend() {
    return player->getTerritories();
}

std::vector<Territory*> CheaterPlayerStrategy::toAttack() {
    std::vector<Territory*> attackList;

    for(auto t : player->getTerritories()) {
        std::vector<Territory*> adjacent = t->getAdjTerritories();
        for(auto adj : adjacent) {
            if(adj->getOwner() != player) {
                attackList.push_back(adj);
            }
        }
    }

    return attackList;
}
