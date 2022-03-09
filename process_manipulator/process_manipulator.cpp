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

