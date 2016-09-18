
#include "fxqa_statussender.h"
#include <sstream>
#include <algorithm>
namespace fxqa_status {

	void Sleep(long time_ms) {
#if defined(_MSC_VER)
		::Sleep(1000);
#elif defined(__GNUC__)
		sleep(time_ms / 1000);
#else
		sleep(time_ms / 1000);
#endif
	}

CLock g_lock;
CLock g_lock2;
#if defined (_MSC_VER)
	void CreateSenderSent(void* param) {
#else
	void* CreateSenderSent(void* param) {
#endif
		SENDER_INFO* sender_info = (SENDER_INFO*)param;

		int pid = getpid();
		while (!sender_info->exitFlag) {
            CQALocker locker(&g_lock2);
			if (sender_info->stopInfoFlag) {
				fxqa_status::Sleep(1000);
				continue;
			}
			CStatusSender* pParent = (CStatusSender*)sender_info->parent;
#if defined(_FXQA_MICROSERVICE_)
			std::string recv_msg = "";
			int ret = pParent->POST(sender_info->flag, sender_info->infoStr, recv_msg);
#else
            std::stringstream ss;
            ss << "{\"PID\":\"" << pid << "\", \"TEST\":\"" << sender_info->flag << "\", \"INFO\":\"" << sender_info->infoStr << "\"}";
             //printf(sendStr);
            std::string sendStr = ss.str();
			
			int ret = pParent->SendMsgToServer(sendStr.c_str());
#endif // _FXQA_MICROSERVICE_
			if (ret == -1) {
                 printf("send error: %s(errno: %d)",strerror(errno),errno);
			}
			fxqa_status::Sleep(1000);
		}

// 		delete sender_info;
// 		sender_info = NULL;
	}

	CStatusSender::CStatusSender(SOCKET_INFO socket_info) : 
		m_socketInfo(), 
		m_senderInfo(NULL), 
		m_bConnected(false),
		m_sTestcaseName(""), 
		m_sTestName("") {
		m_socketInfo = socket_info;
	}

	CStatusSender::~CStatusSender() {
		CloseSocket();
		if (m_senderInfo != NULL) {
			delete m_senderInfo;
			m_senderInfo = NULL;
		}
	}

	bool CStatusSender::CreateSenderThread() {
		if(InitSocket() == false) return false;
		if(ConnectToServer() == false) return false;
#if defined(_MSC_VER)
		InitializeCriticalSection(&m_lock);
#elif defined(__GNUC__)
		pthread_mutexattr_t tma;
		pthread_mutexattr_init(&tma);
		pthread_mutexattr_settype(&tma, PTHREAD_MUTEX_RECURSIVE);
		int nErr = pthread_mutex_init(&m_lock, &tma);
		pthread_mutexattr_destroy(&tma);
#endif
		SENDER_INFO* sender_info = new SENDER_INFO;
		

#if defined(_FXQA_MICROSERVICE_)
		sender_info->infoStr = "testcase=" + this->m_sTestcaseName;
		sender_info->infoStr = "&test=" + this->m_sTestName;
		sender_info->flag = "http://127.0.0.1:10018/heart";
		sender_info->stopInfoFlag = true;
#else
		sender_info->infoStr = "TEST_START";
		sender_info->flag = "TEST_START";
		sender_info->stopInfoFlag = false;
#endif // _FXQA_MICROSERVICE_
		sender_info->exitFlag = false;
		sender_info->parent = this;
		m_senderInfo = sender_info;
		m_socketInfo.sender_thread = NULL;

#if defined(_MSC_VER)
		m_socketInfo.sender_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateSenderSent, m_senderInfo, 0, NULL);
		if(NULL == m_socketInfo.sender_thread) return false;
#elif defined(__GNUC__)
		pthread_t thread = 0;
		nErr = pthread_create(&thread , NULL, &CreateSenderSent, sender_info);
		if (nErr != 0) return false;
#endif
		return true;
	}

	bool  CStatusSender::IsRunning() {
#if defined(_FXQA_MICROSERVICE_)
		if (!m_bConnected) 
			return false;
		return true;
#endif
		if (m_senderInfo == NULL) 
			return false;
		return true;
	}

	void CStatusSender::StopSenderThread() {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->exitFlag = true;
#if defined(_MSC_VER)
		DeleteCriticalSection(&m_lock);
#elif defined(__GNUC__)
		pthread_mutex_destroy(&m_lock);
#endif
	}

	void CStatusSender::SetSenderInfo(const char* flag, const char* str) {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->flag = flag;
		m_senderInfo->infoStr = str;
	}

	void CStatusSender::StartSendInfo() {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->stopInfoFlag = false;
	}

	void CStatusSender::StopSendInfo() {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->stopInfoFlag = true;
	}

	bool CStatusSender::InitSocket() {
#ifdef _MSC_VER
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (::WSAStartup(socketVersion, &wsaData) != 0) {
			return false;
		}
		return true;
#else
		return true;
#endif
	}

	bool CStatusSender::ConnectToServer() {
		if( (m_socketInfo.socket = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
			return false;
		}
		/*
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
		u_long iMode = 1;
		ioctlsocket(m_socketInfo.socket, FIONBIO, &iMode);
#else
		fcntl(m_socketInfo.socket, F_SETFL, O_NONBLOCK);
#endif
		*/
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(m_socketInfo.port);
		servAddr.sin_addr.s_addr = inet_addr(m_socketInfo.ip.c_str());
		int ret = connect(m_socketInfo.socket, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if(ret < 0){
			printf("%s:%d, Socket connect error: %s(errno: %d),status monitor is not running.\n",m_socketInfo.ip.c_str(), m_socketInfo.port, strerror(errno),errno);
			return false;
		}
		m_bConnected = true;
		return true;
	}

	int CStatusSender::SendMsgToServer(const char* msg) {
        return ::send(m_socketInfo.socket, msg, strlen(msg), 0);
	}

	int CStatusSender::SendMsgToServer() {
		return ::send(m_socketInfo.socket, "Running...", strlen("Running..."), 0);
	}

	void CStatusSender::SetTestInfo(std::string testcase_name, std::string test_name) {
		m_sTestcaseName = testcase_name;
		m_sTestName = test_name;
	}

	int CStatusSender::SendStartCrash(std::string testCaseName, std::string testName, std::string testfile, int iHeartTime) {
		this->SetTestInfo(testCaseName, testName);

		std::stringstream ss;
		ss << "production=fxcore&action=start&pid=" << getpid() 
			<< "&testcase=" << testCaseName 
			<< "&test=" << testName 
			<< "&testfile=" << testfile 
			<< "&timeout=" << iHeartTime;

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		int ret = this->POST("http://localhost:10018/heart", post_msg, rec_msg);

		std::stringstream ss1;
		ss1 << "production=fxcore&action=heart&pid=" << getpid() 
			<< "&testcase=" << testCaseName 
			<< "&test=" << testName;
		post_msg = ss1.str();
		this->SetSenderInfo("http://localhost:10018/heart", post_msg.c_str());
		this->StartSendInfo();
		return ret;
	}

	int CStatusSender::SendStartDeath(std::string testCaseName, std::string testName, std::string testfile, int iTimeout) {
		this->SetTestInfo(testCaseName, testName);

		std::stringstream ss;
		ss << "production=fxcore&action=start&pid=" << getpid() 
			<< "&testcase=" << testCaseName 
			<< "&test=" << testName 
			<< "&testfile=" << testfile 
			<< "&timeout=" << iTimeout;

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		int ret = this->POST("http://localhost:10018/health", post_msg, rec_msg);

		return ret;
	}

	int CStatusSender::SendStopCrash() {
		std::stringstream ss;
		ss << "production=fxcore&action=stop&pid=" << getpid();

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		int ret = this->POST("http://localhost:10018/heart", post_msg, rec_msg);
		//this->SetSenderInfo("http://localhost:10018/heart", post_msg);
		this->StopSendInfo();
		return ret;
	}

	int CStatusSender::SendStopDeath() {
		std::stringstream ss;
		ss << "production=fxcore&action=stop&pid=" << getpid();

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		int ret = this->POST("http://localhost:10018/health", post_msg, rec_msg);
		this->StopSendInfo();
		return ret;
	}

	int CStatusSender::SendNoRun(std::string testCaseName, std::string testName, std::string testfile) {
		std::stringstream ss;
		ss << "production=fxcore&pid=" << getpid() 
			<< "&testcase=" << testCaseName 
			<< "&test=" << testName 
			<< "&testfile=" << testfile;

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		return this->POST("http://localhost:10018/norun", post_msg, rec_msg);
	}

	int CStatusSender::POST(std::string url, std::string msg, std::string& recv_msg) {
		char *pHttpPost = "POST %s HTTP/1.1\r\n"  
			"Host: %s:%d\r\n"  
			"Content-Type: application/x-www-form-urlencoded\r\n"  
			"Content-Length: %d\r\n\r\n"  
			"%s";  
 
		char* host = "0.0.0.0";  
		int port = 9090;  

		char* strHttpPost = new char[256 + msg.length()];
		memset(strHttpPost, 0, sizeof(strHttpPost));
		sprintf(strHttpPost, pHttpPost, url.c_str(), host, port, msg.length(), msg.c_str());  

		int iResult = 0;
		iResult = ::send(m_socketInfo.socket, strHttpPost, strlen(strHttpPost), 0);
		if (iResult <= 0) {
			return -1;
		}

		char recvbuf[1024];
		memset(recvbuf, 0, 1024);

		int recvbuflen = 1024;
		iResult = recv(m_socketInfo.socket, recvbuf, recvbuflen, 0);
		if ( iResult <= 0 ) {
			return -1;
		}

		std::string ssbuf = recvbuf;
		std::string sLen = ssbuf.substr(ssbuf.find("Content-Length:")+15);
		sLen = sLen.substr(0, sLen.find("\n"));
		recvbuflen = atoi(sLen.c_str());

		recv_msg = ssbuf.substr(ssbuf.length() - recvbuflen);

		//std::stringstream ss1;
		//ss1 << recvbuf << std::endl;
		//OutputDebugStringA(recv_msg.c_str());

		return 0;
	}

	//void CStatusSender::ReadXBytes(int socket, unsigned int recvbuflen, char* recvbuf)
	//{
	//	int iResult = 0;
	//	do {

	//		iResult = recv(socket, recvbuf, recvbuflen, 0);
	//		if ( iResult > 0 )
	//			printf("Bytes received: %d\n", iResult);
	//		else if ( iResult == 0 )
	//			printf("Connection closed\n");
	//		else
	//			printf("recv failed: %d\n", WSAGetLastError());

	//	} while( iResult > 0 );
	//	//int bytesRead = 0;
	//	//int result;
	//	//while (bytesRead < x)
	//	//{
	//	//	result = read(socket, buffer + bytesRead, x - bytesRead);
	//	//	if (result < 1 )
	//	//	{
	//	//		// Throw your error.
	//	//	}

	//	//	bytesRead += result;
	//	//}
	//}

	bool CStatusSender::CloseSocket() {
#ifdef _MSC_VER
		closesocket(m_socketInfo.socket);
		::WSACleanup();
#else
		close(m_socketInfo.socket);
#endif
		return true;
	}


	// StatusToucher implement.
	//
    CStatusSender*  CStatusToucher::m_sender = NULL;
    CStatusToucher::CStatusToucher(SOCKET_INFO sockInfo) {
        m_socketInfo = sockInfo;
        m_pid = getpid();
    }

	bool CStatusToucher::m_bInit = false;
    bool CStatusToucher::Init() {
        m_sender = new CStatusSender(m_socketInfo);
        if (m_sender == NULL)
            return false;
        if (!m_sender->InitSocket())
            return false;
        if (!m_sender->ConnectToServer())
            return false;
		m_bInit = true;
        return true;
    }

    SENDER_INFO  CStatusToucher::m_senderInfo;
    void CStatusToucher::SetSenderInfo(const char* flag, const char* str) {
		if (!m_bInit) return;
        m_senderInfo.flag = flag;
        m_senderInfo.infoStr = str;
    }

	void CStatusToucher::SetSender(CStatusSender* sender) {
		m_bInit = true;
		m_sender = sender;
		m_pid = getpid();
	}

    int  CStatusToucher::m_pid = -1;
    void CStatusToucher::Touch() {
		if (!m_bInit) return;
		std::stringstream ss;
#if !defined(_FXQA_MICROSERVICE_)
        ss << "{\"PID\":\"" << m_pid << "\", \"TEST\":\"" << m_senderInfo.flag << "\", \"INFO\":\"" << m_senderInfo.infoStr << "\"}";
        std::string sendStr = ss.str();
        m_sender->SendMsgToServer(sendStr.c_str());
#else
		ss << "action=heart&pid=" << m_pid;
		std::string sendStr = ss.str();
		std::string rec_msg = "";
		m_sender->POST("http://localhost:10018/health", sendStr, rec_msg);
#endif // _FXQA_MICROSERVICE_
    }

    CStatusToucher::~CStatusToucher() {
        if (m_sender != NULL) {
            delete m_sender;
            m_sender = NULL;
        }
    }

	// CLogSender implement.
	//
	CStatusSender*  CLogSender::m_sender = NULL;
	CLogSender::CLogSender(SOCKET_INFO sockInfo) {
		m_socketInfo = sockInfo;
		m_pid = getpid();
	}

	bool CLogSender::m_bInit = false;
	bool CLogSender::Init() {
		m_sender = new CStatusSender(m_socketInfo);
		if (m_sender == NULL)
			return false;
		m_bInit = true;
		return true;
	}

	int  CLogSender::m_pid = -1;
	void CLogSender::SentLog(const char* flag, const char* str) {
		if (!m_bInit) return;

		if (!m_sender->InitSocket())
			return;
		if (!m_sender->ConnectToServer())
			return;

		std::stringstream ss;
		ss << "{\"PID\":\"" << m_pid << "\", \"TEST\":\"" << flag << "\", \"INFO\":\"" << str << "\"}";
		std::string sendStr = ss.str();
		m_sender->SendMsgToServer(sendStr.c_str());
	}

	int CLogSender::Log2ELK(std::string logType, std::string msg) {
		if (msg.length() == 0) {
			return 0;
		}
		std::replace( msg.begin(), msg.end(), '\t', ' ');
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
		std::replace( msg.begin(), msg.end(), '\\', '/');
#endif // _FXQA_PLATFORM_WINDOWS_WIN32_

		std::stringstream ss;
		ss << "n=fxcore&s={" << 
			"\"production\":\"fxcore\"," <<
			"\"project\":\"fx_compare\"," <<
			"\"version\":\"" << m_sVersion << "\"," <<
			"\"testcase\":\"" << m_sTestCaseName << "\"," <<
			"\"test\":\"" << m_sTestName << "\"," <<
			"\"testfile\":\"" << m_sTestFile << "\"," <<
			"\"type\":\"" << logType << "\"," << 
			"\"msg\":\"" << msg << "\"}"; 
		std::string post_msg = ss.str();

		char *pHttpPost = "POST %s HTTP/1.1\r\n"  
			"Host: %s:%d\r\n"  
			"Content-Type: application/x-www-form-urlencoded\r\n"  
			"Content-Length: %d\r\n\r\n"  
			"%s";  

		char* host = "0.0.0.0";  
		int port = 9090;  

		char* strHttpPost = new char[256 + post_msg.length()];
		memset(strHttpPost, 0, sizeof(strHttpPost));
		sprintf(strHttpPost, pHttpPost, m_sELKUrl.c_str(), host, port, post_msg.length(), post_msg.c_str());  

		int iResult = 0;
		iResult = ::send(m_socket, strHttpPost, strlen(strHttpPost), 0);
		if (iResult <= 0) {
			return -1;
		}

		char recvbuf[1024];
		memset(recvbuf, 0, 1024);

		int recvbuflen = 1024;
		iResult = recv(m_socket, recvbuf, recvbuflen, 0);
		if ( iResult <= 0 ) {
			return -1;
		}

		std::string ssbuf = recvbuf;
		std::string sLen = ssbuf.substr(ssbuf.find("Content-Length:")+15);
		sLen = sLen.substr(0, sLen.find("\n"));
		recvbuflen = atoi(sLen.c_str());

		m_sELKRes = ssbuf.substr(ssbuf.length() - recvbuflen);

		//std::stringstream ss1;
		//ss1 << recvbuf << std::endl;
		//OutputDebugStringA(recv_msg.c_str());

		return 0;
	}

	std::string CLogSender::m_sELKRes = "";
	std::string CLogSender::GetELKReturn() {
		return m_sELKRes;
	}

	int  CLogSender::m_socket = -1;
	std::string CLogSender::m_sELKUrl = "http://";
	bool CLogSender::InitLinkELK(std::string logServerAddr, int logServerPort) {
#ifdef _MSC_VER
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (::WSAStartup(socketVersion, &wsaData) != 0) {
			return false;
		}
#endif

		std::stringstream ss1;
		ss1 << "http://" << logServerAddr << ":" << logServerPort << "/logs";
		m_sELKUrl = ss1.str();

		if( (m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
			return false;
		}
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(logServerPort);
		servAddr.sin_addr.s_addr = inet_addr(logServerAddr.c_str());
		int ret = connect(m_socket, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if(ret < 0){
			return false;
		}
		return true;
	}

	void CLogSender::DisconnectELK() {
#ifdef _MSC_VER
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		m_sTestFile = "";
		m_sTestName = "";
		m_sTestCaseName = "";
	}

	std::string CLogSender::m_sTestCaseName = "";
	std::string CLogSender::m_sTestName = "";
	std::string CLogSender::m_sTestFile = "";
	std::string CLogSender::m_sVersion = "";
	void CLogSender::SetTestInfo(std::map<std::string, std::string> versionMap, std::string testCaseName, std::string testName, std::string testFile) {
		m_sTestFile = testFile;
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
		std::replace( m_sTestFile.begin(), m_sTestFile.end(), '\\', '/');
#endif
		m_sTestName = testName;
		m_sTestCaseName = testCaseName;
		m_sVersion += versionMap["oldsvn"];
		m_sVersion += "*";
		m_sVersion += versionMap["newsvn"];
	}

	CLogSender::~CLogSender() {
		if (m_sender != NULL) {
			delete m_sender;
			m_sender = NULL;
		}
	}
}

