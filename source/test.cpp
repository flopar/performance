#include <workload.hpp>
#include <system.hpp>


int main(){
	increaseThreadPrio();
	increaseThreadPrio();
	increaseThreadPrio();

	/*HANDLE currProc = GetCurrentProcess();
	int retVal = checkCPUAvailability(std::ref(currProc));
	uint64_t mask = 255;
	retVal = setCPUAvailability(std::ref(currProc), mask);
	std::cout << "New mask has been set\n";
	retVal = checkCPUAvailability(std::ref(currProc));
	workload::Workload worky(100, NULL, true);
	try
	{
		worky.startWL();
		calculateAndShowLoad(60);	
		worky.stopWL();
		worky.finishWorkload();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		worky.stopWL();
		worky.finishWorkload();
	}*/
	return 0;
}
