cmake_minimum_required(VERSION 3.8...3.21)

# in case of older version 
if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# set the c++ standard to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(performance	VERSION 1.0
			DESCRIPTION "Bachelor Thesis - performance"
			LANGUAGES CXX)

add_subdirectory(source)


set_target_properties(test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


#add_subdirectory(system)
#add_subdirectory(workload)
#target_link_libraries(test PUBLIC workload)
#target_include_directories(test PUBLIC 
#	"${PROJECT_BINARY_DIR}"
#	"${PROJECT_SOURCE_DIR}/workload")
