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


// maybe remove it later on
#include <iostream>

#define IDLE_TIME 0
#define USER_TIME 1
#define KERNEL_TIME 2


#ifdef __linux__
#include "system_linux.hpp"
#endif



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
int increaseThreadPrio(HANDLE id = 0);
int decreaseThreadPrio(HANDLE id = 0);
#endif

// Non-OS-Specific methods
uint64_t getSpecificCPUTime(int which);
int convertTime(uint64_t timeToConvert);
int writeOverallStats(double stat, std::string statName);
int getProcessTimes(uint64_t& kernel, uint64_t& user);
int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);
int calculateAndShowLoad(double duration, std::vector<double>& processWLList, std::vector<double>& systemWLList);
double calculateSystemLoad(int duration);

