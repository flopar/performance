#include "process_manipulator.hpp"

template <class T> 
process_manipulator<T>::process_manipulator(T handle)
{
	m_handle = handle;
}

template <class T> 
T process_manipulator<T>::get_handle()
{
	return m_handle;
}

template <class T> 
void process_manipulator<T>::set_handle(T handle)
{
	m_handle = handle;
}

template <class T>
int process_manipulator<T>::getSysCPU()
{
	return m_systemCPUs;
}

template <class T>
int process_manipulator<T>::getProcCPU()
{
	return m_systemCPUs;
}

#ifdef __linux__
template<>
process_manipulator<pid_t>::process_manipulator(pid_t handle)
{
	m_handle = handle;
	int cpus = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
	cpuSet = CPU_ALLOC(cpus);
	setSize = CPU_ALLOC_SIZE(cpus);
	CPU_ZERO_S(setSize, cpuSet);
	sched_getaffinity(handle, setSize, cpuSet);
	m_systemCPUs = m_processCPUs = m_threadCPUs = CPU_COUNT_S(setSize, cpuSet);
	m_offlineCPUs = m_systemCPUs - m_threadCPUs;
}

template<>
process_manipulator<pid_t>::~process_manipulator()
{
	CPU_FREE(cpuSet);
}
template<> 
pid_t process_manipulator<pid_t>::get_handle()
{
	return m_handle;
}

template<>
void process_manipulator<pid_t>::set_handle(pid_t handle)
{
	m_handle = handle;
}

template<>
int process_manipulator<pid_t>::getSysCPU()
{
	return m_systemCPUs;
}

template<>
int process_manipulator<pid_t>::getProcCPU()
{
	return m_processCPUs;
}

template<>
int process_manipulator<pid_t>::getThreadCPU()
{
	return m_threadCPUs;
}

template<>
void process_manipulator<pid_t>::updateCPUs()
{
	m_processCPUs = m_threadCPUs = CPU_COUNT_S(setSize, cpuSet);
}

template<>
int process_manipulator<pid_t>::setThreadCPU(int pos, bool value)
{
	if(pos > m_processCPUs || pos < 0)
	{
		throw std::range_error("CPU position to set is out of range");
		return 1;
	}
	bool cpu_set = CPU_ISSET_S(pos, setSize, cpuSet) ? true : false;
	if(cpu_set != value)
	{	
		if(value)
		{
			CPU_SET_S(pos, setSize, cpuSet);
		}
		else
		{
			CPU_CLR_S(pos, setSize, cpuSet);
		}
		if(sched_setaffinity(m_handle, setSize, cpuSet))
		{
			throw std::runtime_error("Setting the affinity mask for the given process failed\n");
			return 1;
		}
		updateCPUs();
	}
	else
	{
		throw std::runtime_error("Thread CPU already set to the given value!");
		return 1;
	}
}

template<class T>
int process_manipulator<T>::increaseProcessNiceValue()
{
	int retVal = 0, niceValue = 0;
	errno = 0;
	niceValue = getpriority(PRIO_PROCESS, m_handle);
	if(niceValue == -1)
	{
		if(errno != 0)
		{
			throw std::runtime_error("Couldn't get the nice value of process");
			return -1;
		}
		else
		{
			retVal = setpriority(PRIO_PROCESS, m_handle, -2);
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
			retVal = setpriority(PRIO_PROCESS, m_handle, niceValue);
			if(retVal == -1)
			{
				throw std::runtime_error("Couldn't set a new nice value to the given process");
				return -1;
			}
		}
	}
}

template<class T>
int process_manipulator<T>::decreaseProcessNiceValue()
{
	int retVal = 0, niceValue = 0;
	errno = 0;
	niceValue = getpriority(PRIO_PROCESS, m_handle);
	if(niceValue == -1)
	{
		if(errno != 0)
		{
			throw std::runtime_error("Couldn't get the nice value of process");
			return -1;
		}
		else
		{
			retVal = setpriority(PRIO_PROCESS, m_handle, 0);
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
			retVal = setpriority(PRIO_PROCESS, m_handle, niceValue);
			if(retVal == -1)
			{
				throw std::runtime_error("Couldn't set a new nice value to the given process");
				return -1;
			}
		}
	}
}

