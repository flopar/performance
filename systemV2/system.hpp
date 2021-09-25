#include <fstream>
#include <stdint.h>
#include <sys/times.h>
#include <vector>


std::string readCPUTime(int cpu);
std::vector<uint64_t> returnData(int cpu);
int getProcessTimes(uint64_t& kernel, uint64_t& user);
int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle);

