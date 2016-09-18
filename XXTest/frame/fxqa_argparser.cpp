

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "fxqa_argparser.h"

_START_FXQA_TEST_NAMESPACE_


void CARGParser::_parse(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		std::string cmdstr = argv[i];
		int pos = cmdstr.find("--testcase=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sTestcase = cmdstr.substr(pos + 11, cmdstr.length() - epos);

			//int tpos = full_test.find(".");
			//m_sTestcase = full_test.substr(0, tpos);
			//m_sTestcase = full_test.substr(tpos + 1, full_test.length());
		}

		pos = cmdstr.find("--test=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sTest = cmdstr.substr(pos + 7, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--testfile=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sTestfile = cmdstr.substr(pos + 11, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--project=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sProject = cmdstr.substr(pos + 10, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--oldversion=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sOldVersion = cmdstr.substr(pos + 13, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--newversion=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sNewVersion = cmdstr.substr(pos + 13, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--newlib=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sNewLibPath = cmdstr.substr(pos + 9, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--oldlib=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sOldLibPath = cmdstr.substr(pos + 9, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--statusmonitor=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sStatusMonitor = cmdstr.substr(pos + 16, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--timeout=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			m_sTimeout = cmdstr.substr(pos + 10, cmdstr.length() - epos);
		}

		pos = cmdstr.find("--logserver=");
		if (pos != -1) {
			int epos = cmdstr.find("=");
			std::string sLogServer = cmdstr.substr(pos + 12, cmdstr.length() - epos);
			int pos0 = sLogServer.find(":");
			if (pos0 == -1) {
				m_iLogServerPort = 80;
				m_sLogServerAddr = sLogServer;
			} else {
				m_sLogServerAddr = sLogServer.substr(0, pos0);
				m_iLogServerPort = atoi(sLogServer.substr(pos0+1, sLogServer.length() - pos0).c_str());
			}

		}
	}
}

CXMLParser* CARGParser::Convert2XMLParser() {
	CXMLParser* pParser = new CXMLParser;
	pParser->SetProject(this->m_sProject);
	pParser->AddTestcase(this->m_sTestcase);
	pParser->AddTest(this->m_sTestcase, this->m_sTest);

	pParser->SetSvnVersion(this->m_sOldVersion, this->m_sNewVersion);
	pParser->SetTestFile(this->m_sTestfile);
	pParser->SetLibPath(this->m_sOldLibPath, this->m_sNewLibPath);

	pParser->SetMonitor(this->m_sStatusMonitor, this->m_sStatusMonitor);

	int iTimeout = atoi(m_sTimeout.c_str());
	int iHeartTime = atoi(m_sHearttime.c_str());
	if (iHeartTime <= 0) {
		iHeartTime = 30;
	}
	pParser->SetMonitorTime(iHeartTime, iTimeout);

	pParser->SetLogServer(m_sLogServerAddr, m_iLogServerPort);
	return pParser;
}

_END_FXQA_TEST_NAMESPACE_