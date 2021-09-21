#include <iostream>
#include <thread>
#include "workload.hpp"


void print(){
	std::cout << "hi" << std::endl;
}

int main(){
	workload::Workload worky(10,NULL,true);
	worky.startWL();
	std::this_thread::sleep_for(std::chrono::seconds(100));
	worky.stopWL();
	worky.finishWorkload();
	return 0;
}
