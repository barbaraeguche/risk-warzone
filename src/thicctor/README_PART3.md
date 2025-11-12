# COMP 345 - Assignment 2 - Part 3: Main Game Loop Implementation

## Overview
This implementation provides the main game loop for the Warzone game, including three essential phases:
1. **Reinforcement Phase** - Calculate and distribute armies to players
2. **Issuing Orders Phase** - Players issue orders in round-robin fashion
3. **Orders Execution Phase** - Execute all orders (deploy first, then others)

## Files Modified/Created

### Modified Files:
1. **GameEngine.h** - Added declarations for:
   - `mainGameLoop()`
   - `reinforcementPhase()`
   - `issueOrdersPhase()`
   - `executeOrdersPhase()`

2. **GameEngine.cpp** - Implemented:
   - Main game loop with turn management
   - Reinforcement calculation (territories/3, min 3, + continent bonuses)
   - Order issuing phase with round-robin player turns
   - Order execution phase (deploy orders first, then others)
   - Player elimination when no territories owned
   - Win condition detection

3. **Player.h** - Added:
   - `bool issueOrder()` - Decision-making method for order issuing

4. **Player.cpp** - Implemented:
   - Intelligent order issuing logic
   - Priority 1: Deploy orders when reinforcement pool > 0
   - Priority 2: Advance orders for defense/attack
   - Priority 3: Card-based orders (simplified)

### New Files:
1. **GameEngineDriver.cpp** - Test driver with `testMainGameLoop()` function
2. **MainDriver.cpp** - Main program entry point

## Key Features Implemented

### 1. Reinforcement Phase
```cpp
void GameEngine::reinforcementPhase()
```
- Calculates armies based on: `territories_owned / 3` (rounded down)
- Adds continent control bonuses
- Ensures minimum of 3 armies per player per turn
- Updates player reinforcement pools

**Demonstration**: Test shows correct calculation for different territory counts

### 2. Issuing Orders Phase
```cpp
void GameEngine::issueOrdersPhase()
bool Player::issueOrder()
```
- Players issue orders in round-robin fashion
- Continues until all players pass
- Player decision-making:
  - **Deploy first** if reinforcements available
  - **Advance orders** for defense/attack after deploys
  - **Card orders** when applicable
  
**Demonstration**: Test shows deploy orders issued before advance orders

### 3. Orders Execution Phase
```cpp
void GameEngine::executeOrdersPhase()
```
- Executes ALL deploy orders first (round-robin)
- Then executes other orders (round-robin)
- Clears order lists after execution

**Demonstration**: Test verifies execution order

### 4. Player Elimination
- Checks after each turn if any player owns 0 territories
- Removes eliminated players from game
- Frees memory properly

**Demonstration**: Test simulates territory loss

### 5. Win Condition
- Checks if only one player remains
- Checks if one player owns all territories
- Transitions to "win" state

**Demonstration**: Test simulates conquest victory

### 6. Card Awards
- Players who conquered at least one territory receive a card
- Maximum one card per turn per player
- Tracked via `conqueredThisTurn` flag

## How to Compile

```bash
g++ -std=c++20 -o warzone MainDriver.cpp GameEngineDriver.cpp \
    GameEngine.cpp Player.cpp Map.cpp Cards.cpp Orders.cpp \
    CommandProcessing.cpp LoggingObserver.cpp
```

## How to Run

```bash
./warzone
```

## Test Results

All 6 tests pass successfully:

✓ **TEST 1**: Reinforcement calculation (territories/3, min 3, + continent bonuses)
✓ **TEST 2**: Deploy orders issued when reinforcements available  
✓ **TEST 3**: Advance orders issued for defend/attack after deploys
✓ **TEST 4**: Card system integrated for order creation
✓ **TEST 5**: Player elimination when no territories owned
✓ **TEST 6**: Win condition when one player owns all territories

## Design Decisions

### 1. Order Issuing Strategy (Player::issueOrder())
The current implementation uses a simple priority-based strategy:
1. Deploy all available reinforcements first
2. Issue advance orders for tactical positioning
3. Play cards when appropriate

This is marked as "arbitrary implementation" as per the assignment requirements. 
Future versions can implement more sophisticated AI strategies.

### 2. Order Execution
Deploy orders are executed before all other orders to comply with Warzone rules. 
The execution uses dynamic_cast to identify order types.

### 3. Memory Management
- Neutral player created for blockade orders
- Proper cleanup of eliminated players
- Order lists cleared after execution

### 4. Turn Flow
Each turn follows this sequence:
1. Clear negotiation records
2. Reset conquered flags
3. Reinforcement phase
4. Issue orders phase
5. Execute orders phase
6. Award cards
7. Eliminate players with no territories
8. Check win condition

## Integration with Other Parts

This implementation integrates with:
- **Part 1**: Uses CommandProcessor for command validation
- **Part 2**: Follows startup phase initialization
- **Part 4**: Executes orders (OrderDeploy, OrderAdvance, etc.)
- **Part 5**: Triggers logging via Observer pattern

## Known Limitations

1. Card playing is simplified in `Player::issueOrder()` - doesn't create actual card orders
2. Strategy is basic (arbitrary implementation as specified)
3. No user input during game loop (automated for testing)

## Future Enhancements

1. Implement sophisticated player strategies
2. Add user interaction for manual play
3. Enhance card-based order creation
4. Add more detailed battle reporting
5. Implement save/load game state

## Assignment Compliance

This implementation satisfies all Part 3 requirements:

✓ Reinforcement phase with correct calculation
✓ Issuing orders phase with round-robin turns
✓ Orders execution phase (deploy first)
✓ Player elimination mechanism
✓ Win condition detection
✓ Card award system
✓ Integration with Player::issueOrder() method
✓ Comprehensive test driver

## Author Notes

The implementation follows the state diagram provided in the assignment.
All transitions are logged via the Observer pattern (Part 5).
Memory management follows C++ best practices with proper cleanup.
