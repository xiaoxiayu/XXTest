#ifndef _FXQA_TESTLOG_H_
#define _FXQA_TESTLOG_H_

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <direct.h>
#include <io.h>
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#define WMKDIR(a) _wmkdir((a))
#else
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#include "fxqa_config.h"
#include "fxqa_testmemory.h"
#include "fxqa_xmlwriter.h"

#if defined(_FXQA_TOOL_GLOG_)
#   if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
#       define GLOG_NO_ABBREVIATED_SEVERITIES
#       include "windows/glog/logging.h"
#       include "windows/glog/raw_logging.h"
#   else
#       define GLOG_NO_ABBREVIATED_SEVERITIES
#       include "glog/logging_gunc.h"
#       include "glog/raw_logging.h"
#   endif
#endif // _FXQA_TOOL_GLOG_

#if defined(_FXQA_PLATFORM_QTANDROID_) || defined(_FXQA_PLATFORM_QTIOS_)
#   include <QDebug>
#   define FXQA_printf qDebug
#else
#   define FXQA_printf printf
#endif

#define QA_LOG fxqa_test::CPrinter
#define QA_LOG_path fxqa_test::CLOGPlus::rptPath

#define QALOG_RUNNEDFILE_WRITE			0
#define QALOG_RUNNEDFILE_APPEND			1
#define QALOG_RUNNEDFILE_NOLOG			2

#define QARUN(_interface) QA_DLOG(INFO)<<#_interface;_interface;

_START_FXQA_TEST_NAMESPACE_

class CPrinter {
public:
	CPrinter() {};
	~CPrinter() {};

	const CPrinter& operator << (char *msg) const;
	const CPrinter& operator << (std::string msg) const;
	const CPrinter& operator << (double iMsg) const;
};


typedef struct COMPARE_XML_LOG_INFO_TIME {
	double dNewPDFParseTime;
	double dOldPDFParseTime;

	double dNewPageParseTime;
	double dOldPageParseTime;

	double dNewRenderTime;
	double dOldRenderTime;

	double dNewDeleteDocTime;
	double dOldDeleteDocTime;
	double ParsePdfContent_new;
	double ParsePdfContent_old;
	double PdfLoaderSaverNew;
	double PdfLoaderSaverOld;

	COMPARE_XML_LOG_INFO_TIME() {
		dNewPDFParseTime = 0;
		dOldPDFParseTime = 0;
		dNewPageParseTime = 0;
		dOldPageParseTime = 0;
		dNewRenderTime = 0;
		dOldRenderTime = 0;
		dNewDeleteDocTime = 0;
		dOldDeleteDocTime = 0;
		ParsePdfContent_new = 0;
		ParsePdfContent_old = 0;
		PdfLoaderSaverNew = 0;
		PdfLoaderSaverOld = 0;
	}
} LOG_TIMEINFO;
typedef struct COMPARE_XML_LOG_INFO_PAGE {
	int iOldWidth;
	int iOldHeight;
	int iNewWidth;
	int iNewHeight;

	COMPARE_XML_LOG_INFO_PAGE() {
		iOldWidth = 0;
		iOldHeight = 0;
		iNewWidth = 0;
		iNewHeight = 0;
	}

} LOG_PAGEINFO;

class CLOGPlus {
	static FILE* fp_runned_log;
	static std::string m_runedLogFileName;
	static CXMLWriter* m_pXmlLogWriter;
	static std::string m_rptPath;

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	struct timeval {
		long tv_sec, tv_usec;
	};
	static int gettimeofday(struct timeval *tv, void* tz);
#endif
	static void AdjustFileLenInfoStr(int fileLen, char* utf8_adjustBuf);
		
public:
	static std::string rptPath;

	CLOGPlus() {};
	~CLOGPlus() {};
	static bool InitLogRunnedFile(std::string testcaseName, std::string testName, int runnedfileFlag);
	static void LogRunnedFile(std::wstring filePath);
	static void CloseLogRunnedFile();

