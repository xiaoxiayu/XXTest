#include "demo.h"
#include "fxqa_testlog.h"
#include "fxqa_charset.h"
#include "fxqa_testmanager.h"
using namespace std;
_START_FXQA_TEST_NAMESPACE_
	_START_TEST_COMPARE_NAMESPACE_

	CFXQACompare::CFXQACompare() 
	: m_pTestfileMgr(NULL), 
	m_sTestcaseName(), 
	m_sTestName(), 
	m_runedFileFlag(QALOG_RUNNEDFILE_WRITE),
	m_pTestCaseINFO(NULL),
	m_pTestINFO(NULL),
	m_pLogSender(NULL) {

}

CFXQACompare::~CFXQACompare() {
	//printf("~CFXQATest\n");
	if (m_pTestfileMgr) {
		delete m_pTestfileMgr;
		m_pTestfileMgr = NULL;
	}
	m_sTestcaseName = "";
	m_sTestName = "";
}

void CFXQACompare::TestCaseEnvironmentInit(const CXMLParser* pXmlParser, std::string testcaseName) {
	m_pTestCaseINFO = new CFXQATestINFO(pXmlParser, testcaseName);
}

void CFXQACompare::TestCaseEnvironmentClear() {
	if (m_pTestCaseINFO){
		delete m_pTestCaseINFO;
		m_pTestCaseINFO = NULL;
	}
}

void CFXQACompare::TestEnvironmentInit(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName) {
}

void CFXQACompare::TestEnvironmentClear() {
}

// 	void CFXQATestCompare::SetUp(const CXMLParser* pXmlParser, std::string testcaseName, std::string testName){
// 		// Do something...
// 	};
// 
// 	void CFXQATestCompare::Run(std::string testName){
// 		// Do something...
// 	};
// 
// 	void CFXQATestCompare::TearDown(){
// 		// Do something...
// 	};



std::vector<std::wstring> CFXQACompare::LoadTestFiles(std::string testcaseName, std::string testName,
	std::wstring filePath, int runnedFileFlag, std::wstring fileFilter) {
#if defined(_FXQA_MICROSERVICE_)
		std::vector<std::wstring> testfiles;
		testfiles.push_back(filePath);
		return testfiles;
#else
		m_sTestcaseName = testcaseName;
		m_sTestName = testName;
		m_runedFileFlag = runnedFileFlag;
		bool isSkipRunnedFiles = CLOGPlus::InitLogRunnedFile(testcaseName, testName, runnedFileFlag);
		if (NULL == m_pTestfileMgr) {
			m_pTestfileMgr = new CTestFiles;
			m_pTestfileMgr->SetSuffixFilter(fileFilter);
			std::wstring pathSuffix = L"";
			int suffixPos = filePath.rfind(L".");
			if (suffixPos != -1) {
				pathSuffix = filePath.substr(suffixPos, filePath.length() - 1);
			}
			if (pathSuffix.compare(L".txt") != 0) {
				m_pTestfileMgr->ParseFolder(testcaseName, testName, filePath, isSkipRunnedFiles);
			} else {
				m_pTestfileMgr->ParseTxt(testcaseName, testName, filePath, isSkipRunnedFiles);
			}	
		} 
		return m_pTestfileMgr->GetFiles();
#endif // _FXQA_MICROSERVICE_
};


std::vector<std::wstring> CFXQACompare::GetTestFilesBySuffix(std::wstring suffix_str) {
	return m_pTestfileMgr->FilterFilesBySuffix(m_sTestcaseName, m_sTestName, m_runedFileFlag, suffix_str);
}

std::vector<std::wstring> CFXQACompare::GetTestFilesByStr(std::wstring str) {
	return m_pTestfileMgr->FilterFilesByStr(m_sTestcaseName, m_sTestName, m_runedFileFlag, str);
}

void CFXQACompare::CloseTestFiles() {
	CLOGPlus::CloseLogRunnedFile();
	if (m_pTestfileMgr) {
		delete m_pTestfileMgr;
		m_pTestfileMgr = NULL;
	}
}

void CFXQACompare::SetSenderInfo(const char* flag, const char* str) {
	fxqa_status::CStatusToucher::SetSenderInfo(flag, str);
}

