#include <iostream>
#include <stdint.h>
#include <bitset>
#include <ios>
#include <vector>
#include <windows.h>

// Defines
static std::vector<int> schedPrioList = {IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS, REALTIME_PRIORITY_CLASS};
static std::vector<int> threadPrioList = {THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL};


uint64_t mergeFILETIME(FILETIME ft);
int checkCPUAvailability(HANDLE& proc);
int setCPUAvailability(HANDLE& proc, uint64_t mask);
int increaseSchedClass();
int decreaseSchedClass();
int increaseThreadPrio(HANDLE id = 0);
int decreaseThreadPrio(HANDLE id = 0);