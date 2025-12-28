# risk-warzone ䷆
a c++ implementation of a simplified "risk" computer game, compatible with the warzone version of risk. this was made 
as part of our **comp 345 cumulative project**.

## game description 🎮
warzone is a turn-based strategy game where 2-6 players compete to conquer all territories on a map. the game map is 
represented as a connected graph where:
- each node represents a territory owned by a player
- edges represent adjacency between territories
- territories are grouped into continents (connected subgraphs)
- each continent provides bonus reinforcements when fully controlled

**objective:** conquer all territories on the map by deploying armies and attacking adjacent territories.

## features 👾
- **map system:** connected graph implementation with conquest map file loader and validation
- **player strategies:** human, aggressive, benevolent, neutral, and cheater players using strategy pattern
- **order system:** deploy, advance, bomb, blockade, airlift, and negotiate orders with validation
- **card mechanics:** deck and hand management with special ability cards
- **game engine:** complete startup, reinforcement, issuing, and execution phases
- **command processing:** console and file input with adapter pattern
- **observer logging:** game events logged to file using observer pattern
- **tournament mode:** automated multi-game tournaments comparing player strategies

## challenges faced 💢
- **double-free segfaults:** attempted to free a non-owning object that had already been freed by its destructor.

## what we learned 💭
- **modern c++:** smart pointers, RAII principles, move semantics, and c++20 features like `std::ranges`.
- **memory management:** ownership semantics, avoiding memory leaks through proper destructor implementation, and
understanding the rule of three/five for resource management.

## running the project 🏁

### prerequisites
- **ensure a [c++ compiler](https://en.cppreference.com/w/cpp/20.html) is installed:** this project uses c++20.
- **ensure [cmake3.10+](https://cmake.org/download/) is installed:** this project uses v3.10.

### setup instructions
1. **clone the repository**
```bash
git clone https://github.com/barbaraeguche/risk-warzone.git
```

2. **navigate to the project directory**
```bash
cd risk-warzone
```

3. **generate build files**
```bash
cmake . -B ./build
```

4. **build the project**
```bash
cmake --build build
```

5. **run the program**
```bash
./build/Debug/risk_warzone  # windows
./build/bin/risk_warzone    # mac
```

## game rules 🎲
- ### reinforcement
  - territories ÷ 3 (min 3)
  - continent bonuses

- ### combat
  - attacker: 60% kill rate
  - defender: 70% kill rate
  - card on conquest (max 1/turn)

- ### execution
  1. deploy orders first
  2. other orders round-robin
  3. validation on execution

## references ⛓️‍💥
- [warzone](https://www.warzone.com/)
- [conquest maps](http://www.windowsgames.co.uk/conquest_maps.html)