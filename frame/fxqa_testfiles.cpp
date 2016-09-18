
#include "fxqa_testfiles.h"
#include "fxqa_testlog.h"
#include "fxqa_charset.h"
#include <algorithm>

_START_FXQA_TEST_NAMESPACE_
	CTestFiles::CTestFiles() : bParseRunned(false) {

}

CTestFiles::~CTestFiles() {
	bIsParsed = false;
	m_testfiles.clear();
}

bool CTestFiles::IsRunnedFile(std::wstring fileName) {
	std::vector<std::wstring>::iterator it;
	it = std::find (m_runned_pdffiles.begin(), m_runned_pdffiles.end(), fileName);
	if (it != m_runned_pdffiles.end())
		return true;
	return false;
}

void CTestFiles::SplitString (std::wstring& inputwStr, wchar_t splitwChar, std::vector<std::wstring>& outwStrVec) {
	std::wstring::size_type loc_start = inputwStr.find_first_not_of(splitwChar);
	std::wstring::size_type loc_stop = loc_start;
	while(loc_stop != std::wstring::npos) {
		loc_stop = inputwStr.find_first_of(splitwChar, loc_start);
		if (loc_stop != std::wstring::npos)
			outwStrVec.push_back(inputwStr.substr(loc_start, loc_stop - loc_start));
		else
			outwStrVec.push_back(inputwStr.substr(loc_start));
		loc_start = inputwStr.find_first_not_of(splitwChar, loc_stop);
		loc_stop = loc_start;
	}
}

//void CTestFiles::SplitString (std::string& inputStr, char splitChar, std::vector<std::string>& outStrVec) {
//	std::wstring::size_type loc_start = inputStr.find_first_not_of(splitChar);
//	std::wstring::size_type loc_stop = loc_start;
//	while(loc_stop != std::wstring::npos) {
//		loc_stop = inputStr.find_first_of(splitChar, loc_start);
//		if (loc_stop != std::wstring::npos)
//			outStrVec.push_back(inputStr.substr(loc_start, loc_stop - loc_start));
//		else
//			outStrVec.push_back(inputStr.substr(loc_start));
//		loc_start = inputStr.find_first_not_of(splitChar, loc_stop);
//		loc_stop = loc_start;
//	}
//}

void CTestFiles::SetSuffixFilter(std::wstring wFileFilter) {
	SplitString(wFileFilter, '|', m_filters);
	for (std::vector<std::wstring>::iterator iter = m_filters.begin();
		iter != m_filters.end(); iter ++) {
			transform((*iter).begin(), (*iter).end(), (*iter).begin(), toupper);
	}
}

//void CTestFiles::SetSuffixFilter(std::string fileFilter) {
//	s2w
//	SplitString(fileFilter, '|', m_filters);
//	for (std::vector<std::wstring>::iterator iter = m_filters.begin();
//		iter != m_filters.end(); iter ++) {
//			transform((*iter).begin(), (*iter).end(), (*iter).begin(), toupper);
//	}
//}
//

void CTestFiles::ParseRunnedFiles(std::string testcaseName, std::string testName) {
	printf("Parsing compared files\n");
	std::string runnedLogFileName = CLOGPlus::rptPath + "/" + testcaseName + "/" + testName + "_Compared_PDFs.txt";
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
	FILE* fp = NULL;
	HRESULT hr = fopen_s(&fp, runnedLogFileName.c_str(), "rb");
#endif

    std::ifstream fin(runnedLogFileName.c_str());
    int cnt = fin.gcount();
    if ( (fin.rdstate() & std::ifstream::failbit ) != 0 ) {
        return;
    }
    std::string s;
    while (!fin.eof()) {
        getline(fin, s);
		
        if ((s.compare("") != 0)) {
			wchar_t* wStr = NULL;
			int strLen = sU8xU(wStr, (char*)s.c_str(), s.length());
			wStr = new wchar_t[strLen + 1];
			wmemset(wStr, 0, strLen + 1);
#if defined(_FXQA_PLATFORM_WINDOWS_RT_) || defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
			sU8xU(wStr, (char*)s.c_str(), s.length());
#else
			sU8xU(wStr, (char*)s.c_str(), s.length() - 1);
#endif
	
            m_runned_pdffiles.push_back(wStr);

			delete[] wStr;
        }
    }
}

bool CTestFiles::IsTestFile(std::wstring filepathname) {
	if (filepathname.rfind(L".") == -1)
		return false;
	std::wstring suffix_str = QA_GET_STRINGFILE_SUFFIX(filepathname);

	transform(suffix_str.begin(), suffix_str.end(), suffix_str.begin(), toupper);

	std::vector<std::wstring>::iterator itor = m_filters.begin();
	for (; itor != m_filters.end(); ++itor) {
		if (suffix_str.compare(*itor) == 0) {
			return true;
		}
	}
	return false;
}


