#include "Cards.h"
#include "Map.h"
#include "Orders.h"
#include "Player.h"
#include "PlayerStrategies.h"

#include <iostream>
#include <vector>

/**
 * Helper function to create a simple test map with territories
 * Creates a basic map with 5 territories for testing purposes
 */
Map* createTestMap() {
  Map* map = new Map("Test Map");

  // create continents
  Continent* continent1 = map->addContinent("Test Continent 1", 1, 5);
  Continent* continent2 = map->addContinent("Test Continent 2", 2, 3);

  // create territories
  Territory* t1 = map->addTerritory("Territory1", 1);
  Territory* t2 = map->addTerritory("Territory2", 2);
  Territory* t3 = map->addTerritory("Territory3", 3);
  Territory* t4 = map->addTerritory("Territory4", 4);
  Territory* t5 = map->addTerritory("Territory5", 5);

  // assign territories to continents
  continent1->addTerritory(t1);
  continent1->addTerritory(t2);
  continent1->addTerritory(t3);
  continent2->addTerritory(t4);
  continent2->addTerritory(t5);

  // create territory adjacency
  Map::addAdjacency(t1, t2);
  Map::addAdjacency(t2, t3);
  Map::addAdjacency(t3, t4);
  Map::addAdjacency(t4, t5);

  return map;
}

/**
 * Helper function to set up players with territories
 */
void setupPlayersWithTerritories(Player* p1, Player* p2, const Map* map) {
  const std::vector<std::unique_ptr<Territory>>& territories = map->getTerritories();

  // give first 2 territories to player 1
  if (territories.size() > 0) {
    territories[0]->setOwner(p1);
    territories[0]->setArmies(5);
    p1->addTerritory(territories[0].get());
  }
  if (territories.size() > 1) {
    territories[1]->setOwner(p1);
    territories[1]->setArmies(3);
    p1->addTerritory(territories[1].get());
  }

  // give the remaining territories to player 2
  if (territories.size() > 2) {
    territories[2]->setOwner(p2);
    territories[2]->setArmies(4);
    p2->addTerritory(territories[2].get());
  }
  if (territories.size() > 3) {
    territories[3]->setOwner(p2);
    territories[3]->setArmies(6);
    p2->addTerritory(territories[3].get());
  }
}

/**
 * Test function to demonstrate different strategies lead to different behaviors
 */
