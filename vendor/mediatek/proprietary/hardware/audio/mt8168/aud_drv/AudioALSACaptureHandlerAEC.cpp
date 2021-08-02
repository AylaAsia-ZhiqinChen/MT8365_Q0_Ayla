#include "AudioALSACaptureHandlerAEC.h"

#include "AudioALSAHardwareResourceManager.h"

#ifdef MTK_AURISYS_FRAMEWORK_SUPPORT
#include "AudioALSACaptureDataClientAurisysNormal.h"
#endif
#include "AudioALSACaptureDataClient.h"
#include "AudioALSACaptureDataProviderNormal.h"
#include "AudioALSACaptureDataProviderBTSCO.h"
#include "AudioALSACaptureDataProviderBTCVSD.h"
#include "AudioALSACaptureDataProviderEchoRef.h"
#include "AudioALSACaptureDataProviderEchoRefBTSCO.h"
#include "AudioALSACaptureDataProviderEchoRefExt.h"

#ifdef SMART_PA_SUPPORT
#include "AudioSmartPaController.h"
#endif

#include "WCNChipController.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AudioALSACaptureHandlerAEC"

namespace android
{

//static FILE *pOutFile = NULL;

AudioALSACaptureHandlerAEC::AudioALSACaptureHandlerAEC(stream_attribute_t *stream_attribute_target) :
    AudioALSACaptureHandlerBase(stream_attribute_target)
{
    ALOGD("%s()", __FUNCTION__);

    init();
}


AudioALSACaptureHandlerAEC::~AudioALSACaptureHandlerAEC()
{
    ALOGD("%s()", __FUNCTION__);
}


status_t AudioALSACaptureHandlerAEC::init()
{
    mCaptureHandlerType = CAPTURE_HANDLER_AEC;
    memset(&mStreamAttributeTargetEchoRef, 0, sizeof(mStreamAttributeTargetEchoRef));
    return NO_ERROR;
}


status_t AudioALSACaptureHandlerAEC::open()
{
    ALOGD("+%s(), input_device = 0x%x, input_source = 0x%x, sample_rate=%d, num_channels=%d, output_devices=0x%x", __FUNCTION__,
          mStreamAttributeTarget->input_device,
          mStreamAttributeTarget->input_source,
          mStreamAttributeTarget->sample_rate,
          mStreamAttributeTarget->num_channels,
          mStreamAttributeTarget->output_devices);

    ASSERT(mCaptureDataClient == NULL);

    if (mStreamAttributeTarget->input_device != AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET)
    {
        //no need to config analog part while BT case
        mHardwareResourceManager->startInputDevice(mStreamAttributeTarget->input_device);
    }

#if 0   //enable for check EchoRef data is correct
    //mCaptureDataClient = new AudioALSACaptureDataClient(AudioALSACaptureDataProviderNormal::getInstance(), mStreamAttributeTarget);
    //mCaptureDataClient = new AudioALSACaptureDataClient(AudioALSACaptureDataProviderEchoRef::getInstance(), mStreamAttributeTarget);
    mCaptureDataClient = new AudioALSACaptureDataClient(AudioALSACaptureDataProviderEchoRefExt::getInstance(), mStreamAttributeTarget);
#else

    /* get data provider singleton for mic source */
    AudioALSACaptureDataProviderBase *pDataProvider = NULL;

    if (mStreamAttributeTarget->input_device == AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET)
    {
        if (WCNChipController::GetInstance()->IsBTMergeInterfaceSupported() == true)
        {
            pDataProvider = AudioALSACaptureDataProviderBTSCO::getInstance();
        }
        else
        {
            pDataProvider = AudioALSACaptureDataProviderBTCVSD::getInstance();
        }
    }
    else
    {
        pDataProvider = AudioALSACaptureDataProviderNormal::getInstance();
    }
    ASSERT(pDataProvider != NULL);


    /* get data provider singleton for echo ref */
    AudioALSACaptureDataProviderBase *pDataProviderEchoRef = NULL;

    if (mStreamAttributeTarget->input_device == AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET)
    {
        // open BT  echoref data provider // TODO: BT CVSD!?
        pDataProviderEchoRef = AudioALSACaptureDataProviderEchoRefBTSCO::getInstance();
    }
    else
    {
#ifdef SMART_PA_SUPPORT
        if (mStreamAttributeTarget->output_devices == AUDIO_DEVICE_OUT_SPEAKER &&
            AudioSmartPaController::getInstance()->isEchoReferenceSupport())
        {
            pDataProviderEchoRef = AudioALSACaptureDataProviderEchoRefExt::getInstance();
        }
        else
#endif
        {
            pDataProviderEchoRef = AudioALSACaptureDataProviderEchoRef::getInstance();
        }
    }
    ASSERT(pDataProviderEchoRef != NULL);


    /* new data client */
#ifdef MTK_AURISYS_FRAMEWORK_SUPPORT
    mCaptureDataClient = new AudioALSACaptureDataClientAurisysNormal(
        pDataProvider,
        mStreamAttributeTarget,
        pDataProviderEchoRef);
#else
    mCaptureDataClient = new AudioALSACaptureDataClient(pDataProvider, mStreamAttributeTarget);
    if (mCaptureDataClient != NULL)
    {
        memcpy(&mStreamAttributeTargetEchoRef, mStreamAttributeTarget, sizeof(stream_attribute_t));
        mCaptureDataClient->AddEchoRefDataProvider(pDataProviderEchoRef, &mStreamAttributeTargetEchoRef);
    }
#endif
#endif //end of "enable for check EchoRef data is correct"

#if 0
    pOutFile = fopen("/data/vendor/audiohal/RecRaw.pcm", "wb");
    if (pOutFile == NULL)
    {
        ALOGD("%s(), open file fail ", __FUNCTION__);
    }
#endif

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSACaptureHandlerAEC::close()
{
    ALOGD("+%s()", __FUNCTION__);

    ASSERT(mCaptureDataClient != NULL);
    delete mCaptureDataClient;
    mCaptureDataClient = NULL;

    if (audio_is_bluetooth_sco_device(mStreamAttributeTarget->input_device) == false)
    {
        //no need to config analog part while BT case
        mHardwareResourceManager->stopInputDevice(mStreamAttributeTarget->input_device);
    }

#if 0
    if (pOutFile != NULL)
    {
        fclose(pOutFile);
    }
#endif

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSACaptureHandlerAEC::routing(const audio_devices_t input_device)
{
    mHardwareResourceManager->changeInputDevice(input_device);
    return NO_ERROR;
}


ssize_t AudioALSACaptureHandlerAEC::read(void *buffer, ssize_t bytes)
{
    ALOGV("%s()", __FUNCTION__);

    mCaptureDataClient->read(buffer, bytes);
#if 0
    if (pOutFile != NULL)
    {
        fwrite(buffer, sizeof(char), bytes, pOutFile);
    }
#endif

    return bytes;
}

status_t AudioALSACaptureHandlerAEC::UpdateBesRecParam()
{
    ALOGD("+%s()", __FUNCTION__);
    if (mCaptureDataClient != NULL)
    {
        mCaptureDataClient->UpdateBesRecParam();
    }
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

} // end of namespace android
