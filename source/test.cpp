#include "workload.hpp"
#include "system.hpp"


int main(){
	std::vector<uint64_t> check = returnData(0);
	for(size_t i = 0; i < check.size(); i++)
	{
		std::cout << check[i] << std::endl;
	}
	uint64_t kernel, user;
	getProcessTimes(std::ref(kernel), std::ref(user));
	std::cout << "kernel: " << kernel << std::endl << "user: " << user << std::endl;
	/*workload::Workload worky(10,NULL,true);
	worky.startWL();
	std::this_thread::sleep_for(std::chrono::seconds(10));
	worky.stopWL();
	worky.finishWorkload();*/
	return 0;
}
