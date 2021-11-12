add_library(workload STATIC workload.cpp workload.hpp)

# Enable -pthread and -lthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_include_directories(workload PUBLIC ../systemV2)

# link the libraries
target_link_libraries(workload PUBLIC Threads::Threads)
target_link_libraries(workload PUBLIC system)


set_target_properties(workload PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
