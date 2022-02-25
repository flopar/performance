class CPU
{
	private:
		proc_handle m_handle;
		int m_systemCPUs = 0;
		int m_offlineCPUs = 0;
		
		// think about this on linux
		cpu_set_t* CPUset;
		size_t setSize;

	public:
		CPU(proc_handle& handle);
		~CPU();

		
		int get_affinity(pid_t tid);
		int count_SystemCPUs();
		int count_ProcessCPUs();
		int count_OfflineCPUs();
		void setCPU(int cpu, bool value, pid_t tid);
};

