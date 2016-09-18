#ifndef _DEMO_H_
#define _DEMO_H_

#define _END_TEST_COMPARE_NAMESPACE_	}
#define _START_TEST_COMPARE_NAMESPACE_	namespace test_compare { 

#define QATEST_COMPARE	fxqa_test::test_compare

#include "fxqa_config.h"
#include "fxqa_test.h"
#include "fxqa_testfiles.h"
#include "fxqa_testinfo.h"
#include "fxqa_statussender.h"

#if !defined(_FXQA_PLATFORM_WINDOWS_WIN32_) && !defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <dlfcn.h>
#endif


#define QAENVINFO_TESTCASE	GetTestCaseINFO()
#define QAENVINFO_TEST		GetTestINFO()

#ifdef _FXQA_PLATFORM_QTANDROID_
	#define QA_wcscmp wcscmp
#else
	#define QA_wcscmp FXSYS_wcscmp
#endif

_START_FXQA_TEST_NAMESPACE_
	_START_TEST_COMPARE_NAMESPACE_

class CFXQACompare : public CFXQATest {
	CTestFiles*				m_pTestfileMgr;
	int						m_runedFileFlag;
	CFXQATestINFO*			m_pTestCaseINFO;
	CFXQATestINFO*			m_pTestINFO;

	std::string				m_sTestcaseName;
	std::string				m_sTestName;

	std::wstring			m_AppendTextInfo;

	fxqa_status::CLogSender*	m_pLogSender;

public:
	CFXQACompare();
	virtual ~CFXQACompare();

	virtual void			TestCaseEnvironmentInit(const CXMLParser* pXmlParser, std::string testcaseName);
	// After testcase.
	virtual void			TestCaseEnvironmentClear();

	// Before test.
	virtual void			TestEnvironmentInit(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName);
	// After test.
	virtual void			TestEnvironmentClear();

	virtual void			SetUp(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName) = 0;
	virtual void			Run(std::string testName="") = 0;
	virtual void			TearDown() = 0;

	virtual void*			LoadDynamicLibrary(const char* libPath);
	virtual void*			GetAddressFromLib(const void* libHND, const char* funcName);
	virtual void			CloseDynamicLibrary(const void* libHND);

	virtual std::vector<std::wstring>	LoadTestFiles(std::string testcaseName, std::string testName, 
		std::wstring filePath, int runnedFileFlag, std::wstring fileFilter=L".PDF");
	virtual std::vector<std::wstring>	GetTestFilesBySuffix(std::wstring suffix_str);
	virtual std::vector<std::wstring>	GetTestFilesByStr(std::wstring str);
	virtual void						CloseTestFiles();

	virtual void						SetSenderInfo(const char* flag, const char* str="ERROR_MONITOR");
	virtual void						TouchMonitor();

	virtual void						InitLogMonitor(const CXMLParser*);
	virtual void						SendLog(const char* testName, std::wstring wStr);
	virtual void						SendLog(const char* testName, std::string sStr);
	virtual void						DestoryMonitor();

	CFXQATestINFO*				GetTestCaseINFO() {return m_pTestCaseINFO;};
	CFXQATestINFO*				GetTestINFO() {return m_pTestINFO;};

	std::string					GetTestcaseName(){return m_sTestcaseName;};
	std::string					GetTestName(){return m_sTestName;};

	void CreateDirectoryRecur(std::string csDir, std::string csRootDir);
	void CreateDirectoryRecur(std::wstring wsDir, std::wstring wsRootDir);

};
//CFXQATest::~CFXQATest() {}

_END_TEST_COMPARE_NAMESPACE_
	_END_FXQA_TEST_NAMESPACE_
#endif
