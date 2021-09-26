add_library(workload SHARED workload.cpp workload.hpp)

# Enable -pthread and -lthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# link the libraries
target_link_libraries(workload PUBLIC Threads::Threads)

set_target_properties(workload PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
