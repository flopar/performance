add_library(util_windows STATIC util_windows.cpp util_windows.hpp)
set_target_properties(util_windows PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