template<>
void process_manipulator<pid_t>::printPolicy(int policy)
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
template<>
int process_manipulator<pid_t>::changePolicy(int policy)
{
	struct sched_attr curr;
	errno = 0;
	if(!syscall(SYS_sched_getattr, m_handle, &curr, sizeof(curr), 0))
	{
		printPolicy(curr.sched_policy);
		printPolicy(policy);
		curr.sched_policy = policy;
		// Set the lowest priority a thread can have for the given policy
		// this step is necessary because per default this is set to 0 which is not a
		// viable option for real time policies
		curr.sched_priority = sched_get_priority_min(policy);
		if(!syscall(SYS_sched_setattr, m_handle, &curr, 0))
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

template<>
int process_manipulator<pid_t>::increaseThreadPrio(int8_t times)
{
	if(times < 0)
	{
		throw std::invalid_argument("Given argument is not allowed!\n");
		return 1;
	}
	int currPolicy = sched_getscheduler(m_handle);
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
			if(!syscall(SYS_sched_getattr, m_handle, &prio, sizeof(prio), 0))
			{
				if(prio.sched_priority == max_prio)
				{
					throw::std::runtime_error("Max priority already set!");
					return 1;
				}
				else
				{
					if((prio.sched_priority+=times) > max_prio)
					{
						std::cout << "Increase argument was too high!\n" << "Setting the maximum priority allowed: " << max_prio << std::endl;
						prio.sched_priority = max_prio;
					}
					if(syscall(SYS_sched_setattr, m_handle, &prio, 0))
					{
						throw std::runtime_error("Couldn't increase priority of thread(increaseThreadPrio)");
						return 1;
					}
					std::cout << "New thread's priority: " << prio.sched_priority << std::endl;
				}
			}
		}
	}
}

template<>
int process_manipulator<pid_t>::decreaseThreadPrio(int8_t times)
{
	errno = 0;
	int currPolicy = sched_getscheduler(m_handle);
	if(currPolicy == -1)
	{
		throw std::runtime_error("Couldn't get the given thread's policy (decreaseThreadPrio)");
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
			int min_prio = sched_get_priority_min(currPolicy);
			if(!syscall(SYS_sched_getattr, m_handle, &prio, sizeof(prio), 0))
			{
				std::cout << "Thread's current priority: " << prio.sched_priority << std::endl;
				if(prio.sched_priority == min_prio)
				{
					throw::std::runtime_error("Min priority already set!");
					return 1;
				}
				else
				{
					if((prio.sched_priority -=times) < min_prio)
					{
						std::cout << "Decrease argument was too high!\n" << "Setting the minimum priority allowed: " << min_prio << std::endl;
						prio.sched_priority = min_prio;
					}
					if(syscall(SYS_sched_setattr, m_handle, &prio, 0))
					{
						throw std::runtime_error("Couldn't decrease priority of thread(decreaseThreadPrio)");
						return 1;
					}
					std::cout << "New thread's priority: " << prio.sched_priority << std::endl;
				}
			}
		}
	}
}
#endif
#ifdef WIN32
template<>
process_manipulator<HANDLE>::process_manipulator(HANDLE handle)
{
	m_handle = handle;
	m_threadHandle = GetCurrentThread();

	// get the available CPUs
	uint64_t ProcAffinityMask = 0, SystemAffinityMask = 0;
	BOOL ret = GetProcessAffinityMask(m_handle, &ProcAffinityMask, &SystemAffinityMask);
	if (!ret)
	{
		throw std::runtime_error("Couldn't get affinity mask for the given process");
	}
	sysbits = std::bitset<64>{ SystemAffinityMask };
	procbits = std::bitset<64>{ ProcAffinityMask };
	threadbits = procbits;

	m_systemCPUs = sysbits.count();
	m_processCPUs = procbits.count();
	m_threadCPUs = m_processCPUs;
	m_offlineCPUs = m_systemCPUs - m_processCPUs;
}

template<>
HANDLE process_manipulator<HANDLE>::get_handle()
{
	return m_handle;
}

template<>
void process_manipulator<HANDLE>::set_handle(HANDLE handle)
{
	m_handle = handle;
}

template<>
int process_manipulator<HANDLE>::getSysCPU()
{
	return m_systemCPUs;
}

