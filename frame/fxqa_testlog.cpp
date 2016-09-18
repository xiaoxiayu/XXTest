
#include "fxqa_testlog.h"
#include "fxqa_charset.h"
#include "fxqa_common.h"
#include "fxqa_statussender.h"

#include <algorithm>

_START_FXQA_TEST_NAMESPACE_

	void CLOGPlus::AdjustFileLenInfoStr(int fileLen, char* utf8_adjustBuf) {
#if defined(_FXQA_PLATFORM_QTANDROID_)
    QString qBuf;
    qBuf.sprintf("%x", fileLen);
    int len = qBuf.length();
    QString qAdjustBuf;
    if (len < 4) {
        int e = 4 - len;
        QString tems = "";
        for (int i = 0; i < e; i++) {
            tems += "0";
        }
        qAdjustBuf.append(tems);
        qAdjustBuf.append(qBuf);
    } else if (len == 4) {
        qAdjustBuf.append(qBuf);
    }
    std::wstring wtem = qAdjustBuf.toStdWString();
    //wcscpy(utf8_adjustBuf, wtem.c_str());
	strcpy(utf8_adjustBuf, ws2s(wtem).c_str());
#else
		char buf[5];
		memset(buf, 0, 5);
        //itoa(fileLen, buf, 16);
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		sprintf_s(buf, "%x", fileLen);
#else
        sprintf(buf, "%x", fileLen);
#endif
        int len = strlen(buf);
        std::wstring wStr = s2ws(buf);
        memset(utf8_adjustBuf, 0, 5);
        if (len < 4) {
            int e = 4 - len;
            std::wstring tems = L"";
            for (int i = 0; i < e; i++) {
                /*wcscpy(adjustBuf, L"0");
                adjustBuf++;*/
                tems += L"0";
            }
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
            wcscat_s(adjustBuf, 5, tems.c_str());
			wcscat_s(adjustBuf, 5, wStr.c_str());
#else
			tems += wStr;
			sUxU8(utf8_adjustBuf, (wchar_t*)tems.c_str(), 5);
#endif
        } else if (len == 4) {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
			wcscat_s(adjustBuf, 5, wStr.c_str());
#else
            //wcscat(adjustBuf, wStr.c_str());
			sUxU8(utf8_adjustBuf, (wchar_t*)wStr.c_str(), 5);
#endif
        }
#endif

}

std::string CLOGPlus::rptPath;
std::string testcasePath;
FILE* CLOGPlus::fp_runned_log = NULL;
std::string CLOGPlus::m_runedLogFileName = "";
int iRet;
bool CLOGPlus::InitLogRunnedFile(std::string testcaseName, std::string testName, int runnedfileFlag) {
	testcasePath = rptPath + "/" + testcaseName;
	iRet  = ACCESS(testcasePath.c_str(), 0);
	if (iRet != 0) {
		iRet = MKDIR(testcasePath.c_str());
		if (iRet != 0) {
            FXQA_printf("mkdir failed: %s\n", testcasePath.c_str());
		} 
	}

	m_runedLogFileName = testcasePath + "/" +  testName + "_Compared_PDFs.txt";
	std::string strCrashTemp;
    strCrashTemp = "===============================================================\r\n";
	switch (runnedfileFlag) {
	case QALOG_RUNNEDFILE_WRITE: {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fp_runned_log = NULL;
		fopen_s(&fp_runned_log, m_runedLogFileName.c_str(), "wb");
#else
		fp_runned_log = fopen(m_runedLogFileName.c_str(), "wb");
#endif

#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
		fclose(fp_runned_log);
		fp_runned_log = NULL;
#endif
		return false;
								 }
	case QALOG_RUNNEDFILE_APPEND:{
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fp_runned_log = NULL;
		fopen_s(&fp_runned_log, m_runedLogFileName.c_str(), "ab");
#else
		fp_runned_log = fopen(m_runedLogFileName.c_str(), "ab");
#endif

        fwrite(strCrashTemp.c_str(), sizeof(char), strCrashTemp.length(), fp_runned_log);
        fflush(fp_runned_log);

#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
		fclose(fp_runned_log);
		fp_runned_log = NULL;
#endif
		return true;
								 }
	case QALOG_RUNNEDFILE_NOLOG:
	default:
		break;
	}
#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
	fclose(fp_runned_log);
	fp_runned_log = NULL;
#endif
	return false;
}

