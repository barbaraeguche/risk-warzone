#include "Player.h"
#include "../map/Map.h"
#include "../card-deck-hand/Cards.h"
#include "../orders/Orders.h"
#include <iostream>
#include <vector>

/**
 * Test function to demonstrate Player class functionality
 * This function creates player objects and demonstrates all required features
 */
void testPlayers() {
    std::cout << "\n=== PLAYER TESTING ===" << std::endl;
    std::cout << "Testing Player class implementation..." << std::endl;

    // ==================== Test 1: Basic Player Creation ====================
    std::cout << "\n--- Test 1: Basic Player Creation ---" << std::endl;
    
    // Create players using different constructors
    Player* player1 = new Player("Alice");
    Player* player2 = new Player("Bob");
    Player* player3 = new Player(); // Default constructor
    
    std::cout << "Created players:" << std::endl;
    std::cout << "Player 1: " << *player1 << std::endl;
    std::cout << "Player 2: " << *player2 << std::endl;
    std::cout << "Player 3: " << *player3 << std::endl;

    // ==================== Test 2: Territory Management ====================
    std::cout << "\n--- Test 2: Territory Management ---" << std::endl;
    
    // Create mock territories for testing
    Territory* territory1 = new Territory("North America", 1);
    Territory* territory2 = new Territory("South America", 2);
    Territory* territory3 = new Territory("Europe", 3);
    Territory* territory4 = new Territory("Asia", 4);
    Territory* territory5 = new Territory("Africa", 5);
    
    // Set up some armies on territories
    territory1->setArmies(5);
    territory2->setArmies(3);
    territory3->setArmies(7);
    territory4->setArmies(4);
    territory5->setArmies(2);
    
    // Add territories to players
    player1->addTerritory(territory1);
    player1->addTerritory(territory2);
    player2->addTerritory(territory3);
    player2->addTerritory(territory4);
    player3->addTerritory(territory5);
    
    std::cout << "Added territories to players:" << std::endl;
    player1->displayInfo();
    std::cout << std::endl;
    player2->displayInfo();
    std::cout << std::endl;
    player3->displayInfo();

    // ==================== Test 3: Card Management ====================
    std::cout << "\n--- Test 3: Card Management ---" << std::endl;
    
    // Create a deck and draw cards for players
    Deck* deck = new Deck();
    
    // Draw some cards for each player
    for (int i = 0; i < 3; i++) {
        Card* card1 = deck->draw();
        Card* card2 = deck->draw();
        Card* card3 = deck->draw();
        
        if (card1) player1->addCard(card1);
        if (card2) player2->addCard(card2);
        if (card3) player3->addCard(card3);
    }
    
    std::cout << "Players' hands after drawing cards:" << std::endl;
    std::cout << "Player 1 hand: " << *player1->getHand() << std::endl;
    std::cout << "Player 2 hand: " << *player2->getHand() << std::endl;
    std::cout << "Player 3 hand: " << *player3->getHand() << std::endl;

    // ==================== Test 4: Order Management ====================
    std::cout << "\n--- Test 4: Order Management ---" << std::endl;
    
    // Set reinforcement pools
    player1->setReinforcementPool(10);
    player2->setReinforcementPool(8);
    player3->setReinforcementPool(5);
    
    // Issue various types of orders
    std::cout << "Issuing orders..." << std::endl;
    
    // Deploy orders
    player1->issueDeployOrder(territory1, 3);
    player2->issueDeployOrder(territory3, 2);
    
    // Advance orders
    player1->issueAdvanceOrder(territory1, territory3, 2);
    player2->issueAdvanceOrder(territory3, territory1, 1);
    
    // Bomb orders
    player1->issueBombOrder(territory4);
    player2->issueBombOrder(territory1);
    
    // Blockade orders
    player1->issueBlockadeOrder(territory1);
    player2->issueBlockadeOrder(territory3);
    
    // Airlift orders
    player1->issueAirliftOrder(territory1, territory2, 1);
    player2->issueAirliftOrder(territory3, territory4, 2);
    
    // Negotiate orders
    player1->issueNegotiateOrder(player2);
    player2->issueNegotiateOrder(player1);
    
    std::cout << "Orders issued. Order counts:" << std::endl;
    std::cout << "Player 1 orders: " << player1->getOrders()->orders->size() << std::endl;
    std::cout << "Player 2 orders: " << player2->getOrders()->orders->size() << std::endl;
    std::cout << "Player 3 orders: " << player3->getOrders()->orders->size() << std::endl;

    // ==================== Test 5: Strategy Methods ====================
    std::cout << "\n--- Test 5: Strategy Methods (toDefend/toAttack) ---" << std::endl;
    
    // Test toDefend method
    std::vector<Territory*> defendList1 = player1->toDefend();
    std::vector<Territory*> defendList2 = player2->toDefend();
    
    std::cout << "Player 1 territories to defend (" << defendList1.size() << "):" << std::endl;
    for (Territory* territory : defendList1) {
        if (territory) {
            std::cout << "  - " << territory->getName() << std::endl;
        }
    }
    
    std::cout << "Player 2 territories to defend (" << defendList2.size() << "):" << std::endl;
    for (Territory* territory : defendList2) {
        if (territory) {
            std::cout << "  - " << territory->getName() << std::endl;
        }
    }
    
    // Test toAttack method
    std::vector<Territory*> attackList1 = player1->toAttack();
    std::vector<Territory*> attackList2 = player2->toAttack();
    
    std::cout << "Player 1 territories to attack (" << attackList1.size() << "):" << std::endl;
    for (Territory* territory : attackList1) {
        if (territory) {
            std::cout << "  - " << territory->getName() << std::endl;
        }
    }
    
    std::cout << "Player 2 territories to attack (" << attackList2.size() << "):" << std::endl;
    for (Territory* territory : attackList2) {
        if (territory) {
            std::cout << "  - " << territory->getName() << std::endl;
        }
    }

    // ==================== Test 6: Copy Constructor and Assignment ====================
    std::cout << "\n--- Test 6: Copy Constructor and Assignment ---" << std::endl;
    
    // Test copy constructor
    Player* player1Copy = new Player(*player1);
    std::cout << "Original Player 1: " << *player1 << std::endl;
    std::cout << "Copied Player 1: " << *player1Copy << std::endl;
    
    // Test assignment operator
    Player* player2Assigned = new Player("Temp");
    *player2Assigned = *player2;
    std::cout << "Original Player 2: " << *player2 << std::endl;
    std::cout << "Assigned Player 2: " << *player2Assigned << std::endl;

    // ==================== Test 7: Card Playing ====================
    std::cout << "\n--- Test 7: Card Playing ---" << std::endl;
    
    std::cout << "Player 1 hand before playing cards:" << std::endl;
    player1->getHand()->printHand();
    
    // Play all cards in hand
    player1->getHand()->playAll(player1, player1->getOrders(), deck);
    
    std::cout << "Player 1 hand after playing all cards:" << std::endl;
    player1->getHand()->printHand();
    
    std::cout << "Deck after playing cards:" << std::endl;
    deck->printDeck();

    // ==================== Test 8: Territory Ownership ====================
    std::cout << "\n--- Test 8: Territory Ownership ---" << std::endl;
    
    std::cout << "Territory ownership tests:" << std::endl;
    std::cout << "Player 1 owns territory1: " << (player1->ownsTerritory(territory1) ? "Yes" : "No") << std::endl;
    std::cout << "Player 1 owns territory3: " << (player1->ownsTerritory(territory3) ? "Yes" : "No") << std::endl;
    std::cout << "Player 2 owns territory3: " << (player2->ownsTerritory(territory3) ? "Yes" : "No") << std::endl;
    
    // Test removing territory
    player1->removeTerritory(territory1);
    std::cout << "After removing territory1 from Player 1:" << std::endl;
    std::cout << "Player 1 owns territory1: " << (player1->ownsTerritory(territory1) ? "Yes" : "No") << std::endl;
    std::cout << "Player 1 territory count: " << player1->getTerritoryCount() << std::endl;

    // ==================== Test 9: Utility Methods ====================
    std::cout << "\n--- Test 9: Utility Methods ---" << std::endl;
    
    std::cout << "Player statistics:" << std::endl;
    std::cout << "Player 1 total armies: " << player1->getTotalArmies() << std::endl;
    std::cout << "Player 2 total armies: " << player2->getTotalArmies() << std::endl;
    std::cout << "Player 3 total armies: " << player3->getTotalArmies() << std::endl;
    
    std::cout << "Player 1 territory count: " << player1->getTerritoryCount() << std::endl;
    std::cout << "Player 2 territory count: " << player2->getTerritoryCount() << std::endl;
    std::cout << "Player 3 territory count: " << player3->getTerritoryCount() << std::endl;

    // ==================== Cleanup ====================
    std::cout << "\n--- Cleanup ---" << std::endl;
    
    // Clean up players
    delete player1;
    delete player2;
    delete player3;
    delete player1Copy;
    delete player2Assigned;
    
    // Clean up territories
    delete territory1;
    delete territory2;
    delete territory3;
    delete territory4;
    delete territory5;
    
    // Clean up deck
    delete deck;
    
    std::cout << "All Player tests completed successfully!" << std::endl;
    std::cout << "=== END PLAYER TESTING ===" << std::endl;
}
