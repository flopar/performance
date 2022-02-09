// Usefull defines
#define STR_ERR "String_ERROR" 

// Linux specific headers
#include <sys/times.h>
#include <errno.h>
#include <sys/resource.h>
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>

// Standard library headers
#include <fstream>
#include <vector>
#include <thread>
#include <stdint.h>
#include <iostream>

// Struct defined as the linux man pages require
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
int decreaseProcessNiceValue(int pid=0);
int increaseProcessNiceValue(int pid=0);
void printPolicy(int policy);
int changePolicy(int policy, int pid);
int increaseThreadPrio(int id = 0);
int decreaseThreadPrio(int id = 0);
