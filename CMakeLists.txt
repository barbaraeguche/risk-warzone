cmake_minimum_required(VERSION 3.10)
project(comp_345)

# set c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include executables directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# executables
add_executable(comp_345
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
)

# include headers
target_include_directories(comp_345 PRIVATE src)

# change compiler warning level
target_compile_options(comp_345 PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror -Wno-sign-compare>
)
