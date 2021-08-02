#include "AudioALSACaptureHandlerHotword.h"

#include "AudioALSAHardwareResourceManager.h"

#include "AudioALSACaptureDataClient.h"
#include "AudioALSACaptureDataProviderNormal.h"
#include "AudioALSACaptureDataClientSoundTrigger.h"
#include "AudioALSACaptureDataProviderSoundTrigger.h"
#include "AudioALSACaptureDataProviderHwCM.h"
#include "AudioALSACaptureDataClientVoiceRecProc.h"

#include "AudioVUnlockDL.h"
#include "AudioALSAAdspVaController.h"

#define LOG_TAG "AudioALSACaptureHandlerHotword"

//For test
//#define MTK_ADSP_VA_ST_TIMEOUT_SUPPORT

namespace android
{

static FILE *pOutFile = NULL;

AudioALSACaptureHandlerHotword::AudioALSACaptureHandlerHotword(stream_attribute_t *stream_attribute_target) :
    AudioALSACaptureHandlerBase(stream_attribute_target),
    mStDevSupport(0),
    mStDevInUse(false)
{
    ALOGD("%s()", __FUNCTION__);

    init();
}


AudioALSACaptureHandlerHotword::~AudioALSACaptureHandlerHotword()
{
    ALOGD("+%s()", __FUNCTION__);

    ALOGD("%-s()", __FUNCTION__);
}


status_t AudioALSACaptureHandlerHotword::init()
{
    ALOGD("%s()", __FUNCTION__);

    return NO_ERROR;
}


status_t AudioALSACaptureHandlerHotword::open()
{
    struct sound_trigger_device *stdev = AudioALSAAdspVaController::getInstance()->getSoundTriggerDevice();
    int ret = NO_ERROR;

    ALOGD("+%s(), input_device = 0x%x, input_source = 0x%x, sample_rate=%d, num_channels=%d",
          __FUNCTION__, mStreamAttributeTarget->input_device, mStreamAttributeTarget->input_source, mStreamAttributeTarget->sample_rate,
          mStreamAttributeTarget->num_channels);

    ASSERT(mCaptureDataClient == NULL);

    if (stdev != NULL && stdev->get_parameters) {
        ret = stdev->get_parameters(0, &mStDevSupport);
        if ((ret != 0) || (mStDevSupport <= 0))
            mStDevSupport = 0;
    }

    if (mStDevSupport
#ifndef MTK_ADSP_VA_ST_TIMEOUT_SUPPORT
        && mStreamAttributeTarget->input_source == AUDIO_SOURCE_HOTWORD
#endif
        ) 
    {
        mCaptureDataClient = new AudioALSACaptureDataClientSoundTrigger(AudioALSACaptureDataProviderSoundTrigger::getInstance(), mStreamAttributeTarget);
        mStDevInUse = true;
    }
    else
    {
        mCaptureDataClient = new AudioALSACaptureDataClientVoiceRecProc(AudioALSACaptureDataProviderHwCM::getInstance(), mStreamAttributeTarget);
        mStDevInUse = false;
    }


    //TODO if need start input device, now think default record only support build in mic
    if (!mStDevInUse)
        mHardwareResourceManager->startInputDevice(AUDIO_DEVICE_IN_BUILTIN_MIC);

#if 0
    //============Voice UI&Unlock REFERECE=============
    AudioVUnlockDL *VUnlockhdl = AudioVUnlockDL::getInstance();
    if (VUnlockhdl != NULL)
    {
        struct timespec systemtime;
        clock_gettime(CLOCK_MONOTONIC, &systemtime);
        VUnlockhdl->SetUplinkStartTime(systemtime);
    }
    //===========================================
#endif
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSACaptureHandlerHotword::close()
{
    ALOGD("+%s()", __FUNCTION__);

    //TODO record resource
    if (!mStDevInUse)
        mHardwareResourceManager->stopInputDevice(AUDIO_DEVICE_IN_BUILTIN_MIC);

    ASSERT(mCaptureDataClient != NULL);
    delete mCaptureDataClient;

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSACaptureHandlerHotword::routing(const audio_devices_t input_device)
{
    //TODO routing path control, not support build-in mic and headphone switch
    //mHardwareResourceManager->changeInputDevice(input_device);
    return NO_ERROR;
}


ssize_t AudioALSACaptureHandlerHotword::read(void *buffer, ssize_t bytes)
{

    int retVal;

    ALOGV("%s()", __FUNCTION__);

    retVal = mCaptureDataClient->read(buffer, bytes);
#ifdef MTK_ADSP_VA_ST_TIMEOUT_SUPPORT
    if ((retVal != bytes) && mStDevInUse)
    {
        //switch input device from soundtrigger to normal input
        mStDevInUse = false;
        delete mCaptureDataClient;
		mHardwareResourceManager->startInputDevice(mStreamAttributeTarget->input_device);
        mCaptureDataClient = new AudioALSACaptureDataClientVoiceRecProc(AudioALSACaptureDataProviderHwCM::getInstance(), mStreamAttributeTarget);
        mCaptureDataClient->read(buffer, bytes);
    }
#endif

    return bytes;
}

} // end of namespace android
