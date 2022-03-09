add_library(proc_man_linux_impl STATIC process_manipulator_impl.cpp)
target_include_directories(proc_man_linux_impl PUBLIC 
	../..
	../../utility/linux)
target_link_libraries(proc_man_linux_impl PUBLIC util_linux)
