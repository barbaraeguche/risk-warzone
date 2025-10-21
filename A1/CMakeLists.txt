# create executable for A1
add_executable(comp_345_a1
	src/MainDriver.cpp
	src/Map.cpp
	src/MapDriver.cpp
	src/Player.cpp
	src/PlayerDriver.cpp
	src/Orders.cpp
	src/OrdersDriver.cpp
	src/Cards.cpp
	src/CardsDriver.cpp
	src/GameEngine.cpp
	src/GameEngineDriver.cpp
)

# headers from other directories
target_include_directories(comp_345_a1 PUBLIC
	include
	src
)

# change compiler warning level
target_compile_options(comp_345_a1 PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>
)