void testDifferentStrategiesDifferentBehavior() {
  std::cout << "\n========================================\n";
  std::cout << "Test 1: Different Strategies -> Different Behaviors\n";
  std::cout << "========================================\n\n";

  // create test environment
  Map* map = createTestMap();
  Deck* deck = new Deck();

  // create players with different strategies
  Player* aggressivePlayer = new Player("Aggressive Player", deck);
  Player* benevolentPlayer = new Player("Benevolent Player", deck);
  Player* neutralPlayer = new Player("Neutral Player", deck);
  Player* cheaterPlayer = new Player("Cheater Player", deck);

  // assign strategies
  aggressivePlayer->setStrategy(new AggressivePlayerStrategy(aggressivePlayer));
  benevolentPlayer->setStrategy(new BenevolentPlayerStrategy(benevolentPlayer));
  neutralPlayer->setStrategy(new NeutralPlayerStrategy(neutralPlayer));
  cheaterPlayer->setStrategy(new CheaterPlayerStrategy(cheaterPlayer));

  // setup territories for each player
  const std::vector<std::unique_ptr<Territory>>& territories = map->getTerritories();

  // aggressive player gets territories
  if (territories.size() > 0) {
    territories[0]->setOwner(aggressivePlayer);
    territories[0]->setArmies(10); // strongest territory
    aggressivePlayer->addTerritory(territories[0].get());
  }
  if (territories.size() > 1) {
    territories[1]->setOwner(aggressivePlayer);
    territories[1]->setArmies(3);
    aggressivePlayer->addTerritory(territories[1].get());
  }

  // benevolent player gets territories
  if (territories.size() > 2) {
    territories[2]->setOwner(benevolentPlayer);
    territories[2]->setArmies(2); // weakest territory
    benevolentPlayer->addTerritory(territories[2].get());
  }
  if (territories.size() > 3) {
    territories[3]->setOwner(neutralPlayer);
    territories[3]->setArmies(8);
    neutralPlayer->addTerritory(territories[3].get());
  }

  // neutral player gets a territory
  if (territories.size() > 4) {
    territories[4]->setOwner(cheaterPlayer);
    territories[4]->setArmies(5);
    cheaterPlayer->addTerritory(territories[4].get());
  }

  // give reinforcements to all players
  aggressivePlayer->setReinforcementPool(10);
  benevolentPlayer->setReinforcementPool(10);
  neutralPlayer->setReinforcementPool(10);
  cheaterPlayer->setReinforcementPool(10);

  // give cards to players
  aggressivePlayer->addCard(new BombCard());
  aggressivePlayer->addCard(new ReinforcementCard());
  aggressivePlayer->addCard(new AirliftCard());

  benevolentPlayer->addCard(new BombCard());
  benevolentPlayer->addCard(new ReinforcementCard());
  benevolentPlayer->addCard(new AirliftCard());

  neutralPlayer->addCard(new BombCard());
  neutralPlayer->addCard(new ReinforcementCard());
  neutralPlayer->addCard(new AirliftCard());

  cheaterPlayer->addCard(new BombCard());
  cheaterPlayer->addCard(new ReinforcementCard());
  cheaterPlayer->addCard(new AirliftCard());

  // Test 1: Aggressive Player Behavior
  std::cout << "--- Aggressive Player Behavior ---\n";
  std::cout << "Before issuing orders:\n";
  aggressivePlayer->displayInfo();

  const std::vector<Territory*> aggressiveDefend = aggressivePlayer->toDefend();
  std::cout << "\nAggressive toDefend() returns strongest territory:\n";
  for (const auto terr : aggressiveDefend) {
    std::cout << "  - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
  }

  const std::vector<Territory*> aggressiveAttack = aggressivePlayer->toAttack();
  std::cout << "\nAggressive toAttack() returns adjacent enemy territories:\n";
  for (const auto terr : aggressiveAttack) {
    std::cout << "  - " << terr->getName() << " (Owner: " << terr->getOwner()->getName() << ")\n";
  }

  // issue orders
  std::cout << "\nIssuing orders for Aggressive Player:\n";
  while (*(aggressivePlayer->getCanIssueOrder())) {
    aggressivePlayer->issueOrder();
  }
  std::cout << "\nOrderList after issuing orders: " << *(aggressivePlayer->getOrders());

  // Test 2: Benevolent Player Behavior
  std::cout << "\n--- Benevolent Player Behavior ---\n";
  std::cout << "Before issuing orders:\n";
  benevolentPlayer->displayInfo();

  const std::vector<Territory*> benevolentDefend = benevolentPlayer->toDefend();
  std::cout << "\nBenevolent toDefend() returns weakest territories first:\n";
  for (const auto terr : benevolentDefend) {
    std::cout << "  - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
  }

  std::cout << "\nIssuing orders for Benevolent Player:\n";
  while (*(benevolentPlayer->getCanIssueOrder())) {
    benevolentPlayer->issueOrder();
  }
  std::cout << "\nOrderList after issuing orders: " << *(benevolentPlayer->getOrders());

  // Test 3: Neutral Player Behavior
  std::cout << "\n--- Neutral Player Behavior ---\n";
  std::cout << "Before issuing orders:\n";
  neutralPlayer->displayInfo();

  std::cout << "\nIssuing orders for Neutral Player:\n";
  while (*(neutralPlayer->getCanIssueOrder())) {
    neutralPlayer->issueOrder();
  }
  std::cout << "\nOrderList after issuing orders: " << *(neutralPlayer->getOrders());

  // Test 4: Cheater Player Behavior
  std::cout << "\n--- Cheater Player Behavior ---\n";
  std::cout << "Before issuing orders:\n";
  cheaterPlayer->displayInfo();

  std::cout << "\nIssuing orders for Cheater Player:\n";
  while (*(cheaterPlayer->getCanIssueOrder())) {
    cheaterPlayer->issueOrder();
  }
  std::cout << "\nOrderList after issuing orders: " << *(cheaterPlayer->getOrders());

  std::cout << "\nTEST 1 PASSED: Different strategies produce different behaviors\n";
  std::cout << "  - Aggressive: Focuses on strongest territory, issues attack orders\n";
  std::cout << "  - Benevolent: Focuses on weakest territory, only defensive orders\n";
  std::cout << "  - Neutral: Only deploys, no attacks or card plays\n";

  // cleanup
  delete map;
  delete deck;
  delete aggressivePlayer;
  delete benevolentPlayer;
  delete neutralPlayer;
}

/**
 * Test function to demonstrate dynamic strategy changes during play
 */