template<>
int process_manipulator<HANDLE>::getProcCPU()
{
	return m_systemCPUs;
}

template<>
int process_manipulator<HANDLE>::getThreadCPU()
{
	return m_threadCPUs;
}

template<>
int process_manipulator<HANDLE>::increaseThreadPrio(int8_t times)
{
	if (times < 0)
	{
		std::invalid_argument("Given argument is not allowed!\n");
		return 1;
	}
	int currThreadPrio = GetThreadPriority(m_thread_handle);
	if (currThreadPrio == THREAD_PRIORITY_ERROR_RETURN)
	{
		std::runtime_error("The thread's priority could not be gotten!\n");
		return 1;
	}
	std::cout << "Current thread prio: " << currThreadPrio << std::endl;
	if (currThreadPrio == threadPrioList.back())
	{
		throw std::runtime_error("Thread Priority cannot be increased. Highest priority is already set!");
		return 1;
	}
	int* iterator = std::find(&threadPrioList.front(), &threadPrioList.back(), currThreadPrio);
	for(int i = 0 ; i < times; i++)
	{
		if (*iterator == THREAD_PRIORITY_TIME_CRITICAL && i != times)
		{
			std::cout << "Increase argument was too high!\n" << "Setting the maximum priority allowed: " << THREAD_PRIORITY_TIME_CRITICAL << std::endl;
			currThreadPrio = THREAD_PRIORITY_TIME_CRITICAL;
		}
		iterator++;
	}
	currThreadPrio = *iterator;
	if (!SetThreadPriority(m_thread_handle, currThreadPrio))
	{
		std::runtime_error("The priority could not be set!\n");
		return 1;
	}
	std::cout << "New thread prio: " << GetThreadPriority(m_thread_handle) << std::endl;
	return 0;
}

template<>
int process_manipulator<HANDLE>::decreaseThreadPrio(int8_t times)
{
	if (times < 0)
	{
		std::invalid_argument("Given argument is not allowed!\n");
		return 1;
	}
	int currThreadPrio = GetThreadPriority(m_thread_handle);
	if (currThreadPrio == THREAD_PRIORITY_ERROR_RETURN)
	{
		std::runtime_error("The thread's priority could not be gotten!\n");
		return 1;
	}
	std::cout << "Current thread prio: " << currThreadPrio << std::endl;
	if (currThreadPrio == threadPrioList.front())
	{
		throw std::invalid_argument("Thread Priority cannot be decreased. Lowest priority is already set!");
		return 1;
	}
	int* iterator = std::find(&threadPrioList.front(), &threadPrioList.back(), currThreadPrio);
	for (int i = times; i > 0; i--)
	{
		if (*iterator == THREAD_PRIORITY_IDLE && i != times)
		{
			std::cout << "Decrease argument was too high!\n" << "Setting the minimum priority allowed: " << THREAD_PRIORITY_IDLE << std::endl;
			currThreadPrio = THREAD_PRIORITY_IDLE;
		}
		iterator--;
	}
	currThreadPrio = *iterator;
	if (!SetThreadPriority(m_thread_handle, currThreadPrio))
	{
		std::runtime_error("The priority could not be set!\n");
		return 1;
	}
	std::cout << "New thread prio: " << GetThreadPriority(m_thread_handle) << std::endl;
	return 0;
}

template<>
int process_manipulator<HANDLE>::increaseSchedClass(int8_t times)
{
	if (times < 0)
	{
		std::invalid_argument("Given argument is not allowed!\n");
		return 1;
	}
	// Get current PrioClass and if its not the highest increase the value of it
	uint32_t currProcClassPrio = GetPriorityClass(GetCurrentProcess());
	if (!currProcClassPrio)
	{
		std::runtime_error("Couldn't get the class priority for the given process!\n");
		return 1;
	}
	std::cout << "Current prio class " << currProcClassPrio << std::endl;
	if (currProcClassPrio == schedPrioList.back())
	{
		throw std::invalid_argument("Priority Class cannot be increased. Highest class is already set!");
		return 1;
	}
	int* iterator = std::find(&schedPrioList.front(), &schedPrioList.back(), currProcClassPrio);
	for (int i = 0; i < times; i++)
	{
		if (*iterator == schedPrioList.back() && i != times)
		{
			std::cout << "Increase argument was too high!\n" << "Setting the maximum priority allowed: " << schedPrioList.back() << std::endl;
			currProcClassPrio = schedPrioList.back();
		}
		iterator++;
	}
	currProcClassPrio = *iterator;
	if (!SetPriorityClass(m_handle, currProcClassPrio))
	{
		std::runtime_error("The class could not be set!\n");
		return 1;
	}
	std::cout << "New class prio: " << GetPriorityClass(m_handle) << std::endl;
	return 0;
}

