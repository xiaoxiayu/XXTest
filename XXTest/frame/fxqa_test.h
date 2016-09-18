#ifndef _FXQA_TEST_H_
#define _FXQA_TEST_H_

#include "fxqa_config.h"
#include "fxqa_xmlparser.h"
#include "fxqa_testfiles.h"
#include "fxqa_testinfo.h"

#if !defined(_FXQA_PLATFORM_WINDOWS_WIN32_) && !defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <dlfcn.h>
#endif

_START_FXQA_TEST_NAMESPACE_

class CFXQATest {
public:
	CFXQATest();
	virtual ~CFXQATest() = 0;

	// Before testcase.
	virtual void			TestCaseEnvironmentInit(const CXMLParser* pXmlParser, std::string testcaseName) = 0;
	// After testcase.
	virtual void			TestCaseEnvironmentClear() = 0;

	// Before test.
	virtual void			TestEnvironmentInit(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName) = 0;
	// After test.
	virtual void			TestEnvironmentClear() = 0;

	virtual void			SetUp(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName) = 0;
	virtual void			Run(std::string testName="") = 0;
	virtual void			TearDown() = 0;
};
//CFXQATest::~CFXQATest() {}

_END_FXQA_TEST_NAMESPACE_

#endif
