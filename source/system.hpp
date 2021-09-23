#include <fstream>
#include <stdint.h>
#include <sys/times.h>

std::string readCPUTime(int cpu)
{
	std::string text;
	std::ifstream inputStream("/proc/stat");
	for(size_t i = 0; i <= cpu; i++)
	{
		getline(inputStream, text);
	}
	//always returns the last line read
	return text;
}

std::vector<uint64_t> returnData(int cpu)
{
	// CPU 0 -> System
	std::size_t searchPos = 5, strEndPos = 0;
	// If the we want a certain cpu's data we need to set the search position to 4 because
	// of the file's layout
	if(cpu)
	{
		searchPos = 4;
	}
	std::string compound = readCPUTime(cpu);
	std::vector<uint64_t> data;
	while(strEndPos != std::string::npos)
	{
		strEndPos = compound.find(" ", searchPos + 1);
		data.push_back(stoull(std::string(compound, searchPos, strEndPos - searchPos), nullptr));
		searchPos = strEndPos + 1;
	}
	return data;
}

int getProcessTimes(uint64_t& kernel, uint64_t& user)
{
	struct tms time;
	std::cout << "clocks" << times(&time) << std::endl;
	user = static_cast<uint64_t>(time.tms_utime);
	kernel = static_cast<uint64_t>(time.tms_stime);
	return 0;
}