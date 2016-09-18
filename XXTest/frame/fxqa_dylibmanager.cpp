
#include "fxqa_dylibmanager.h"
#include "fxqa_charset.h"

namespace fxqa_lib {
	CDylibManager::CDylibManager(std::string dylibPath) : m_libHND(NULL) {
		m_libHND = LoadDynamicLibrary(dylibPath.c_str());
	}

	CDylibManager::~CDylibManager() {
		CloseDynamicLibrary(m_libHND);
	}

	void* CDylibManager::GetAddress(const char* funcName) {
		if (NULL == m_libHND) return NULL;
		return GetAddressFromLib(m_libHND, funcName);
	}

	void* CDylibManager::LoadDynamicLibrary(const char* libPath) {
		if (m_libHND) return m_libHND;
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
		std::wstring libPath_w = s2ws(libPath);
		HMODULE libHND = LoadLibrary(libPath_w.c_str());
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
		HMODULE libHND = LoadPackagedLibrary(L"newapi.dll", 0);
#else
		void * libHND = dlopen(libPath, RTLD_LAZY | RTLD_LOCAL);
		if (!libHND) {  
            fprintf (stderr, "%s\n", dlerror());
			// exit(1);  
		}  
#endif
		return libHND;
	};

	void* CDylibManager::GetAddressFromLib(const void* libHND, const char* funcName) {
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
		void* funcAddress = GetProcAddress((HMODULE)libHND, funcName);
#else
		void* funcAddress = dlsym((void*)libHND, funcName);
#endif
		return funcAddress;
	}

	void CDylibManager::CloseDynamicLibrary(const void* libHND) {
		if (libHND == NULL) return;
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
		FreeLibrary((HMODULE)libHND);
#else
		dlclose((void*)libHND);
#endif
	}
}
