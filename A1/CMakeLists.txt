# create executable for A1
add_executable(comp_354_a1
	src/MainDriver.cpp
	src/core/player/Player.cpp
	src/core/player/PlayerDriver.cpp
	src/core/orders/Orders.cpp
	src/core/orders/OrdersDriver.cpp
	src/core/card-deck-hand/CardsDriver.cpp
	src/core/game-engine/GameEngine.cpp
	src/core/game-engine/GameEngineDriver.cpp
)

# headers from other directories
target_include_directories(comp_354_a1 PRIVATE
	include
	src/core
	src/core/player
	src/core/orders
	src/core/card-deck-hand
	src/core/game-engine
)