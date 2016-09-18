#ifndef _FXQA_DYLIBMANAGER_H_
#define _FXQA_DYLIBMANAGER_H_

#include <cstdio>
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>

namespace fxqa_lib {
	class CDylibManager {
		void* m_libHND;
		void* LoadDynamicLibrary(const char* libPath);
		void* GetAddressFromLib(const void* libHND, const char* funcName);
		void CloseDynamicLibrary(const void* libHND);
	public:
		CDylibManager(std::string dylibPath);
		~CDylibManager();

		void* GetAddress(const char* funcName);
	};
}
#endif
