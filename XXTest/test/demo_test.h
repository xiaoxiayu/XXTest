#ifndef _DEMO_TEST_H_
#define _DEMO_TEST_H_

#include "demo.h"
#include "fxqa_testlog.h"
#include "fxqa_xmlparser.h"



_START_FXQA_TEST_NAMESPACE_
	_START_TEST_COMPARE_NAMESPACE_

class  CDemoTest: public CFXQACompare {
	CXMLParser* m_pXmlParser;
	std::vector<std::wstring> m_testfiles;
	std::vector<std::string> m_comparedFiles;


	LOG_TIMEINFO m_timeInfo;
	std::map<std::string, std::string> m_logInfoMap;

public:
	CDemoTest();
	~CDemoTest();

	void SetUp(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName);
	bool InitLibrary();
	void Run(std::string testName="");
	void TearDown();

	std::wstring m_wsFileRootPath;
	std::string m_sReportPath;

};

_END_TEST_COMPARE_NAMESPACE_
	_END_FXQA_TEST_NAMESPACE_
#endif
