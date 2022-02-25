add_library(system_windows STATIC system_windows.cpp)

set_target_properties(system PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
