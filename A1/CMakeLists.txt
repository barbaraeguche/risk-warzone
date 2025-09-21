# create executable for A1
add_executable(comp_354_a1 src/MainDriver.cpp)

# headers from other directories
target_include_directories(comp_354_a1 PRIVATE
	include
	src/core
)