void CFXQACompare::TouchMonitor() {
	fxqa_status::CStatusToucher::Touch();
}

void CFXQACompare::InitLogMonitor(const CXMLParser* xmlParse) {
	//fxqa_status::CLogSender::Init();
	fxqa_status::SOCKET_INFO sockInfo_log = {};
	if (const_cast<CXMLParser*>(xmlParse)->GetMonitor("log").size() == 2) {
		std::string monitor_ip = const_cast<CXMLParser*>(xmlParse)->GetMonitor("log")[0];
		std::string monitor_port = const_cast<CXMLParser*>(xmlParse)->GetMonitor("log")[1];
		sockInfo_log.ip = monitor_ip;
		sockInfo_log.port = atoi(monitor_port.c_str());
	}
	m_pLogSender = new fxqa_status::CLogSender(sockInfo_log);
	bool bSuccess = m_pLogSender->Init();
	if (!bSuccess) {
		delete m_pLogSender;
		m_pLogSender = NULL;
	}
}

void CFXQACompare::DestoryMonitor() {
	if (m_pLogSender) {
		delete m_pLogSender;
		m_pLogSender = NULL;
	}
}

void CFXQACompare::SendLog(const char* testName, std::wstring wStr) {
	if (!m_pLogSender) return;

	std::string sStr = "";
	WstringToUTF8(wStr, sStr);
	m_pLogSender->SentLog(testName, sStr.c_str());
}

void CFXQACompare::SendLog(const char* testName, std::string sStr) {
	if (!m_pLogSender) return;

	m_pLogSender->SentLog(testName, sStr.c_str());
}


void* CFXQACompare::LoadDynamicLibrary(const char* libPath) {
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	std::wstring libPath_w = s2ws(libPath);
	HMODULE libHND = LoadLibrary(libPath_w.c_str());
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	std::wstring libPath_w = s2ws(libPath);
	HMODULE libHND = LoadPackagedLibrary(libPath_w.c_str(), 0);
#else
	void * libHND = dlopen(libPath, RTLD_LAZY | RTLD_LOCAL);
	if (!libHND) {  
		fprintf (stderr, "%s\n", dlerror());  
		// exit(1);  
	}  
#endif
	return libHND;
};

void* CFXQACompare::GetAddressFromLib(const void* libHND, const char* funcName) {
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	void* funcAddress = GetProcAddress((HMODULE)libHND, funcName);
#else
	void* funcAddress = dlsym((void*)libHND, funcName);
#endif
	return funcAddress;
}

void CFXQACompare::CloseDynamicLibrary(const void* libHND) {
	if (libHND == NULL) return;
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	FreeLibrary((HMODULE)libHND);
#else
	dlclose((void*)libHND);
#endif
}

void CFXQACompare::CreateDirectoryRecur(std::string csDir, std::string csRootDir)
{
	int nStart = csRootDir.length()+1;
	int nFind = csDir.find('/', nStart);;
	while(nFind != -1)
	{
		string SubDir = csDir.substr(0,nFind);
		MKDIR(SubDir.c_str());
		nFind = csDir.find('/', nFind+1);
	}
}

void CFXQACompare::CreateDirectoryRecur(std::wstring wsDir, std::wstring wsRootDir)
{
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	if(ACCESS(ws2s(wsDir).c_str(), 0) == 0){
		return;
	}
#else
	DIR* dirptr = NULL;
	if(dirptr = opendir(ws2s(wsDir).c_str())){
		closedir(dirptr);
		return;
	}
#endif
	int nStart = wsRootDir.length()+1;
	int nFind = wsDir.find('/', nStart);
	while(nFind != -1)
	{
		wstring SubDir = wsDir.substr(0,nFind);
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
		if(ACCESS(ws2s(SubDir).c_str(), 0) != 0){
			WMKDIR(SubDir.c_str());
		}
#else
		if(dirptr = opendir(ws2s(SubDir).c_str())){
			closedir(dirptr);	
		}else{
			MKDIR(ws2s(SubDir).c_str());
		}
#endif
		nFind = wsDir.find('/', nFind+1);
	}
}

//#ifndef _FXQA_PDFIUM_



//#endif // ndef _FXQA_PDFIUM_

_END_TEST_COMPARE_NAMESPACE_
	_END_FXQA_TEST_NAMESPACE_

