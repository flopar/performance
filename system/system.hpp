#include <fstream>
#include <stdint.h>
#include <vector>
#include <functional>
#include <thread>
#include <algorithm>
#include <chrono>


// maybe remove it later on
#include <iostream>

#define IDLE_TIME 0
#define USER_TIME 1
#define KERNEL_TIME 2


#ifdef __linux__
#include "system_linux.hpp"
#elif WIN32
#include "system_windows.hpp"
#endif


// Non-OS-Specific methods
uint64_t getSpecificCPUTime(int which);
int convertTime(uint64_t timeToConvert);
int writeOverallStats(double stat, std::string statName);
int getProcessTimes(uint64_t& kernel, uint64_t& user);
int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);
int calculateAndShowLoad(double duration, std::vector<double>& processWLList, std::vector<double>& systemWLList);
double calculateSystemLoad(int duration);

