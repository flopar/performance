#include "util_linux.hpp"

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
	// If we want a certain cpu's data we need to set the search position to 4 because of
	// of the name "cpuN", where 4 is the position of the first space in the entry
	if(cpu > std::thread::hardware_concurrency() || cpu < 0)
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

