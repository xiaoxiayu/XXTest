#ifndef _FXQA_TESTMEMORY_H_
#define _FXQA_TESTMEMORY_H_

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <Windows.h>
#include "Psapi.h"
#elif defined(_FXQA_PLATFORM_MAC_) || defined(_FXQA_PLATFORM_QTIOS_)
#include <sys/time.h>
#include <sys/resource.h>
#else
#include "linux_define.h"
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#endif

#include "fxqa_config.h"

#define MEM_700M (1024*1024*700)
#define MEM_500M (1024*1024*500)
#define MEM_50M (1024*1024*50)
#define MEM_1M (1024*1024)
#define MEM_256K (256*1024)
#define MEM_1B 1

_START_FXQA_TEST_NAMESPACE_

	typedef struct FXQA_MEMORY_INFO {
		size_t PeakWorkingSetSize;
		size_t WorkingSetSize;

		size_t VmUseage;
} FXQA_MEMINFO;

class CMemoryState {
public:
	CMemoryState() {};
	~CMemoryState() {};

	static FXQA_MEMINFO GetMemoryInfo();
};
_END_FXQA_TEST_NAMESPACE_
#endif // _FXQA_TESTMEMORY_H_