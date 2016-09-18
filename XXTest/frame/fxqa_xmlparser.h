#ifndef _FXQA_XMLPARSER_H
#define _FXQA_XMLPARSER_H
#include "tinyxml.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "fxqa_config.h"

_START_FXQA_TEST_NAMESPACE_

class CXMLParser {
	TiXmlDocument* m_xmlDocument;

	std::string m_txtPath;
	std::string m_sTestfile;
	std::wstring m_wsTestfile;
	int			m_iDeathTimeout;
	int			m_iHeartTime;
	std::string m_sLogServerAddr;
	int			m_iLogServerPort;

	std::string m_prj_name;
	std::vector<std::string> m_testcase;
	std::map< std::string, std::vector<std::string> > m_test;
	std::map< std::string, std::map<std::string, std::string> > m_testcase_config;
	std::map< std::string, std::map<std::string, std::string> > m_test_config;
	std::map<std::string, std::string> m_common_config;
	std::map<std::wstring, std::wstring> m_common_wconfig;

	std::map<std::string, std::wstring> m_wpath_win;
	std::map<std::string, std::wstring> m_wpath_linux;
	std::map<std::string, std::wstring> m_wpath_mac;
	std::map<std::string, std::wstring> m_wpath_ios;
	std::map<std::string, std::wstring> m_wpath_android;
	std::map<std::string, std::wstring> m_wpath_winrt;
	std::map<std::string, std::string> m_path_win;
	std::map<std::string, std::string> m_path_linux;
	std::map<std::string, std::string> m_path_mac;
	std::map<std::string, std::string> m_path_ios;
	std::map<std::string, std::string> m_path_android;
	std::map<std::string, std::string> m_path_winrt;
	std::map< std::string, std::map<std::string, std::wstring> > m_wpath;
	std::map< std::string, std::map<std::string, std::string> > m_path;

	std::map<std::string, std::string> m_common_svn;
	std::map< std::string, std::map<std::string, std::string> > m_common_lib_win_path;
	std::map< std::string, std::map<std::string, std::string> > m_common_lib_linux_path;
	std::map< std::string, std::map<std::string, std::string> > m_common_lib_mac_path;
	std::map< std::string, std::map<std::string, std::string> > m_common_lib_android_path;
	std::map< std::string, std::map<std::string, std::string> > m_common_lib_ios_path;
	std::map< std::string, std::map<std::string, std::string> > m_common_lib_winrt_path;

	std::map< std::string, std::vector<std::string> > m_monitor;

	std::vector<std::string> m_common_lib_win_run_path;
	std::vector<std::string> m_common_lib_linux_run_path;
	std::vector<std::string> m_common_lib_mac_run_path;
	std::vector<std::string> m_common_lib_android_run_path;
	std::vector<std::string> m_common_lib_ios_run_path;
	std::vector<std::string> m_common_lib_winrt_run_path;
	std::vector<std::string> m_common_lib_service_run_path;

	bool					Parse(std::string xmlPath);
	std::string				GetWorkPath();
	std::wstring			GetWWorkPath();
public:
	CXMLParser(std::string xmlPath);
	CXMLParser() {};
	~CXMLParser();

	bool							LoadXML(const char* xmlPath);
	bool							IsLoadXMLConfig();

	bool							IsUseTxtTestFiles();

	std::string						GetTestPrjName() {return m_prj_name;};

	std::vector<std::string>			GetTestCase();
	std::string							GetTestCaseConfig(std::string testCaseName, std::string cfgName);
	std::map<std::string, std::string>	GetTestCaseConfig(std::string testCaseName);

	std::vector<std::string>			GetTest(std::string testCaseName);
	std::map<std::string, std::string>	GetTestConfig(std::string testName);
	std::string							GetTestConfig(std::string testName, std::string cfgName);

	std::map<std::string, std::string>		GetSVNVersion(std::string testName="");
	std::map<std::string, std::string>		GetConfig();
	std::map<std::wstring, std::wstring>	GetWConfig();

	std::map< std::string, std::map<std::string, std::wstring> >	GetAllWPath();
	std::map<std::string, std::wstring>				GetWPath();
	
	std::string					GetPath_Files();
	std::string					GetPath_Report();
	std::string					GetPath_Compare();
	std::string					GetPath_Base();

	std::wstring				GetWPath_Files();
	std::wstring				GetWPath_Compare();
	std::wstring				GetWPath_Base();
	std::wstring				GetWPath_Report();

	std::vector<std::string>	GetMonitor(std::string monitorName);

	std::string					GetLibraryPath(std::string libType, std::string svnInfo);
	std::vector<std::string>	GetRunLibraryPath();
	std::map< std::string, std::map<std::string, std::string> > GetWindowsLibraryPath() {return m_common_lib_win_path;};
	std::map< std::string, std::map<std::string, std::string> > GetLinuxLibraryPath() {return m_common_lib_linux_path;};
	std::map< std::string, std::map<std::string, std::string> > GetMacLibraryPath() {return m_common_lib_mac_path;};
	std::map< std::string, std::map<std::string, std::string> > GetIOSLibraryPath() {return m_common_lib_ios_path;};
	std::map< std::string, std::map<std::string, std::string> > GetAndroidLibraryPath() {return m_common_lib_android_path;};

	//// Set Variable.
	void	SetProject(std::string name);
	void	AddTestcase(std::string testcase);
	void	AddTest(std::string testcase, std::string test);

	void	SetSvnVersion(std::string oldVersion, std::string newVersion);
	void	SetTestFile(std::string testfile);
	
	void	SetLibPath(std::string oldPath, std::string newPath);
	void	SetMonitor(std::string crash, std::string death);

	std::string GetTestFile();
	int			GetDeathTimeout();
	int			GetHeartTime();

	void		SetMonitorTime(int heart_time, int death_time);

	std::string	GetLogServerAddr();
	int			GetLogServerPort();
	void		SetLogServer(std::string logServer, int port);
};

_END_FXQA_TEST_NAMESPACE_

#endif // _FXQA_XMLPARSER_H