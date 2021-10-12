#include "workload.hpp"

using namespace workload;

Workload::Workload(unsigned int workloadPercentage, unsigned int* WLPriority, bool async)
{
	if(workloadPercentage < 0 || workloadPercentage > 100)
	{
		throw std::invalid_argument("workload cannot be less than 0 or higher than 100");
	}
	runningSimulation.store(false);
	startSimulation.store(false);
	asyncWorkload = async;
	std::cout << "-- We will create " << deviceThreads << " Threads" << std::endl;
	for (unsigned int i = 0; i < deviceThreads; i++)
	{
		// check if we can spare the keyword this when making the simulateWorkload func
		// static
		threadList.emplace_back(&Workload::simulateWorkload, this, workloadPercentage, i, WLPriority);
	}
}

void Workload::startWL() 
{
	if(static_cast<unsigned int>(threadList.size()) == deviceThreads)
	{
		std::cout << "All threads have been created\n";
		startSimulation.store(true);
		runningSimulation.store(true);
	}
	else
	{
		throw std::runtime_error("Some threads could not be created");
	}
}
void Workload::stopWL() 
{
	std::cout << "Stopping simulation\n";
	runningSimulation.store(false);
}

void Workload::simulateWorkload(unsigned int workload, unsigned int randomize, unsigned int* WLPriority)
{
	// maybe rewrite this with condition variables -> std::condition_variable::wait_until()
	while(!startSimulation.load())
	{
		//Busy waiting for all threads to be created
	}
	unsigned int countingVar = 0;
	long long    sleepTime = (long long)(500 - workload * 5);
	std::cout << "we are in the func" << std::endl;
	while(runningSimulation.load())
	{
		auto start = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		auto end = std::chrono::system_clock::now();
		while (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() < 500)
		{
			countingVar++;
			end = std::chrono::system_clock::now();
		}
	}
}

void Workload::finishWorkload()
{
	std::cout << "-- Terminating workload...\n";
	for (size_t i = 0; i < threadList.size(); i++)
	{	
		// Check if thread is joinable. On the contrary std::invalid_argument will be thrown
		if(threadList[i].joinable())
		{
			threadList[i].join();
		}
		else
		{
			std::hash<std::thread::id> hasher;
			throw std::invalid_argument("Thread with ID: " + std::to_string(static_cast<uint64_t>(hasher(threadList[i].get_id()))) + " is not joinable\n");
		}
	}
	std::cout << "-- Workload threads finished\n";
}
