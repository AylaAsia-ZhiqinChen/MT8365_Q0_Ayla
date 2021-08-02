#include <hidl/LegacySupport.h>
#include <string.h>
#include <signal.h>

#include "log_hidl_service.h"

namespace vendor {
namespace mediatek {
namespace hardware {
namespace log {
namespace V1_0 {
namespace implementation {

using ::android::wp;
using ::android::status_t;
using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;

void sighand(int value) {
	LOGI("receive signal : %d)", value);
}
int cpp_main() {
	LOGI("log_hidl_sys_service_main 2018_07_200 runs");

	// setup signal, signal will be used to notify threads for termination
		struct sigaction actions;
		actions.sa_flags = 0;
		sigemptyset(&actions.sa_mask);
		actions.sa_handler = sighand;
		sigaction(SIGUSR1, &actions, NULL);
		sigaction(SIGINT,&actions,NULL);
		sigaction(SIGTERM, &actions, NULL);

	//::android::hardware::configureRpcThreadpool(20, true);
	//sp<LogHidlService> mMobilelogConnectService = new LogHidlService("mobilelogd");
	LogHidlService* mMobilelogConnectService = new LogHidlService("MobileLogHidlServer");
	mMobilelogConnectService->InitService();
	LogHidlService* mMdlogConnectService = new LogHidlService("ModemLogHidlServer");
	mMdlogConnectService->InitService();
    LogHidlService* mATMlogConnectService = new LogHidlService("ATMWiFiHidlServer");
	mATMlogConnectService->InitService();
    LogHidlService* mconnFWConnectService = new LogHidlService("ConnsysFWHidlServer");
    mconnFWConnectService->InitService();
	//sp<ILog> mMobilelogConnectService = new LogHidlService("MobileLogHidlServer");
	//"com.mediatek.mdlogger.socket1"
	//::android::hardware::joinRpcThreadpool();
	return 0;
}
LogHidlService::LogHidlService(const char* name){
	 m_Recipient = nullptr;
	 m_Callback  = nullptr;
	 m_LogHidl = nullptr;
     m_threadID = -1;
	 m_stop = -1;
	 m_socketID = -1;
     m_thread = 0;

	 memset(m_Name, '\0', sizeof(char) * 64);
	 strncpy(m_Name, name, sizeof(m_Name) - 1);
	 m_Name[sizeof(m_Name) - 1] = '\0';
	//InitService();
    LOGI("[%s] LogHidlService::LogHidlService()", m_Name);

}
LogHidlService::~LogHidlService() {
	LOGI("[%s] ~LogHidlService", m_Name);
	deinitSocket();
}

void LogHidlService::InitService() {
    if (InitHidl()) {
		initSocket();
    }

}
bool LogHidlService::InitHidl()
{
	LOGI("[%s] [LOGHIDL CTRL] Begin InitHidl", m_Name);
	sleep(1);
	if(m_LogHidl == nullptr)
	{
		
		int retrycount = 5;
		//do {
			m_LogHidl = ILog::getService(m_Name);
			if(m_LogHidl != nullptr)
			{
			    LOGD("[%s]success to get log servcie hidl", m_Name);
				//break;
			} else {
			    LOGE("[%s]failed to get log servcie hidl, retrycount = %d", m_Name, retrycount);
			}
			

		//}while(retrycount-- > 0);
		
		
	}
	if(m_LogHidl == nullptr) {
		LOGE("[%s]failed to get log servcie hidl", m_Name);
		return false;
	}
	m_Recipient = new LogHidlDeathRecipient(this);
	m_LogHidl->linkToDeath(m_Recipient, 0);

	m_Callback = new LogHidlCallback(this);
	
	m_LogHidl->setCallback(m_Callback);

	LOGI("[%s] [socket id :%d] [LOGHIDL CTRL] End InitHidl", m_Name, m_socketID);
	return true;
}

int LogHidlService::initSocket()
{
	int count = 0;
	m_socketID = -1;
	m_stop = 0;
	const char * socket_name = NULL;
	if (strcmp(m_Name, "MobileLogHidlServer") == 0) {
     socket_name ="mobilelogd";

  } else if(strcmp(m_Name, "ModemLogHidlServer") == 0) {
    socket_name ="com.mediatek.mdlogger.socket1";
  } else if(strcmp(m_Name, "ConnsysFWHidlServer") == 0) {
    socket_name ="connsysfwlogd";
  } else {
    LOGE("not support hal service, for m_Name = %s", m_Name);
	return 0;
  }
	LOGI("initSocket() To connect server:(%s)", socket_name);
	while(m_socketID < 0) 
	{
		count++;
		m_socketID = socket_local_client(socket_name, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
		if (m_socketID < 0) {
        LOGI("[%s] errno = %d, string = %s, m_socketID = %d", m_Name, errno, strerror(errno), m_socketID);
		} else {
           break;
                }
		if(count == 20) {
		   LOGE("not connect with socket service, for m_Name = %s", m_Name);
		   return 0;
		}
                usleep(50*1000);
	}

	LOGI("[%s] to %s connect successful",m_Name, socket_name);
	m_threadID = pthread_create(&m_thread, NULL, wait_msg,  this);
	if(m_threadID)
	{
		LOGI("[%s] Failed to create socket thread!", m_Name);
		return 0;
	}
	
	
	return 1;
}

void LogHidlService::deinitSocket()
{
	LOGI("[Socket] deinitSocket!");
	if(m_threadID == 0)
	{
		m_stop = 1;
		pthread_kill(m_thread, SIGUSR1);
		//pthread_join(m_thread, NULL);
	}

    if (m_socketID > 0)
    {
       	close (m_socketID);
        m_socketID = -1;
    }
}

void *LogHidlService::wait_msg(void *p)
{
	if(p == NULL)
	{
		LOGI("[Socket] socket thread parameter error!");
		return NULL;
	}
	
	LogHidlService *pServer = (LogHidlService *)p;
	char data[256];
	int ret = 0;

   memset(data, '\0', sizeof(char) * 256);
	LOGI("[wait_msg] pSocket->m_socketID = %d,pSocket->m_socketID = %s", 
	       pServer->m_socketID, pServer->m_Name);

	while(pServer->m_stop == 0 && pServer->m_socketID > 0)
	{
		memset(data, '\0', sizeof(char) * 256);
		ret = read(pServer->m_socketID, data, sizeof(data) - 1);
		if(ret >0)
		{
			LOGI("Socket id = %d, read data len = %d, rawdata = %s!", pServer->m_socketID, ret, data);				
			data[ret] = '\0';
			if (pServer->m_LogHidl) {
				LOGI("[%s], sendToServer, data = %s!", pServer->m_Name, data);
	            pServer->m_LogHidl->sendToServer(data);		
	        } 
		}
		else if (ret == 0)
		{
		    LOGE("Socket id = %d, read data len = %d", pServer->m_socketID, ret);
			pServer->SocketServerDead(); 
		} else {
            LOGI("[%s] socket read errno = %d, string = %s",
   	               pServer->m_Name, errno, strerror(errno));
            usleep(100000); // wake up every 0.1sec 
		}
	}
	return NULL;
}

void LogHidlService::SocketServerDead(){
	LOGI("[%s] SocketServerDead",  m_Name);
	deinitSocket();
	initSocket();
}
void LogHidlService::SendMsgToSocketServer(const char* msg) {
	int nWritten = 0;
    if(m_socketID <= 0) {
		LOGE("[%s], retry to connect to log deamon", m_Name);
		if(!initSocket()) {
			char failResp[256] = {0};
			sprintf(failResp, "%s,0", msg);
			m_LogHidl->sendToServer(failResp);
			LOGE("[%s] reconnect to log deamon fail, send [%s] to server", m_Name, failResp);
			return;
		}
		
    }
	LOGI("send mssage [%s] to log socket id = %d", msg,  m_socketID);
	if((nWritten = write(m_socketID, msg, strlen(msg) + 1)) < 0)
	{
		LOGI("[%d] socket write error: %s", m_socketID,strerror(errno));
	}
	else
	{
		LOGE("[%d] write %d Bytes, total = %zd", m_socketID,nWritten, strlen(msg));
	}
}

void LogHidlService::handleHidlDeath() {
	m_Recipient = nullptr;
	m_Callback  = nullptr;
	m_LogHidl = nullptr;
	
	InitHidl();
}
//hidl client receive msg from server
Return<bool> LogHidlCallback::callbackToClient(const hidl_string& data) 
{
	LOGI("callbackToClient, receive data [%s]from hidl server",  data.c_str());

	char msg[256] = { 0 };
	memset(msg, '\0', 256 * sizeof(char));
	strncpy(msg, data.c_str(),strlen(data.c_str()));
	msg[sizeof(msg) - 1] = '\0';

   if(mLog != nullptr) {
   	  mLog->SendMsgToSocketServer(msg);
   }
	/*
	int nWritten = 0;
   
	LOGI("send mssage [%s] to log socket id = %d", msg,  m_soketID);
	if((nWritten = write(m_soketID, msg, strlen(msg) + 1)) < 0)
	{
		LOGI("[%d] socket write error: %s", m_soketID,strerror(errno));
	}
	else
	{
		LOGI("[%d] write %d Bytes, total = %zd", m_soketID,nWritten, strlen(msg));
	}*/
	
    return true;
}


void LogHidlDeathRecipient::serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/)
{
   if(mLog != nullptr) {
   	  mLog->handleHidlDeath();
   }
}

}  // implementation
}  // namespace V1_0
}  // namespace log
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor

