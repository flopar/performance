/*-- HEADERS --*/
#include<cstdint>
#include<string>
#include<fstream>
#include<vector>
#include<thread>

/*-- DEFINES --*/
#define STR_ERR "String_ERROR" 

std::string readCPUTime(int cpu);
std::vector<uint64_t> returnData(int cpu);
