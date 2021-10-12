#include <workload.hpp>
#include <system.hpp>


int main(){
	try
	{
		workload::Workload worky(10,NULL,true);
		worky.startWL();
		calculateAndShowLoad(5);	
		worky.stopWL();
		worky.finishWorkload();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
