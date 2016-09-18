#include "demo_test.h"
#include "fxqa_testmanager.h"
#include <stdio.h>
using namespace std;

_START_FXQA_TEST_NAMESPACE_
	_START_TEST_COMPARE_NAMESPACE_

	FXQA_TEST_REGISTER(CFXQACompare, DemoTest, CDemoTest);


CDemoTest::CDemoTest() : m_pXmlParser(NULL) {
	
	//		m_bsave_image_with_dir_structure = false;
}

CDemoTest::~CDemoTest() {

}

void CDemoTest::SetUp(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName) {
	TouchMonitor();
}

void CDemoTest::TearDown() {
}

bool CDemoTest::InitLibrary() {	
	return true;
}

void CDemoTest::Run(std::string testName) 
{
	CLOGPlus::RecoveryTime(m_sReportPath, testName, m_timeInfo);
	std::vector<std::wstring>::iterator it_testfile = m_testfiles.begin();
	for (; it_testfile != m_testfiles.end(); ++it_testfile) {
	}
}

_END_TEST_COMPARE_NAMESPACE_
	_END_FXQA_TEST_NAMESPACE_