#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include <map>
#include <chrono>
#include <string.h>
#include <sstream>

#ifdef __linux__
// For testing purposes
#include <unistd.h>
#elif WIN32
#include <windows.h>
#endif

#define MAX_PRIO 1

namespace workload
{
class Workload
{
  private:
      std::vector<std::thread> m_threadList;
      std::atomic_bool m_runningSimulation, m_startSimulation;
      std::atomic<uint32_t> m_workload;
      // Device dependable
      unsigned int             m_deviceThreads = std::thread::hardware_concurrency();
      bool                     m_asyncWorkload;

  public:
      Workload(unsigned int workloadPercentage, unsigned int* WLPriority, bool async);
      ~Workload() = default;
      void finishWorkload();
      void simulateWorkload(unsigned int randomize, unsigned int* WLPriority);
      void setWorkload(uint32_t workload);

      // Simulation switch
      void startWL();
      void stopWL();
};
}
