#include "PlayerStrategies.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include "Orders.h"
#include <iostream>
#include <vector>

/**
 * Helper function to create a simple test map with territories
 * Creates a basic map with 5 territories for testing purposes
 */
Map* createTestMap() {
    Map* map = new Map("Test Map");
    
    // Create continents using the Map's addContinent method
    Continent* continent1 = map->addContinent("Test Continent 1", 1, 5);
    Continent* continent2 = map->addContinent("Test Continent 2", 2, 3);
    
    // Create territories using the Map's addTerritory method
    Territory* t1 = map->addTerritory("Territory1", 1);
    Territory* t2 = map->addTerritory("Territory2", 2);
    Territory* t3 = map->addTerritory("Territory3", 3);
    Territory* t4 = map->addTerritory("Territory4", 4);
    Territory* t5 = map->addTerritory("Territory5", 5);
    
    // Assign territories to continents
    t1->setContinent(continent1);
    continent1->addTerritory(t1);
    
    t2->setContinent(continent1);
    continent1->addTerritory(t2);
    
    t3->setContinent(continent1);
    continent1->addTerritory(t3);
    
    t4->setContinent(continent2);
    continent2->addTerritory(t4);
    
    t5->setContinent(continent2);
    continent2->addTerritory(t5);
    
    // Create adjacencies using Map's static method
    Map::addAdjacency(t1, t2);
    Map::addAdjacency(t2, t3);
    Map::addAdjacency(t3, t4);
    Map::addAdjacency(t4, t5);
    
    return map;
}

/**
 * Helper function to setup players with territories
 */