CXMLWriter* CLOGPlus::m_pXmlLogWriter = NULL;
std::string CLOGPlus::m_rptPath = "";
bool CLOGPlus::InitReportFile(std::string reportPath, std::string rptPath) {
#if defined(_FXQA_MICROSERVICE_)
	return true;
#endif // _FXQA_MICROSERVICE_

	std::string absPath = reportPath + "/" + rptPath;
	m_pXmlLogWriter = new CXMLWriter(absPath);
	m_rptPath = reportPath;
	return true;
}

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
int CLOGPlus::gettimeofday(struct timeval *tv, void* tz) {
#define EPOCHFILETIME (116444736000000000ULL)
	FILETIME ft;
	LARGE_INTEGER li;
	unsigned __int64 tt;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	tv->tv_sec = tt / 1000000;
	tv->tv_usec = tt % 1000000;

	return 0;
}
#endif

double CLOGPlus::GetCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double time_now = static_cast<double>(tv.tv_sec) * 1000000 + tv.tv_usec;

	return time_now * 0.000001;
}

bool CLOGPlus::Report(std::string key, std::wstring wReportMsg){
	m_pXmlLogWriter->Create(key.c_str(), wReportMsg);
	return true;
}

bool CLOGPlus::Report(std::string key, std::string reportMsg){
	m_pXmlLogWriter->Create(key.c_str(), reportMsg);
	return true;
}

void CLOGPlus::LogRunnedFile(std::wstring filePath) {
	if (!fp_runned_log) {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fopen_s(&fp_runned_log, m_runedLogFileName.c_str(), "ab");
		if (!fp_runned_log) return;
#else
		return;
#endif
	}

	std::wstring log_info = filePath + L"\r\n";

	char* utf8_log_info = NULL;
    int utf8_len = sUxU8(utf8_log_info, (wchar_t*)log_info.c_str(), log_info.length());
    utf8_log_info = new char[utf8_len];
    memset(utf8_log_info, 0, utf8_len);
    sUxU8(utf8_log_info, (wchar_t*)log_info.c_str(), log_info.length());

    fwrite(utf8_log_info, sizeof(char), utf8_len, fp_runned_log);
    fflush(fp_runned_log);

    delete[]  utf8_log_info;
}

void CLOGPlus::CloseLogRunnedFile() {
#if defined(_FXQA_MICROSERVICE_)
	return;
#endif // _FXQA_MICROSERVICE_
	if (fp_runned_log) {
		fclose(fp_runned_log);
		fp_runned_log = NULL;
	}
}

void CLOGPlus::CloseReportFile() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif 

	if (m_pXmlLogWriter) {
		delete m_pXmlLogWriter;
	}
}


void CLOGPlus::InfoMapSprintf(std::map<std::string, std::string>& infoMap, std::string format, ...) {
	size_t len = std::count(format.begin(), format.end(), '%');

	if ((len < 1) || (len % 2 != 0)) {
		return;
	}

	va_list argp;

	char* s_para = NULL;
	int	i_para = -1;
	double	f_para = -1;
	std::string para = "";

	va_start(argp, format);

	//para = va_arg(argp, char*);

	std::string key = "";
	std::string val = "";

	for(int argno = 0; argno < len; ++argno) {
		int pos = format.find("%");
		std::string typeStr = format.substr(pos + 1, 1);
		format = format.substr(pos + 1, format.length());
		const char* type = typeStr.c_str();
		switch(type[0]) {
		case 'd':
			i_para = va_arg(argp, int);
			para = IntToString(i_para).c_str();
			break;
		case 'f':
			f_para = va_arg(argp, double);
			para = IntToString(f_para).c_str();
			break;
		case 's':
			s_para = va_arg(argp, char*);
			para = s_para;
			break;
		}
		if (argno % 2 == 0) {
			key = para;
		} else {
			val = para;
			infoMap.insert(std::make_pair(key, val));
		}
		para = "";
	}
	va_end( argp );
}

bool CLOGPlus::CreateTestProject(std::string testPrjName) {
#if defined(_FXQA_MICROSERVICE_)
	return true;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return false;

	m_pXmlLogWriter->CreateTestProject(testPrjName.c_str());
	return true;
}

bool CLOGPlus::CreateTestCase(std::string testCaseName) {
	if (m_pXmlLogWriter == NULL) return false;

	m_pXmlLogWriter->CreateTestCase(testCaseName.c_str());
	return true;
}

bool CLOGPlus::CreateTest(std::string testName) {
	if (m_pXmlLogWriter == NULL) return false;

	m_pXmlLogWriter->CreateTest(testName.c_str());
	return true;
}

void CLOGPlus::Bug(const char* bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(bugValue);
}

void CLOGPlus::Bug(const char* typeValue, std::string filePath) {
#if defined(_FXQA_MICROSERVICE_)
	fxqa_status::CLogSender::Log2ELK("bug", filePath);
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, filePath);
}

