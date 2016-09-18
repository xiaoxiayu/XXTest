#ifndef _FXQA_TESTMANAGER_H
#define _FXQA_TESTMANAGER_H
#include <string>
#include "fxqa_config.h"
#include "fxqa_xmlparser.h"
#include "fxqa_test.h"
#include "fxqa_statussender.h"
#include "fxqa_testlog.h"
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
#include <windows.h>
#include <DbgHelp.h>
#include <memory>
#endif

#if !defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
#if __cplusplus > 199711L
#			include <memory>	
#	else
#		include <tr1/memory>
#	endif
#endif

#define QA_TEST_filter(prj) fxqa_test::CFXQATestManager<prj>::m_test_filter
#define RUN_TEST(prj, xml)	fxqa_test::FXQA_RunTest<prj>(xml);


_START_FXQA_TEST_NAMESPACE_


 
template<typename testPrj>
class CFXQATestManager {
   typedef typename std::map<std::string, testPrj*>::iterator  TestPrjMapItorType;
public:
	CFXQATestManager(){};
	CFXQATestManager(const char* testName, testPrj* className){
		FXQA_RegeditTest(testName, className);
	};
	//CFXQATestManager(const char* testName, CFXQATest* className);
	
	~CFXQATestManager(){};

	bool IsTestClassExist(const char* testName){
		TestPrjMapItorType l_it;
		l_it = m_test_collect.find(testName);
		if(l_it == m_test_collect.end()) {
			return false;
		}
		return true;
	};

