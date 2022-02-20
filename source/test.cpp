#include <workload.hpp>
#include <system.hpp>

#define NORMAL 0
#define CRITICAL 1
/* 
 * this function will write statistic to a csv File on if the type of "list" is an iterable object
 */
int writeRuntimeStats(std::vector<double> list, std::string statName)
{
	std::ofstream statisticsFile;
	statisticsFile.open("runtime_statistics.csv", std::ios::in | std::ios::app | std::ios::binary);
	if (!statisticsFile.is_open())
	{
		throw std::runtime_error("Couldn't open file to write");
		return -1;
	}
	statisticsFile << statName << ",";
	int lenList = list.size() - 1;
	for (int i = 0; i < lenList; i++)
	{
		if (i < lenList - 1)
		{
			statisticsFile << list[i] << ",";
		}
		else
		{
			std::cout << "newline at" << std::endl;
			statisticsFile << list[i] << std::endl;
		}
	}
	statisticsFile.close();
	return 0;
}

void test(int mode, unsigned int percentage, bool async)
{
	std::vector<double> procWorkload, sysWorkload;

	// flag to indicate normal or critical workload for the workload class
	unsigned int prio = NULL;
#ifdef WIN32
	bool incrementClass = true;
#endif
#ifdef __linux__
	bool incrementNice = true;
#endif
	try
	{	
		/* If mode was set to CRITICAL we will increase this thread's priortiy and set a
		 * real time scheduling policy depending on the calling OS */ 
		if(mode)
		{
#ifdef __linux__
			// Change Policy
			changePolicy(SCHED_RR, 0);
			prio = MAX_PRIO;
			try
			{
				// Increase process nice value
				while(incrementNice)
				{
					std::cout << "increasing nice value...\n";
					increaseProcessNiceValue();
				}
			}
			catch(const std::exception& e)
			{
				incrementNice = false;
				std::cout << e.what() << std::endl;
			}
#endif
#ifdef WIN32
			// Change Priority Class
			try
			{
				while(incrementClass)
				{
					std::cout << "increasing class prio...\n";
					increaseSchedClass();
				}
			}
			catch(const std::exception& e)
			{
				incrementClass = false;
				std::cout << e.what() << std::endl;
			}
#endif
		}
		// Create Workload, start it, analyze it, stop it and write the results to a file
		workload::Workload worky(percentage, &prio, async);
		worky.startWL();
		calculateAndShowLoad(60, std::ref(procWorkload), std::ref(sysWorkload));	
		worky.stopWL();
		worky.finishWorkload();
		
		writeRuntimeStats(procWorkload, "ProcessWorkload");
		writeRuntimeStats(sysWorkload, "SystemWorkload");
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


int main(int argc, char* argv[])
{
	//test(CRITICAL, 10, true);	
	//test(NORMAL, 90, false);
	
	size_t system = 0, proc = 0, offline = 53;
	checkCPUAvailability(std::ref(proc), std::ref(system), std::ref(offline));
	std::cout << "system: " << system << std::endl << "proc: " << proc << std::endl << "offline: " << offline << std::endl;
	/*cpu_set_t* test;
	size_t size;
	test = CPU_ALLOC(static_cast<int>(sysconf(_SC_NPROCESSORS_CONF)));	
	size = CPU_ALLOC_SIZE(static_cast<int>(sysconf(_SC_NPROCESSORS_CONF)));
	std::cout << "memory allocated: " << size << std::endl << "size of test: " << sizeof(test) << std::endl;
	CPU_ZERO_S(size,test);	
	std::cout << "get affinity returned: " << sched_getaffinity(0, sizeof(test), test) << std::endl;
	std::cout << "number of cpus set: " << CPU_COUNT_S(size,test) << std::endl;
	std::cout << "test: " << test[0] << std::endl;
	CPU_FREE(test);
	*/
	return 0;
}
