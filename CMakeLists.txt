cmake_minimum_required(VERSION 3.10)
project(risk_warzone)

# set c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include executables directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# executables
add_executable(risk_warzone
	src/Cards.cpp
	src/CardsDriver.cpp
	src/CommandProcessing.cpp
	src/CommandProcessingDriver.cpp
	src/GameEngine.cpp
	src/GameEngineDriver.cpp
	src/LoggingObserver.cpp
	src/LoggingObserverDriver.cpp
	src/MainDriver.cpp
	src/Map.cpp
	src/MapDriver.cpp
	src/Orders.cpp
	src/OrdersDriver.cpp
	src/Player.cpp
	src/PlayerDriver.cpp
	src/PlayerStrategies.cpp
	src/PlayerStrategyDriver.cpp
	src/TournamentDriver.cpp
	src/Utility.cpp
)

# include headers
target_include_directories(risk_warzone PRIVATE src)

# change compiler warning level
target_compile_options(risk_warzone PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror -Wno-sign-compare>
)
