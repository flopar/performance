add_library(system STATIC system.cpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_subdirectory(linux)
	target_include_directories(system PUBLIC linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_subdirectory(windows)
	target_include_directories(system PUBLIC windows)
endif()

target_link_libraries(system system_linux)

set_target_properties(system PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)



