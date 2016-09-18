#ifndef _FXQA_ARGVPARSER_H
#define _FXQA_ARGVPARSER_H

#include <iostream>

#include "fxqa_config.h"
#include "fxqa_xmlparser.h"

_START_FXQA_TEST_NAMESPACE_

class CARGParser {
	std::string	m_sProject;
	std::string	m_sTest;
	std::string	m_sTestcase;
	std::string	m_sTestfile;
	std::string m_sOldVersion;
	std::string m_sNewVersion;
	std::string m_sOldLibPath;
	std::string m_sNewLibPath;
	std::string m_sStatusMonitor;
	std::string m_sHearttime;
	std::string m_sTimeout;
	std::string m_sLogServerAddr;
	int			m_iLogServerPort;
public:
	CARGParser(int argc, char** argv): 
	  m_sProject(""),
	  m_sTest(""),
	  m_sTestcase(""),
	  m_sTestfile(""),
	  m_sNewVersion(""),
	  m_sOldVersion(""),
	  m_sStatusMonitor(""),
	  m_sHearttime(""),
	  m_sTimeout(""),
	  m_sLogServerAddr(""),
	  m_iLogServerPort(80) {
		_parse(argc, argv);
	};
	~CARGParser() {};

	void _parse(int argc, char** argv);

	std::string GetTestCase() {
		return m_sTestcase;
	};

	std::string GetTestFile() {
		return m_sTestfile;
	};

	std::string GetProject() {
		return m_sProject;
	};
	
	CXMLParser* Convert2XMLParser();
};

_END_FXQA_TEST_NAMESPACE_

#endif // _FXQA_ARGVPARSER_H