std::vector<std::wstring> CTestFiles::m_testfiles; 
bool CTestFiles::bIsParsed = false;
void CTestFiles::ParseFolder(std::string testcaseName, std::string testName, std::wstring folderPath, bool testFileSkip) {
	if (bIsParsed) {
		return;
	}
	if (testFileSkip && !bParseRunned) {
		ParseRunnedFiles(testcaseName, testName);
		bParseRunned = true;
	}

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	_wfinddata64_t wFileInfo;
	intptr_t Handle = _wfindfirsti64((folderPath + L"/*").c_str(), &wFileInfo);

	if (-1 == Handle) {
		printf("can't find test files folder path\n");
		//LOG(INFO) << "can not match the folder path\n";
		return;
	}
	do {
		if (wFileInfo.attrib & _A_SUBDIR) {
			if( (wcscmp(wFileInfo.name, L".") != 0 ) &&(wcscmp(wFileInfo.name, L"..") != 0)) {
				std::wstring newPath = folderPath + L"/" + wFileInfo.name;
				ParseFolder(testcaseName, testName, newPath, testFileSkip);
			}
		}
		else {
			std::wstring filename(wFileInfo.name);
			if (IsTestFile(filename)) {
				std::wstring filepath = folderPath + L"/" + filename;
				if (testFileSkip) {
					if (!IsRunnedFile(filepath)){
						m_testfiles.push_back(filepath);
					}
				} else {
					m_testfiles.push_back(filepath);
				}
			}
		}
	}while (_wfindnext64(Handle, &wFileInfo) == 0);
	_findclose(Handle);
#elif defined(__GNUC__)
	DIR *d;

	struct dirent *file; 
	struct stat sb; 
	int ret = -1;
	std::string sFolderPath = ws2s(folderPath);

	if(!(d = opendir(sFolderPath.c_str()))) {
		printf("error opendir %s!!!\n",folderPath.c_str());
		return;
	}
	while((file = readdir(d)) != NULL) {
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;
		std::string filepath = file->d_name;
		std::string tempath0 = sFolderPath + "/";
		filepath = tempath0 + filepath;
		std::wstring wFilePath = s2ws(filepath);

		ret = stat(filepath.c_str(), &sb);
		if (ret < 0) {
			perror("get file stat error.");
		}

		if(S_ISDIR(sb.st_mode)) {

			ParseFolder(testcaseName, testName, wFilePath, testFileSkip);
		}

		if (IsTestFile(wFilePath)) {
			if (testFileSkip) {
				if (!IsRunnedFile(wFilePath))
					m_testfiles.push_back(wFilePath);
			} else {
				m_testfiles.push_back(wFilePath);
			}
		}
	}
	closedir(d);
#endif
}

void CTestFiles::ParseTxt(std::string testcaseName, std::string testName, std::wstring txtPath, bool testFileSkip) {
	if (bIsParsed) {
		return;
	}
	if (testFileSkip && !bParseRunned) {
		ParseRunnedFiles(testcaseName, testName);
		bParseRunned = true;
	}

	std::ifstream fin(ws2s(txtPath).c_str());
	int cnt = fin.gcount();
	if ( (fin.rdstate() & std::ifstream::failbit ) != 0 ) {
		return;
	}
	std::string s;
	while (!fin.eof()) {
		getline(fin, s);

		if ((s.compare("") != 0)) {
			wchar_t* wStr = NULL;
			int strLen = sU8xU(wStr, (char*)s.c_str(), s.length());
			wStr = new wchar_t[strLen + 1];
			wmemset(wStr, 0, strLen + 1);
			sU8xU(wStr, (char*)s.c_str(), s.length());

			if (IsTestFile(wStr)) {
				if (testFileSkip) {
					if (!IsRunnedFile(wStr)){
						m_testfiles.push_back(wStr);
					}
				} else {
					m_testfiles.push_back(wStr);
				}
			}

			delete[] wStr;
		}
	}
}

std::vector<std::wstring> CTestFiles::FilterFilesBySuffix(std::string testcaseName, std::string testName, 
	int runedFileFlag, std::wstring suffix_str) {
		if (m_testfiles.size() == 0 || suffix_str.length() <= 0) {
			return m_testfiles;
		}
		std::vector<std::wstring> suffixs;
		SplitString(suffix_str, '|', suffixs);

		for (std::vector<std::wstring>::iterator iter = suffixs.begin();
			iter != suffixs.end(); iter ++) {
				transform((*iter).begin(), (*iter).end(), (*iter).begin(), toupper);
		}

		if (runedFileFlag == QALOG_RUNNEDFILE_APPEND)
			ParseRunnedFiles(testcaseName, testName);

		std::vector<std::wstring> filterFiles;
		for (std::vector<std::wstring>::iterator filesIter = m_testfiles.begin();
			filesIter != m_testfiles.end(); filesIter ++) {
				if ((*filesIter).rfind(L".") == -1)
					continue;
				std::wstring file_suffix = QA_GET_STRINGFILE_SUFFIX((*filesIter));
				transform(file_suffix.begin(), file_suffix.end(), file_suffix.begin(), toupper);

				std::vector<std::wstring>::iterator suffix_itor = suffixs.begin();
				for (; suffix_itor != suffixs.end(); ++suffix_itor) {
					if (file_suffix.compare((*suffix_itor)) == 0) {
						if ((runedFileFlag == QALOG_RUNNEDFILE_APPEND)
							&& IsRunnedFile(*filesIter)) {
								break;
						} else {
							filterFiles.push_back((*filesIter));
						}
					}
				}
		}

		return filterFiles;
}

std::vector<std::wstring> CTestFiles::FilterFilesByStr(std::string testcaseName, std::string testName, 
	int runedFileFlag, std::wstring str) {
		if (m_testfiles.size() == 0 || str.length() <= 0) {
			return m_testfiles;
		}

		if (runedFileFlag == QALOG_RUNNEDFILE_APPEND)
			ParseRunnedFiles(testcaseName, testName);

		std::vector<std::wstring> filterFiles;
		for (std::vector<std::wstring>::iterator filesIter = m_testfiles.begin();
			filesIter != m_testfiles.end(); filesIter ++) {				
				if ((*filesIter).find(str) != -1) {
					if ((runedFileFlag == QALOG_RUNNEDFILE_APPEND) 
						&& !IsRunnedFile(*filesIter)) {
							filterFiles.push_back(*filesIter);
					}
				}
		}

		return filterFiles;
}

_END_FXQA_TEST_NAMESPACE_
