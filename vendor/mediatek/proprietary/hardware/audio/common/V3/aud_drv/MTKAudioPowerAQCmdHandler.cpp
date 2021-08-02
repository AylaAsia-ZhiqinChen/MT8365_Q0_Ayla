#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
//#include <linux/rtpm_prio.h>
#include <media/AudioParameter.h>

#include "AudioAssert.h"
#include "AudioALSAStreamManager.h"
#include "MTKAudioPowerAQManager.h"
#include "MTKAudioPowerAQCmdHandler.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG    "MTKAudioPowerAQCmdHandler"

namespace android {

#define UNIX_DOMAIN "com.mtk.poweraq"
#define QLEN 128
#define PPC_CMD_LEN 128

static String8 keyPowerAQParamSet = String8("mw.aud.se.ppc.infoset");
static String8 keyPowerAQParamGet = String8("mw.aud.se.ppc.infoget");
static String8 keyPowerAQPPCCheck = String8("check_ppc_version");
static String8 keyPowerAQSwipCheck = String8("check_swip_version");
static String8 keyPowerAQReset = String8("mw.aud.audreset");

static int makeAddr(const char* name, struct sockaddr_un* pAddr, socklen_t* pSockLen)
{
    int nameLen = strlen(name);
    if (nameLen >= (int) sizeof(pAddr->sun_path) -1)  /* too long */
        return -1;
    pAddr->sun_path[0] = '\0';  /* abstract namespace */
    strcpy(pAddr->sun_path+1, name);
    pAddr->sun_family = AF_LOCAL;
    *pSockLen = 1 + nameLen + offsetof(struct sockaddr_un, sun_path);
    return 0;
}


static int exitSocketAccept(void)
{
    struct sockaddr_un sockAddr;
    socklen_t sockLen;
    int result = 1;
    int fd = 0;
    char toybuf[PPC_CMD_LEN];
    ALOGD("exitSocketAccept ");

    memset(toybuf, 0, PPC_CMD_LEN);

    ALOGD("exitSocketAccept debug log--000");

    if (makeAddr("com.mtk.poweraq", &sockAddr, &sockLen) < 0) {
        ALOGE("makeAddr() fail");
        return -1;
    }
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        ALOGE("client socket() fail");
        return -1;
    }

    if (connect(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
        ALOGE("client connect() fail");
        return -1;
    }

    if (write(fd, toybuf, PPC_CMD_LEN) < 0) {
        ALOGE("client write() %s fail \n", toybuf);
        return -1;
    }

    close(fd);



    return 0;
}


MTKAudioPowerAQCmdHandler *MTKAudioPowerAQCmdHandler::mMTKAudioPowerAQCmdHandler = NULL;

MTKAudioPowerAQCmdHandler* MTKAudioPowerAQCmdHandler::getInstance()
{
    static pthread_mutex_t _mLock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&_mLock);

    if (mMTKAudioPowerAQCmdHandler == NULL)
        mMTKAudioPowerAQCmdHandler = new MTKAudioPowerAQCmdHandler();
    if (mMTKAudioPowerAQCmdHandler == NULL)
        ASSERT(mMTKAudioPowerAQCmdHandler != NULL);

    pthread_mutex_unlock(&_mLock);
    return mMTKAudioPowerAQCmdHandler;
}


void MTKAudioPowerAQCmdHandler::freeInstance()
{
    ALOGD("%s()", __FUNCTION__);
    if (mMTKAudioPowerAQCmdHandler != NULL) {

        delete mMTKAudioPowerAQCmdHandler;

        mMTKAudioPowerAQCmdHandler = NULL;
    }
}

