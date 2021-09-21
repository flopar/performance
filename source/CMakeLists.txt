add_executable(test test.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(test PUBLIC Threads::Threads)

add_library(workload STATIC workload.cpp workload.hpp)
target_link_libraries(test PUBLIC workload)
