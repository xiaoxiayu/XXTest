
#include "fxqa_testinfo.h"
#include "fxqa_testlog.h"

_START_FXQA_TEST_NAMESPACE_
	CFXQATestINFO::CFXQATestINFO() 
	: sOldSvn(""),
	sNewSvn(""),
	iRunedFileFlag(QALOG_RUNNEDFILE_WRITE),
	wsPathFilecompare(L""),
	wsPathReport(L""),
	wsPathFiles(L""),
	wsBasePathFiles(L""),
	iSaveImageLimitcounts(-1),
	iTestSaveImageLimitcounts(-1),
	sTestcaseName(""),
	sTestName("") ,
	wsFolderName(L""),
	wsBaseFolderName(L""),
	bOutputWithDirStructure(false),
	bSaveBmp(false){
}


CFXQATestINFO::CFXQATestINFO(const CXMLParser* pXmlParser, std::string testcaseName)
	: sOldSvn(""), 
	sNewSvn(""),
	iRunedFileFlag(QALOG_RUNNEDFILE_WRITE), 
	bIsTxtTestFiles(false),
	wsPathFilecompare(L""),
	wsPathReport(L""),
	wsPathFiles(L""),
	wsBasePathFiles(L""),
	iSaveImageLimitcounts(-1),
	iTestSaveImageLimitcounts(-1),
	sTestcaseName(testcaseName),
	wsFolderName(L""),
	wsBaseFolderName(L""),
	bOutputWithDirStructure(false),
	bSaveBmp(false),
	sTestName("") {
		CXMLParser* _pXmlParser = const_cast<CXMLParser*>(pXmlParser);
		std::map<std::string, std::string> cfg = _pXmlParser->GetConfig();
		std::map<std::string, std::string> m_SvnVersion = _pXmlParser->GetSVNVersion();
		std::string flg = cfg["testfile_log"];
		sOldSvn = m_SvnVersion.find("oldsvn")->second;
		sNewSvn = m_SvnVersion.find("newsvn")->second;
		std::string save_flg = cfg["output_with_dir_structrue"];
		std::string save_bmp_flg = cfg["save_bmp"];
		if(save_flg.compare("TRUE")==0)
			bOutputWithDirStructure = true;
		if(save_bmp_flg.compare("TRUE") == 0){
			bSaveBmp = true;
		}
		int runnedfile_flg = -1;
		if (flg.compare("nolog") == 0) {
			iRunedFileFlag = QALOG_RUNNEDFILE_NOLOG;
		} else if (flg.compare("append") == 0) {
			iRunedFileFlag = QALOG_RUNNEDFILE_APPEND;
		} else {
			iRunedFileFlag = QALOG_RUNNEDFILE_WRITE;
		}

		bIsTxtTestFiles = _pXmlParser->IsUseTxtTestFiles();

		std::map<std::string, std::wstring> cfg_path = _pXmlParser->GetWPath();
		iSaveImageLimitcounts = atoi(cfg["save_image_limitcounts"].c_str());
		iTestSaveImageLimitcounts = atoi(cfg["test_save_image_limitcounts"].c_str());
		std::string file_cmp = cfg["filecmp_after_generate_copypaste"];

		wsBasePathFiles = _pXmlParser->GetWPath_Base();
		wsPathFiles = _pXmlParser->GetWPath_Files();

		wsPathFilecompare = _pXmlParser->GetWPath_Compare();
		wsPathReport = _pXmlParser->GetWPath_Report();

		wsFolderName = wsPathFiles.substr( wsPathFiles.rfind(L"/")+1);
		wsBaseFolderName = wsBaseFolderName.substr(wsBaseFolderName.rfind(L"/")+1);
}

CFXQATestINFO::CFXQATestINFO(const CXMLParser* pXmlParser, std::string testcaseName, std::string testNsme) {

}

CFXQATestINFO::~CFXQATestINFO() {

}

_END_FXQA_TEST_NAMESPACE_
