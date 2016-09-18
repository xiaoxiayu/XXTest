
#include "fxqa_xmlparser.h"
#include "fxqa_charset.h"

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
	CXMLParser::CXMLParser(std::string xmlPath) : 
		m_xmlDocument(NULL), 
		m_prj_name("testprj"), 
		m_txtPath("none"),
		m_iDeathTimeout(0),
		m_iHeartTime(0),
		m_sLogServerAddr(""),
		m_iLogServerPort(80){
		m_xmlDocument = new TiXmlDocument();
		if (Parse(xmlPath) == false) {
			delete m_xmlDocument;
			m_xmlDocument = NULL;
		}
}

CXMLParser::~CXMLParser() {
	if (m_xmlDocument) {
		delete m_xmlDocument;
	}
}

bool CXMLParser::LoadXML(const char* xmlPath) {
	if (m_xmlDocument != NULL) {
		return true;
	}
	m_xmlDocument = new TiXmlDocument();
	if (m_xmlDocument->LoadFile(xmlPath) == false) {
		delete m_xmlDocument;
		m_xmlDocument = NULL;
		return false;
	}
	return true;
}

bool CXMLParser::IsLoadXMLConfig() {
	if (m_xmlDocument == NULL) {
		return false;
	}
	return true;
}

bool CXMLParser::Parse(std::string xmlPath) {
	if (m_xmlDocument->LoadFile(xmlPath.c_str())) {
		TiXmlElement* RootElement = m_xmlDocument->RootElement();
		TiXmlAttribute* FirstAttri = RootElement->FirstAttribute();
		if (FirstAttri != NULL) {
			std::string attName = FirstAttri->Name();
			if (attName.compare("name") == 0) {
				m_prj_name = FirstAttri->Value();
			}
		}
		TiXmlElement* FirstElement = RootElement->FirstChildElement();
		for (; FirstElement != NULL; FirstElement = FirstElement->NextSiblingElement() ) {
			std::string sCurKey = FirstElement->Value();
			TiXmlAttribute* sCurAttri = FirstElement->FirstAttribute();					
			if (sCurKey.compare("testcase") == 0) {
				std::string testCaseName = "";
				if (sCurAttri != NULL) {
					std::string attName = sCurAttri->Name();
					if (attName.compare("name") == 0) {
						testCaseName = sCurAttri->Value();
						sCurAttri = sCurAttri->Next();
						attName = sCurAttri->Name();
						if (attName.compare("run") == 0) {
							std::string runState = sCurAttri->Value();
							if (runState.compare("1") == 0) {
								m_testcase.push_back(testCaseName);
							}
						}
					}
				}
				std::map<std::string, std::string> testCaseCfg;
				std::vector<std::string> testNameVec;
				TiXmlElement* SecElement = FirstElement->FirstChildElement();
				for (; SecElement != NULL; SecElement = SecElement->NextSiblingElement()) {
					sCurKey = SecElement->Value();
					if (sCurKey.compare("config") == 0) {
						TiXmlElement* CfgElement = SecElement->FirstChildElement();
						for (; CfgElement != NULL; CfgElement = CfgElement->NextSiblingElement()) {
							sCurKey = CfgElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = CfgElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
							}
							testCaseCfg.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}

						m_testcase_config.insert(std::pair< std::string, std::map<std::string, std::string> >(testCaseName, testCaseCfg));
					} else if (sCurKey.compare("test") == 0) {
						TiXmlAttribute* sCurAttri = SecElement->FirstAttribute();
						if (sCurAttri != NULL) {
							std::string attName =sCurAttri->Name();
							if (attName.compare("name") == 0) {
								std::string testName = sCurAttri->Value();
								sCurAttri = sCurAttri->Next();
								attName = sCurAttri->Name();
								if (attName.compare("run") == 0) {
									std::string runState = sCurAttri->Value();
									if (runState.compare("1") == 0) {
										testNameVec.push_back(testName);
										TiXmlElement* CfgElement = SecElement->FirstChildElement();
										for (; CfgElement != NULL; CfgElement = CfgElement->NextSiblingElement()) {
											sCurKey = CfgElement->Value();
											std::map<std::string, std::string> testCfg;
											if (sCurKey.compare("config") == 0) {
												TiXmlElement* testCfgElement = CfgElement->FirstChildElement();
												for (; testCfgElement != NULL; testCfgElement = testCfgElement->NextSiblingElement()) {
													sCurKey = testCfgElement->Value();
													std::string sCfgValue = "";
													const char* cfgValue = testCfgElement->GetText();
													if (cfgValue != 0) {
														sCfgValue = cfgValue;
													}
													testCfg.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
												}
												m_test_config.insert(std::pair< std::string, std::map<std::string, std::string> >(testName, testCfg));
											}
										}
									}
								}
							}
						}
					}
				}
				m_test.insert(std::pair< std::string, std::vector<std::string> >(testCaseName, testNameVec));
			} else if (sCurKey.compare("config") == 0) {
				TiXmlElement* SecElement = FirstElement->FirstChildElement();
				for (; SecElement != NULL; SecElement = SecElement->NextSiblingElement()) {
					sCurKey = SecElement->Value();
					std::string sCfgValue = "";
					const char* cfgValue = SecElement->GetText();
					if (cfgValue != 0) {
						sCfgValue = cfgValue;
					}
					m_common_config.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
					m_common_wconfig.insert(std::pair<std::wstring, std::wstring>(s2ws(sCurKey), s2ws(sCfgValue)));
				}
			} else if (sCurKey.compare("monitor") == 0) {
				TiXmlElement* SecElement = FirstElement->FirstChildElement();
				for (; SecElement != NULL; SecElement = SecElement->NextSiblingElement()) {
					sCurKey = SecElement->Value();
					std::string sCfgValue = "";
					const char* cfgValue = SecElement->GetText();
					if (cfgValue != 0) {
						sCfgValue = cfgValue;
					}
					std::vector<std::string> socket_info;
					TiXmlAttribute* sCurAttri = SecElement->FirstAttribute();
					if (sCurAttri != NULL) {
						std::string attName =sCurAttri->Name();
						if (attName.compare("run") == 0) {
							std::string runState = sCurAttri->Value();
							if (runState.compare("1") == 0) {
								sCurAttri = sCurAttri->Next();
								if (sCurAttri != NULL) {
									attName =sCurAttri->Name();
									if (attName.compare("port") == 0) {
										std::string portStr = sCurAttri->Value();
										socket_info.push_back(sCfgValue); // IP.
										socket_info.push_back(portStr); // PORT.

										m_monitor.insert(std::pair<std::string, std::vector<std::string> >(sCurKey, socket_info));
									}
								}
							}
						}


					}
				}
			} else if (sCurKey.compare("path") == 0) {
				TiXmlElement* SecElement = FirstElement->FirstChildElement();
				for (; SecElement != NULL; SecElement = SecElement->NextSiblingElement()) {
					std::string sPlantForm = SecElement->Value();
					if (sPlantForm.compare("windows") == 0) {
						TiXmlElement* ThirdElement = SecElement->FirstChildElement();
						for (; ThirdElement != NULL; ThirdElement = ThirdElement->NextSiblingElement()) {
							sCurKey = ThirdElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = ThirdElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
								while(-1 != sCfgValue.find('\\')){	
									std::string s1 = "/";
									sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
								}
							}
							m_wpath_win.insert(std::pair<std::string, std::wstring>(sCurKey, s2ws(sCfgValue)));
							m_path_win.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}	
						m_wpath.insert(std::pair< std::string, std::map<std::string, std::wstring> >(sPlantForm, m_wpath_win));
						m_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sPlantForm, m_path_win));
					} else if (sPlantForm.compare("linux") == 0) {
						TiXmlElement* ThirdElement = SecElement->FirstChildElement();
						for (; ThirdElement != NULL; ThirdElement = ThirdElement->NextSiblingElement()) {
							sCurKey = ThirdElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = ThirdElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
								while(-1 != sCfgValue.find('\\')){	
									std::string s1 = "/";
									sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
								}
							}
							m_wpath_linux.insert(std::pair<std::string, std::wstring>(sCurKey, s2ws(sCfgValue)));
							m_path_linux.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}
						m_wpath.insert(std::pair< std::string, std::map<std::string, std::wstring> >(sPlantForm, m_wpath_linux));
						m_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sPlantForm, m_path_linux));
					} else if (sPlantForm.compare("mac") == 0) {
						TiXmlElement* ThirdElement = SecElement->FirstChildElement();
						for (; ThirdElement != NULL; ThirdElement = ThirdElement->NextSiblingElement()) {
							sCurKey = ThirdElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = ThirdElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
								while(-1 != sCfgValue.find('\\')){	
									std::string s1 = "/";
									sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
								}
							}
							m_wpath_mac.insert(std::pair<std::string, std::wstring>(sCurKey, s2ws(sCfgValue)));
							m_path_mac.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}
						m_wpath.insert(std::pair< std::string, std::map<std::string, std::wstring> >(sPlantForm, m_wpath_mac));
						m_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sPlantForm, m_path_mac));
					} else if (sPlantForm.compare("ios") == 0) {
						TiXmlElement* ThirdElement = SecElement->FirstChildElement();
						for (; ThirdElement != NULL; ThirdElement = ThirdElement->NextSiblingElement()) {
							sCurKey = ThirdElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = ThirdElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
								while(-1 != sCfgValue.find('\\')){	
									std::string s1 = "/";
									sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
								}
							}
							m_wpath_ios.insert(std::pair<std::string, std::wstring>(sCurKey, s2ws(sCfgValue)));
							m_path_ios.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}
						m_wpath.insert(std::pair< std::string, std::map<std::string, std::wstring> >(sPlantForm, m_wpath_ios));
						m_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sPlantForm, m_path_ios));
					} else if (sPlantForm.compare("android") == 0) {
						TiXmlElement* ThirdElement = SecElement->FirstChildElement();
						for (; ThirdElement != NULL; ThirdElement = ThirdElement->NextSiblingElement()) {
							sCurKey = ThirdElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = ThirdElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
								while(-1 != sCfgValue.find('\\')){	
									std::string s1 = "/";
									sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
								}
							}
							m_wpath_android.insert(std::pair<std::string, std::wstring>(sCurKey, s2ws(sCfgValue)));
							m_path_android.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}
						m_wpath.insert(std::pair< std::string, std::map<std::string, std::wstring> >(sPlantForm, m_wpath_android));
						m_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sPlantForm, m_path_android));
					} else if (sPlantForm.compare("winrt") == 0) {
						TiXmlElement* ThirdElement = SecElement->FirstChildElement();
						for (; ThirdElement != NULL; ThirdElement = ThirdElement->NextSiblingElement()) {
							sCurKey = ThirdElement->Value();
							std::string sCfgValue = "";
							const char* cfgValue = ThirdElement->GetText();
							if (cfgValue != 0) {
								sCfgValue = cfgValue;
								while(-1 != sCfgValue.find('\\')){	
									std::string s1 = "/";
									sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
								}
							}
							m_wpath_winrt.insert(std::pair<std::string, std::wstring>(sCurKey, s2ws(sCfgValue)));
							m_path_winrt.insert(std::pair<std::string, std::string>(sCurKey, sCfgValue));
						}
						m_wpath.insert(std::pair< std::string, std::map<std::string, std::wstring> >(sPlantForm, m_wpath_winrt));
						m_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sPlantForm, m_path_winrt));
					} else if (sPlantForm.compare("txt") == 0) {
						std::string sCfgValue = SecElement->GetText();
						while(-1 != sCfgValue.find('\\')){	
							std::string s1 = "/";
							sCfgValue.replace(sCfgValue.find('\\'),s1.length(),s1);
						}
						m_txtPath = sCfgValue;
					}
				}
			} else if (sCurKey.compare("library") == 0) {
				TiXmlElement* SecElement = FirstElement->FirstChildElement();
				std::map<std::string, std::string> platformPathMap;
				for (; SecElement != NULL; SecElement = SecElement->NextSiblingElement()) {
					sCurKey = SecElement->Value();
					std::string sLibType = "";
					TiXmlElement* platformElement = SecElement->FirstChildElement();
					std::map<std::string, std::string> svnAndLibPathMap;
					for (; platformElement != NULL; platformElement = platformElement->NextSiblingElement()) {
						sLibType = platformElement->Value();
						TiXmlAttribute* sCurAttri = platformElement->FirstAttribute();
						std::string slibPath = "";
						if (sCurAttri != NULL) {
							std::string svnInfo = "";
							std::string attName =sCurAttri->Name();
							if ((0 == attName.compare("newsvn")) || (0 == attName.compare("oldsvn"))) {
								svnInfo = sCurAttri->Value();
							}

							const char* libPath = platformElement->GetText();
							if (libPath != 0) {
								slibPath = libPath;
							}
							svnAndLibPathMap.insert(std::pair<std::string, std::string>(svnInfo, slibPath));
							m_common_svn.insert(make_pair(attName,svnInfo));
						}
						sCurAttri = sCurAttri->Next();
						if (sCurAttri != NULL) {
							std::string runstate = "";
							std::string attName =sCurAttri->Name();
							if (attName.compare("run") == 0) {
								runstate = sCurAttri->Value();
								if (runstate.compare("1") == 0) {
									if (sCurKey.compare("windows") == 0) {
										m_common_lib_win_run_path.push_back(slibPath);
									} else if (sCurKey.compare("linux") == 0) {
										m_common_lib_linux_run_path.push_back(slibPath);
									} else if (sCurKey.compare("mac") == 0) {
										m_common_lib_mac_run_path.push_back(slibPath);
									} else if (sCurKey.compare("android") == 0) {
										slibPath = GetWorkPath() + slibPath;
										m_common_lib_android_run_path.push_back(slibPath);
									} else if (sCurKey.compare("ios") == 0) {
										slibPath = GetWorkPath() + slibPath;
										m_common_lib_ios_run_path.push_back(slibPath);
									} else if (sCurKey.compare("winrt") == 0) {
										slibPath = GetWorkPath() + slibPath;
										m_common_lib_winrt_run_path.push_back(slibPath);
									}
								}
							}
						}
					}
					if (sCurKey.compare("windows") == 0) {
						m_common_lib_win_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sLibType, svnAndLibPathMap));
					} else if (sCurKey.compare("linux") == 0) {
						m_common_lib_linux_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sLibType, svnAndLibPathMap));
					} else if (sCurKey.compare("mac") == 0) {
						m_common_lib_mac_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sLibType, svnAndLibPathMap));
					} else if (sCurKey.compare("android") == 0) {
						m_common_lib_android_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sLibType, svnAndLibPathMap));
					} else if (sCurKey.compare("ios") == 0) {
						m_common_lib_ios_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sLibType, svnAndLibPathMap));
					} else if (sCurKey.compare("winrt") == 0) {
						m_common_lib_winrt_path.insert(std::pair< std::string, std::map<std::string, std::string> >(sLibType, svnAndLibPathMap));
					}
				}
			}
		}
	} else {
		return false;
	}
	return true;
}