	static bool InitReportFile(std::string reportPath, std::string testsuites);
	static void CloseReportFile();
	static void Flush() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pXmlLogWriter) return;
		m_pXmlLogWriter->Flush();
	};

	static bool CreateTestProject(std::string testPrjName);
	static bool CreateTestCase(std::string testCaseName);
	static bool CreateTest(std::string testName);

	static void FinishTestProject() {
#if defined(_FXQA_MICROSERVICE_)
			return;
#endif // _FXQA_MICROSERVICE_
			if (!m_pXmlLogWriter) return;
			m_pXmlLogWriter->EndElement();
	};
	static void FinishTestCase() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pXmlLogWriter) return;
		m_pXmlLogWriter->EndElement();
	};

	static void FinishTest() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
	if (!m_pXmlLogWriter) return;
	m_pXmlLogWriter->EndElement();
	};

	static void FinishAll() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pXmlLogWriter) return;
		m_pXmlLogWriter->EndElementAll();
	};

	static void Crash(int errCode) {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pXmlLogWriter) return;
		m_pXmlLogWriter->CreateCrash(errCode);
	};

	static void Bug(const char* typeValue);
	static void Bug(const char* typeValue, std::string bugValue);
	static void Bug(const char* typeValue, std::wstring bugValue);
	static void Bug(const char* typeValue, std::map<std::string, std::string>& infoMap);
	static void Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string bugValue);
	static void Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring bugValue);
	static void Bug(const char* typeValue, int pageIndex);
	static void Bug(const char* typeValue, int pageIndex, std::string bugValue);
	static void Bug(const char* typeValue, int pageIndex, std::wstring bugValue);
	static void Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap);
	static void Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string bugValue);
	static void Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring bugValue);


	static void Info(const char* infoValue);
	static void Info(const char* typeValue, std::string infoValue);
	static void Info(const char* typeValue, std::wstring infoValue);
	static void Info(const char* typeValue, std::map<std::string, std::string>& infoMap);
	static void Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string infoValue);
	static void Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring infoValue);
	static void Info(const char* infoValue, int pageIndex);
	static void Info(const char* typeValue, int pageIndex, std::string infoValue);
	static void Info(const char* typeValue, int pageIndex, std::wstring infoValue);
	static void Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap);
	static void Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string infoValue);
	static void Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring infoValue);

	static void TimeAll(LOG_TIMEINFO timeInfo, std::string testFile);
	static void TimeAll(LOG_TIMEINFO timeInfo, std::wstring testFile);
	static void TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::string testFile);
	static void TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::wstring testFile);
	static void TimeAll(std::string testName, std::string timeInfo, int writeFlag=QALOG_RUNNEDFILE_WRITE);
	static void RecoveryTime(std::string sPath, std::string testName, LOG_TIMEINFO &m_timeInfo);
	static void InitTotalTime(std::string sPath, std::string testName, std::map<std::string,double> &timeInfo);

	static void PrintOutputFile(std::wstring filePath);

	static void CreateTestInfo(const char* testName, const char* testStatus, std::map<std::string, std::string> infoMap) {
		if (!m_pXmlLogWriter) return;
		m_pXmlLogWriter->CreateTestInfo(testName, testStatus, infoMap);
	};

	static void CreateTestCaseInfo(const char* testCaseName, const char* testStatus, std::map<std::string, std::string> infoMap) {
		if (!m_pXmlLogWriter) return;
		m_pXmlLogWriter->CreateTestCaseInfo(testCaseName, testStatus, infoMap);
	};

	static void InfoMapSprintf(std::map<std::string, std::string>& infoMap, std::string format, ...);

	char* GetSuccessLog(const char* info) const;

	static double GetCurrentTime();
	static FXQA_MEMINFO GetMemoryInfo() {return CMemoryState::GetMemoryInfo();};

	static bool Report(std::string key, std::wstring wReportMs);
	static bool Report(std::string key, std::string reportMsg);
	static bool Report(std::string key, std::map<std::string, std::string> infoMap, std::string reportMsg="");

	static bool ReportTimeAll(std::string key, std::map<std::string, std::string> infoMap, std::string reportMsg="");

	static void InfoMapToString(std::map<std::string, std::string> infoMap, std::string& infoStr);
	//static std::string POSTLog();
};

_END_FXQA_TEST_NAMESPACE_

#endif // _FXQA_TESTLOG_H_
