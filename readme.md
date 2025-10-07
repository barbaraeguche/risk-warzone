# comp-345

- entire a1 directory
```bash
cd A1
```

- compile from a1
```bash
g++ -std=c++20 $(find src -name "*.cpp") \
    -I src/core/map -I src/core/player -I src/core/orders \
    -I src/core/card-deck-hand -I src/core/game-engine \
    -o comp_345_a1
```

- run a1
```bash
./comp_345_a1
```