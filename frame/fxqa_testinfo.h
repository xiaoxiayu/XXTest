#ifndef _FXQA_TESTINFO_H_
#define _FXQA_TESTINFO_H_

#include <string>
#include "fxqa_xmlparser.h"
#include "fxqa_config.h"

_START_FXQA_TEST_NAMESPACE_
class CFXQATestINFO {
public:
	CFXQATestINFO();
	CFXQATestINFO(const CXMLParser* pXmlParser);
	CFXQATestINFO(const CXMLParser* pXmlParser, std::string testcaseName);
	CFXQATestINFO(const CXMLParser* pXmlParser, std::string testcaseName, std::string testNsme);
	~CFXQATestINFO();

public:
	std::string sTestcaseName;
	std::string sTestName;
	int			iRunedFileFlag;
	bool		bIsTxtTestFiles;

	std::string sOldSvn;
	std::string sNewSvn;

	std::wstring wsPathFiles;
	std::wstring wsBasePathFiles;
	std::wstring wsPathFilecompare;
	std::wstring wsPathReport;
	int iSaveImageLimitcounts;
	int iTestSaveImageLimitcounts;
	bool bOutputWithDirStructure;
	bool bSaveBmp;
	std::wstring	wsFolderName;
	std::wstring	wsBaseFolderName;
};
_END_FXQA_TEST_NAMESPACE_
#endif
