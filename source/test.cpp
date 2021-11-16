#include <workload.hpp>
#include <system.hpp>

#define NORMAL 0
#define CRITICAL 1
/* 
 * this function will write statistic to a csv File on if the type of "list" is an iterable object
 */
int writeRuntimeStats(auto list, std::string statName)
{
	std::ofstream statisticsFile;
	statisticsFile.open("runtime_statistics.csv", std::ios::in | std::ios::app | std::ios::binary);
	if(!statisticsFile.is_open())
	{
		throw std::runtime_error("Couldn't open file to write");
		return -1;
	}
	/* problems occured while using list.back() -> newline would come too often */
	statisticsFile << statName << ",";
	int lenList = list.size()-1;
	for(int i = 0; i < lenList; i++)
	{
		if(i < lenList-1)
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
	std::vector<double> procWorkload, sysWorkload, test1,test2;
	bool check_prio = true;
#ifdef __linux__
	bool check_nice = true;
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
			try
			{
				// Increase Thread Prio, if for some reason is not set to max
				while(check_prio)
				{
					std::cout << "increasing prio...\n";
					increaseThreadPrio();
				}
			}
			catch(const std::exception& e)
			{
				check_prio = false;
				std::cout << e.what() << std::endl;
			}
			try
			{
				// Increase process nice value
				while(check_nice)
				{
					std::cout << "increasing nice value...\n";
					increaseProcessNiceValue();
				}
			}
			catch(const std::exception& e)
			{
				check_nice = false;
				std::cout << e.what() << std::endl;
			}
#endif
		}
		// Create Workload, start it, analyze it, stop it and write the results to a file
		workload::Workload worky(percentage, NULL, async);
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
	test(CRITICAL, 10, true);	
	//test(NORMAL, 90, false);
	return 0;
}










	// With sched_setscheduler you can change the policy of that thread (and its priortiy if the
	// given policy is a realtime policy like RR or FIFO)
/*	errno = 0;
	struct sched_attr prio;
	std::cout << "get_attr " << syscall(SYS_sched_getattr, 0, &prio, sizeof(prio), 0) << std::endl;
	std::cout << "Errno " << errno << std::endl << std::endl;
	std::cout << "policy of attr " << prio.sched_policy << "\nprio of attr " << prio.sched_priority << "\nnice of attr " << prio.sched_nice << std::endl;	
	prio.sched_policy = SCHED_RR;
	std::cout << "policy after set " << prio.sched_policy << std::endl;
	std::cout << "set_attr " << syscall(SYS_sched_setattr, 0, &prio, 0) << std::endl;
	std::cout << "Errno " << errno << std::endl;
	newcap.rlim_cur = 2;
	newcap.rlim_max = 10;
	int curr_proc = getpid();
	std::cout <<"prlimit: " << prlimit(curr_proc, RLIMIT_NICE, &newcap, &oldcap) << std::endl;
	std::cout << "Limit prio: " << RLIMIT_RTPRIO << std::endl;
	std::cout << "Errno: " << errno << std::endl;
	std::cout << "Limit nice: " << RLIMIT_NICE << std::endl;
	std::cout << "Prio of proc: " << getpriority(PRIO_PROCESS, curr_proc) << std::endl;
	std::cout << "Errno: " << errno << std::endl;	
	std::cout << "setprio returned: " << setpriority(PRIO_PROCESS, curr_proc, 21) << std::endl;
	std::cout << "Prio of proc: " << getpriority(PRIO_PROCESS, curr_proc) << std::endl;
	std::cout << "Errno: " << errno << std::endl;
	std::cout << "setprio returned: " << setpriority(PRIO_PROCESS, curr_proc, 15) << std::endl;
	std::cout << "Prio of proc: " << getpriority(PRIO_PROCESS, curr_proc) << std::endl;
	*/