bool CXMLParser::IsUseTxtTestFiles() {
	if (m_common_config["testfiles_use_txt"].compare("TRUE") == 0) {
		return true;
	}
	return false;
}

std::string	CXMLParser::GetWorkPath() {
#if defined(_FXQA_PLATFORM_QTIOS_)
	QStringList ios_doc_path_qsl = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
	QString ios_doc_path_qs = ios_doc_path_qsl.join("");
	std::string work_path = ios_doc_path_qs.toUtf8().constData();
	return work_path;
#elif defined(_FXQA_PLATFORM_QTANDROID_)
	return "/sdcard";
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	ApplicationData^  appData = Windows::Storage::ApplicationData::Current;
	Windows::Storage::StorageFolder^ appLocalFolder = appData->LocalFolder;
	String^ appLocalPath = appLocalFolder->Path;
	return ws2s(appLocalPath->Data());
#endif
	return "";
}

std::wstring	CXMLParser::GetWWorkPath() {
#if defined(_FXQA_PLATFORM_QTIOS_)
	QStringList ios_doc_path_qsl = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
	QString ios_doc_path_qs = ios_doc_path_qsl.join("");
	std::string work_path = ios_doc_path_qs.toUtf8().constData();
	return s2ws(work_path);
#elif defined(_FXQA_PLATFORM_QTANDROID_)
	return L"/sdcard";
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	ApplicationData^  appData = Windows::Storage::ApplicationData::Current;
	Windows::Storage::StorageFolder^ appLocalFolder = appData->LocalFolder;
	String^ appLocalPath = appLocalFolder->Path;
	return appLocalPath->Data();
#endif
	return L"";
}

