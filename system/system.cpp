#include "system.hpp"

#ifdef __linux__
std::string readCPUTime(int cpu)
{
	std::string text;
	std::ifstream inputStream("/proc/stat");
	for(size_t i = 0; i <= cpu; i++)
	{
		getline(inputStream, text);
	}
	// Check if the fail bit is set
	if(inputStream.fail())
	{
		return STR_ERR; 
	}
	else
	{
		//always returns the last line read
		return text;
	}
}

std::vector<uint64_t> returnData(int cpu)
{
	// CPU 0 -> System
	std::size_t searchPos = 5, strEndPos = 0;
	// If we want a certain cpu's data we need to set the search position to 4 because of
	// of the name "cpuN", where 4 is the position of the first space in the entry
	if(cpu > std::thread::hardware_concurrency())
	{
		throw std::range_error("Given CPU is out of range");
	}
	if(cpu)
	{
		searchPos = 4;
	}
	std::string compound = readCPUTime(cpu);
	std::vector<uint64_t> data;
	if(compound == STR_ERR)
	{
		throw std::runtime_error("Could not read data line from the given CPU");
	}
	else
	{
		while(strEndPos != std::string::npos)
		{
			strEndPos = compound.find(" ", searchPos + 1);
			data.push_back(stoull(std::string(compound, searchPos, strEndPos - searchPos), nullptr));
			searchPos = strEndPos + 1;
		}
		return data;
	}
}

int increaseProcessNiceValue(int pid)
{
	int retVal = 0, niceValue = 0;
	errno = 0;
	niceValue = getpriority(PRIO_PROCESS, pid);
	if(niceValue == -1)
	{
		if(errno != 0)
		{
			throw std::runtime_error("Couldn't get the nice value of process");
			return -1;
		}
		else
		{
			retVal = setpriority(PRIO_PROCESS, pid, -2);
			if(retVal == -1)
			{
				throw std::runtime_error("Couldn't set a new nice value to the given process");
				return -1;
			}
		}
	}
	else
	{
		if(niceValue <= -19)
		{
			throw std::runtime_error("Can't increase the process's priority. The max priority is already set");
			return -1;
		}
		else
		{
			niceValue--;
			retVal = setpriority(PRIO_PROCESS, pid, niceValue);
			if(retVal == -1)
			{
				throw std::runtime_error("Couldn't set a new nice value to the given process");
				return -1;
			}
		}
	}
}

int decreaseProcessNiceValue(int pid)
{
	int retVal = 0, niceValue = 0;
	errno = 0;
	niceValue = getpriority(PRIO_PROCESS, pid);
	if(niceValue == -1)
	{
		if(errno != 0)
		{
			throw std::runtime_error("Couldn't get the nice value of process");
			return -1;
		}
		else
		{
			retVal = setpriority(PRIO_PROCESS, pid, 0);
			if(retVal == -1)
			{
				throw std::runtime_error("Couldn't set a new nice value to the given process");
				return -1;
			}
		}
	}
	else
	{
		if(niceValue >= 20)
		{
			throw std::runtime_error("Can't decrease the process's priority. The min priority is already set");
			return -1;
		}
		else
		{
			niceValue++;
			retVal = setpriority(PRIO_PROCESS, pid, niceValue);
			if(retVal == -1)
			{
				throw std::runtime_error("Couldn't set a new nice value to the given process");
				return -1;
			}
		}
	}
}

void printPolicy(int policy)
{
	switch(policy)
	{
		case SCHED_FIFO:
			std::cout << "Current scheduling policy is SCHED_FIFO\n";
			break;
		case SCHED_RR:
			std::cout << "Current scheduling policy is SCHED_RR\n";
			break;
		case SCHED_IDLE:
			std::cout << "Current scheduling policy is SCHED_IDLE\n";
			break;
		case SCHED_OTHER:
			std::cout << "Current scheduling policy is SCHED_OTHER\n";
			break;
		case SCHED_BATCH:
			std::cout << "Current scheduling policy is SCHED_BATCH\n";
			break;
		default:
			break;
	}
}

