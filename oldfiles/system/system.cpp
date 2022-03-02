#include "system.hpp"

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
int writeOverallStats(double stat, std::string statName)
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
				//std::cout << "Current process workload " << process_workload << std::endl;
				//std::cout << "Current system workload " << system_workload << std::endl;
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
		double notIdle = convertTime(getSpecificCPUTime(IDLE_TIME));
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
