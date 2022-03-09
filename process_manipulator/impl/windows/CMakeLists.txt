add_library(proc_man_windows_impl STATIC process_manipulator_impl.cpp)
target_include_directories(proc_man_windows_impl PUBLIC 
	../..
	../../utility/windows)
target_link_libraries(proc_man_windows_impl PUBLIC util_windows)