int changePolicy(int policy, int pid)
{
	struct sched_attr curr;
	errno = 0;
	if(!syscall(SYS_sched_getattr, pid, &curr, sizeof(curr), 0))
	{
		printPolicy(curr.sched_policy);
		printPolicy(policy);
		curr.sched_policy = policy;
		curr.sched_priority = 99;
		if(!syscall(SYS_sched_setattr, pid, &curr, 0))
		{
			std::cout << "Current policy changed" << std::endl;
		}
		else
		{
			throw std::runtime_error("Couldn't change policy\n");
			std::cout << "Errno " << errno << std::endl;
			return 1;
		}
	}
	else
	{
		throw std::runtime_error("Could not get current attributes of the current policy");
		std::cout << "Errno " << errno << std::endl;
		return 1;
	}
}
#endif

#ifdef WIN32
// This is necessary because Filetime is a struct with two 32-bit attributes
uint64_t mergeFILETIME(FILETIME ft)
{
	uint64_t merge = 0;
	merge |= static_cast<uint64_t>(ft.dwLowDateTime);
	merge |= static_cast<uint64_t>(ft.dwHighDateTime) << 32;
	return merge;
}

int checkCPUAvailability(HANDLE& proc)
{
	uint64_t ProcAffinityMask = 0, SystemAffinityMask = 0;
	BOOL ret = GetProcessAffinityMask(proc, &ProcAffinityMask, &SystemAffinityMask);
	if (!ret)
	{
		throw std::runtime_error("Couldn't get affinity mask for the given process");
		return 1;
	}
	else
	{
		std::cout << "Process cpu availability: " << std::uppercase << std::hex << ProcAffinityMask << std::endl;
		std::cout << "Proc mask binary equivalent: " << std::bitset<8>{ProcAffinityMask} << std::endl;
		std::cout << "System cpu availability: " << std::uppercase << std::hex << SystemAffinityMask << std::endl;
		std::cout << "System mask binary equivalent: " << std::bitset<8>{ProcAffinityMask} << std::endl;
		return 0;
	}
}

// this function needs rethinking....
int setCPUAvailability(HANDLE& proc, uint64_t mask)
{
	BOOL ret = SetProcessAffinityMask(proc, mask);
	if (!ret)
	{
		throw std::runtime_error("Setting the afinity masked for the given process failes");
		return 1;
	}
	return 0;
}


