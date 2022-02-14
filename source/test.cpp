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
	size_t proc = 0, system = 0;
	checkCPUAvailability(std::ref(proc), std::ref(system));
	std::cout << "proc: " << proc << std::endl << "system: " << system << std::endl;
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
		/*
		// Create Workload, start it, analyze it, stop it and write the results to a file
		workload::Workload worky(percentage, &prio, async);
		worky.startWL();
		calculateAndShowLoad(60, std::ref(procWorkload), std::ref(sysWorkload));	
		worky.stopWL();
		worky.finishWorkload();
		
		writeRuntimeStats(procWorkload, "ProcessWorkload");
		writeRuntimeStats(sysWorkload, "SystemWorkload");
		*/
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


int main(int argc, char* argv[])
{
	//test(CRITICAL, 10, true);	
	test(NORMAL, 90, false);
	return 0;
}