#include "system.hpp"

using namespace SimulationSystem;


SystemInterface::~SystemInterface() 
{
  std::cout << "-- Finishing Measurement\n";
}

#ifdef __linux__
std::string SimulationSystem::SystemInterface::readCPUTime(int cpu) 
{
  std::string text;
  std::ifstream inputStream("/proc/stat");
  for (size_t i = 0; i <= cpu; i++)
  {
    getline(inputStream, text);
  }
  // always returns the last line read
  return text;
}

// This function retrieves the information of the passed cpu number. CPU=0 
std::vector<unsigned long long> SimulationSystem::SystemInterface::returnData(int cpu)
{
  // CPU = 0 -> System
  std::size_t searchPos = 5, strEndPos = 0;
  if (cpu)
  {
    searchPos = 4;
  }
  std::string          compound = SystemInterface::readCPUTime(cpu);
  std::vector<unsigned long long> data;
  while (strEndPos != std::string::npos)
  {
    strEndPos = compound.find(" ", searchPos + 1);
    data.push_back(stoull(std::string(compound, searchPos, strEndPos - searchPos), nullptr));
    searchPos = strEndPos + 1;
  }
  return data;
}

int SimulationSystem::SystemInterface::getProcessTimes(unsigned long long& kernel, unsigned long long& user,
                                                  unsigned long long& creation)
{
  struct tms time;
  times(&time);
  user     = static_cast<unsigned long long>(time.tms_utime);
  kernel   = static_cast<unsigned long long>(time.tms_stime);
  creation = 0; // change this maybe read from file;
  return 0;
}
#endif

int SimulationSystem::SystemInterface::getCPUTimes(unsigned long long& kernel, unsigned long long& user,
                                              unsigned long long& idle)
{
#ifdef __linux__
  std::vector<unsigned long long> measurement = returnData(0);
  user                                        = measurement[0] + measurement[1];
  kernel                                      = measurement[2];
  idle                                        = measurement[3];
  return 0;
#elif _WIN32
  // free time
  FILETIME FT_idle_time;
  // kernel time
  FILETIME FT_kernel_time;
  // user time
  FILETIME FT_user_time;
  BOOL     ret = GetSystemTimes(&FT_idle_time, &FT_kernel_time, &FT_user_time);
  if (ret == 0)
  {
    std::cout << "GetSystemTimes failed..." << std::endl;
    return 1;
  }
  user   = merge_FILETIME(FT_user_time);
  kernel = merge_FILETIME(FT_kernel_time);
  idle   = merge_FILETIME(FT_idle_time);
  return 0;
#else
//MacOS not implemented yet
#error
#endif
}

#ifdef _WIN32
// Merge the attributes of FILETIME into a single variable
unsigned long long SimulationSystem::SystemInterface::merge_FILETIME(FILETIME ft)
{
  unsigned long long merge = 0;
  merge |= static_cast<unsigned long long>(ft.dwLowDateTime);
  merge |= static_cast<unsigned long long>(ft.dwHighDateTime) << 32;
  return merge;
}


// Flo:         you could also just call GetCurrentProcess instead of verify_if_is_curr_process but I 
//              invested some time understanding how the system works and i would like to keep it if possible
//              ps: Roman, Christian, Laura & Ricardo let my child live please   ?(  ????  )?
//
// Christian:   any dead code must go one day, harr harr harr     (???) ?
// Roman:       No comment yet -> TBD
// Laura:       No comment yet -> TBD
// Ricardo:     No comment yet -> TBD
const HANDLE SimulationSystem::SystemInterface::verifyIfCurrProc(std::string nameToBeVerified)
{
  HANDLE processSnap;
  processSnap                                                       = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  unsigned long                                    checkprocessId   = GetCurrentProcessId();
  unsigned long                                    currentProcessId = 0;
  PROCESSENTRY32                                   currentProcess;
  int                                              check = 0;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
  // do this otherwise Process32First will fail
  currentProcess.dwSize = sizeof(PROCESSENTRY32);
  std::string processName;
  if (!Process32First(processSnap, &currentProcess))
  {
    std::cout << "failed to get process list\n";
  }
  // Parse the list of processes until we find the right one
  while (processName != nameToBeVerified)
  {
    processName      = conv1.to_bytes(currentProcess.szExeFile);
    currentProcessId = currentProcess.th32ProcessID;
    Process32Next(processSnap, &currentProcess);
  }
  CloseHandle(processSnap);
  // Check wether the id of our current Process matches the test's executable id
  if (checkprocessId == currentProcessId)
  {
    return GetCurrentProcess();
  }
  return NULL;
}

int SimulationSystem::SystemInterface::getProcessTimes(unsigned long long& kernel, unsigned long long& user,
                                      unsigned long long& creation, unsigned long long* exit, HANDLE processHandle)
{
  FILETIME ProcessKernelTime, ProcessUserTime, ProcessCreationTime, exitTime;
  BOOL ret;
  // exitTime should be undefinde because the current process is still running
  ret = GetProcessTimes(processHandle, &ProcessCreationTime, &exitTime, &ProcessKernelTime, &ProcessUserTime);
  if (ret == 0)
  {
    std::cout << "GetProcessTimes failed..." << std::endl;
    return 1;
  }
  user     = merge_FILETIME(ProcessUserTime);
  kernel   = merge_FILETIME(ProcessKernelTime);
  creation = merge_FILETIME(ProcessCreationTime);
  *exit    = merge_FILETIME(exitTime);
  return 0;
}
#endif

