#include <workload.hpp>
#include <system.hpp>

int main(){
/*
	std::cout << "sched_get_prio_min fifo: " << sched_get_priority_min(SCHED_FIFO) << std::endl;
	std::cout << "sched_get_prio_max fifo: " << sched_get_priority_max(SCHED_FIFO) << std::endl;
	std::cout << "sched_get_prio_min rr: " << sched_get_priority_min(SCHED_RR) << std::endl;
	std::cout << "sched_get_prio_max rr: " << sched_get_priority_max(SCHED_RR) << std::endl;
	std::cout << "sched_get_prio_min other: " << sched_get_priority_min(SCHED_OTHER) << std::endl;
	std::cout << "sched_get_prio_max other: " << sched_get_priority_max(SCHED_OTHER) << std::endl;
	std::cout << "sched_get_prio_min idle: " << sched_get_priority_min(SCHED_IDLE) << std::endl;
	std::cout << "sched_get_prio_max idle: " << sched_get_priority_max(SCHED_IDLE) << std::endl;
	std::cout << "sched_get_prio_min batch: " << sched_get_priority_min(SCHED_BATCH) << std::endl;
	std::cout << "sched_get_prio_max batch: " << sched_get_priority_max(SCHED_BATCH) << std::endl;

	std::cout << std::endl << std::endl;



	// Policies
	std::cout << "sched_getscheduler " << sched_getscheduler(0) << std::endl;
	std::cout << "Errno: " << errno << std::endl;
	std::cout << "FIFO: " << SCHED_FIFO << std::endl;
	std::cout << "RR: " << SCHED_RR << std::endl;
	std::cout << "OTHER: " << SCHED_OTHER << std::endl;
	std::cout << "IDLE: " << SCHED_IDLE << std::endl;
	std::cout << "BATCH: " << SCHED_BATCH << std::endl;
	
	// With sched_setscheduler you can change the policy of that thread (and its priortiy if the
	// given policy is a realtime policy like RR or FIFO)
*/
	errno = 0;
	struct sched_attr prio;
	std::cout << "get_attr " << syscall(SYS_sched_getattr, 0, &prio, sizeof(prio), 0) << std::endl;
	std::cout << "Errno " << errno << std::endl << std::endl;
	std::cout << "policy of attr " << prio.sched_policy << "\nprio of attr " << prio.sched_priority << "\nnice of attr " << prio.sched_nice << std::endl;	
	prio.sched_policy = SCHED_RR;
	std::cout << "policy after set " << prio.sched_policy << std::endl;
	std::cout << "set_attr " << syscall(SYS_sched_setattr, 0, &prio, 0) << std::endl;
	std::cout << "Errno " << errno << std::endl;


	struct rlimit oldcap, newcap;
	std::cout << "getrlimit: " << getrlimit(RLIMIT_NICE, &oldcap) << std::endl;
	std::cout << "rlimit_nice soft: " << oldcap.rlim_cur << std::endl << "rlimit_nice hard: " << oldcap.rlim_max << std::endl;	
	
	newcap.rlim_cur = 2;
	newcap.rlim_max = 10;
	int curr_proc = getpid();
	
	std::cout <<"prlimit: " << prlimit(curr_proc, RLIMIT_NICE, &newcap, &oldcap) << std::endl;
	



	std::cout << "Limit prio: " << RLIMIT_RTPRIO << std::endl;
	std::cout << "Errno: " << errno << std::endl;
	std::cout << "Limit nice: " << RLIMIT_NICE << std::endl;
	std::cout << "Prio of proc: " << getpriority(PRIO_PROCESS, curr_proc) << std::endl;
	std::cout << "Errno: " << errno << std::endl;	
	std::cout << "setprio returned: " << setpriority(PRIO_PROCESS, curr_proc, 21) << std::endl;
	std::cout << "Prio of proc: " << getpriority(PRIO_PROCESS, curr_proc) << std::endl;
	std::cout << "Errno: " << errno << std::endl;
	std::cout << "setprio returned: " << setpriority(PRIO_PROCESS, curr_proc, 15) << std::endl;
	std::cout << "Prio of proc: " << getpriority(PRIO_PROCESS, curr_proc) << std::endl;
	
	std::cout << "Errno: " << errno << std::endl;
	errno = 0;


	//std::cout << "pthread_getsched " << pthread_getschedparam(




/*	
	workload::Workload worky(10, NULL, true);
	try
	{
		worky.startWL();
		calculateAndShowLoad(10);	
		worky.stopWL();
		worky.finishWorkload();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		worky.stopWL();
		worky.finishWorkload();
	}
	*/
	return 0;
}
