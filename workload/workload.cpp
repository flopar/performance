#include "workload.hpp"
#include "system.hpp"

using namespace workload;

Workload::Workload(unsigned int workloadPercentage, unsigned int* WLPriority, bool async)
{
	if(workloadPercentage < 0 || workloadPercentage > 100)
	{
		throw std::invalid_argument("workload cannot be less than 0 or higher than 100");
	}
	m_runningSimulation.store(false);
	m_startSimulation.store(false);
	m_asyncWorkload = async;
	std::cout << "-- Checking system...\n";
	double currentWorkload = calculateSystemLoad(10000);
	if(currentWorkload != -1)
	{
		std::cout << "-- Current system workload: " << currentWorkload << std::endl;
	}
	m_workload.store(workloadPercentage);
	std::cout << "-- We will create " << m_deviceThreads << " Threads" << std::endl;
	for (unsigned int i = 0; i < m_deviceThreads; i++)
	{
		m_threadList.emplace_back(&Workload::simulateWorkload, this, i, WLPriority);
	}
}
void Workload::setWorkload(uint32_t workload)
{
	m_workload.store(workload);
}
void Workload::startWL() 
{
	if(static_cast<unsigned int>(m_threadList.size()) == m_deviceThreads)
	{
		std::cout << "All threads have been created\n";
		m_startSimulation.store(true);
		m_runningSimulation.store(true);
	}
	else
	{
		throw std::runtime_error("Some threads could not be created");
	}
}
void Workload::stopWL() 
{
	std::cout << "Stopping simulation\n";
	m_runningSimulation.store(false);
}

void Workload::simulateWorkload(unsigned int randomize, unsigned int* WLPriority)
{
	// maybe rewrite this with condition variables -> std::condition_variable::wait_until()
	if(*WLPriority == MAX_PRIO)
	{
		bool increase = true;
		try
		{
			while(increase)
			{
				increaseThreadPrio(0);
			}
		}
		catch(const std::exception& e)
		{
			std::cout << "Thread #" << randomize << " " << e.what() << std::endl;
			increase = false;
		}
	}
	std::cout << "increased prio";
	while(!m_startSimulation.load())
	{
		//Busy waiting for all threads to be created
	}
	unsigned int countingVar = 0;
	long long    sleepTime;	
	if(m_asyncWorkload)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(randomize*50));
	}
	std::cout << "starting sim";
	while(m_runningSimulation.load())
	{
		sleepTime = (long long)(500 - m_workload.load() * 5);
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
	for (size_t i = 0; i < m_threadList.size(); i++)
	{	
		// Check if thread is joinable. Otherwise std::invalid_argument will be thrown
		if(m_threadList[i].joinable())
		{
			m_threadList[i].join();
		}
		else
		{
			std::hash<std::thread::id> hasher;
			std::ostringstream ss;
			ss << static_cast<uint64_t>(hasher(m_threadList[i].get_id()));
			throw std::invalid_argument("Thread with ID: " + ss.str() + " is not joinable\n");
		}
	}
	std::cout << "-- Workload threads finished\n";
}
