#include "workload.hpp"

using namespace workload;

Workload::Workload(unsigned int workloadPercentage, unsigned int* WLPriority, bool async)
{
	this->runningSimulation = false;
	this->asyncWorkload     = async;
	std::cout << "-- We will create " << deviceThreads << " Threads" << std::endl;
	for (unsigned int i = 0; i < deviceThreads; i++)
	{
		threadList.emplace_back(&Workload::simulateWorkload, this, workloadPercentage, i, WLPriority);
	}
}

void Workload::startWL() 
{
	runningSimulation = true;
}
void Workload::stopWL() 
{
	runningSimulation = false;
}
bool Workload::getRunningSim() 
{
	return this->runningSimulation;
}

void Workload::simulateWorkload(unsigned int workload, unsigned int randomize, unsigned int* WLPriority)
{
	unsigned int a         = 0;
	long long    sleepTime = (long long)(500 - workload * 5);
	std::cout << "we are in the func" << std::endl;
	if (asyncWorkload)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(randomize * 50));
	}
	while (runningSimulation)
	{
		auto start = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		auto end = std::chrono::system_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() < 500)
		{
			a++;
			end = std::chrono::system_clock::now();
		}
	}
}

void Workload::finishWorkload()
{
	std::cout << "-- Terminating workload...\n";
	for (size_t i = 0; i < threadList.size(); i++)
	{
		threadList[i].join();
	}
	std::cout << "-- Workload threads finished\n";
}