MTKAudioPowerAQCmdHandler::MTKAudioPowerAQCmdHandler()
    : mPowerAQManager(MTKAudioPowerAQManager::getInstance()),
      mStreamManager(AudioALSAStreamManager::getInstance()),
      mEnable(true),
      mThreadAlive(false),
      hcmdHandlerThread(0),
      ppcCmd(NULL),
      mPpcCmdLen(0),
      mIdentity(0)
{
    int ret;

    mPpcCmdLen = PPC_CMD_LEN;
    ppcCmd = new char[mPpcCmdLen];
    if (!ppcCmd)
        ASSERT(ppcCmd != NULL);
    memset(ppcCmd, 0, mPpcCmdLen);

    ret = pthread_create(&hcmdHandlerThread, NULL, cmdHandlerThread, (void *)this);
    if (ret != 0) {
        ALOGE("%s() create thread fail!!", __FUNCTION__);
        return;
    }

}

MTKAudioPowerAQCmdHandler::~MTKAudioPowerAQCmdHandler()
{
    mEnable = false;
    ALOGD("~MTKAudioPowerAQCmdHandler mThreadAlive:%d",mThreadAlive);
    if (mThreadAlive == true)
	exitSocketAccept();

    pthread_join(hcmdHandlerThread, NULL);
    ALOGD("pthread_join hcmdHandlerThread done");

    if (ppcCmd) {
        delete[] ppcCmd;
        ppcCmd = NULL;
    }
}

void MTKAudioPowerAQCmdHandler::cmdParse(void)
{
    /// parse key value pairs
    String8 keyValuePairs = String8((const char *)ppcCmd);
    ALOGD("+%s(): %s", __FUNCTION__, keyValuePairs.string());

    AudioParameter param = AudioParameter(keyValuePairs);

    /// parse key value pairs
    String8 value_str;

    char value[PROPERTY_VALUE_MAX];
    property_get("vendor.streamout.poweraq.id", value, "0");
    mIdentity = atoi(value);

    if (param.get(keyPowerAQParamSet, value_str) == NO_ERROR) {
        param.remove(keyPowerAQParamSet);
        /* set parameters to poweraq */
        if (mIdentity) {
            mPowerAQManager->setIdentity(ppc_handler_t(mIdentity-1));
            mPowerAQManager->SetParameter(value_str.string());
        } else {
            for (int i=0;i < PPC_HANDLER_NUM;i++) {
                mPowerAQManager->setIdentity(ppc_handler_t(i));
                mPowerAQManager->SetParameter(value_str.string());
            }
        }
        goto SET_PARAM_EXIT;
    }

    if (param.get(keyPowerAQParamGet, value_str) == NO_ERROR) {
        param.remove(keyPowerAQParamGet);
        /* get parameters from poweraq */
        if (mIdentity) {
            mPowerAQManager->setIdentity(ppc_handler_t(mIdentity-1));
            mPowerAQManager->SetParameter(value_str.string());
        } else {
            for (int i=0;i < PPC_HANDLER_NUM;i++) {
                mPowerAQManager->setIdentity(ppc_handler_t(i));
                mPowerAQManager->GetParameter(value_str.string());
            }
        }
        goto SET_PARAM_EXIT;
    }

    if (param.get(keyPowerAQReset, value_str) == NO_ERROR) {
        param.remove(keyPowerAQReset);
        /* audio reset for poweraq, no smooth mute to optimize socket communication efficiency */
        //for (int i=0;i < PPC_HANDLER_NUM;i++) {
        //    mPowerAQManager->setIdentity(ppc_handler_t(i));
        //    mPowerAQManager->SetParameter("mute 1");
        //}
        //usleep(25000);      // sleep most t = 512*2/44100 = 23.7ms
        mStreamManager->standbyAllOutputStreams();
        goto SET_PARAM_EXIT;
    }

    if (param.get(keyPowerAQPPCCheck, value_str) == NO_ERROR) {
        param.remove(keyPowerAQPPCCheck);
        /* check_ppc_version from poweraq using keyValuePairs remove = */
        String8 str1(keyPowerAQPPCCheck);
        str1.append(" ");
        str1.append(value_str.string());

        /* ppc version is platform related instead of instance related */
        //for (int i=0;i < PPC_HANDLER_NUM;i++) {
            mPowerAQManager->setIdentity(ppc_handler_t(0));
            mPowerAQManager->GetParameter(str1.string());
        //}
        goto SET_PARAM_EXIT;
    }

    if (param.get(keyPowerAQSwipCheck, value_str) == NO_ERROR) {
        param.remove(keyPowerAQSwipCheck);
        /* check_swip_version from poweraq using keyValuePairs remove = */
        String8 str1(keyPowerAQSwipCheck);
        str1.append(" ");
        str1.append(value_str.string());

        /* swip version is platform related instead of instance related */
        //for (int i=0;i < PPC_HANDLER_NUM;i++) {
            mPowerAQManager->setIdentity(ppc_handler_t(0));
            mPowerAQManager->GetParameter(str1.string());
        //}
        goto SET_PARAM_EXIT;
    }

    if (param.size()) {
        ALOGW("%s(), still have param.size() = %zu, remain param = \"%s\"",
              __FUNCTION__, param.size(), param.toString().string());
    }

SET_PARAM_EXIT:
    ALOGD("-%s(): %s ", __FUNCTION__, keyValuePairs.string());
}

