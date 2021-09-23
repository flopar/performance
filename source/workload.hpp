#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include <map>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#endif
namespace workload
{
class Workload
{
  private:
      std::vector<std::thread> threadList;
      std::atomic_bool        runningSimulation;
      // Device dependable
      unsigned int             deviceThreads = std::thread::hardware_concurrency();
      bool                     asyncWorkload;

  public:
      Workload(unsigned int workloadPercentage, unsigned int* WLPriority, bool async);
      ~Workload() = default;
      void finishWorkload();
      void simulateWorkload(unsigned int workload, unsigned int randomize, unsigned int* WLPriority);

      // Simulation switch
      void startWL();
      void stopWL();
};
}
