#ifndef _FXQA_XMLWRITER_H
#define _FXQA_XMLWRITER_H
#include "tinyxml.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>

#include "fxqa_config.h"

_START_FXQA_TEST_NAMESPACE_

class CXMLWriter {
	std::string m_xmlFilePath;
	FILE* m_fp;

	void InfoMapToString(std::map<std::string, std::string> infoMap, std::string& infoStr);
	void WStringToUtf8(std::wstring infoValue, char*& utf8Str);
public:
	CXMLWriter();
	CXMLWriter(std::string xmlFile);
	~CXMLWriter();

	void Flush();

	std::list<std::string> m_eleList;

	void EndElement();
	void EndElementAll();

	void Create(const char* key, std::string value);
	void Create(const char* key, std::wstring value);

	void CreateCrash(int errCode=0);
	void CreateTestSuites();
	void CreateTestProject(const char* prjName);
	void CreateTestCase(const char* testcaseName);
	void CreateTest(const char* testName);

	void CreateBug(const char* bugValue);
	void CreateBug(std::string bugValue);
	void CreateBug(std::string typeValue, std::string bugValue);
	void CreateBug(std::string typeValue, std::wstring bugValue);
	void CreateBug(std::map<std::string, std::string> infoMap, std::string bugValue);
	void CreateBug(std::map<std::string, std::string>  infoMap, std::wstring bugValue);
	void CreateBug(const char* typeValue, std::map<std::string, std::string> infoMap, std::string bugValue);
	void CreateBug(const char* typeValue, std::map<std::string, std::string> infoMap, std::wstring bugValue);

	void CreateBug(const char* bugValue, int pageIndex);
	void CreateBug(std::string bugValue, int pageIndex);
	void CreateBug(std::string typeValue, int pageIndex, std::string bugValue);
	void CreateBug(std::string typeValue, int pageIndex, std::wstring bugValue);
	void CreateBug(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::string bugValue);
	void CreateBug(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::wstring bugValue);
	
	void CreateInfo(const char* infoValue);
	void CreateInfo(std::string infoValue);
	void CreateInfo(std::string typeValue, std::string infoValue);
	void CreateInfo(std::string typeValue, std::wstring infoValue);
	void CreateInfo(std::map<std::string, std::string> infoMap, std::string infoValue);
	void CreateInfo(std::map<std::string, std::string> infoMap, std::wstring infoValue);
	void CreateInfo(const char* typeValue, std::map<std::string, std::string> infoMap, std::string infoValue);
	void CreateInfo(const char* typeValue, std::map<std::string, std::string> infoMap, std::wstring infoValue);

	void CreateInfo(const char* infoValue, int pageIndex);
	void CreateInfo(std::string infoValue, int pageIndex);
	void CreateInfo(std::string typeValue, int pageIndex, std::string infoValue);
	void CreateInfo(std::string typeValue, int pageIndex, std::wstring infoValue);
	void CreateInfo(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::string infoValue);
	void CreateInfo(const char* typeValue, int pageIndex, std::map<std::string, std::string> infoMap, std::wstring infoValue);
	
	void CreateTestInfo(const char* testName, const char* eleValue, std::map<std::string, std::string> infoMap);
	void CreateTestCaseInfo(const char* testCaseName, const char* eleValue, std::map<std::string, std::string> infoMap);
};

_END_FXQA_TEST_NAMESPACE_

#endif // _FXQA_XMLPARSER_H