	void RunTest(fxqa_test::CXMLParser* xmlParse) {
		 		std::vector<std::string> testcase = xmlParse->GetTestCase();
		 		if (testcase.size() == 0) {
		 			return;
		 		}
		 		if (m_test_collect.size() == 0) {
		 			return;
		 		}
		 
		 		fxqa_status::SOCKET_INFO sockInfo_crash = {};
		 		if (xmlParse->GetMonitor("crash").size() == 2) {
		 			std::string  crash_monitor_ip = xmlParse->GetMonitor("crash")[0];
		 			std::string crash_monitor_port = xmlParse->GetMonitor("crash")[1];
		 			sockInfo_crash.ip = crash_monitor_ip;
		 			sockInfo_crash.port = atoi(crash_monitor_port.c_str());
		 		}
		 		fxqa_status::CStatusSender CrashMoniter(sockInfo_crash);
		 		CrashMoniter.CreateSenderThread();

#if !defined(_FXQA_MICROSERVICE_)
		 		fxqa_status::SOCKET_INFO sockInfo_death = {};
		 		if (xmlParse->GetMonitor("death").size() == 2) {
		 			std::string death_monitor_ip = xmlParse->GetMonitor("death")[0];
		 			std::string death_monitor_port = xmlParse->GetMonitor("death")[1];
		 			sockInfo_death.ip = death_monitor_ip;
		 			sockInfo_death.port = atoi(death_monitor_port.c_str());
		 		}
		 		fxqa_status::CStatusToucher DeathMoniter(sockInfo_death);
		 		bool bIsDeathMoniter = DeathMoniter.Init();
#endif // _FXQA_MICROSERVICE_

		 		for (std::vector<std::string>::iterator it_testcase = testcase.begin(); it_testcase != testcase.end(); ++it_testcase) {
		 			std::string testCaseName = *it_testcase;
		             std::string filter_testcase = m_test_filter.substr(0, m_test_filter.find('.'));
		             if (filter_testcase.compare("*") != 0 && testCaseName.compare(filter_testcase) != 0) {
#if !defined(_FXQA_MICROSERVICE_)
		                 CrashMoniter.SetSenderInfo((testCaseName + "." + "*").c_str(), "NO_RUN");
		                 if (bIsDeathMoniter) {
		 					fxqa_status::CStatusToucher::SetSenderInfo((testCaseName + "." + "*").c_str(), "NO_RUN");
		 					fxqa_status::CStatusToucher::Touch();
		 				}
		 
		                 fxqa_status::Sleep(1000); //Wait Sender send End Info.
#else
						 CrashMoniter.SendNoRun(testCaseName, "*", xmlParse->GetTestFile());
#endif // _FXQA_MICROSERVICE_
		                 continue;
		             }
		 			
		 			if (IsTestClassExist(testCaseName.c_str())) {
						m_test_collect[testCaseName.c_str()]->TestCaseEnvironmentInit(xmlParse, testCaseName);
		 				std::vector<std::string> testName_v = xmlParse->GetTest(testCaseName);
						
						CLOGPlus::CreateTestCase(testCaseName);

						int test_count = 0;
						double testcase_time_s = CLOGPlus::GetCurrentTime();
		 				for (std::vector<std::string>::iterator it_test = testName_v.begin(); it_test != testName_v.end(); ++it_test, ++test_count) {
		 					std::string testName = *it_test;
		                     std::string filter_test = m_test_filter.substr(m_test_filter.find('.') + 1);
		                     if (filter_test.compare("*") != 0 && testName.compare(filter_test) != 0) {
		                         continue;
		                     }
		 					if (CrashMoniter.IsRunning()) {	
#if !defined(_FXQA_MICROSERVICE_)	
		 						CrashMoniter.SetSenderInfo((testCaseName + "." + testName).c_str(), "ERROR_MONITOR_C");
								fxqa_status::Sleep(1000); //Wait Sender send End Info.
#else
								CrashMoniter.SendStartCrash(testCaseName, testName, xmlParse->GetTestFile(), xmlParse->GetHeartTime());
								CrashMoniter.StartSendInfo();

								int iTimeout = xmlParse->GetDeathTimeout();
								if (iTimeout > 0) {
									fxqa_status::CStatusToucher::SetSender(&CrashMoniter);
									CrashMoniter.SendStartDeath(testCaseName, testName, xmlParse->GetTestFile(), xmlParse->GetDeathTimeout());
								}
#endif // _FXQA_MICROSERVICE_			
		 					}
#if defined(_FXQA_MICROSERVICE_)
							fxqa_status::CLogSender::InitLinkELK(xmlParse->GetLogServerAddr(), xmlParse->GetLogServerPort());
							fxqa_status::CLogSender::SetTestInfo(xmlParse->GetSVNVersion(), testCaseName, testName, xmlParse->GetTestFile());
#endif  // _FXQA_MICROSERVICE_
							CLOGPlus::CreateTest(testName);

							double test_time_s = CLOGPlus::GetCurrentTime();

		 					m_test_collect[testCaseName.c_str()]->TestEnvironmentInit(xmlParse, testCaseName, testName);
		 					//init  while each test case run
		                    m_test_collect[testCaseName.c_str()]->SetUp(xmlParse, testCaseName, testName);
		 					
		 					m_test_collect[testCaseName.c_str()]->Run(testName);
		 					
		 					m_test_collect[testCaseName.c_str()]->TearDown();
		 
		 					m_test_collect[testCaseName.c_str()]->TestEnvironmentClear();
		 
							CLOGPlus::FinishTest();

							double test_time_e = CLOGPlus::GetCurrentTime();

							std::map<std::string, std::string> infoMap;
							CLOGPlus::InfoMapSprintf(infoMap, "%s%f", "time", (test_time_e - test_time_s));
							CLOGPlus::CreateTestInfo(testName.c_str(), "Success", infoMap);
#if defined(_FXQA_MICROSERVICE_)
							fxqa_status::CLogSender::DisconnectELK();
#endif  // _FXQA_MICROSERVICE_
		 					if (CrashMoniter.IsRunning()) {
#if !defined(_FXQA_MICROSERVICE_)	
		 						CrashMoniter.SetSenderInfo((testCaseName + "." + testName).c_str(), "TEST_END_C");
		 						fxqa_status::Sleep(1100); //Wait Sender send End Info.
		 						CrashMoniter.StopSendInfo();
#else
								CrashMoniter.SendStopCrash();
								int iTimeout = xmlParse->GetDeathTimeout();
								if (iTimeout > 0) {
									CrashMoniter.SendStopDeath();
								}
#endif
		 					}
	
#if !defined(_FXQA_MICROSERVICE_)	
		 					if (bIsDeathMoniter) {
		 						fxqa_status::Sleep(1000);
		 						fxqa_status::CStatusToucher::SetSenderInfo((testCaseName + "." + testName).c_str(), "TEST_END_D");
		 						fxqa_status::CStatusToucher::Touch();
		 						fxqa_status::Sleep(1100);
		 					}
#endif
		 				}
						
						CLOGPlus::FinishTestCase();
						double testcase_time_e = CLOGPlus::GetCurrentTime();
						std::map<std::string, std::string> infoMap;
						CLOGPlus::InfoMapSprintf(infoMap, "%s%d%s%f","test", test_count, "time", (testcase_time_e - testcase_time_s));
						CLOGPlus::CreateTestCaseInfo(testCaseName.c_str(), "Success", infoMap);
		 			} else {
#if !defined(_FXQA_MICROSERVICE_)	
		                 CrashMoniter.SetSenderInfo((testCaseName + "." + "*").c_str(), "NO_RUN");
		                 if (bIsDeathMoniter) {
		                     fxqa_status::Sleep(1000);
		                     fxqa_status::CStatusToucher::SetSenderInfo((testCaseName + "." + "*").c_str(), "NO_RUN");
		                     fxqa_status::CStatusToucher::Touch();
		                 }
		 
		                 fxqa_status::Sleep(1000); //Wait Sender send End Info.
#else
						 CrashMoniter.SendNoRun(testCaseName, "*", xmlParse->GetTestFile());
#endif
		                 continue;
		 			}
		 			m_test_collect[testCaseName.c_str()]->TestCaseEnvironmentClear();
		 		}
		 		
		 		if (CrashMoniter.IsRunning()) {
		 			CrashMoniter.StopSenderThread();
		 		}
		 		fxqa_status::Sleep(1000);
	};
	std::map<std::string, testPrj*> m_test_collect;

public:
    static std::string m_test_filter;
};
template<class testPrj>
std::string CFXQATestManager<testPrj>::m_test_filter = "";

