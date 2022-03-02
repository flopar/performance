/*-- HEADERS --*/
#ifdef __linux__ 
#include <sys/times.h>
#include <errno.h>
#include <sys/resource.h>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <unistd.h>
// util header
#include <util_linux.hpp>
#endif
#ifdef WIN32
#include<windows.h>
#include<util_windows.hpp>
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
#include <bitset>


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
#ifdef WIN32
static std::vector<int> schedPrioList = { IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, REALTIME_PRIORITY_CLASS };
static std::vector<int> threadPrioList = { THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL };
#endif

template<class T>
class process_manipulator
{
	private:
		/* -- Process Identification Attribute -- *
		 *		Linux:		PID
		 *		Windows:	HANDLE
		 * -------------------------------------- */
		T m_handle;

		/* -- Available CPUs -- */
		int m_systemCPUs = 0, m_processCPUs = 0, m_offlineCPUs = 0, m_threadCPUs = 0;
#ifdef __linux__
		cpu_set_t* cpuSet;
		size_t setSize;
#endif
#ifdef WIN32
		// This is the main thread of the calling process and is set in the class's constructor 		
		HANDLE m_threadHandle;

		// CPU utility varibales for a smoother and cleaner way of setting CPUs
		std::bitset<64> sysbits, procbits, threadbits;
#endif

	public:
		/* -- Constructor & Destructor -- */
		process_manipulator(T handle);
		~process_manipulator();
	
		/* -- Setters & getters -- */
		T get_handle();
		void set_handle(T handle);
		int getSysCPU();
		int getProcCPU();
		int getThreadCPU();

		
		/* -- System Times -- */
		int getProcessTimes(uint64_t& kernel, uint64_t& user);
		uint64_t getSpecificSystemTime(int which);
		int getSystemTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);
		
		/* -- Priorities -- */
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
		int increaseSchedClass(int8_t times=1);
		int decreaseSchedClass(int8_t times=1);
#else
#error "MacOS & ARM64 not implemented"
#endif

		/* -- CPU manipulation -- */
		void updateCPUs();
		int setThreadCPU(int pos, bool value);
#ifdef __linux__
#elif WIN32
		void updateCPUs(std::bitset<64> ProcessAffMask = 0, std::bitset<64> ThreadAffMask = 0);
		int setProcessCPU(uint8_t pos, bool value);
		int setThreadCPU(uint8_t pos, bool value);
#else
#error "MacOS & ARM64 not implemented"
#endif
};
