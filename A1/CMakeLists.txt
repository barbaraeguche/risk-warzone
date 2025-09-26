# create executable for A1
add_executable(comp_345_a1
	src/MainDriver.cpp
	src/core/player/Player.cpp
	src/core/player/PlayerDriver.cpp
	src/core/order/Order.cpp
	src/core/order/OrderDriver.cpp
	src/core/card-deck-hand/CardsDriver.cpp
	src/core/game-engine/GameEngine.cpp
	src/core/game-engine/GameEngineDriver.cpp
)

# headers from other directories
target_include_directories(comp_345_a1 PRIVATE
	include
	src/core
	src/core/player
	src/core/order
	src/core/card-deck-hand
	src/core/game-engine
)

# change compiler warning level
target_compile_options(comp_345_a1 PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)