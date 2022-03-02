add_library(process_manipulator STATIC process_manipulator.cpp process_manipulator.hpp)
set_target_properties(process_manipulator PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BIANRY_DIR}/lib)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_subdirectory(utility/linux)
	target_include_directories(process_manipulator PUBLIC utility/linux)
	target_link_libraries(process_manipulator 
		util_linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_subdirectory(utility/windows)
	target_include_directories(process_manipulator PUBLIC utility/windows)
	target_link_libraries(process_manipulator 
		util_windows)
endif()
