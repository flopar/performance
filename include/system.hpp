#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <codecvt>
#include <tchar.h>
#include <tlhelp32.h>
#endif
#ifdef __linux__
#include <sys/times.h>
#endif
#include <thread>
#include <fstream>
#include "utilities.hpp"



namespace SimulationSystem
{
class SystemInterface
{
public:
  explicit SystemInterface() = default;
  ~SystemInterface();
#ifdef __linux__
  std::string readCPUTime(int cpu);
  std::vector<unsigned long long> returnData(int cpu);
#elif _WIN32
  unsigned long long merge_FILETIME(FILETIME ft);
  const HANDLE       verifyIfCurrProc(std::string nameToBeVerified);
#else
//MacOS not implemented yet
#error
#endif
  int getCPUTimes(unsigned long long& kernel, unsigned long long& user, unsigned long long& idle);
  int getProcessTimes(unsigned long long& kernel, unsigned long long& user, unsigned long long& creation);
};
int showCPULoad(unsigned int& duration, NumberValues& InternalMeasurements);
}