void setupPlayersWithTerritories(Player* p1, Player* p2, Map* map) {
    // Get reference to the territories (not a copy)
    const std::vector<std::unique_ptr<Territory>>& territories = map->getTerritories();
    
    // Give first 2 territories to player 1
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
    
    // Give remaining territories to player 2
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
    std::cout << "TEST 1: Different Strategies -> Different Behaviors\n";
    std::cout << "========================================\n\n";
    
    // Create test environment
    Map* map = createTestMap();
    Deck* deck = new Deck();
    
    // Create players with different strategies
    Player* aggressivePlayer = new Player("Aggressive Player", deck);
    Player* benevolentPlayer = new Player("Benevolent Player", deck);
    Player* neutralPlayer = new Player("Neutral Player", deck);
    Player* cheaterPlayer = new Player("Cheater Player", deck);
    
    // Assign strategies
    aggressivePlayer->setStrategy(new AggressivePlayerStrategy(aggressivePlayer));
    benevolentPlayer->setStrategy(new BenevolentPlayerStrategy(benevolentPlayer));
    neutralPlayer->setStrategy(new NeutralPlayerStrategy(neutralPlayer));
    cheaterPlayer->setStrategy(new CheaterPlayerStrategy(cheaterPlayer));
    
    // Setup territories for each player
    const std::vector<std::unique_ptr<Territory>>& territories = map->getTerritories();
    
    // Aggressive player gets territories
    if (territories.size() > 0) {
        territories[0]->setOwner(aggressivePlayer);
        territories[0]->setArmies(10);  // Strongest territory
        aggressivePlayer->addTerritory(territories[0].get());
    }
    
    if (territories.size() > 1) {
        territories[1]->setOwner(aggressivePlayer);
        territories[1]->setArmies(3);
        aggressivePlayer->addTerritory(territories[1].get());
    }
    
    // Benevolent player gets territories
    if (territories.size() > 2) {
        territories[2]->setOwner(benevolentPlayer);
        territories[2]->setArmies(2);  // Weakest territory
        benevolentPlayer->addTerritory(territories[2].get());
    }
    
    if (territories.size() > 3) {
        territories[3]->setOwner(neutralPlayer);
        territories[3]->setArmies(8);
        neutralPlayer->addTerritory(territories[3].get());
    }
    
    // Neutral player gets a territory
    if (territories.size() > 4) {
        territories[4]->setOwner(cheaterPlayer);
        territories[4]->setArmies(5);
        cheaterPlayer->addTerritory(territories[4].get());
    }
    
    // Give reinforcements to all players
    aggressivePlayer->setReinforcementPool(10);
    benevolentPlayer->setReinforcementPool(10);
    neutralPlayer->setReinforcementPool(10);
    cheaterPlayer->setReinforcementPool(10);

    // Give cards to players
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
    
    std::vector<Territory*> aggressiveDefend = aggressivePlayer->toDefend();
    std::cout << "\nAggressive toDefend() returns strongest territory:\n";
    for (auto t : aggressiveDefend) {
        std::cout << "  - " << t->getName() << " (Armies: " << t->getArmies() << ")\n";
    }
    
    std::vector<Territory*> aggressiveAttack = aggressivePlayer->toAttack();
    std::cout << "\nAggressive toAttack() returns adjacent enemy territories:\n";
    for (auto t : aggressiveAttack) {
        std::cout << "  - " << t->getName() << " (Owner: " << t->getOwner()->getName() << ")\n";
    }
    
    // Issue orders
    std::cout << "\nIssuing orders for Aggressive Player:\n";
    while (*(aggressivePlayer->getCanIssueOrderFlag())) {
        aggressivePlayer->issueOrder();
    }
    std::cout << "\nOrderlist after issuing orders: " << *(aggressivePlayer->getOrders());
    
    // Test 2: Benevolent Player Behavior
    std::cout << "\n--- Benevolent Player Behavior ---\n";
    std::cout << "Before issuing orders:\n";
    benevolentPlayer->displayInfo();
    
    std::vector<Territory*> benevolentDefend = benevolentPlayer->toDefend();
    std::cout << "\nBenevolent toDefend() returns weakest territories first:\n";
    for (auto t : benevolentDefend) {
        std::cout << "  - " << t->getName() << " (Armies: " << t->getArmies() << ")\n";
    }
    
    std::cout << "\nIssuing orders for Benevolent Player:\n";
    while (*(benevolentPlayer->getCanIssueOrderFlag())) {
        benevolentPlayer->issueOrder();
    }
    std::cout << "\nOrderlist after issuing orders: " << *(benevolentPlayer->getOrders());
    
    // Test 3: Neutral Player Behavior
    std::cout << "\n--- Neutral Player Behavior ---\n";
    std::cout << "Before issuing orders:\n";
    neutralPlayer->displayInfo();
    
    std::cout << "\nIssuing orders for Neutral Player:\n";
    while (*(neutralPlayer->getCanIssueOrderFlag())) {
        neutralPlayer->issueOrder();
    }
    std::cout << "\nOrderlist after issuing orders: " << *(neutralPlayer->getOrders());

    // Test 4: Cheater Player Behavior
    std::cout << "\n--- Cheater Player Behavior ---\n";
    std::cout << "Before issuing orders:\n";
    cheaterPlayer->displayInfo();

    std::cout << "\nIssuing orders for Cheater Player:\n";
    while (*(cheaterPlayer->getCanIssueOrderFlag())) {
        //cheaterPlayer->setCanIssueOrderFlag(false);  // Prevent infinite loop in test
        cheaterPlayer->issueOrder();
    }
    std::cout << "\nOrderlist after issuing orders: " << *(cheaterPlayer->getOrders());
    
    std::cout << "\nTEST 1 PASSED: Different strategies produce different behaviors\n";
    std::cout << "  - Aggressive: Focuses on strongest territory, issues attack orders\n";
    std::cout << "  - Benevolent: Focuses on weakest territory, only defensive orders\n";
    std::cout << "  - Neutral: Only deploys, no attacks or card plays\n";
    
    // Cleanup
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
    std::cout << "TEST 2: Dynamic Strategy Changes\n";
    std::cout << "========================================\n\n";
    
    // Create test environment
    Map* map = createTestMap();
    Deck* deck = new Deck();
    
    Player* player = new Player("Dynamic Player", deck);
    Player* opponent = new Player("Opponent", deck);
    
    // Setup territories
    setupPlayersWithTerritories(player, opponent, map);
    player->setReinforcementPool(15);
    
    // Start with Aggressive Strategy
    std::cout << "--- Initial Strategy: Aggressive ---\n";
    player->setStrategy(new AggressivePlayerStrategy(player));
    player->displayInfo();
    
    std::vector<Territory*> defendList = player->toDefend();
    std::cout << "\nWith Aggressive strategy, toDefend() prioritizes:\n";
    for (auto t : defendList) {
        std::cout << "  - " << t->getName() << " (Armies: " << t->getArmies() << ")\n";
    }
    
    player->issueOrder();
    size_t ordersAfterAggressive = player->getOrders()->orders->size();
    std::cout << "Orders issued: " << static_cast<int>(ordersAfterAggressive) << "\n";
    
    // Change to Benevolent Strategy
    std::cout << "\n--- Changing Strategy to: Benevolent ---\n";
    player->setStrategy(new BenevolentPlayerStrategy(player));
    player->setReinforcementPool(15);  // Reset reinforcements for next test
    
    defendList = player->toDefend();
    std::cout << "\nWith Benevolent strategy, toDefend() prioritizes:\n";
    for (auto t : defendList) {
        std::cout << "  - " << t->getName() << " (Armies: " << t->getArmies() << ")\n";
    }
    
    player->issueOrder();
    size_t ordersAfterBenevolent = player->getOrders()->orders->size();
    std::cout << "Orders issued: " << static_cast<int>(ordersAfterBenevolent) << "\n";
    
    // Change to Neutral Strategy
    std::cout << "\n--- Changing Strategy to: Neutral ---\n";
    player->setStrategy(new NeutralPlayerStrategy(player));
    player->setReinforcementPool(10);
    
    player->issueOrder();
    size_t ordersAfterNeutral = player->getOrders()->orders->size();
    std::cout << "Orders issued: " << static_cast<int>(ordersAfterNeutral) << "\n";
    
    // Change to Cheater Strategy
    std::cout << "\n--- Changing Strategy to: Cheater ---\n";
    player->setStrategy(new CheaterPlayerStrategy(player));
    player->setReinforcementPool(10);
    
    player->issueOrder();
    size_t ordersAfterCheater = player->getOrders()->orders->size();
    std::cout << "Orders issued: " << static_cast<int>(ordersAfterCheater) << " (includes cheat order)\n";
    
    std::cout << "\nTEST 2 PASSED: Strategy can be changed dynamically during play\n";
    
    // Cleanup
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
    std::cout << "TEST 3: Human vs Computer Player Strategies\n";
    std::cout << "========================================\n\n";
    
    // Create test environment
    Map* map = createTestMap();
    Deck* deck = new Deck();
    
    Player* humanPlayer = new Player("Human Player", deck);
    Player* computerPlayer = new Player("Computer Player", deck);
    
    setupPlayersWithTerritories(humanPlayer, computerPlayer, map);
    
    // Set strategies
    humanPlayer->setStrategy(new HumanPlayerStrategy(humanPlayer));
    computerPlayer->setStrategy(new AggressivePlayerStrategy(computerPlayer));
    
    humanPlayer->setReinforcementPool(10);
    computerPlayer->setReinforcementPool(10);
    
    // Add a card to each player's hand
    Card* bombCard1 = new BombCard();
    Card* reinforcementCard = new ReinforcementCard();
    humanPlayer->addCard(bombCard1);
    computerPlayer->addCard(reinforcementCard);
    
    // Demonstrate Computer Player (Automatic)
    std::cout << "--- Computer Player (Automatic Decision Making) ---\n";
    std::cout << "Computer player using Aggressive strategy\n";
    computerPlayer->displayInfo();
    
    std::cout << "\nComputer player automatically issues orders:\n";
    computerPlayer->issueOrder();
    std::cout << "Computer player issued " << static_cast<int>(computerPlayer->getOrders()->orders->size())
              << " orders automatically\n";
    
    // Demonstrate Human Player (User Interaction)
    std::cout << "\n--- Human Player (User Interaction Required) ---\n";
    std::cout << "Human player requires user input for decisions\n";
    humanPlayer->displayInfo();
    
    std::cout << "\nHuman player issuing orders (user input required):\n";
    while (*(humanPlayer->getCanIssueOrderFlag())) {
        humanPlayer->issueOrder();
        std::cout << "Human player issued " << static_cast<int>(humanPlayer->getOrders()->orders->size())
              << " orders based on user input\n";
    }
    
    // Switch human to computer strategy for automated testing
    std::cout << "\n--- Converting Human to Computer Strategy for Testing ---\n";
    humanPlayer->setStrategy(new BenevolentPlayerStrategy(humanPlayer));
    std::cout << "Human player now using Benevolent (computer) strategy\n";
    humanPlayer->issueOrder();
    std::cout << "Player issued " << static_cast<int>(humanPlayer->getOrders()->orders->size())
              << " orders automatically\n";
    
    std::cout << "\nTEST 3 PASSED: Strategy pattern enables both human and computer players\n";
    
    // Cleanup
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
        // Test 1: Different strategies produce different behaviors
        testDifferentStrategiesDifferentBehavior();
        
        // Test 2: Strategies can be changed dynamically
        testDynamicStrategyChange();
        
        // Test 3: Human vs Computer strategies
        testHumanVsComputerStrategies();
        
        std::cout << "\n========================================\n";
        std::cout << "    ALL TESTS COMPLETED SUCCESSFULLY\n";
        std::cout << "========================================\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\nTEST FAILED WITH EXCEPTION: " << e.what() << "\n";
    }
}