void CLOGPlus::Bug(const char* typeValue, std::wstring filePath) {
#if defined(_FXQA_MICROSERVICE_)
	std::string utf8Str;
	WstringToUTF8(filePath, utf8Str);

	fxqa_status::CLogSender::Log2ELK("bug", utf8Str);
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, filePath);
}

void CLOGPlus::Bug(const char* typeValue, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	fxqa_status::CLogSender::Log2ELK("bug", infoStr);
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, infoMap, "All_TestFiles");
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << infoStr << "," << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(bugValue, utf8Str);

	std::stringstream ss;
	ss << infoStr << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex){
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, pageIndex);
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::string bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << "page=" << pageIndex << "," << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, pageIndex);
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::wstring bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string utf8Str;
	WstringToUTF8(bugValue, utf8Str);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, pageIndex, bugValue);
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	fxqa_status::CLogSender::Log2ELK("bug", infoStr);
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, pageIndex, infoMap, "All_TestFiles");
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << infoStr << "," << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, pageIndex, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(bugValue, utf8Str);

	std::stringstream ss;
	ss << infoStr << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateBug(typeValue, pageIndex, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Info(const char* bugValue, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	fxqa_status::CLogSender::Log2ELK("info", infoStr);
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(bugValue, infoMap, "All_TestFiles");
	infoMap.clear();
}

void CLOGPlus::Info(const char* infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << infoValue; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(infoValue);
}

void CLOGPlus::Info(const char* typeValue, std::string filePath) {
#if defined(_FXQA_MICROSERVICE_)
	fxqa_status::CLogSender::Log2ELK("info", filePath);
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, filePath);
}

void CLOGPlus::Info(const char* typeValue, std::wstring filePath) {
#if defined(_FXQA_MICROSERVICE_)
	std::string utf8Str;
	WstringToUTF8(filePath, utf8Str);

	fxqa_status::CLogSender::Log2ELK("info", utf8Str);
	return;
#endif
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, filePath);
}

void CLOGPlus::Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string testfile) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << infoStr << "," << testfile; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, infoMap, testfile);
	infoMap.clear();
}

void CLOGPlus::Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring testfile) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::string utf8str = "";
	WstringToUTF8(testfile, utf8str);

	std::stringstream ss;
	ss << infoStr << "," << utf8str; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, infoMap, testfile);
	infoMap.clear();
}

void CLOGPlus::Info(const char* infoValue, int pageIndex) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(infoValue, pageIndex);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::string infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoValue; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, pageIndex, infoValue);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::wstring infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string utf8Str;
	WstringToUTF8(infoValue, utf8Str);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, pageIndex, infoValue);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoStr; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, pageIndex, infoMap, "All_TestFiles");
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoStr << "," << infoValue; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, pageIndex, infoMap, infoValue);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(infoValue, utf8Str);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoStr << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo(typeValue, pageIndex, infoMap, infoValue);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, std::string testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::stringstream ss;
	ss << infoStr << "," << testFile; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pXmlLogWriter == NULL) return;

	m_pXmlLogWriter->CreateInfo("Time_All", attriMap, testFile);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, std::wstring testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(testFile, utf8Str);

	std::stringstream ss;
	ss << infoStr << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pXmlLogWriter == NULL) return;
	m_pXmlLogWriter->CreateInfo("Time_All", attriMap, testFile);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::string testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoStr << "," << testFile; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pXmlLogWriter == NULL) return;
	m_pXmlLogWriter->CreateInfo("Time_All", pageIndex, attriMap, testFile);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::wstring testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(testFile, utf8Str);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoStr << "," << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pXmlLogWriter == NULL) return;
	m_pXmlLogWriter->CreateInfo("Time_All", pageIndex, attriMap, testFile);
}

void CLOGPlus::TimeAll(std::string testName, std::string timeInfo, int writeFlag) {
	std::string rptTimePath = m_rptPath + "/" + testName + "_time.txt";
	FILE* fp = NULL;
	if (writeFlag == QALOG_RUNNEDFILE_WRITE) {
		fp = fopen(rptTimePath.c_str(), "wb");
	} else {
		fp = fopen(rptTimePath.c_str(), "ab+");
	}
	fwrite(timeInfo.c_str(), sizeof(char), timeInfo.length(), fp);
	fflush(fp);
	fclose(fp);
}

void CLOGPlus::PrintOutputFile(std::wstring filePath) {
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	char* utf8_log_info = NULL;
	int utf8_len = sUxU8(utf8_log_info, (wchar_t*)filePath.c_str(), filePath.length());
	utf8_log_info = new char[utf8_len+1];
	memset(utf8_log_info, 0, utf8_len+1);
	sUxU8(utf8_log_info, (wchar_t*)filePath.c_str(), filePath.length());

	std::cout << "*FXQA_OUTPUT*:" << utf8_log_info << std::endl;

	delete[]  utf8_log_info;
#else
	std::string sFilePath = ws2s(filePath);
	std::cout << "*FXQA_OUTPUT*:" << sFilePath << std::endl;
#endif
}

