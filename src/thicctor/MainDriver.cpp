#include <iostream>

// Forward declarations of test functions
void testMainGameLoop();
void testStartupPhase();

int main() {
    std::cout << "========================================\n";
    std::cout << "COMP 345 - Assignment 2 - Part 3 Demo\n";
    std::cout << "Main Game Loop Implementation\n";
    std::cout << "========================================\n\n";

    // Run the main game loop test
    testMainGameLoop();

    std::cout << "\n\nPress Enter to exit...";
    std::cin.get();

    return 0;
}
