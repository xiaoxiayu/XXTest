
#include "fxqa_xmlwriter.h"
#include "fxqa_charset.h"
#include <time.h>

#if defined(_FXQA_PLATFORM_QTIOS_)
#include <QApplication>
#include <QStandardPaths>
#endif

#ifdef _FXQA_PLATFORM_WINDOWS_RT_
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
#endif

_START_FXQA_TEST_NAMESPACE_

CXMLWriter::CXMLWriter() {
}

CXMLWriter::CXMLWriter(std::string xmlFile) : m_fp(NULL) {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
	fopen_s(&m_fp, xmlFile.c_str(), "wb");
#else
	m_fp = fopen(xmlFile.c_str(), "wb");
#endif
	if (m_fp == NULL) {
		std::cout << "ReportPath ERROR." << std::endl; 
		exit(-1);
	}
	char UTF8BOM[3]={'\xEF','\xBB','\xBF'};
	fwrite(UTF8BOM, sizeof(char), 3, m_fp);
	fprintf(m_fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	time_t t = time(0); 
	char time_str[64]; 
	strftime( time_str, sizeof(time_str), "%Y/%m/%d %X",localtime(&t) ); 
	fprintf(m_fp, "<testsuites start=\"%s\">\n", time_str);
}

void CXMLWriter::InfoMapToString(std::map<std::string, std::string> infoMap, std::string& infoStr) {
	std::map<std::string, std::string>::iterator infoItor;
	for (infoItor = infoMap.begin(); infoItor != infoMap.end(); infoItor++) {
		infoStr += infoItor->first;
		infoStr += "=";
		infoStr += infoItor->second;
		infoStr += " ";
	}
}

void CXMLWriter::WStringToUtf8(std::wstring wStr, char*& utf8Str) {
	int utf8_len = sUxU8(utf8Str, (wchar_t*)wStr.c_str(), wStr.length());
	if (utf8_len <= 0) return;

	utf8Str = new char[utf8_len+1];
	memset(utf8Str, 0, utf8_len+1);
	sUxU8(utf8Str, (wchar_t*)wStr.c_str(), wStr.length());
}

CXMLWriter::~CXMLWriter() {
	if (m_fp) {
		fprintf(m_fp, "</testsuites>\n");
		fclose(m_fp);
	}
}

void CXMLWriter::Flush() {
	fflush(m_fp);
}

void CXMLWriter::EndElementAll() {
	std::string whiteSpace = "";
	std::list<std::string>::reverse_iterator  itor = m_eleList.rbegin();
	for (; itor != m_eleList.rend(); ++itor) {
		if ((*itor).compare("Test") == 0) {
			whiteSpace = "\t\t\t\t";
		} else if ((*itor).compare("Testcase") == 0) {
			whiteSpace = "\t\t\t";
		} else if ((*itor).compare("Testproject") == 0) {
			whiteSpace = "\t\t";
		} else {
			whiteSpace = "";
		}

		fprintf(m_fp, "%s</%s>\n", whiteSpace.c_str(), (*itor).c_str());
	}
	m_eleList.clear();
}

void CXMLWriter::Create(const char* key, std::string value) {
	fprintf(m_fp, "<%s>%s</%s>\n", key, value.c_str(), key);
}

void CXMLWriter::Create(const char* key, std::wstring value) {
	char* utf8Value = NULL;
	WStringToUtf8(value, utf8Value);

	fprintf(m_fp, "<%s>%s</%s>\n", key, utf8Value, key);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::EndElement() {
	std::list<std::string>::reverse_iterator  itor = m_eleList.rbegin();
	std::string ele = *itor;
	std::string whiteSpace = "";

	if (ele.compare("Test") == 0) {
		whiteSpace = "\t\t\t\t";
	} else if (ele.compare("TestCase") == 0) {
		whiteSpace = "\t\t\t";
	} else if (ele.compare("TestProject") == 0) {
		whiteSpace = "\t\t";
	} else {
		whiteSpace = "";
	}

	fprintf(m_fp, "%s</%s>\n", whiteSpace.c_str(), ele.c_str());

	m_eleList.pop_back();
}

void CXMLWriter::CreateTestSuites() {
	fprintf(m_fp, "<TestSuites>\n");
}

void CXMLWriter::CreateTestProject(const char* prjName) {
	fprintf(m_fp, "\t<TestProject name=\"%s\">\n", (char*)prjName);
	m_eleList.push_back("TestProject");
}

void CXMLWriter::CreateTestCase(const char* testcaseName) {
	fprintf(m_fp, "\t\t<TestCase name=\"%s\">\n", (char*)testcaseName);
	m_eleList.push_back("TestCase");
}

void CXMLWriter::CreateTest(const char* testName) {
	fprintf(m_fp, "\t\t\t<Test name=\"%s\">\n", (char*)testName);
	m_eleList.push_back("Test");
}

void CXMLWriter::CreateCrash(int errCode) {
	fprintf(m_fp, "<Crash errcode=\"%d\"/>\n", errCode);
}

/////

void CXMLWriter::CreateBug(const char* bugValue) {
	fprintf(m_fp, "\t\t\t\t<Bug type=\"common\" page=\"None\" detail=\"None\">%s</Bug>\n", bugValue);
}


void CXMLWriter::CreateBug(std::string bugValue) {
	fprintf(m_fp, "\t\t\t\t<Bug type=\"common\" page=\"None\" detail=\"None\">%s</Bug>\n", bugValue.c_str());
}

void CXMLWriter::CreateBug(std::string typeValue, std::string bugValue) {
	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"None\" detail=\"None\">%s</Bug>\n", typeValue.c_str(), bugValue.c_str());
}

void CXMLWriter::CreateBug(std::string typeValue, std::wstring bugValue) {
	char* utf8Value = NULL;
	WStringToUtf8(bugValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"None\" detail=\"None\">%s</Bug>\n", typeValue.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateBug(std::map<std::string, std::string> infoMap, std::string bugValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);
	fprintf(m_fp, "\t\t\t\t<Bug type=\"common\" page=\"None\" detail=\"%s\">%s</Bug>\n", infoStr.c_str(), bugValue.c_str());
}

void CXMLWriter::CreateBug(std::map<std::string, std::string> infoMap, std::wstring bugValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	char* utf8Value = NULL;
	WStringToUtf8(bugValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Bug type=\"common\" page=\"None\" detail=\"%s\">%s</Bug>\n", infoStr.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateBug(const char* typeValue, std::map<std::string, std::string>  infoMap, std::string bugValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);
	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"None\" detail=\"%s\">%s</Bug>\n", typeValue, infoStr.c_str(), bugValue.c_str());
}

void CXMLWriter::CreateBug(const char* typeValue, std::map<std::string, std::string>  infoMap, std::wstring bugValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	char* utf8Value = NULL;
	WStringToUtf8(bugValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"None\" detail=\"%s\">%s</Bug>\n", typeValue, infoStr.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateBug(const char* bugValue, int pageIndex) {
	fprintf(m_fp, "\t\t\t\t<Bug type=\"common\" page=\"%d\" detail=\"None\">%s</Bug>\n", pageIndex, bugValue);
}

void CXMLWriter::CreateBug(std::string bugValue, int pageIndex) {
	fprintf(m_fp, "\t\t\t\t<Bug type=\"common\" page=\"%d\" detail=\"None\">%s</Bug>\n", pageIndex, bugValue.c_str());
}

void CXMLWriter::CreateBug(std::string typeValue, int pageIndex, std::string bugValue) {
	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"%d\" detail=\"None\">%s</Bug>\n", typeValue.c_str(), pageIndex, bugValue.c_str());
}

void CXMLWriter::CreateBug(std::string typeValue, int pageIndex, std::wstring bugValue) {
	char* utf8Value = NULL;
	WStringToUtf8(bugValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"%d\" detail=\"None\">%s</Bug>\n", typeValue.c_str(), pageIndex, utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateBug(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::string bugValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);
	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"%d\" detail=\"%s\">%s</Bug>\n", typeValue, pageIndex, infoStr.c_str(), bugValue.c_str());
}

void CXMLWriter::CreateBug(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::wstring bugValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	char* utf8Value = NULL;
	WStringToUtf8(bugValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Bug type=\"%s\" page=\"%d\" detail=\"%s\">%s</Bug>\n", typeValue, pageIndex, infoStr.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

/////////

void CXMLWriter::CreateInfo(std::string infoValue) {
	fprintf(m_fp, "\t\t\t\t<Info type=\"common\" page=\"None\" detail=\"None\">%s</Info>\n", infoValue.c_str());
}

void CXMLWriter::CreateInfo(const char* infoValue) {
	fprintf(m_fp, "\t\t\t\t<Info type=\"common\" page=\"None\" detail=\"None\">%s</Info>\n", infoValue);
}

void CXMLWriter::CreateInfo(std::string typeValue, std::string infoValue) {
	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"None\" detail=\"None\">%s</Info>\n", typeValue.c_str(), infoValue.c_str());
}

void CXMLWriter::CreateInfo(std::string typeValue, std::wstring infoValue) {
	char* utf8Value = NULL;
	WStringToUtf8(infoValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"None\" detail=\"None\">%s</Info>\n", typeValue.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateInfo(std::map<std::string, std::string> infoMap, std::string infoValue) {
	std::map<std::string, std::string>::iterator infoItor;
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);
	fprintf(m_fp, "\t\t\t\t<Info type=\"Common\" page=\"None\" detail=\"%s\">%s</Info>\n", infoStr.c_str(), infoValue.c_str());
}

void CXMLWriter::CreateInfo(std::map<std::string, std::string> infoMap, std::wstring infoValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	char* utf8Value = NULL;
	WStringToUtf8(infoValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Info type=\"Common\" page=\"None\" detail=\"%s\">%s</Info>\n", infoStr.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateInfo(const char* typeValue, std::map<std::string, std::string> infoMap, std::string infoValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"None\" detail=\"%s\">%s</Info>\n", typeValue, infoStr.c_str(), infoValue.c_str());
}

void CXMLWriter::CreateInfo(const char* typeValue, std::map<std::string, std::string> infoMap, std::wstring infoValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	char* utf8Value = NULL;
	WStringToUtf8(infoValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"None\" detail=\"%s\">%s</Info>\n", typeValue, infoStr.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateInfo(const char* infoValue, int pageIndex) {
	fprintf(m_fp, "\t\t\t\t<Info type=\"common\" page=\"%d\" detail=\"None\" >%s</Info>\n", pageIndex, infoValue);
}

void CXMLWriter::CreateInfo(std::string infoValue, int pageIndex) {
	fprintf(m_fp, "\t\t\t\t<Info type=\"common\" page=\"%d\" detail=\"None\" >%s</Info>\n", pageIndex, infoValue.c_str());
}

void CXMLWriter::CreateInfo(std::string typeValue, int pageIndex, std::string infoValue) {
	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"%d\" detail=\"None\">%s</Info>\n", typeValue.c_str(), pageIndex, infoValue.c_str());
}

void CXMLWriter::CreateInfo(std::string typeValue, int pageIndex, std::wstring infoValue) {
	char* utf8Value = NULL;
	WStringToUtf8(infoValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"%d\" detail=\"None\">%s</Info>\n", typeValue.c_str(), pageIndex, utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}

void CXMLWriter::CreateInfo(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::string infoValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"%d\" detail=\"%s\">%s</Info>\n", typeValue, pageIndex, infoStr.c_str(), infoValue.c_str());
}

void CXMLWriter::CreateInfo(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::wstring infoValue) {
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	char* utf8Value = NULL;
	WStringToUtf8(infoValue, utf8Value);

	fprintf(m_fp, "\t\t\t\t<Info type=\"%s\" page=\"%d\" detail=\"%s\">%s</Info>\n", typeValue, pageIndex, infoStr.c_str(), utf8Value);

	if (utf8Value!= NULL)
		delete[] utf8Value;
}


/////////////

void CXMLWriter::CreateTestInfo(const char* testName, const char* eleValue, std::map<std::string, std::string> infoMap) {
	std::string infoStr = "";
	std::map<std::string, std::string>::iterator infoItor;
	for (infoItor = infoMap.begin(); infoItor != infoMap.end(); infoItor++) {
		infoStr += infoItor->first;
		infoStr += "=\"";
		infoStr += infoItor->second;
		infoStr += "\" ";
	}
	fprintf(m_fp, "\t\t\t<TestInfo name=\"%s\" %s>%s</TestInfo>\n", testName, infoStr.c_str(), eleValue);
}

void CXMLWriter::CreateTestCaseInfo(const char* testCaseName, const char* eleValue, std::map<std::string, std::string> infoMap) {
	std::string infoStr = "";
	std::map<std::string, std::string>::iterator infoItor;
	for (infoItor = infoMap.begin(); infoItor != infoMap.end(); infoItor++) {
		infoStr += infoItor->first;
		infoStr += "=\"";
		infoStr += infoItor->second;
		infoStr += "\" ";
	}
	fprintf(m_fp, "\t\t<TestCaseInfo name=\"%s\" %s>%s</TestCaseInfo>\n", testCaseName, infoStr.c_str(), eleValue);
}

_END_FXQA_TEST_NAMESPACE_
