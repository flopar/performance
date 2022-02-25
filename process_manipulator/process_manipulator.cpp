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

#ifdef __linux__
template<>
process_manipulator<pid_t>::process_manipulator(pid_t handle)
{
	m_handle = handle;
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
