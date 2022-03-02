CPU::CPU(proc_handle handle)
{
	this.m_handle = handle;

	// linux only
	systemCPUs = get_nprocs();
	offlineCPUs = systemCPUs - get_nprocs_conf();
	CPUset = CPU_ALLOC(get_nprocs());
	setSize = CPU_ALLOC_SIZE(get_nprocs());
	CPU_ZERO_S(setSize, CPUset);
	std::cout << "creating\n";
}

/*
 * Description: This function delivers the number of CPUs avaiable to the given pid.
 *		It is specified that the id passed on to "sched_getaffinity" changes
 *		the affinity of a spcific thread, hence the function does not differentiate
 *		between a TID and PID. If 0 is passed the calling thread will have its affinity
 *		changed. If a TID is passsed, that thread will have its affinity changed and (last
 *		but not least) if a PID is passed, that PID main thread will have its affinity
 *		changed.
 */

int CPU::getAffinity(pid_t tid)
{
	if(sched_getaffinity(tid, setSize, CPUset))
	{
		throw std::runtime_error("Failed getting the affinity mask for the given thread/process!\n");
		return 1;
	}
	return 0;
}

CPU::~CPU()
{
	CPU_FREE(CPUset);
	std::cout << "Deleting\n";
}

int CPU::count_SystemCPUs()
{
	return systemCPUs;
}

int CPU::count_OfflineCPUs()
{
	return offlineCPUs;
}

int CPU::count_ProcessCPUs(pid_t tid)
{
	return CPU_COUNT_S(setSize, CPUset);
}

void CPU::setCPU(int cpu, bool value, pid_t tid)
{
	// check input
	if((cpu < 0) || (cpu > systemCPUs))
	{
		throw std::runtime_error("Given CPU is ambigious!\n");
	}
	getAffinity(tid);
	int cpuSet = CPU_ISSET_S(cpu, setSize, CPUset);
	if(value)
	{
		if(!cpuSet)
		{
			CPU_SET_S(cpu, setSize, CPUset);
		}
	}
	else
	{
		if(cpuSet)
		{
			CPU_CLR_S(cpu, setSize, CPUset);
		}
	}
}