std::vector<std::string> CXMLParser::GetTestCase() {
	return m_testcase;
}

std::vector<std::string> CXMLParser::GetTest(std::string testCaseName) {
	//std::map<std::string, std::vector<std::string>>::iterator l_it;; 
	//l_it = m_test.find("");
	//if(l_it == m_test.end()) {
	//	return m_test[testCaseName];
	//}
	return m_test[testCaseName];
}

std::map<std::string, std::string> CXMLParser::GetConfig() {
	return m_common_config;
}

std::map<std::wstring, std::wstring> CXMLParser::GetWConfig() {
	return m_common_wconfig;
}

std::map< std::string, std::map<std::string, std::wstring> > CXMLParser::GetAllWPath() {
	return m_wpath;
}

std::vector<std::string> CXMLParser::GetMonitor(std::string monitorName) {
	return m_monitor[monitorName];
}

std::string CXMLParser::GetLibraryPath(std::string libType, std::string svnInfo) {
#if defined(_FXQA_PLATFORM_QTIOS_)
	return m_common_lib_ios_path[libType][svnInfo];
#elif defined(_FXQA_PLATFORM_QTANDROID_)
	return m_common_lib_android_path[libType][svnInfo];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	return m_common_lib_win_path[libType][svnInfo];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	return m_common_lib_winrt_path[libType][svnInfo];
#elif defined(__linux__)
	return m_common_lib_linux_path[libType][svnInfo];
#elif defined(__APPLE__)
	return m_common_lib_mac_path[libType][svnInfo];
#endif
	return "NO DEFINES";
}

