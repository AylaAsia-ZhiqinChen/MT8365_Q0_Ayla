#ifndef MTKAudioPowerAQCmdHandler_H
#define MTKAudioPowerAQCmdHandler_H

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <cutils/log.h>
#include <utils/threads.h>


namespace android {

class AudioALSAStreamManager;
class MTKAudioPowerAQManager;

class MTKAudioPowerAQCmdHandler
{
public:

    static MTKAudioPowerAQCmdHandler *getInstance();

    void freeInstance();

private:

    MTKAudioPowerAQCmdHandler(void);

    ~MTKAudioPowerAQCmdHandler();

    void cmdParse(void);

    static void *cmdHandlerThread(void *arg);

    static MTKAudioPowerAQCmdHandler *mMTKAudioPowerAQCmdHandler;

    MTKAudioPowerAQManager *mPowerAQManager;

    AudioALSAStreamManager *mStreamManager;

    /**
    * cmd Handler thread
    */
    bool mEnable;
    bool mThreadAlive;
    pthread_t hcmdHandlerThread;
    char *ppcCmd;
    uint32_t mPpcCmdLen;

    /**
     * PowerAQ Tool will set parameter to which instance
     *  0. Normal/Fast(default tool will set parameter to both instance)
     *  1. Normal
     *  2. Fast
     */
    int mIdentity;

};
}
#endif //MTKAudioPowerAQCmdHandler_H
