#include <fstream>
#include <stdint.h>
#include <sys/times.h>
#include <vector>
#include <ios>
#include <functional>
#include <thread>
// maybe remove it later on
#include <iostream>


#define STR_ERR "String_ERROR" 


std::string readCPUTime(int cpu);
std::vector<uint64_t> returnData(int cpu);
int getProcessTimes(uint64_t& kernel, uint64_t& user);
int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);
int calculateAndShowLoad(double duration);
