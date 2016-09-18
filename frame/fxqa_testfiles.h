#ifndef _FXQA_TESTFILES_H
#define _FXQA_TESTFILES_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "stdio.h"
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <windows.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#endif
#include "fxqa_config.h"

#define QA_GET_STRINGFILE_SUFFIX(path)	path.substr(path.rfind(L"."), path.length() - 1);

_START_FXQA_TEST_NAMESPACE_
class CTestFiles {
	static bool bIsParsed;
	static std::vector<std::wstring> m_testfiles;
	std::vector<std::wstring> m_runned_pdffiles;
	std::vector<std::wstring> m_filters;
	bool bParseRunned;

	bool IsRunnedFile(std::wstring fileName);
	void SplitString (std::wstring& inputwStr, wchar_t splitwChar, std::vector<std::wstring>& outwStrVec);
	bool IsTestFile(std::wstring suffix_str);
public:
	CTestFiles();
	~CTestFiles();

	void SetSuffixFilter(std::wstring wSuffixFilter);
	void SetSuffixFilter(std::string suffixFilter);

	void ParseRunnedFiles(std::string testcaseName, std::string testName);
	void ParseFolder(std::string testcaseName, std::string testName, std::wstring folderPath, bool testFileSkips);
	void ParseTxt(std::string testcaseName, std::string testName, std::wstring txtPath, bool testFileSkips);

	std::vector<std::wstring> GetFiles() {bIsParsed = true; return m_testfiles;};
	std::vector<std::wstring> GetRunnedFiles() {return m_runned_pdffiles;};
	std::wstring GetCurrentFile();

	std::vector<std::wstring> FilterFilesBySuffix(std::string testcaseName, std::string testName, int runedFileFlag, std::wstring suffix_str);
	std::vector<std::wstring> FilterFilesByStr(std::string testcaseName, std::string testName, int runedFileFlag, std::wstring str);
};

_END_FXQA_TEST_NAMESPACE_
#endif
