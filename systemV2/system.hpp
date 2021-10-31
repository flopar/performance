#include <fstream>
#include <stdint.h>
#include <vector>
#include <ios>
#include <functional>
#include <thread>
#include <ios>
#include <bitset>
#include <algorithm>
#include <chrono>
#include <fstream>

// maybe remove it later on
#include <iostream>

#define STR_ERR "String_ERROR" 
#define IDLE_TIME 0
#define USER_TIME 1
#define KERNEL_TIME 2


// windows headers, defines and methods
#ifdef WIN32
#include <windows.h>
static std::vector<int> schedPrioList = {IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, REALTIME_PRIORITY_CLASS};
static std::vector<int> threadPrioList = {THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL};

uint64_t mergeFILETIME(FILETIME ft);
int setCPUAvailability(HANDLE& proc, uint64_t mask);
int checkCPUAvailability(HANDLE& proc);
int increaseSchedClass();
int decreaseSchedClass();
#endif

// linux headers, defines and methods
#ifdef __linux__
#include <sys/times.h>
#include <errno.h>
#include <sys/resource.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

struct sched_attr{
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

std::string readCPUTime(int cpu);
std::vector<uint64_t> returnData(int cpu);
void printPolicy(int policy);
int changePolicy(int policy, int pid);
int decreaseProcessNiceValue(int pid=0);
int increaseProcessNiceValue(int pid=0);

#endif

// Non-OS-Specific methods
uint64_t getSpecificCPUTime(int which);
int convertTime(uint64_t timeToConvert);
template<typename type> int writeOverallStats(type stat, std::string statName);
template<typename type> int writeRuntimeStats(type list, std::string statName);
int getProcessTimes(uint64_t& kernel, uint64_t& user);
int increaseThreadPrio(int id=0);
int decreaseThreadPrio(int id=0);
int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);
int calculateAndShowLoad(double duration);