int SimulationSystem::showCPULoad(unsigned int& duration, NumberValues& InternalMeasurements)
{
  int checkFailure = 0, maxCycleRepetitions = 0;
  // System Times
  unsigned long long kernel_time = 0, pre_kernel_time = 0, user_time = 0, pre_user_time = 0, idle_time = 0,
                            pre_idle_time = 0;
  // Process Times
  unsigned long long preProcessKernelTime = 0, ProcessKernelTime = 0, preProcessUserTime = 0, ProcessUserTime = 0,
                     preProcessIdleTime = 0, ProcessIdleTime = 0, ProcessCreationTime = 0, ProcessExitTime = 0;

  double Workload = 0, WorkloadSum = 0;
  double ProcessWL = 0, ProcessWLSum = 0;
  double average = static_cast<double>(duration), numberOfCycles = static_cast<double>(duration);
  std::cout << "-- Workload-thread(s): START..." << std::endl;
  SimulationSystem::SystemInterface SI = SimulationSystem::SystemInterface();
  while (numberOfCycles)
  {
    if (static_cast<int>(numberOfCycles) == static_cast<int>(average * 0.75))
    {
      std::cout << "-- 25% completed...\n";
    }
    if (static_cast<int>(numberOfCycles) == static_cast<int>(average * 0.5))
    {
      std::cout << "-- 50% completed...\n";
    }
    if (static_cast<int>(numberOfCycles) == static_cast<int>(average * 0.25))
    {
      std::cout << "-- 75% completed...\n";
    }
    checkFailure += SI.getCPUTimes(std::ref(pre_kernel_time), std::ref(pre_user_time), std::ref(pre_idle_time));

#ifdef _WIN32
    checkFailure += SI.getProcessTimes(std::ref(preProcessKernelTime), std::ref(preProcessUserTime),
                                    std::ref(ProcessCreationTime), &ProcessExitTime, GetCurrentProcess());
    if (checkFailure == 0)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      checkFailure += SI.getCPUTimes(std::ref(kernel_time), std::ref(user_time), std::ref(idle_time));
      checkFailure += SI.getProcessTimes(std::ref(ProcessKernelTime), std::ref(ProcessUserTime),
                                      std::ref(ProcessCreationTime), &ProcessExitTime, GetCurrentProcess());
      if (checkFailure == 0)
      {
        // CPU Workload
        kernel_time -= pre_kernel_time;
        idle_time -= pre_idle_time;
        user_time -= pre_user_time;

        // Process Workload
        ProcessKernelTime -= preProcessKernelTime;
        ProcessUserTime -= preProcessUserTime;

        // On windows the idle time is included in the kernel_time
        ProcessWL = 100.0 * static_cast<double>(ProcessKernelTime + ProcessUserTime) /
                    static_cast<double>(kernel_time + user_time);
        ProcessWLSum += ProcessWL;
        // We need to considerate that kernel_time also includes the idle_time of the processor.
        // So subtract it from that value in order to get a meaningful equation
        Workload = 100.0 * static_cast<double>(kernel_time - idle_time + user_time) /
                   static_cast<double>(kernel_time + user_time);

#elif __linux__
    checkFailure += SI.getProcessTimes(std::ref(preProcessKernelTime), std::ref(preProcessUserTime), std::ref(ProcessCreationTime));
    if (checkFailure == 0)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      checkFailure += SI.getCPUTimes(std::ref(kernel_time), std::ref(user_time), std::ref(idle_time));   
      checkFailure += SI.getProcessTimes(std::ref(ProcessKernelTime), std::ref(ProcessUserTime), std::ref(ProcessCreationTime));     
      if (checkFailure == 0)
      {
        // CPU Workload
        kernel_time -= pre_kernel_time;
        idle_time -= pre_idle_time;
        user_time -= pre_user_time;

        // Process Workload
        ProcessKernelTime -= preProcessKernelTime;
        ProcessUserTime -= preProcessUserTime;
        ProcessWL = 100.0 * static_cast<double>(ProcessKernelTime + ProcessUserTime) /
                    static_cast<double>(kernel_time + user_time + idle_time);
        ProcessWLSum += ProcessWL;
        Workload = 100.0 * static_cast<double>(kernel_time + user_time) /
                   static_cast<double>(kernel_time + user_time + idle_time);
#else
#error
#endif
        WorkloadSum += Workload;
        numberOfCycles--;

        // Reset the repetition counter if we could successfully estimate the load
        maxCycleRepetitions = 0;
      }
      else
      {
        // If we repeat the cycle too many times the estimated load will be more unprecise as it is already
        if (maxCycleRepetitions == 1)
        {
          std::cout << "-- We repeated the cycle unsuccessfully\n"
                    << "-- Aborting...\n";
          return 1;
        }
        maxCycleRepetitions++;
      }
    }
    else
    {
      // If we repeat the cycle too many times the estimated load will be more unprecise as it is already
      if (maxCycleRepetitions == 1)
      {
        std::cout << "-- We repeated the cycle unsuccessfully\n"
                  << "-- Aborting...\n";
        return 1;
      }
      maxCycleRepetitions++;
    }
  }
  InternalMeasurements[AVG_PROC_WL] = static_cast<unsigned int>(ProcessWLSum / average);
  InternalMeasurements[AVG_CPU_WL] =  static_cast<unsigned int>(WorkloadSum / average);
  std::cout << "-- Workload thread : END..." << std::endl;
  return 0;
}