const CPrinter& CPrinter::operator << (char *msg) const {
	FXQA_printf("%s", msg);
	return *this;
}

const CPrinter& CPrinter::operator << (std::string msg) const {
	FXQA_printf("%s", msg.c_str());
	return *this;
}

const CPrinter& CPrinter::operator << (double iMsg) const {
	std::string msg = IntToString(iMsg);
	FXQA_printf("%s", msg.c_str());
	return *this;
}


void CLOGPlus::RecoveryTime(std::string sPath, std::string testName, LOG_TIMEINFO &m_timeInfo) {
	std::string sTimePath = sPath + "/" + testName + "_TimeAll_time.txt";
	std::vector<std::string> lines;
	std::map<std::string, double> m;

	std::string str;
	std::ifstream file(sTimePath.c_str());
	if(file.is_open()){
		while(getline(file,str)){
			std::string sKey = str.substr(0, str.find("="));
			double nValue = std::atof((str.substr(str.find("=")+1, str.length())).c_str());
			m.insert(std::pair<std::string,double>(sKey, nValue));
		}
		m_timeInfo.dNewRenderTime += m["PDFRENDER_NEW"];
		m_timeInfo.dOldRenderTime += m["PDFRENDER_OLD"];
		m_timeInfo.dNewPDFParseTime += m["PDFPARSE_NEW"];
		m_timeInfo.dOldPDFParseTime += m["PDFPARSE_OLD"];
		m_timeInfo.dNewPageParseTime += m["PAGEPARSE_NEW"];
		m_timeInfo.dOldPageParseTime += m["PAGEPARSE_OLD"];
		m_timeInfo.dNewDeleteDocTime += m["CLOSEPDF_NEW"];
		m_timeInfo.dOldDeleteDocTime += m["CLOSEPDF_OLD"];
		m_timeInfo.PdfLoaderSaverNew+=m["PDFLoaderSaverNew"];
		m_timeInfo.PdfLoaderSaverOld+=m["PDFLoaderSaverOld"];
	}

}

void CLOGPlus::InitTotalTime(std::string sPath, std::string testName, std::map<std::string,double> &timeInfo) {
	std::string sTimePath = sPath + "/" + testName + "_TimeAll_time.txt";
	std::vector<std::string> lines;
	std::map<std::string, double> m;

	std::string str;
	std::ifstream file(sTimePath.c_str());
	if(file.is_open()){
		while(getline(file,str)){
			std::string sKey = str.substr(0, str.find("="));
			double nValue = std::atof((str.substr(str.find("=")+1, str.length())).c_str());
			m.insert(std::pair<std::string,double>(sKey, nValue));
		}
		std::map<std::string,double>::iterator iEnd,iteratorTime = timeInfo.begin();
		iEnd = timeInfo.end();
		double a=0;
		for(;iteratorTime!= iEnd; iteratorTime++)
		{
			a=m[iteratorTime->first];
			iteratorTime->second = a;
		//	timeInfo.at(iteratorTime->first) =m[iteratorTime->first];
		}
	}

}

void CLOGPlus::InfoMapToString(std::map<std::string, std::string> infoMap, std::string& infoStr) {
	std::map<std::string, std::string>::iterator infoItor;
	for (infoItor = infoMap.begin(); infoItor != infoMap.end(); infoItor++) {
		infoStr += infoItor->first;
		infoStr += "=";
		infoStr += infoItor->second;
		infoStr += " ";
	}
}

//std::string CLOGPlus::POSTLog(std::string url, std::string msg) {
//	//char *pHttpPost = "POST %s HTTP/1.1\r\n"  
//	//	"Host: %s:%d\r\n"  
//	//	"Content-Type: application/x-www-form-urlencoded\r\n"  
//	//	"Content-Length: %d\r\n\r\n"  
//	//	"%s";  
//
//	//char* host = "0.0.0.0";  
//	//int port = 9090;  
//
//	////char strHttpPost[1024] = {0};
//	//char* strHttpPost = new char[256 + msg.length()];
//	//memset(strHttpPost, 0, sizeof(strHttpPost));
//	//sprintf(strHttpPost, pHttpPost, url.c_str(), host, port, msg.length(), msg.c_str());  
//	//::read();
//	//return ::send(m_socketInfo.socket, strHttpPost, strlen(strHttpPost), 0);
//	return "";
//}
_END_FXQA_TEST_NAMESPACE_
