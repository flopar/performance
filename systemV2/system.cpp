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
	// If the we want a certain cpu's data we need to set the search position to 4 because of
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
#endif


int getProcessTimes(uint64_t& kernel, uint64_t& user)
{
	struct tms time;
	if(times(&time) == (clock_t)-1)
	{
		throw std::runtime_error("times() failes. Process times cannot be read\n");
	}
	user = static_cast<uint64_t>(time.tms_utime);
	kernel = static_cast<uint64_t>(time.tms_stime);
	return 0;
}

int getCPUTimes(uint64_t& kernel, uint64_t& user, uint64_t& idle)
{
	std::vector<uint64_t> measurements;
	measurements = returnData(0);	
	// we have to add the values because measurements[0] are only the normal processes in user
	// mode, while measurements[1] are niced processes in user mode. Togheter they represent the
	// whole user mode
	user = measurements[0] + measurements[1];
	kernel = measurements[2];
	// not idle vorkommt????
	idle = measurements[3];
	return 0;
}
int calculateAndShowLoad(double duration)
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

	while(duration)
	{
		check_return_val += getCPUTimes(std::ref(pre_kernel_time), std::ref(pre_user_time), std::ref(pre_idle_time));
		check_return_val += getProcessTimes(std::ref(pre_process_kernel_time), std::ref(pre_process_user_time));
		if(check_return_val == 0)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			check_return_val += getCPUTimes(std::ref(kernel_time), std::ref(user_time), std::ref(idle_time));
			check_return_val += getProcessTimes(std::ref(process_kernel_time), std::ref(process_user_time));
			if(check_return_val == 0)
			{
				// CPU Times
				kernel_time -= pre_kernel_time;
				user_time -= pre_user_time;
				idle_time -= pre_idle_time;
				
				// Process Times
				process_kernel_time -= pre_process_kernel_time;
				process_user_time -= pre_process_user_time;				

				process_workload = 100.0 * static_cast<double>(process_kernel_time + process_user_time) / static_cast<double>(kernel_time + user_time + idle_time);
				process_workload_sum += process_workload;
				std::cout << "Current process workload " << process_workload << std::endl;

				system_workload = 100.0 * static_cast<double>(kernel_time + user_time) / static_cast<double>(kernel_time + user_time + idle_time);
				system_workload_sum += system_workload;
				std::cout << "Current system workload " << system_workload << std::endl;
				duration--;
			}
			else
			{
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
	avg_system_wl = system_workload_sum / average;
	avg_process_wl = process_workload_sum / average;
	std::cout << "Average process workload " << avg_process_wl << std::endl;
	std::cout << "Average system workload " << avg_system_wl << std::endl;
}