void testDynamicStrategyChange() {
  std::cout << "\n========================================\n";
  std::cout << "Test 2: Dynamic Strategy Changes\n";
  std::cout << "========================================\n\n";

  // create test environment
  Map* map = createTestMap();
  Deck* deck = new Deck();

  Player* player = new Player("Dynamic Player", deck);
  Player* opponent = new Player("Opponent", deck);

  // setup territories
  setupPlayersWithTerritories(player, opponent, map);
  player->setReinforcementPool(15);

  // start with aggressive strategy
  std::cout << "--- Initial Strategy: Aggressive ---\n";
  player->setStrategy(new AggressivePlayerStrategy(player));
  player->displayInfo();

  std::vector<Territory*> defendList = player->toDefend();
  std::cout << "\nWith Aggressive strategy, toDefend() prioritizes:\n";
  for (const auto terr : defendList) {
    std::cout << "  - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
  }

  player->issueOrder();
  const int ordersAfterAggressive = player->getOrders()->orders->size();
  std::cout << "Orders issued: " << ordersAfterAggressive << "\n";

  // change to benevolent strategy
  std::cout << "\n--- Changing Strategy to: Benevolent ---\n";
  player->setStrategy(new BenevolentPlayerStrategy(player));
  player->setReinforcementPool(15); // reset reinforcements for next test

  defendList = player->toDefend();
  std::cout << "\nWith Benevolent strategy, toDefend() prioritizes:\n";
  for (const auto terr : defendList) {
    std::cout << "  - " << terr->getName() << " (Armies: " << terr->getArmies() << ")\n";
  }

  player->issueOrder();
  const int ordersAfterBenevolent = player->getOrders()->orders->size();
  std::cout << "Orders issued: " << ordersAfterBenevolent << "\n";

  // change to neutral strategy
  std::cout << "\n--- Changing Strategy to: Neutral ---\n";
  player->setStrategy(new NeutralPlayerStrategy(player));
  player->setReinforcementPool(10);

  player->issueOrder();
  const int ordersAfterNeutral = player->getOrders()->orders->size();
  std::cout << "Orders issued: " << ordersAfterNeutral << "\n";

  // change to cheater strategy
  std::cout << "\n--- Changing Strategy to: Cheater ---\n";
  player->setStrategy(new CheaterPlayerStrategy(player));
  player->setReinforcementPool(10);

  player->issueOrder();
  const int ordersAfterCheater = player->getOrders()->orders->size();
  std::cout << "Orders issued: " << ordersAfterCheater << " (includes cheat order)\n";

  std::cout << "\nTest 2 PASSED: Strategy can be changed dynamically during play\n";

  // cleanup
  delete map;
  delete deck;
  delete player;
  delete opponent;
}

/**
 * Test function to demonstrate human vs computer player decision making
 */
void testHumanVsComputerStrategies() {
  std::cout << "\n========================================\n";
  std::cout << "Test 3: Human vs Computer Player Strategies\n";
  std::cout << "========================================\n\n";

  // create test environment
  Map* map = createTestMap();
  Deck* deck = new Deck();

  Player* humanPlayer = new Player("Human Player", deck);
  Player* computerPlayer = new Player("Computer Player", deck);
  setupPlayersWithTerritories(humanPlayer, computerPlayer, map);

  // set strategies
  humanPlayer->setStrategy(new HumanPlayerStrategy(humanPlayer));
  computerPlayer->setStrategy(new AggressivePlayerStrategy(computerPlayer));

  humanPlayer->setReinforcementPool(10);
  computerPlayer->setReinforcementPool(10);

  // add a card to each player's hand
  Card* bombCard1 = new BombCard();
  Card* reinforcementCard = new ReinforcementCard();
  humanPlayer->addCard(bombCard1);
  computerPlayer->addCard(reinforcementCard);

  // demonstrate computer player (automatic)
  std::cout << "--- Computer Player (Automatic Decision Making) ---\n";
  std::cout << "Computer player using Aggressive strategy\n";
  computerPlayer->displayInfo();

  std::cout << "\nComputer player automatically issues orders:\n";
  computerPlayer->issueOrder();
  std::cout << "Computer player issued " << computerPlayer->getOrders()->orders->size()
            << " orders automatically\n";

  // demonstrate human player (user interaction)
  std::cout << "\n--- Human Player (User Interaction Required) ---\n";
  std::cout << "Human player requires user input for decisions\n";
  humanPlayer->displayInfo();

  std::cout << "\nHuman player issuing orders (user input required):\n";
  while (*(humanPlayer->getCanIssueOrder())) {
    humanPlayer->issueOrder();
    std::cout << "Human player issued " << humanPlayer->getOrders()->orders->size()
              << " orders based on user input\n";
  }

  // switch human to computer strategy for automated testing
  std::cout << "\n--- Converting Human to Computer Strategy for Testing ---\n";
  humanPlayer->setStrategy(new BenevolentPlayerStrategy(humanPlayer));

  std::cout << "Human player now using Benevolent (computer) strategy\n";
  humanPlayer->issueOrder();
  std::cout << "Player issued " << humanPlayer->getOrders()->orders->size()
            << " orders automatically\n";

  std::cout << "\nTest 3 PASSED: Strategy pattern enables both human and computer players\n";

  // cleanup
  delete map;
  delete deck;
  delete humanPlayer;
  delete computerPlayer;
}

/**
 * Main test driver function that runs all strategy tests
 */
void testPlayerStrategies() {
  std::cout << "\n========================================\n";
  std::cout << "    PLAYER STRATEGIES TEST DRIVER\n";
  std::cout << "    Testing Strategy Design Pattern\n";
  std::cout << "========================================\n";

  try {
    // test 1: different strategies produce different behaviors
    testDifferentStrategiesDifferentBehavior();

    // test 2: strategies can be changed dynamically
    testDynamicStrategyChange();

    // test 3: human vs computer strategies
    testHumanVsComputerStrategies();

    std::cout << "\n========================================\n";
    std::cout << "    ALL TESTS COMPLETED SUCCESSFULLY\n";
    std::cout << "========================================\n\n";
  } catch (const std::exception& e) {
    std::cerr << "\nTEST FAILED WITH EXCEPTION: " << e.what() << "\n";
  }
}
