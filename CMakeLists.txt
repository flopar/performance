cmake_minimum_required(VERSION 3.12...3.21)

# in case of older version 
if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# set the c++ standard to at least c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(performance	VERSION 1.0
			DESCRIPTION "Bachelor Thesis - performance"
			LANGUAGES CXX)

# System Name must be asked after the project() command 
# otherwise the variable will be blank

# Enable -pthread and -lthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


# Always add the main source file
add_subdirectory(test)


# Allows the build of the library
#add_subdirectory(workload)
add_subdirectory(process_manipulator)

# Adds the directories to the include search path
target_include_directories(test PUBLIC 
	process_manipulator)
	#workload)

# Links the libraries to the main target
target_link_libraries(test
	Threads::Threads
	process_manipulator)
	#workload)


