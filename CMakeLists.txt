cmake_minimum_required(VERSION 3.7...3.21)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(performance	VERSION 1.0
			DESCRIPTION "Bachelor Thesis - performance"
			LANGUAGES CXX)

add_subdirectory(source)
#add_subdirectory(system)
#add_subdirectory(workload)

set_target_properties(test PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
