#include <workload.hpp>
#include <system.hpp>


int main(){
	
	workload::Workload worky(100,NULL,true);
	worky.startWL();
	calculateAndShowLoad(60);	
	worky.stopWL();
	worky.finishWorkload();
	return 0;
}
