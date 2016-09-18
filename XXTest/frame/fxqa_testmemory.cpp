
#include "fxqa_testmemory.h"


_START_FXQA_TEST_NAMESPACE_

	FXQA_MEMINFO CMemoryState::GetMemoryInfo() {
		FXQA_MEMINFO memInfo;
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(::GetCurrentProcess(),&pmc,sizeof(pmc));
		memInfo.PeakWorkingSetSize = pmc.PeakWorkingSetSize;
		memInfo.WorkingSetSize = pmc.WorkingSetSize;
		
#elif defined(_FXQA_PLATFORM_MAC_) || defined(_FXQA_PLATFORM_QTIOS_)
		struct rusage usage;
		if (0 == getrusage(RUSAGE_SELF, &usage)) {
			memInfo.WorkingSetSize = usage.ru_maxrss;
		} else {
			memInfo.WorkingSetSize = -1;
		}

#else
		using std::ios_base;
		using std::ifstream;
		using std::string;
		// 'file' stat seems to give the most reliable results
		//
		ifstream stat_stream("/proc/self/stat",ios_base::in);
		// dummy vars for leading entries in stat that we don't care about
		//
		string pid, comm, state, ppid, pgrp, session, tty_nr;
		string tpgid, flags, minflt, cminflt, majflt, cmajflt;
		string utime, stime, cutime, cstime, priority, nice;
		string O, itrealvalue, starttime;

		unsigned long vsize;
		long rss;
		stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
			>> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
			>> utime >> stime >> cutime >> cstime >> priority >> nice
			>> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
		stat_stream.close();
		long page_size_byte = sysconf(_SC_PAGE_SIZE); // in case x86-64 is configured to use 2MB pages
		memInfo.VmUseage     = vsize;
		memInfo.WorkingSetSize = rss * page_size_byte;
#endif
		return memInfo;
	}

_END_FXQA_TEST_NAMESPACE_