int increaseSchedClass()
{
	// Get current PrioClass and if its not the highest increase the value of it
	uint32_t currProcClassPrio = GetPriorityClass(GetCurrentProcess());
	std::cout << "Current prio class " << currProcClassPrio << std::endl;
	if (currProcClassPrio == schedPrioList.back())
	{
		throw std::invalid_argument("Priority Class cannot be increased. Highest class is already set!");
		return 1;
	}
	else
	{
		int index = 0;
		int* iterator = &schedPrioList.front();
		while (iterator != &schedPrioList.back())
		{
			if (*iterator == currProcClassPrio)
			{
				index++;
				SetPriorityClass(GetCurrentProcess(), schedPrioList.at(index));
				std::cout << "New prio class: " << GetPriorityClass(GetCurrentProcess()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		
	}
}

int decreaseSchedClass()
{
	// Get current PrioClass and if its not the lowest decrease the value of it
	uint32_t currProcClassPrio = GetPriorityClass(GetCurrentProcess());
	std::cout << "Current prio class " << currProcClassPrio << std::endl;
	if (currProcClassPrio == schedPrioList.front())
	{
		throw std::invalid_argument("Priority Class cannot be decreased. Lowest class is already set!");
		return 1;
	}
	else
	{
		// Find the index in the list and decrease it by 1
		int index = 0;
		int* iterator = &schedPrioList.front();
		while (iterator != &schedPrioList.back())
		{
			if (*iterator == currProcClassPrio)
			{
				index--;
				SetPriorityClass(GetCurrentProcess(), schedPrioList.at(index));
				std::cout << "New prio class: " << GetPriorityClass(GetCurrentProcess()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 1;
	}
}

#endif

#ifdef WIN32
int increaseThreadPrio(HANDLE id)
{
	int currThreadPrio;
	if (id == 0)
	{
		currThreadPrio = GetThreadPriority(GetCurrentThread());
	}
	else
	{
		currThreadPrio = GetThreadPriority(id);
	}
	std::cout << "Current thread prio: " << currThreadPrio << std::endl;
	if (currThreadPrio == threadPrioList.back())
	{
		throw std::invalid_argument("Thread Priority cannot be increased. Highest priority is already set!");
		return 1;
	}
	else
	{
		int index = 0;
		int* iterator = &threadPrioList.front();
		while (iterator != &threadPrioList.back())
		{
			if (*iterator == currThreadPrio)
			{
				index++;
				if (id == 0)
				{
					SetThreadPriority(GetCurrentThread(), threadPrioList.at(index));
				}
				else
				{
					SetThreadPriority(id, threadPrioList.at(index));
				}
				std::cout << "New thread Prio: " << GetThreadPriority(GetCurrentThread()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 1;
	}
}
#elif __linux__
int increaseThreadPrio(int id)
{
	int currPolicy = sched_getscheduler(id);
	if(currPolicy == -1)
	{
		throw std::runtime_error("Couldn't get the given thread's policy (increaseThreadPrio)");
		return 1;
	}
	else
	{ 
		if(currPolicy != SCHED_FIFO && currPolicy != SCHED_RR)
		{
			printPolicy(currPolicy);
			throw std::runtime_error("You cannot change the priority of the given thread while this policy is set!");
			return 1;
		}
		else
		{
			printPolicy(currPolicy);
			struct sched_attr prio;
			errno = 0;
			int max_prio = sched_get_priority_max(currPolicy);
			if(!syscall(SYS_sched_getattr, id, &prio, sizeof(prio), 0))
			{
				if(prio.sched_priority == max_prio)
				{
					throw::std::runtime_error("Max priority already set!");
					return 1;
				}
				else
				{
					prio.sched_priority++;
					if(syscall(SYS_sched_setattr, id, &prio, 0))
					{
						throw std::runtime_error("Couldn't decrease priority of thread(decreaseThreadPrio)");
						return 1;
					}
				}
			}
		}
	}
}
#endif

// make additional argument with the id of a thread
#ifdef WIN32
int decreaseThreadPrio(HANDLE id)
{
	int currThreadPrio;
	if (id == 0)
	{
		currThreadPrio = GetThreadPriority(GetCurrentThread());
	}
	else
	{
		currThreadPrio = GetThreadPriority(id);
	}
	std::cout << "Thread prio: " << currThreadPrio << std::endl;
	if (currThreadPrio == threadPrioList.front())
	{
		throw std::invalid_argument("Thread Priority cannot be decreased. Lowest priority is already set!");
		return 1;
	}
	else
	{
		int index = 0;
		int* iterator = &threadPrioList.front();
		while (iterator != &threadPrioList.back())
		{
			if (*iterator == currThreadPrio)
			{
				index--;
				if (id == 0)
				{
					SetThreadPriority(GetCurrentThread(), threadPrioList.at(index));
				}
				else
				{
					SetThreadPriority(id, threadPrioList.at(index));
				}
				std::cout << "New thread Prio: " << GetThreadPriority(GetCurrentThread()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 1;
	}
}
#elif __linux__
int decreaseThreadPrio(int id)
{
	errno = 0;
	int currPolicy = sched_getscheduler(id);
	if(currPolicy == -1)
	{
		throw std::runtime_error("Couldn't get the given thread's policy (decreaseThreadPrio)");
		return 1;
	}
	else
	{
		if(currPolicy != SCHED_FIFO || currPolicy != SCHED_RR)
		{
			printPolicy(currPolicy);
			throw std::runtime_error("You cannot change the priority of the given thread while this policy is set!");
			return 1;
		}
		else
		{
			printPolicy(currPolicy);
			struct sched_attr prio;
			errno = 0;
			int min_prio = sched_get_priority_min(currPolicy);
			if(!syscall(SYS_sched_getattr, id, &prio, sizeof(prio), 0))
			{
				if(prio.sched_priority == min_prio)
				{
					throw::std::runtime_error("Min priority already set!");
					return 1;
				}
				else
				{
					prio.sched_priority--;
					if(syscall(SYS_sched_setattr, id, &prio, 0))
					{
						throw std::runtime_error("Couldn't decrease priority of thread(decreaseThreadPrio)");
						return 1;
					}
				}
			}
		}
	}
}
#endif


int getProcessTimes(uint64_t& kernel, uint64_t& user)
{
#ifdef __linux__
	struct tms time;
	if(times(&time) == (clock_t)-1)
	{
		throw std::runtime_error("times() failes. Process times cannot be read\n");
	}
	user = static_cast<uint64_t>(time.tms_utime);
	kernel = static_cast<uint64_t>(time.tms_stime);
	return 0;
#endif
#ifdef WIN32
	FILETIME kernelTime, userTime, creationTime, exitTime;
	BOOL ret;
	ret = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	if (!ret)
	{
		throw std::runtime_error("GetProcessTimes failed!");
		return 1;
	}
	user = mergeFILETIME(userTime);
	kernel = mergeFILETIME(kernelTime);
	return 0;
#endif
}

uint64_t getSpecificCPUTime(int which)
{
	uint64_t kernel ,idle, user;
	kernel = idle = user = 0;
	int retval = getCPUTimes(std::ref(kernel), std::ref(user), std::ref(idle));
	if(retval)
	{
		throw std::invalid_argument("Couldn't get specific time");	
		return -1;
	}
	else
	{
		if(which == KERNEL_TIME)
		{
			return kernel;
		}
		if(which == USER_TIME)
		{
			return user;
		}
		if(which == IDLE_TIME)
		{
			return idle;
		}
	}
}


/*---------------------------------------------
 * This function returns the given time in ms *
 ---------------------------------------------*/
int convertTime(uint64_t timeToConvert)
{
#ifdef __linux__
	// get system frequency => 1/ret pro sec
	long ret = sysconf(_SC_CLK_TCK);
	if(ret!=-1)
	{
		std::cout << "sys freq " << ret << std::endl;
		return static_cast<int>(timeToConvert/ret);		
	}
	else
	{
		throw std::runtime_error("Failed to get system's frequency");
		return -1;
	}
#elif WIN32
	// the time represents timeToConvert in 100ns intervals
	return static_cast<int>(timeToConvert*0.1);	
#else
// MacOS not implemented yet
#endif
}

int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle)
{
#ifdef __linux__
	std::vector<uint64_t> measurements;
	measurements = returnData(0);	
	// we have to add the values because measurements[0] are only the normal processes in user
	// mode, while measurements[1] are niced processes in user mode. Together they represent the
	// whole user mode
	user = measurements[0] + measurements[1];
	kernel = measurements[2];
	idle = measurements[3];
	return 0;
#elif WIN32
	FILETIME idle_time, kernel_time, user_time;
	BOOL ret = GetSystemTimes(&idle_time, &kernel_time, &user_time);
	if(!ret)
	{
		throw std::runtime_error("GetSystemTimes failed!");
		return 1;
	}
	user = mergeFILETIME(user_time);
	kernel = mergeFILETIME(kernel_time);
	idle = mergeFILETIME(idle_time);
	return 0;
#elif
// MacOS not implemented yet
#endif
}
int writeOverallStats(auto stat, std::string statName)
{
	std::ofstream statisticsFile;
	statisticsFile.open("overall_stats.csv", std::ios::in | std::ios::app | std::ios::binary);
	if(!statisticsFile.is_open())
	{
		throw std::runtime_error("Couldn't open the file to write");
		return -1;
	}
	statisticsFile << statName << ": " << stat << std::endl; 
	statisticsFile.close();
	return 0;
}
double calculateSystemLoad(int duration)
{
	
	// System Times
	uint64_t kernel = 0, pre_kernel = 0;
	uint64_t user = 0, pre_user = 0;
	uint64_t idle = 0, pre_idle = 0;

	double workload = 0;
	int ret = 0;
	ret += getCPUTimes(std::ref(pre_kernel), std::ref(pre_user), std::ref(pre_idle));
	if(!ret)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		ret += getCPUTimes(std::ref(kernel), std::ref(user), std::ref(idle));
		if(!ret)
		{
			kernel -= pre_kernel;
			user -= pre_user;
			idle -= pre_idle;
			std::cout << "kerne " << kernel << std::endl << "user " << user << std::endl << "idle " << idle << std::endl;
#ifdef __linux__
			workload = 100.0 * static_cast<double>(kernel + user) / static_cast<double>(kernel + user + idle);
			std::cout << "workload " << workload;
#elif WIN32
			workload = 100.0 * static_cast<double>(kernel - idle + user) / static_cast<double>(kernel + user);
#else
			//MacOS not implemented
#endif
			return workload;
		}
		else
		{
			throw std::runtime_error("(calculateSystemLoad) GetCPUTimes failed!\n");
			return static_cast<double>(-1);
		}
	}
	else
	{
		throw std::runtime_error("(calculateSystemLoad) GetCPUTimes failed!\n");
		return static_cast<double>(-1);
	}
	
}
int calculateAndShowLoad(double duration, std::vector<double>& processWLList, std::vector<double>& systemWLList)
{
	// Error Checking Value
	int check_return_val = 0;


	// System Times
	uint64_t kernel_time = 0, pre_kernel_time = 0;
	uint64_t user_time = 0, pre_user_time = 0;
	uint64_t idle_time = 0, pre_idle_time = 0;
	
	// Process Times
	uint64_t pre_process_kernel_time = 0, process_kernel_time = 0;
	uint64_t pre_process_user_time = 0, process_user_time = 0;

	double system_workload = 0, system_workload_sum = 0;
	double process_workload = 0, process_workload_sum = 0;
	double average = duration;
	double avg_process_wl = 0, avg_system_wl = 0;


	// For not idle vor
	auto startDuration = std::chrono::system_clock::now();
	int pre_notIdle = convertTime(getSpecificCPUTime(IDLE_TIME));

	while(duration)
	{
		check_return_val += getCPUTimes(std::ref(pre_kernel_time), std::ref(pre_user_time), std::ref(pre_idle_time));
		check_return_val += getProcessTimes(std::ref(pre_process_kernel_time), std::ref(pre_process_user_time));
		if(!check_return_val)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			check_return_val += getCPUTimes(std::ref(kernel_time), std::ref(user_time), std::ref(idle_time));
			check_return_val += getProcessTimes(std::ref(process_kernel_time), std::ref(process_user_time));
			if(!check_return_val)
			{
				// CPU Times
				kernel_time -= pre_kernel_time;
				user_time -= pre_user_time;
				idle_time -= pre_idle_time;
				
				// Process Times
				process_kernel_time -= pre_process_kernel_time;
				process_user_time -= pre_process_user_time;				
#ifdef __linux__
				process_workload = 100.0 * static_cast<double>(process_kernel_time + process_user_time) / static_cast<double>(kernel_time + user_time + idle_time);
				system_workload = 100.0 * static_cast<double>(kernel_time + user_time) / static_cast<double>(kernel_time + user_time + idle_time);
#elif WIN32
				process_workload = 100.0 * static_cast<double>(process_kernel_time + process_user_time) / static_cast<double>(kernel_time + user_time);
				system_workload = 100.0 * static_cast<double>(kernel_time - idle_time + user_time) / static_cast<double>(kernel_time + user_time);
#else
				//MacOS not implemented yet
#endif
				process_workload_sum += process_workload;
				system_workload_sum += system_workload;
				processWLList.push_back(process_workload);
				systemWLList.push_back(system_workload);
				std::cout << "Current process workload " << process_workload << std::endl;
				std::cout << "Current system workload " << system_workload << std::endl;
				duration--;
			}
			else
			{
				// until I think of something pls don't fail!!!

				// we got no failure at the beggining, but we can still fail now
				// in case of failure we shall repeat the calls 
				// Herr Kirchmeier: do you think is a good idea? if yes how is 1
				// repetition enough?
			}
		}
		else
		{
			// what if we fail from the beggining? -> can we even fail? make sense? idk
		}
	}
	auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startDuration).count();
	if(pre_notIdle != -1)
	{
		int notIdle = convertTime(getSpecificCPUTime(IDLE_TIME));
		notIdle -= pre_notIdle;
		std::cout << "idle time " << notIdle << "ms" << std::endl;
		notIdle = totalDuration - notIdle;
		std::cout << "The amount of time the processor has not been idle: " << notIdle << "ms" << std::endl;
		writeOverallStats(notIdle, "NotIdleTime");

	}
	std::cout << "total duration " << totalDuration << std::endl;		
	avg_system_wl = system_workload_sum / average;
	avg_process_wl = process_workload_sum / average;
	writeOverallStats(avg_system_wl, "AverageSystemWorkload");
	writeOverallStats(avg_process_wl, "AverageProcessWorkload");
	std::cout << "Average process workload " << avg_process_wl << std::endl;
	std::cout << "Average system workload " << avg_system_wl << std::endl;
	return 0;
}
