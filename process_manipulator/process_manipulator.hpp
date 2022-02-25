/*-- HEADERS --*/
#ifdef __linux__ 
#include <sys/times.h>
#include <errno.h>
#include <sys/resource.h>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#endif
#ifdef WIN32
#include<windows.h>
#endif

// cross-platfrom headers
#include<cstdint>
#include<time.h>
// exceptions
#include<stdexcept>
// std::ref
#include<functional>

// Standard library headers
#include <vector>
#include <thread>
#include <iostream>
#include <util_linux.hpp>

/*-- DEFINES --*/
#define IDLE_TIME 0
#define USER_TIME 1
#define KERNEL_TIME 2
#ifdef __linux__
// Struct defined as the linux man pages require, but for some reason
// is not defined in the header files
struct sched_attr
{
	uint32_t size;			
	uint32_t sched_policy;
	uint64_t sched_flags;
	int32_t sched_nice;
	uint32_t sched_priority;
	/* The remaining fields are for SCHED_DEADLINE*/
	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
};
#endif
template<class T>
class process_manipulator
{
	private:
		T m_handle;
//		CPU m_cpu;
	public:
		// Constructor & deconstructor
		// constructor shall construct the cpu with the given handle as well
		process_manipulator(T handle);
		~process_manipulator() = default;
	

			
		// Setters & getters
		T get_handle();
		void set_handle(T handle);
		
		// System Times
		int getProcessTimes(uint64_t& kernel, uint64_t& user);
		uint64_t getSpecificSystemTime(int which);
		int getSystemTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);
		
		// Priorities	
		int increaseThreadPrio(int8_t times=1);
		int decreaseThreadPrio(int8_t times=1);
#ifdef __linux__
		// Linux specific priorities
		int decreaseProcessNiceValue();
		int increaseProcessNiceValue();
		void printPolicy(int policy);
		int changePolicy(int which);
#elif WIN32
		// Windows specific priorities
#else
		// MacOS & ARM64 no implemented
#endif

		// CPU manipulation for the given process and its threads
		

};
