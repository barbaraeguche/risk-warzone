# create executable for A1
add_executable(comp_345_a2
	src/CommandProcessing.cpp
	src/CommandProcessing.h
	src/CommandProcessingDriver.cpp
)

# headers from other directories
target_include_directories(comp_345_a2 PUBLIC src)

# change compiler warning level
target_compile_options(comp_345_a2 PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>
)