template<>
int process_manipulator<HANDLE>::decreaseSchedClass(int8_t times)
{
	if (times < 0)
	{
		std::invalid_argument("Given argument is not allowed!\n");
		return 1;
	}
	// Get current PrioClass and if its not the highest increase the value of it
	uint32_t currProcClassPrio = GetPriorityClass(GetCurrentProcess());
	if (!currProcClassPrio)
	{
		std::runtime_error("Couldn't get the class priority for the given process!\n");
		return 1;
	}
	std::cout << "Current prio class " << currProcClassPrio << std::endl;
	if (currProcClassPrio == schedPrioList.front())
	{
		throw std::invalid_argument("Priority Class cannot be decreased. Lowest class is already set!");
		return 1;
	}
	int* iterator = std::find(&schedPrioList.front(), &schedPrioList.back(), currProcClassPrio);
	for (int i = times; i > 0; i--)
	{
		if (*iterator == schedPrioList.front() && i != times)
		{
			std::cout << "Decrease argument was too high!\n" << "Setting the minimum priority allowed: " << schedPrioList.front() << std::endl;
			currProcClassPrio = schedPrioList.front();
		}
		iterator--;
	}
	currProcClassPrio = *iterator;
	if (!SetPriorityClass(m_handle, currProcClassPrio))
	{
		std::runtime_error("The class could not be set!\n");
		return 1;
	}
	std::cout << "New class prio: " << GetPriorityClass(m_handle) << std::endl;
	return 0;
}

template <>
void process_manipulator<HANDLE>::updateCPUs()
{
	m_processCPUs = procbits.count();
	m_threadCPUs = threadbits.count();
}

template<>
int process_manipulator<HANDLE>::setProcessCPU(uint8_t pos, bool value)
{
	uint64_t ProcAffinityMask;
	if (pos > m_processCPUs || pos < 0)
	{
		throw std::range_error("CPU position to set is out of range");
		return 1;
	}
	if (procbits[pos] != value)
	{
		procbits.set(pos, value);
		ProcAffinityMask = procbits.to_ullong();
		int check = SetProcessAffinityMask(m_handle, std::ref(ProcAffinityMask));
		if (!check)
		{
			throw std::runtime_error("Setting the afinity masked for the given process failes");
			return 1;
		}
		updateCPUs();
		return 0;
	}
	else
	{
		throw std::runtime_error("Process CPU already set to the given value!");
		return 1;
	}
}

template<>
int process_manipulator<HANDLE>::setThreadCPU(uint8_t pos, bool value)
{
	uint64_t ThreadAffinityMask;
	if (pos > m_processCPUs || pos < 0)
	{
		throw std::range_error("CPU position to set is out of range");
		return 1;
	}
	if (threadbits[pos] != value)
	{
		threadbits.set(pos, value);
		ThreadAffinityMask = threadbits.to_ullong();
		int check = SetThreadAffinityMask(m_handle, std::ref(ThreadAffinityMask));
		if (!check)
		{
			throw std::runtime_error("Setting the afinity masked for the given process failes");
			return 1;
		}
		updateCPUs();
		return 0;
	}
	else
	{
		throw std::runtime_error("Thread CPU already set to the given value!");
		return 1;
	}
}

#endif
template<class T>
int process_manipulator<T>::getProcessTimes(uint64_t& kernel, uint64_t& user)
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
	ret = GetProcessTimes(m_handle, &creationTime, &exitTime, &kernelTime, &userTime);
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

template<class T>
uint64_t process_manipulator<T>::getSpecificSystemTime(int which)
{
	uint64_t kernel ,idle, user;
	kernel = idle = user = 0;
	int retval = getSystemTimes(std::ref(kernel), std::ref(user), std::ref(idle));
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

template<class T>
int process_manipulator<T>::getSystemTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle)
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
