#ifndef _FXQA_COMMON_H_
#define _FXQA_COMMON_H_

#include "fxqa_config.h"

#include "stdio.h"

_START_FXQA_TEST_NAMESPACE_

class CFXQACommon {
	CFXQACommon() {};
	~CFXQACommon() {};
public:
	static FILE* qa_wfopen(const wchar_t* filePath, wchar_t* mode);
	static FILE* qa_fopen(const char* filePath, char* mode);
};

_END_FXQA_TEST_NAMESPACE_

#endif