std::vector<std::string> CXMLParser::GetRunLibraryPath() {
#if defined(_FXQA_MICROSERVICE_)
	return m_common_lib_service_run_path;
#else
#if defined(_FXQA_PLATFORM_QTIOS_)
	return m_common_lib_ios_run_path;
#elif defined(_FXQA_PLATFORM_QTANDROID_)
	return m_common_lib_android_run_path;
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	return m_common_lib_win_run_path;
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	return m_common_lib_winrt_run_path;
#elif defined(__linux__)
	return m_common_lib_linux_run_path;
#elif defined(__APPLE__)
	return m_common_lib_mac_run_path;
#endif
	return m_common_lib_mac_run_path;
#endif // _FXQA_MICROSERVICE_
}


std::wstring  CXMLParser::GetWPath_Files() {
#if defined(_FXQA_MICROSERVICE_)
	return m_wsTestfile;
#else
	std::wstring wPath = L"";
	if (m_common_config["testfiles_use_txt"].compare("TRUE") == 0) {
		return s2ws(m_txtPath);
	}
#if defined(_FXQA_PLATFORM_QTANDROID_)
	wPath = GetWWorkPath() + m_wpath["android"]["files"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	wPath = GetWWorkPath() + m_wpath["ios"]["files"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	wPath = GetWWorkPath() + m_wpath["winrt"]["files"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	wPath = m_wpath["windows"]["files"];
#elif defined(__linux__)
	wPath = m_wpath["linux"]["files"];
#else
	wPath = m_wpath["mac"]["files"];
#endif
	return wPath;
#endif // _FXQA_MICROSERVICE_
}

std::string	CXMLParser::GetPath_Files() {
#if defined(_FXQA_MICROSERVICE_)
	return m_sTestfile;
#else
	std::string Path = "";
	if (m_common_config["testfiles_use_txt"].compare("TRUE") == 0) {
		return m_txtPath;
	}
#if defined(_FXQA_PLATFORM_QTANDROID_)
	Path = GetWorkPath() + m_path["android"]["files"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	Path = GetWorkPath() + m_path["ios"]["files"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	Path = GetWorkPath() + m_path["winrt"]["files"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	Path = m_path["windows"]["files"];
#elif defined(__linux__)
	Path = m_path["linux"]["files"];
#else
	Path = m_path["mac"]["files"];
#endif
	return Path;
#endif //_FXQA_MICROSERVICE_
}

std::map<std::string, std::wstring>  CXMLParser::GetWPath() {
#if defined(_FXQA_PLATFORM_QTANDROID_)
	return m_wpath["android"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	return m_wpath["ios"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	return m_wpath["windows"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	return m_wpath["winrt"];
#elif defined(__linux__)
	return m_wpath["linux"];
#else
	return m_wpath["mac"];
#endif
	return m_wpath["windows"];
}

std::string  CXMLParser::GetPath_Compare() {
	std::string Path = "";
#if defined(_FXQA_PLATFORM_QTANDROID_)
	Path = GetWorkPath() + m_path["android"]["comparefile"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	Path = GetWorkPath() + m_path["ios"]["comparefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	Path = m_path["windows"]["comparefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	Path = GetWorkPath() + m_path["winrt"]["comparefile"];
#elif defined(__linux__)
	Path = m_path["linux"]["comparefile"];
#else
	Path = m_path["mac"]["comparefile"];
#endif
	return Path;
}

std::wstring CXMLParser::GetWPath_Base(){
	std::wstring wPath = L"";
#if defined(_FXQA_PLATFORM_QTANDROID_)
	wPath = GetWWorkPath() + m_wpath["android"]["basefile"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	wPath = GetWWorkPath() + m_wpath["ios"]["basefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	wPath = GetWWorkPath() + m_wpath["winrt"]["basefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	wPath = m_wpath["windows"]["basefile"];
#elif defined(__linux__)
	wPath = m_wpath["linux"]["basefile"];
#else
	wPath = m_wpath["mac"]["basefile"];
#endif
	return wPath;
}

std::string CXMLParser::GetPath_Base(){
	std::string Path = "";
#if defined(_FXQA_PLATFORM_QTANDROID_)
	Path = GetWorkPath() + m_path["android"]["basefile"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	Path = GetWorkPath() + m_path["ios"]["basefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	Path = GetWorkPath() + m_path["winrt"]["basefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	Path = m_path["windows"]["basefile"];
#elif defined(__linux__)
	Path = m_path["linux"]["basefile"];
#else
	Path = m_path["mac"]["basefile"];
#endif
	return Path;
}

std::wstring  CXMLParser::GetWPath_Compare() {
	std::wstring wPath = L"";
#if defined(_FXQA_PLATFORM_QTANDROID_)
	wPath = GetWWorkPath() +  m_wpath["android"]["comparefile"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	wPath = GetWWorkPath() +  m_wpath["ios"]["comparefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	wPath = m_wpath["windows"]["comparefile"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	wPath = GetWWorkPath() + m_wpath["winrt"]["comparefile"];
#elif defined(__linux__)
	wPath = m_wpath["linux"]["comparefile"];
#else
	wPath = m_wpath["mac"]["comparefile"];
#endif
	return wPath;
}

std::string  CXMLParser::GetPath_Report() {
	std::string Path = "";
#if defined(_FXQA_PLATFORM_QTANDROID_)
	Path = GetWorkPath() + m_path["android"]["report"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	Path = GetWorkPath() + m_path["ios"]["report"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	Path = m_path["windows"]["report"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	Path = GetWorkPath() + m_path["winrt"]["report"];
#elif defined(__linux__)
	Path = m_path["linux"]["report"];
#else
	Path = m_path["mac"]["report"];
#endif
	return Path;
}

std::wstring  CXMLParser::GetWPath_Report() {
	std::wstring wPath = L"";
#if defined(_FXQA_PLATFORM_QTANDROID_)
	wPath = GetWWorkPath() + m_wpath["android"]["report"];
#elif defined(_FXQA_PLATFORM_QTIOS_)
	wPath = GetWWorkPath() +  m_wpath["ios"]["report"];
#elif defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	wPath = m_wpath["windows"]["report"];
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	wPath = GetWWorkPath() + m_wpath["winrt"]["report"];
#elif defined(__linux__)
	wPath = m_wpath["linux"]["report"];
#else
	wPath = m_wpath["mac"]["report"];
#endif
	return wPath;
}

std::map<std::string, std::string> CXMLParser::GetSVNVersion(std::string testName)
{
	return m_common_svn;
}

std::string	CXMLParser::GetTestCaseConfig(std::string testName, std::string cfgName) {
	return m_testcase_config[testName][cfgName];
}

std::map<std::string, std::string> CXMLParser::GetTestCaseConfig(std::string testName) {
	return m_testcase_config[testName];
}

std::map<std::string, std::string> CXMLParser::GetTestConfig(std::string testName) {
	return m_test_config[testName];
}

std::string CXMLParser::GetTestConfig(std::string testName, std::string cfgName) {
	return m_test_config[testName][cfgName];
}

void CXMLParser::SetProject(std::string name) {
	m_prj_name = name;
}

void CXMLParser::AddTestcase(std::string testcase) {
	m_testcase.push_back(testcase);
}

void CXMLParser::AddTest(std::string testcase, std::string test) {
	std::vector<std::string> testNameVec;
	testNameVec.push_back(test);
	m_test.insert(std::pair< std::string, std::vector<std::string> >(testcase, testNameVec));
}

void CXMLParser::SetSvnVersion(std::string oldVersion, std::string newVersion) {
	m_common_svn.insert(make_pair("oldsvn", oldVersion));
	m_common_svn.insert(make_pair("newsvn", newVersion));
}

void CXMLParser::SetTestFile(std::string testfile) {
	m_sTestfile = testfile;
	m_wsTestfile = s2ws(testfile.c_str());
}


void CXMLParser::SetLibPath(std::string oldPath, std::string newPath) {
	m_common_lib_service_run_path.push_back(newPath);
	m_common_lib_service_run_path.push_back(oldPath);
}

void  CXMLParser::SetMonitor(std::string crash, std::string death) {
	std::vector<std::string> crash_info;
	std::string tem = crash.substr(0, crash.find(":"));
	crash_info.push_back(tem);
	tem = crash.substr(crash.find(":") + 1, crash.length());
	crash_info.push_back(tem);

	std::vector<std::string> death_info;
	tem = death.substr(0, death.find(":"));
	death_info.push_back(tem);
	tem = death.substr(death.find(":") + 1, death.length());
	death_info.push_back(tem);

	m_monitor.insert(std::pair<std::string, std::vector<std::string> >("crash", crash_info));
	m_monitor.insert(std::pair<std::string, std::vector<std::string> >("death", death_info));
}

std::string CXMLParser::GetTestFile() {
	return m_sTestfile;
}

int CXMLParser::GetDeathTimeout() {
	return m_iDeathTimeout;
}

int CXMLParser::GetHeartTime() {
	return m_iHeartTime;
}

void CXMLParser::SetMonitorTime(int heart_time, int death_time) {
	m_iHeartTime = heart_time;
	m_iDeathTimeout = death_time;
}
//std::string	CXMLParser::GetTestCase(){
//	return m_testcase.pop_back();
//}
//
//std::string	CXMLParser::GetTest(){
//	return m_test.pop_back();
//}

std::string CXMLParser::GetLogServerAddr() {
	return this->m_sLogServerAddr;
}

int CXMLParser::GetLogServerPort() {
	return this->m_iLogServerPort;
}

void CXMLParser::SetLogServer(std::string logServer, int port) {
	this->m_sLogServerAddr = logServer;
	this->m_iLogServerPort = port;
}

_END_FXQA_TEST_NAMESPACE_
