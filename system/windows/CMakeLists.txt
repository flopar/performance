add_library(system STATIC system.cpp)

set_target_properties(system PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
