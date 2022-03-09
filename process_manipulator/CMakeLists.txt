add_library(process_manipulator STATIC process_manipulator.cpp process_manipulator.hpp)
set_target_properties(process_manipulator PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BIANRY_DIR}/lib)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	# let cmake know about the subdirectory
	add_subdirectory(utility/linux)
	add_subdirectory(impl/linux)
	# tell cmake to add the subdirectory to the system's include path
	target_include_directories(process_manipulator PUBLIC utility/linux)
	target_link_libraries(process_manipulator PUBLIC 
		proc_man_linux_impl)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_subdirectory(utility/windows)
	add_subdirectory(impl/windows)
	target_include_directories(process_manipulator PUBLIC utility/windows)
	target_link_libraries(process_manipulator PUBLIC 
		proc_man_windows_impl)
else()
	message(FATA_ERROR "Implementation does not exist for the current OS")
endif()