template <class TestUnitClass>
class TestUnitFactoryImpl {
public:
#if (__cplusplus > 199711L) || defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	static std::shared_ptr<TestUnitFactoryImpl> m_dataMemMgr;
#else
	static std::tr1::shared_ptr<TestUnitFactoryImpl> m_dataMemMgr;
#endif
	TestUnitFactoryImpl() {/*printf("TestUnitFactoryImpl\n");*/ };
	~TestUnitFactoryImpl() {/* printf("~TestUnitFactoryImpl\n");*/};
	virtual CFXQATest* CreateTest() { return new TestUnitClass; }
};

template<class testPrj>
CFXQATestManager<testPrj>* _FXQA_GetTestMgr() {
	static CFXQATestManager<testPrj> testMgr;
	return &testMgr;
};

template<class testPrj>
void FXQA_RegeditTest(const char* testName, testPrj* testClass) {
	CFXQATestManager<testPrj>* testMgr = _FXQA_GetTestMgr<testPrj>();
	testMgr->m_test_collect.insert(std::pair<const char*, testPrj*>(testName, testClass));
}


#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
static void DumpMiniDump(std::wstring dumpLogName, PEXCEPTION_POINTERS excpInfo)
{
	HANDLE hFile = CreateFile(dumpLogName.c_str(), GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 
	if (hFile == NULL) return;

	if (excpInfo == NULL)
	{
		// Generate exception to get proper context in dump
		__try 
		{
			//OutputDebugString(_T("raising exception\r\n"));
			fprintf(stderr, "raising exception:\n");
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(dumpLogName, GetExceptionInformation()),
			EXCEPTION_CONTINUE_EXECUTION) 
		{
		}
	} 
	else
	{
		//OutputDebugString(_T("writing minidump\r\n"));
		//fprintf(stderr, "#############writing minidump: %s ###########\r\n", dumpLogName);
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpNormal,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
	CloseHandle( hFile );
}
#endif //_FXQA_PLATFORM_WINDOWS_WIN32_ dump

#if (__cplusplus > 199711L) || defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
#define FXQA_TEST_REGISTER(prjClass, testName, testClass) \
    TestUnitFactoryImpl<testClass>* _##testName = new TestUnitFactoryImpl<testClass>;\
    std::shared_ptr< TestUnitFactoryImpl<testClass> > _dataMemMgr_##testName(_##testName);\
	prjClass* _createTest_##testName = (prjClass*)_##testName->CreateTest();\
	std::shared_ptr< prjClass > _dataMemMgr2_##testName(_createTest_##testName);\
    CFXQATestManager<prjClass> _##testClass(#testName, _createTest_##testName);
#else
#define FXQA_TEST_REGISTER(prjClass, testName, testClass) \
    TestUnitFactoryImpl<testClass>* _##testName = new TestUnitFactoryImpl<testClass>;\
    std::tr1::shared_ptr< TestUnitFactoryImpl<testClass> > _dataMemMgr_##testName(_##testName);\
	prjClass* _createTest_##testName = (prjClass*)_##testName->CreateTest();\
	std::tr1::shared_ptr< prjClass > _dataMemMgr2_##testName(_createTest_##testName);\
	CFXQATestManager<prjClass> _##testClass(#testName, _createTest_##testName);
#endif

template<class testPrj>
void FXQA_RunTest(CXMLParser* xmlParse){
	int errCode = 0;
	std::wstring dumpPath = L"";
	std::string reportPath = xmlParse->GetPath_Report();
	std::string prjName = xmlParse->GetTestPrjName();
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
	__try {
		dumpPath = xmlParse->GetWPath_Report() + L"/" + A2U(prjName.c_str()) + L".dmp";
#else
	try {
#endif // _FXQA_PLATFORM_WINDOWS_WIN32_

		CLOGPlus::InitReportFile(reportPath, "TestReport_" + prjName + ".xml");
		CLOGPlus::CreateTestProject(prjName);
		_FXQA_GetTestMgr<testPrj>()->RunTest(xmlParse);
		CLOGPlus::FinishTestProject();
		CLOGPlus::CloseReportFile();

#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
	} __except( errCode=GetExceptionCode(), DumpMiniDump(dumpPath.c_str(), GetExceptionInformation() ), EXCEPTION_EXECUTE_HANDLER) {
		OutputDebugString(L"Crash ERROR!!!\n");
#else
	} catch (...) {
#endif // _FXQA_PLATFORM_WINDOWS_WIN32_
		//fprintf(stderr, "Crash ERROR: %d", errCode);
		std::string testcase = xmlParse->GetTestCase().back();
		std::string test = xmlParse->GetTest(testcase).back();
		std::cout << "{\"type\":\"crash\",\"project\":\"" 
			<< xmlParse->GetTestPrjName().c_str() 
			<< "\",\"testfile\":\"" << xmlParse->GetTestFile().c_str() 
			<< "\",\"testcase\":\"" << testcase 
			<< "\",\"test\":\"" << test << "\"}";
		CLOGPlus::Crash(errCode);
		CLOGPlus::FinishAll();
		CLOGPlus::Flush();
		CLOGPlus::CloseLogRunnedFile();
		CLOGPlus::CloseReportFile();
	}
};

_END_FXQA_TEST_NAMESPACE_
#endif // _FXQA_TESTMANAGER_H
