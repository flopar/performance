#include "system_windows.hpp"

// This is necessary because Filetime is a struct with two 32-bit attributes
uint64_t mergeFILETIME(FILETIME ft)
{
	uint64_t merge = 0;
	merge |= static_cast<uint64_t>(ft.dwLowDateTime);
	merge |= static_cast<uint64_t>(ft.dwHighDateTime) << 32;
	return merge;
}

int getCPUMasks(uint64_t& ProcAffinityMask, uint64_t& SystemAffinityMask, HANDLE proc)
{
	if (proc == 0)
	{
		proc = GetCurrentProcess();
	}
	BOOL ret = GetProcessAffinityMask(proc, &ProcAffinityMask, &SystemAffinityMask);
	if (!ret)
	{
		throw std::runtime_error("Couldn't get affinity mask for the given process");
		return 1;
	}
	return 0;
}

void checkCPUAvailability(size_t& processCPUs, size_t& systemCPUs, HANDLE proc)
{
	uint64_t ProcAffinityMask = 0, SystemAffinityMask = 0;
	int check = getCPUMasks(ProcAffinityMask, SystemAffinityMask, proc);
	if (!check)
	{
		std::bitset systembits = std::bitset<128>{ SystemAffinityMask };
		std::bitset procbits = std::bitset<128>{ ProcAffinityMask };
		std::cout << "proc count: " << procbits.count() << std::endl;
		systemCPUs = systembits.count();
		processCPUs = procbits.count();
	}
}

int setProcessSpecificAvailability(uint8_t pos, bool value, HANDLE proc)
{
	if (proc == 0)
	{
		proc = GetCurrentProcess();
	}
	uint64_t ProcAffinityMask = 0, SystemAffinityMask = 0;
	int check = getCPUMasks(ProcAffinityMask, SystemAffinityMask, proc);
	std::bitset procbits = std::bitset<128>{ ProcAffinityMask };
	if (pos > procbits.count() || pos < 0)
	{
		throw std::range_error("CPU position to set is out of range");
		return 1;
	}
	procbits.set(pos, value);
	ProcAffinityMask = procbits.to_ullong();
	check = SetProcessAffinityMask(proc, ProcAffinityMask);
	if (!check)
	{
		throw std::runtime_error("Setting the afinity masked for the given process failes");
		return 1;
	}
	return 0;
}

int increaseSchedClass(HANDLE proc)
{
	if (proc == 0)
	{
		proc = GetCurrentProcess();
	}
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
				SetPriorityClass(proc, schedPrioList.at(index));
				std::cout << "New prio class: " << GetPriorityClass(GetCurrentProcess()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 0;
	}
}

int decreaseSchedClass(HANDLE proc)
{
	if (proc == 0)
	{
		proc = GetCurrentProcess();
	}
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
				SetPriorityClass(proc, schedPrioList.at(index));
				std::cout << "New prio class: " << GetPriorityClass(GetCurrentProcess()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 0;
	}
}

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
					id = GetCurrentThread();
				}
				SetThreadPriority(id, threadPrioList.at(index));
				std::cout << "New thread Prio: " << GetThreadPriority(GetCurrentThread()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 0;
	}
}

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
					id = GetCurrentThread();
				}
				SetThreadPriority(id, threadPrioList.at(index));
				std::cout << "New thread Prio: " << GetThreadPriority(GetCurrentThread()) << std::endl;
				return 0;
			}
			index++;
			iterator++;
		}
		return 0;
	}
}
