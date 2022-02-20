#include "system_linux.hpp"


/*
 *Description:  This function returns an unique_pointer in order to tell the user an allocation took place
 *		and at the same time telling him he won't have to care much about it
 */
// TODO: find a way to make use of unique_pointers 
int getSizeAndSet(pid_t tid, std::unique_ptr<cpu_set_t>& set, size_t&setSize)
{
	*set = CPU_ALLOC(get_nprocs());
	setSize = CPU_ALLOC_SIZE(get_nprocs());
	CPU_ZERO_S(setSize, *set);
	if(sched_getaffinity(tid, setSize, *set))
	{
		throw std::runtime_error("Failed getting the affinity mask for the given thread/process!\n");
		return 1;
	}
	return 0;
}

/*
 * Description: This function delivers the number of CPUs avaiable to the given pid.
 *		It is specified that the id passed on to "sched_getaffinity" changes
 *		the affinity of a spcific thread, hence the function does not differentiate
 *		between a TID and PID. If 0 is passed the calling thread will have its affinity
 *		changed. If a TID is passsed, that thread will have its affinity changed and (last
 *		but not least) if a PID is passed, that PID main thread will have its affinity
 *		changed.
 */
int checkCPUAvailability(size_t& processCPUs, size_t& systemCPUs, size_t& offlineCPUs, pid_t tid)
{	
	// Allocate memory for the affinity mask and get the needed size for the mask
	std::unique_ptr<cpu_set_t> affinity;
	// get the allocated memory size
	size_t size;
	if(getSizeAndSet(tid, &affinity, std::ref(size)))
	{
		throw std::runtime_error("Getting size and cpu set failed!\n");
		return 1;
	}
	processCPUs = static_cast<size_t>(CPU_COUNT_S(size, affinity));
	systemCPUs = static_cast<size_t>(get_nprocs());
	offlineCPUs = static_cast<size_t>(get_nprocs_conf()) - systemCPUs;
	CPU_FREE(affinity);
	return 0;
}

int setProcessSpecificAvailability(int cpu, bool value, pid_t tid)
{
	// check user input
	if((cpu < 0) || (cpu > get_nprocs()))
	{
		throw std::runtime_error("Given CPU is ambigious\n");
		return 1;
	}
	cpu_set_t* affinity;
	size_t size;
	if(getSizeAndSet(tid, &affinity, std::ref(size)))
	{
		throw std::runtime_error("Getting size and cpu set failed!\n");
		return 1;
	}
	// check if cpu is set
	int cpuSet = CPU_ISSET_S(cpu, size, affinity);
	if(value)
	{
		if(!cpuSet)
		{
			CPU_SET_S(cpu, size, affinity);
		}
	}
	else
	{
		if(cpuSet)
		{
			CPU_CLR_S(cpu, size, affinity);
		}
	}
}

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
	if(cpu > std::thread::hardware_concurrency() || cpu < 0)
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
