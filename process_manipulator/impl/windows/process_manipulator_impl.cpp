#include<process_manipulator.hpp>
#include<util_windows.hpp>

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

template <>
process_manipulator<HANDLE>::~process_manipulator() {}

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
	int currThreadPrio = GetThreadPriority(m_threadHandle);
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
	if (!SetThreadPriority(m_threadHandle, currThreadPrio))
	{
		std::runtime_error("The priority could not be set!\n");
		return 1;
	}
	std::cout << "New thread prio: " << GetThreadPriority(m_threadHandle) << std::endl;
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
	int currThreadPrio = GetThreadPriority(m_threadHandle);
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
	if (!SetThreadPriority(m_threadHandle, currThreadPrio))
	{
		std::runtime_error("The priority could not be set!\n");
		return 1;
	}
	std::cout << "New thread prio: " << GetThreadPriority(m_threadHandle) << std::endl;
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
int process_manipulator<HANDLE>::setProcessCPU(int pos, bool value)
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
int process_manipulator<HANDLE>::setThreadCPU(int pos, bool value)
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

template<>
TimesCollection process_manipulator<HANDLE>::getProcessTimes()
{
	FILETIME kernelTime, userTime, creationTime, exitTime;
	BOOL ret;
	ret = GetProcessTimes(m_handle, &creationTime, &exitTime, &kernelTime, &userTime);
	if (!ret)
	{
		throw std::runtime_error("GetProcessTimes failed!");
		return 1;
	}
	uint64_t user = 0, kernel = 0;
	user = mergeFILETIME(userTime);
	kernel = mergeFILETIME(kernelTime);
	return TimesCollection{user,kernel,NULL};
}

template<>
TimesCollection process_manipulator<HANDLE>::getSystemTimes()
{
	FILETIME idle_time, kernel_time, user_time;
	BOOL ret = GetSystemTimes(&idle_time, &kernel_time, &user_time);
	if(!ret)
	{
		throw std::runtime_error("GetSystemTimes failed!");
		return 1;
	}
	uint64_t user = mergeFILETIME(user_time);
	uint64_t kernel = mergeFILETIME(kernel_time);
	uint64_t idle = mergeFILETIME(idle_time);
	return TimesCollection{user, kernel, idle};
}

