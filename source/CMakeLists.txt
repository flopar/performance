add_executable(test test.cpp)

#add_library(system STATIC system.cpp system.hpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_library(workload STATIC workload.cpp workload.hpp)
target_link_libraries(workload PUBLIC Threads::Threads)
target_link_libraries(test PUBLIC workload)
