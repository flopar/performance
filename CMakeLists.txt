cmake_minimum_required(VERSION 3.8...3.21)

# in case of older version 
if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# set the c++ standard to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message("System: ${CMAKE_SYSTEM_NAME}")
project(performance	VERSION 1.0
			DESCRIPTION "Bachelor Thesis - performance"
			LANGUAGES CXX)

add_subdirectory(source)

# Allows the build of the library
add_subdirectory(system)
add_subdirectory(workload)

# Adds the directories to the include search path
target_include_directories(test PUBLIC 
	system
	workload)

# Links the libraries to the main target
target_link_libraries(test 
	system 
	workload)