void *MTKAudioPowerAQCmdHandler::cmdHandlerThread(void *arg)
{
    int ret;
    MTKAudioPowerAQCmdHandler *pPowerAQCmdHandler = static_cast<MTKAudioPowerAQCmdHandler *>(arg);
    struct sockaddr_un sockAddr;
    socklen_t sockLen;
    int clientSock;
    int fd;

    // force to set priority
    struct sched_param sched_p;
    sched_getparam(0, &sched_p);
    sched_p.sched_priority =  sched_get_priority_max(SCHED_RR);    // highest priority
    if (0 != sched_setscheduler(0, SCHED_RR, &sched_p)) {
        ALOGE("[%s] failed, errno: %d", __FUNCTION__, errno);
    } else {
        sched_getparam(0, &sched_p);
        ALOGD("sched_setscheduler ok, priority: %d", sched_p.sched_priority);
    }

    if (makeAddr(UNIX_DOMAIN, &sockAddr, &sockLen) < 0) {
        ALOGE("server makeAddr fail");
        goto exit;
    }

    fd = socket(AF_UNIX,SOCK_STREAM,PF_UNIX);
    if (fd < 0) {
        ALOGE("server socket() fail");
        goto exit;
    }

    if (bind(fd, (const struct sockaddr*) &sockAddr, sockLen) < 0) {
        close(fd);
        ALOGE("server bind() fail");
        goto exit;
    }

    if (listen(fd, QLEN) < 0) {
        ALOGE("server listen() fail");
        close(fd);
        goto exit;
    }

    while(pPowerAQCmdHandler->mEnable)
    {
	pPowerAQCmdHandler->mThreadAlive = true;
        clientSock = accept(fd, NULL, NULL);
        if (clientSock < 0) {
            ALOGE("server accept fail %d", clientSock);
            ret = -1;
            continue;
            //goto exit;
        }

        ret = read(clientSock, (void*)(pPowerAQCmdHandler->ppcCmd), (pPowerAQCmdHandler->mPpcCmdLen));
        if (ret < 0) {
            ALOGE("%s read error :%d", __func__, ret);
            ret = -1;
            continue;
            //goto exit;
        }

        if ((uint32_t)ret != pPowerAQCmdHandler->mPpcCmdLen) {
            ALOGE("%s read size:%d is not equal to req :%d\n", __func__, ret, pPowerAQCmdHandler->mPpcCmdLen);
            ret = -1;
            continue;
            //goto exit;
        }

        close(clientSock);

        pPowerAQCmdHandler->cmdParse();

    }

exit:
    ALOGD("-%s(), pid: %d, tid: %d", __FUNCTION__, getpid(), gettid());
    pPowerAQCmdHandler->mThreadAlive = false ;
    ALOGD("mThreadAlive %d", pPowerAQCmdHandler->mThreadAlive);
    pthread_exit(NULL);
    return NULL;
}

}
