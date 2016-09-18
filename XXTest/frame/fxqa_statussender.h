#ifndef _FXQA_STATUSSENDER_H
#define _FXQA_STATUSSENDER_H

#include "fxqa_define.h"

#include <stdio.h>
#include <string>
#include <map>
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <process.h>
#pragma comment(lib, "WS2_32")
//#include <WinSock2.h>
//#include <Windows.h>
#else
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif


namespace fxqa_status {

typedef struct fxqa_sender_info {
    std::string    	flag;
    std::string		infoStr;
	bool			exitFlag;
	bool			stopInfoFlag;

	void*			parent;
}SENDER_INFO;

typedef struct fxqa_socket_info {
	std::string		ip;
	int				port;
	int				socket;

	void*			sender_thread;
}SOCKET_INFO;

class CLock
{
private:
	FXQA_LOCK m_section;
public:
	CLock(void) {
#if defined(_MSC_VER)
		InitializeCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutexattr_t tma;
		pthread_mutexattr_init(&tma);
		pthread_mutexattr_settype(&tma, PTHREAD_MUTEX_RECURSIVE);
		int nErr = pthread_mutex_init(&m_section, &tma);
		pthread_mutexattr_destroy(&tma);
#endif
	}
	~CLock(void) {
#if defined(_MSC_VER)
		DeleteCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutex_destroy(&m_section);
#endif
	}
	void lock() {
#if defined(_MSC_VER)
		EnterCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutex_lock(&m_section);
#else
		//do nothing
#endif	
	}
	void unLock() {
#if defined(_MSC_VER)
		LeaveCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutex_unlock(&m_section);
#else
		//do nothing
#endif
	}
};

class CQALocker {
private:
	CLock * m_pLock;
public:

	CQALocker(CLock * pLock) {
		m_pLock = pLock;
		pLock->lock();
	}
	~CQALocker() {
		m_pLock->unLock();
	}
};

class CStatusSender {
	SOCKET_INFO		m_socketInfo;
	SENDER_INFO*	m_senderInfo;
	FXQA_LOCK		m_lock;
	bool			m_bConnected;
	std::string		m_sTestcaseName;
	std::string		m_sTestName;
		
public:
	CStatusSender(SOCKET_INFO socket_info);
	~CStatusSender();
		
	bool					CreateSenderThread();
	bool					IsRunning();
	FXQA_LOCK				GetLock() {return m_lock;};

	void					StopSenderThread();
	void					SetSenderInfo(const char* flag, const char* str);
	void					StartSendInfo();
	void					StopSendInfo();

	bool					InitSocket();
	bool					ConnectToServer();
	bool					CloseSocket();
	int						SendMsgToServer(const char* msg);
	int						SendMsgToServer();

	int						POST(std::string url, std::string msg, std::string& recv_msg);
	void					SetTestInfo(std::string testcase_name, std::string test_name);
	int						SendStartCrash(std::string testCaseName, std::string testName, std::string testfile, int iHeartTime);
	int						SendStartDeath(std::string testCaseName, std::string testName, std::string testfile, int iTimeout);
	int						SendStopCrash();
	int						SendStopDeath();
	int						SendNoRun(std::string testCaseName, std::string testName, std::string testfile);

	//void					ReadXBytes(int socket, unsigned int recvbuflen, char* recvbuf);
};

class CStatusToucher {
	static bool				m_bInit;
    static int              m_pid;
    SOCKET_INFO             m_socketInfo;
    static CStatusSender*   m_sender;
    static SENDER_INFO      m_senderInfo;

public:
    CStatusToucher(SOCKET_INFO sockInfo);
    ~CStatusToucher();

    bool Init();
	static void SetSender(CStatusSender* sender);
    static void SetSenderInfo(const char* flag, const char* str);
    static void Touch();

};

class CLogSender {
	static bool				m_bInit;
	static int              m_pid;
	SOCKET_INFO             m_socketInfo;
	static CStatusSender*   m_sender;
	static int				m_socket;
	static std::string		m_sELKUrl;
	static std::string		m_sELKRes;
	static std::string		m_sTestCaseName;
	static std::string		m_sTestName;
	static std::string		m_sTestFile;
	static std::string		m_sVersion;

public:
	CLogSender(SOCKET_INFO sockInfo);
	~CLogSender();

	bool Init();
	static void SetSenderInfo(const char* flag, const char* str);
	static void SentLog(const char* flag, const char* str);
	
	static bool			InitLinkELK(std::string logServerAddr, int logServerPort);
	static void			SetTestInfo(std::map<std::string, std::string> versionMap, std::string testCaseName, std::string testName, std::string testFile);
	static void			DisconnectELK();
	static int			Log2ELK(std::string logType, std::string msg);
	static std::string	GetELKReturn();
};


void					Sleep(long time_ms);

} // namespace fxqa_status

#endif // _FXQA_STATUSSENDER_H
