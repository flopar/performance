add_library(system_linux STATIC system_linux.cpp)

set_target_properties(system_linux PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
