#define LOG_TAG "AudioALSAStreamManager"

#include "AudioALSAStreamManager.h"

#include <cutils/properties.h>

#include <tinyalsa/asoundlib.h> // TODO(Harvey): move it

#include "WCNChipController.h"

#include "AudioALSAStreamOut.h"
#include "AudioALSAStreamIn.h"
#include "AudioDeviceInt.h"
#include "AudioALSAPlaybackHandlerBase.h"
#include "AudioALSAPlaybackHandlerNormal.h"
#include "AudioALSAPlaybackHandlerFast.h"
#include "AudioALSAPlaybackHandlerVoice.h"
#include "AudioALSAPlaybackHandlerBTSCO.h"
#include "AudioALSAPlaybackHandlerBTCVSD.h"
#include "AudioALSAPlaybackHandlerFMTransmitter.h"
#include "AudioALSAPlaybackHandlerHDMI.h"

#include "AudioALSACaptureHandlerBase.h"
#include "AudioALSACaptureHandlerNormal.h"
#include "AudioALSACaptureHandlerVoice.h"
#include "AudioALSACaptureHandlerFMRadio.h"
#include "AudioALSACaptureHandlerBT.h"

#include "AudioALSACaptureHandlerAEC.h"

#if defined(MTK_SPEAKER_MONITOR_SUPPORT)
#include "AudioALSACaptureHandlerSpkFeed.h"
#endif

#include "AudioALSASpeechPhoneCallController.h"
#include "AudioALSAFMController.h"

//#include "AudioALSAVolumeController.h"
//#include "AudioVolumeInterface.h"
#include "AudioVolumeFactory.h"
#include "AudioSmartPaController.h"

#include "AudioALSAVoiceWakeUpController.h"

#include "AudioALSAHardwareResourceManager.h" // TODO(Harvey): move it

#include "AudioALSASampleRateController.h"


#include "AudioCompFltCustParam.h"
#include "SpeechDriverInterface.h"
#include "SpeechDriverFactory.h"
#include "AudioALSADriverUtility.h"
#include "SpeechEnhancementController.h"
#include "SpeechVMRecorder.h"
#include <AudioEventThreadManager.h>
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
#include "AudioHfpController.h"
#include "AudioALSAPlaybackHandlerHfpClient.h"
#include "AudioALSACaptureHandlerNull.h"
#endif

#if defined(MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT)
#include "AudioParamParser.h"
#include "AudioALSAParamTuner.h"
#include "SpeechParamParser.h"
#endif


static struct pcm_config mLoopbackConfig; // TODO(Harvey): move it to AudioALSAHardwareResourceManager later
static struct pcm *mLoopbackPcm = NULL; // TODO(Harvey): move it to AudioALSAHardwareResourceManager & AudioALSAPlaybackDataDispatcher later
static struct pcm_config mLoopbackUlConfig; // TODO(Harvey): move it to AudioALSAHardwareResourceManager later
static struct pcm *mLoopbackUlPcm = NULL; // TODO(Harvey): move it to AudioALSAHardwareResourceManager & AudioALSAPlaybackDataDispatcher later
static struct pcm *mHdmiPcm = NULL; // TODO(Harvey): move it to AudioALSAHardwareResourceManager & AudioALSAPlaybackDataDispatcher later

namespace android {

/*==============================================================================
 *                     Property keys
 *============================================================================*/

const char PROPERTY_KEY_VOICE_WAKE_UP_NEED_ON[PROPERTY_KEY_MAX] = "persist.vendor.vow.vw_need_on";
const char PROPERTY_KEY_SET_BT_NREC[PROPERTY_KEY_MAX] = "persist.vendor.debug.set_bt_aec";
const char *PROPERTY_KEY_BESLOUDNESS_SWITCH_STATE = "persist.vendor.audiohal.besloudness_state";
const char *PROPERTY_KEY_FIRSTBOOT_STATE = "persist.vendor.audiohal.firstboot";
#define BESLOUDNESS_SWITCH_DEFAULT_STATE (1)


/*==============================================================================
 *                     Singleton Pattern
 *============================================================================*/

AudioALSAStreamManager *AudioALSAStreamManager::mStreamManager = NULL;
AudioALSAStreamManager *AudioALSAStreamManager::getInstance() {
    static AudioLock mGetInstanceLock;
    AL_AUTOLOCK(mGetInstanceLock);

    if (mStreamManager == NULL) {
        mStreamManager = new AudioALSAStreamManager();
    }
    ASSERT(mStreamManager != NULL);
    return mStreamManager;
}

/*==============================================================================
 *                     Callback Function
 *============================================================================*/

#if defined(MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT)
void CallbackAudioXmlChanged(AppHandle *appHandle, const char *audioTypeName) {
    ALOGD("+%s(), audioType = %s", __FUNCTION__, audioTypeName);
    // reload XML file
    AppOps *appOps = appOpsGetInstance();
    if (appOps == NULL) {
        ALOGE("%s(), Error: AppOps == NULL", __FUNCTION__);
        ASSERT(0);
        return;
    }

    if (appOps->appHandleReloadAudioType(appHandle, audioTypeName) == APP_ERROR) {
        ALOGE("%s(), Reload xml fail!(audioType = %s)", __FUNCTION__, audioTypeName);
    } else {
        if (strcmp(audioTypeName, audioTypeNameList[AUDIO_TYPE_SPEECH]) == 0) {
            //"Speech"
            AudioALSAStreamManager::getInstance()->updateSpeechNVRAMParam((int)AUDIO_TYPE_SPEECH);
        } else if (strcmp(audioTypeName, audioTypeNameList[AUDIO_TYPE_SPEECH_DMNR]) == 0) {
            //"SpeechDMNR"
            AudioALSAStreamManager::getInstance()->updateSpeechNVRAMParam((int)AUDIO_TYPE_SPEECH_DMNR);
        } else if (strcmp(audioTypeName, audioTypeNameList[AUDIO_TYPE_SPEECH_GENERAL]) == 0) {
            //"SpeechGeneral"
            AudioALSAStreamManager::getInstance()->updateSpeechNVRAMParam((int)AUDIO_TYPE_SPEECH_GENERAL);
        }



    }
}
#endif

void callbackPhoneCallReopen(int audioEventType, void *caller, void *arg) {
    ALOGD("%s(), audioEventType = %d, caller(%p), arg(%p)",
          __FUNCTION__, audioEventType, caller, arg);
    AudioALSAStreamManager *streamManager = NULL;
    streamManager = static_cast<AudioALSAStreamManager *>(caller);
    if (streamManager == NULL) {
        ALOGE("%s(), streamManager is NULL!!", __FUNCTION__);
    } else {
        streamManager->phoneCallReopen();
    }
}

bool speakerStatusChangeCb(const DeviceStatus status) {
    ALOGV("%s(), status = %d", __FUNCTION__, status);
    bool ret = true;
#ifdef MTK_VOW_BARGE_IN_SUPPORT
    if (AudioALSAStreamManager::getInstance()->getVoiceWakeUpNeedOn() == true) {
        bool isSpeakerPlaying = (status == DEVICE_STATUS_ON) ? true : false;
        ret = AudioALSAVoiceWakeUpController::getInstance()->updateSpeakerPlaybackStatus(isSpeakerPlaying);
    }
#endif
    return ret;
}


/*==============================================================================
 *                     Constructor / Destructor / Init / Deinit
 *============================================================================*/

AudioALSAStreamManager::AudioALSAStreamManager() :
    mStreamOutIndex(0),
    mStreamInIndex(0),
    mPlaybackHandlerIndex(0),
    mCaptureHandlerIndex(0),
    mSpeechPhoneCallController(AudioALSASpeechPhoneCallController::getInstance()),
    mPhoneCallSpeechOpen(false),
    mFMController(AudioALSAFMController::getInstance()),
    mAudioALSAVolumeController(AudioVolumeFactory::CreateAudioVolumeController()),
    mSpeechDriverFactory(SpeechDriverFactory::GetInstance()),
    mMicMute(false),
    mAudioMode(AUDIO_MODE_NORMAL),
    mLoopbackEnable(false),
    mHdmiEnable(false),
    mFilterManagerNumber(0),
    mBesLoudnessStatus(false),
    mBesLoudnessControlCallback(NULL),
    mAudioSpeechEnhanceInfoInstance(AudioSpeechEnhanceInfo::getInstance()),
    mHeadsetChange(false),
    mAudioALSAVoiceWakeUpController(AudioALSAVoiceWakeUpController::getInstance()),
    mVoiceWakeUpNeedOn(false),
    mForceDisableVoiceWakeUpForSetMode(false),
    mBypassPostProcessDL(false),
    mBGSDlGain(0xFF),
    mBGSUlGain(0),
    mBypassDualMICProcessUL(false),
    mBtHeadsetName(NULL),
    mVolumeIndex(0) {
    ALOGD("%s()", __FUNCTION__);

    mStreamOutVector.clear();
    mStreamInVector.clear();

    mPlaybackHandlerVector.clear();
    mCaptureHandlerVector.clear();

    mFilterManagerVector.clear();

    // resume voice wake up need on
    char property_value[PROPERTY_VALUE_MAX];
    property_get(PROPERTY_KEY_VOICE_WAKE_UP_NEED_ON, property_value, "0"); //"0": default off
    const bool bVoiceWakeUpNeedOn = (property_value[0] == '0') ? false : true;
    //the default on setting will control by framework due to init model need be set first by framework. But still need to handle mediaserver died case
    if (bVoiceWakeUpNeedOn == true) {
        if (mAudioALSAVoiceWakeUpController->getVoiceWakeUpStateFromKernel()) {
            setVoiceWakeUpNeedOn(true);
        }
    }
    mAudioCustParamClient = NULL;
    mAudioCustParamClient = AudioCustParamClient::GetInstance();

#ifdef MTK_BESLOUDNESS_SUPPORT
    property_get(PROPERTY_KEY_FIRSTBOOT_STATE, property_value, "1"); //"1": first boot, "0": not first boot
    int firstboot = atoi(property_value);
     if (firstboot == 1) {
        property_set(PROPERTY_KEY_FIRSTBOOT_STATE, "0");
        property_set(PROPERTY_KEY_BESLOUDNESS_SWITCH_STATE, BESLOUDNESS_SWITCH_DEFAULT_STATE ? "1":"0");
    }
    property_get(PROPERTY_KEY_BESLOUDNESS_SWITCH_STATE, property_value, BESLOUDNESS_SWITCH_DEFAULT_STATE ? "1":"0");
    int besloudness_enable = atoi(property_value);
    if (besloudness_enable) {
        mBesLoudnessStatus = true;
    } else {
        mBesLoudnessStatus = false;
    }
    ALOGD("firstboot %d mBesLoudnessStatus %d", firstboot, mBesLoudnessStatus);
#else
    mBesLoudnessStatus = false;
    ALOGD("%s(), mBesLoudnessStatus [%d] (Always) \n", __FUNCTION__, mBesLoudnessStatus);
#endif

    if (getSWVersion() == true) {
        SpeechEnhancementController::GetInstance()->SetSMNROn();
        mBypassDualMICProcessUL = true;
    }

#if defined(MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT)
    AppHandle *mAppHandle;
    AppOps *appOps = appOpsGetInstance();
    if (appOps == NULL) {
        ALOGE("%s(), Error: AppOps == NULL", __FUNCTION__);
        ASSERT(0);
        return;
    }

    /* Init AppHandle */
    ALOGD("%s() appHandleGetInstance", __FUNCTION__);
    mAppHandle = appOps->appHandleGetInstance();
    ALOGD("%s() appHandleRegXmlChangedCb", __FUNCTION__);

    /* XML changed callback process */
    appOps->appHandleRegXmlChangedCb(mAppHandle, CallbackAudioXmlChanged);
#endif
    AudioEventThreadManager::getInstance()->registerCallback(AUDIO_EVENT_PHONECALL_REOPEN,
                                                             callbackPhoneCallReopen, this);

    mBluetoothAudioOffloadHostIf = NULL;
    mBluetoothAudioOffloadSession = 0;
    mA2dpSuspend = -1;
    mA2dpPlaybackPaused = false;
}


AudioALSAStreamManager::~AudioALSAStreamManager() {
    ALOGD("%s()", __FUNCTION__);
    if (mBtHeadsetName) {
        free((void *)mBtHeadsetName);
        mBtHeadsetName = NULL;
    }
    mStreamManager = NULL;
    AudioEventThreadManager::getInstance()->unregisterCallback(AUDIO_EVENT_PHONECALL_REOPEN);
}


/*==============================================================================
 *                     Implementations
 *============================================================================*/

AudioMTKStreamOutInterface *AudioALSAStreamManager::openOutputStream(
    uint32_t devices,
    int *format,
    uint32_t *channels,
    uint32_t *sampleRate,
    status_t *status,
    uint32_t output_flag) {
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(mStreamVectorLock);
    AL_AUTOLOCK(mLock);

    if (format == NULL || channels == NULL || sampleRate == NULL || status == NULL) {
        ALOGE("%s(), NULL pointer!! format = %p, channels = %p, sampleRate = %p, status = %p",
              __FUNCTION__, format, channels, sampleRate, status);
        if (status != NULL) { *status = INVALID_OPERATION; }
        return NULL;
    }

    ALOGD("%s(), devices = 0x%x, format = 0x%x, channels = 0x%x, sampleRate = %d, status = 0x%x",
          __FUNCTION__, devices, *format, *channels, *sampleRate, *status);

    // stream out flags
#if 1 // TODO(Harvey): why.........
    mStreamOutIndex = (uint32_t)(*status);
#endif
    //const uint32_t flags = 0; //(uint32_t)(*status);

    // create stream out
    AudioALSAStreamOut *pAudioALSAStreamOut = new AudioALSAStreamOut();
    pAudioALSAStreamOut->set(devices, format, channels, sampleRate, status, output_flag);
    if (*status != NO_ERROR) {
        ALOGE("-%s(), set fail, return NULL", __FUNCTION__);
        delete pAudioALSAStreamOut;
        pAudioALSAStreamOut = NULL;
        return NULL;
    }

#ifdef MTK_HDMI_FORCE_AUDIO_CLK
    if (pAudioALSAStreamOut->getStreamOutType() == STREAM_OUT_HDMI_STEREO &&
        !isHdmiStreamOutExist()) {
        AudioALSAPlaybackHandlerHDMI::forceClkOn(pAudioALSAStreamOut->getStreamAttribute());
    }
#endif

    // save stream out object in vector
#if 0 // TODO(Harvey): why.........
    pAudioALSAStreamOut->setIdentity(mStreamOutIndex);
    mStreamOutVector.add(mStreamOutIndex, pAudioALSAStreamOut);
    mStreamOutIndex++;
#else
    pAudioALSAStreamOut->setIdentity(mStreamOutIndex);
    mStreamOutVector.add(mStreamOutIndex, pAudioALSAStreamOut);
#endif

    // setup Filter for ACF/HCF/AudEnh/VibSPK // TODO Check return status of pAudioALSAStreamOut->set
    AudioMTKFilterManager *pAudioFilterManagerHandler = new AudioMTKFilterManager(*sampleRate, popcount(*channels), *format, pAudioALSAStreamOut->bufferSize());
    if (pAudioFilterManagerHandler != NULL) {
        if (pAudioFilterManagerHandler->init() == NO_ERROR) {
            mFilterManagerVector.add(mStreamOutIndex, pAudioFilterManagerHandler);
        } else {
            delete pAudioFilterManagerHandler;
        }
    }
    //mFilterManagerNumber++;

    ALOGD("-%s(), out = %p, status = 0x%x, mStreamOutVector.size() = %zu",
          __FUNCTION__, pAudioALSAStreamOut, *status, mStreamOutVector.size());


    return pAudioALSAStreamOut;
}

void AudioALSAStreamManager::closeOutputStream(AudioMTKStreamOutInterface *out) {
    ALOGD("+%s(), out = %p, mStreamOutVector.size() = %zu", __FUNCTION__, out, mStreamOutVector.size());
    AL_AUTOLOCK(mStreamVectorLock);
    AL_AUTOLOCK(mLock);

    if (out == NULL) {
        ALOGE("%s(), Cannot close null output stream!! return", __FUNCTION__);
        return;
    }

    AudioALSAStreamOut *pAudioALSAStreamOut = static_cast<AudioALSAStreamOut *>(out);
    ASSERT(pAudioALSAStreamOut != 0);

#ifdef MTK_HDMI_FORCE_AUDIO_CLK
    uint32_t type = pAudioALSAStreamOut->getStreamOutType();
#endif

    uint32_t dFltMngindex = mFilterManagerVector.indexOfKey(pAudioALSAStreamOut->getIdentity());

    if (dFltMngindex < mFilterManagerVector.size()) {
        AudioMTKFilterManager *pAudioFilterManagerHandler = static_cast<AudioMTKFilterManager *>(mFilterManagerVector[dFltMngindex]);
        ALOGD("%s, remove mFilterManagerVector Success [%u]/[%zu] [%u], pAudioFilterManagerHandler=%p",
              __FUNCTION__, dFltMngindex, mFilterManagerVector.size(), pAudioALSAStreamOut->getIdentity(), pAudioFilterManagerHandler);
        ASSERT(pAudioFilterManagerHandler != 0);
        mFilterManagerVector.removeItem(pAudioALSAStreamOut->getIdentity());
        delete pAudioFilterManagerHandler;
    } else {
        ALOGD("%s, Remove mFilterManagerVector Error [%u]/[%zu]", __FUNCTION__, dFltMngindex, mFilterManagerVector.size());
    }

    mStreamOutVector.removeItem(pAudioALSAStreamOut->getIdentity());
    delete pAudioALSAStreamOut;

#ifdef MTK_HDMI_FORCE_AUDIO_CLK
    if ((type == STREAM_OUT_HDMI_STEREO ||
         type == STREAM_OUT_HDMI_MULTI_CHANNEL) &&
        !isHdmiStreamOutExist()) {
        AudioALSAPlaybackHandlerHDMI::forceClkOff();
    }
#endif

    ALOGD("-%s(), mStreamOutVector.size() = %zu", __FUNCTION__, mStreamOutVector.size());
}


AudioMTKStreamInInterface *AudioALSAStreamManager::openInputStream(
    uint32_t devices,
    int *format,
    uint32_t *channels,
    uint32_t *sampleRate,
    status_t *status,
    audio_in_acoustics_t acoustics,
    uint32_t input_flag) {
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(mStreamVectorLock);
    AL_AUTOLOCK(mLock);

    if (format == NULL || channels == NULL || sampleRate == NULL || status == NULL) {
        ALOGE("%s(), NULL pointer!! format = %p, channels = %p, sampleRate = %p, status = %p",
              __FUNCTION__, format, channels, sampleRate, status);
        if (status != NULL) { *status = INVALID_OPERATION; }
        return NULL;
    }

    ALOGD("%s(), devices = 0x%x, format = 0x%x, channels = 0x%x, sampleRate = %d, status = %d, acoustics = 0x%x",
          __FUNCTION__, devices, *format, *channels, *sampleRate, *status, acoustics);
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
    if (AudioHfpController::getInstance()->getHfpEnable()
#ifdef MTK_SPEAKER_MONITOR_SUPPORT
        && devices != AUDIO_DEVICE_IN_SPK_FEED
#endif
        ) {
        ALOGW("%s reject input stream while hfp client is running", __FUNCTION__);
        if (status) *status = INVALID_OPERATION;
        return NULL;
    }
#endif

#if 1 // TODO(Harvey): why.........
    mStreamInIndex = (uint32_t)(*status);
#endif

    // create stream in
    AudioALSAStreamIn *pAudioALSAStreamIn = new AudioALSAStreamIn();
#ifdef UPLINK_LOW_LATENCY
    pAudioALSAStreamIn->set(devices, format, channels, sampleRate, status, acoustics, input_flag);
#else
    pAudioALSAStreamIn->set(devices, format, channels, sampleRate, status, acoustics);
#endif
    if (*status != NO_ERROR) {
        ALOGE("-%s(), set fail, return NULL", __FUNCTION__);
        delete pAudioALSAStreamIn;
        pAudioALSAStreamIn = NULL;
        return NULL;
    }

    // save stream in object in vector
#if 0 // TODO(Harvey): why.........
    pAudioALSAStreamIn->setIdentity(mStreamInIndex);
    mStreamInVector.add(mStreamInIndex, pAudioALSAStreamIn);
    mStreamInIndex++;
#else
    pAudioALSAStreamIn->setIdentity(mStreamInIndex);
    mStreamInVector.add(mStreamInIndex, pAudioALSAStreamIn);
#endif

    // when first stream in is ready to open
    if (mStreamInVector.size() == 1) {
        // make sure voice wake up is closed before any capture stream start (only 1st open need to check)
        if (mAudioALSAVoiceWakeUpController->getVoiceWakeUpEnable() == true) {
            ALOGD("%s(), force close voice wake up", __FUNCTION__);
            mAudioALSAVoiceWakeUpController->setVoiceWakeUpEnable(false);
        }
    }

    ALOGD("-%s(), in = %p, status = 0x%x, mStreamInVector.size() = %zu",
          __FUNCTION__, pAudioALSAStreamIn, *status, mStreamInVector.size());
    return pAudioALSAStreamIn;
}


void AudioALSAStreamManager::closeInputStream(AudioMTKStreamInInterface *in) {
    ALOGD("+%s(), in = %p", __FUNCTION__, in);
    AL_AUTOLOCK(mStreamVectorLock);
    AL_AUTOLOCK(mLock);

    if (in == NULL) {
        ALOGE("%s(), Cannot close null input stream!! return", __FUNCTION__);
        return;
    }

    AudioALSAStreamIn *pAudioALSAStreamIn = static_cast<AudioALSAStreamIn *>(in);
    ASSERT(pAudioALSAStreamIn != 0);

    mStreamInVector.removeItem(pAudioALSAStreamIn->getIdentity());
    delete pAudioALSAStreamIn;


    // make sure voice wake up is resume when all capture stream stop if need
    if (mVoiceWakeUpNeedOn == true &&
        mStreamInVector.size() == 0 &&
        mForceDisableVoiceWakeUpForSetMode == false) {
        ALOGD("%s(), resume voice wake up", __FUNCTION__);
        ASSERT(mAudioALSAVoiceWakeUpController->getVoiceWakeUpEnable() == false); // TODO(Harvey): double check, remove it later
        mAudioALSAVoiceWakeUpController->setVoiceWakeUpEnable(true);
    }

    ALOGD("-%s(), mStreamInVector.size() = %zu", __FUNCTION__, mStreamInVector.size());
}

AudioALSAPlaybackHandlerBase *AudioALSAStreamManager::createPlaybackHandler(
    stream_attribute_t *stream_attribute_source) {
    ALOGD("+%s(), mAudioMode = %d, output_devices = 0x%x", __FUNCTION__, mAudioMode, stream_attribute_source->output_devices);
    AL_AUTOLOCK(mAudioModeLock);

    // Init input stream attribute here
    stream_attribute_source->audio_mode = mAudioMode; // set mode to stream attribute for mic gain setting
    stream_attribute_source->mVoIPEnable = isModeInVoipCall();

    // just use what stream out is ask to use
    // stream_attribute_source->sample_rate = AudioALSASampleRateController::getInstance()->getPrimaryStreamOutSampleRate();

    //for DMNR tuning
    stream_attribute_source->BesRecord_Info.besrecord_dmnr_tuningEnable = mAudioSpeechEnhanceInfoInstance->IsAPDMNRTuningEnable();
    stream_attribute_source->bBypassPostProcessDL = mBypassPostProcessDL;
   

    //todo:: enable ACF if support
    if (stream_attribute_source->sample_rate > 48000) {
        stream_attribute_source->bBypassPostProcessDL = true;
    }

    // create
    AudioALSAPlaybackHandlerBase *pPlaybackHandler = NULL;
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
    if (AudioHfpController::getInstance()->getHfpEnable() &&
        audio_is_linear_pcm(stream_attribute_source->audio_format)) {
        pPlaybackHandler = new AudioALSAPlaybackHandlerHfpClient(stream_attribute_source);
        goto endOfCreate;
    }
#endif
    if (isModeInPhoneCall() == true) {
        pPlaybackHandler = new AudioALSAPlaybackHandlerVoice(stream_attribute_source);
    } else {
        switch (stream_attribute_source->output_devices) {
        case AUDIO_DEVICE_OUT_EARPIECE:
        case AUDIO_DEVICE_OUT_SPEAKER:
        case AUDIO_DEVICE_OUT_WIRED_HEADSET:
        case AUDIO_DEVICE_OUT_WIRED_HEADPHONE: {
#ifdef DOWNLINK_LOW_LATENCY
            if (AUDIO_OUTPUT_FLAG_FAST & stream_attribute_source->mAudioOutputFlags) {
                pPlaybackHandler = new AudioALSAPlaybackHandlerFast(stream_attribute_source);
            } else
#endif
                pPlaybackHandler = new AudioALSAPlaybackHandlerNormal(stream_attribute_source);
            break;
        }
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT: {
            if (WCNChipController::GetInstance()->IsBTMergeInterfaceSupported() == true) {
                pPlaybackHandler = new AudioALSAPlaybackHandlerBTSCO(stream_attribute_source);
            } else {
                pPlaybackHandler = new AudioALSAPlaybackHandlerBTCVSD(stream_attribute_source);
            }
            break;
        }
        case AUDIO_DEVICE_OUT_AUX_DIGITAL: {
            pPlaybackHandler = new AudioALSAPlaybackHandlerHDMI(stream_attribute_source);
            break;
        }
        case AUDIO_DEVICE_OUT_FM: {
            pPlaybackHandler = new AudioALSAPlaybackHandlerFMTransmitter(stream_attribute_source);
            break;
        }
        default: {
            ALOGE("%s(), No implement for such output_devices(0x%x)", __FUNCTION__, stream_attribute_source->output_devices);

#ifdef DOWNLINK_LOW_LATENCY
            if (AUDIO_OUTPUT_FLAG_FAST & stream_attribute_source->mAudioOutputFlags) {
                pPlaybackHandler = new AudioALSAPlaybackHandlerFast(stream_attribute_source);
            } else
#endif
                pPlaybackHandler = new AudioALSAPlaybackHandlerNormal(stream_attribute_source);
            break;
        }
        }

        uint32_t dFltMngindex = mFilterManagerVector.indexOfKey(stream_attribute_source->mStreamOutIndex);
        ALOGD("%s() ApplyFilter [%u]/[%zu] Device [0x%x]", __FUNCTION__, dFltMngindex, mFilterManagerVector.size(), stream_attribute_source->output_devices);

        if (dFltMngindex < mFilterManagerVector.size()) {
            pPlaybackHandler->setFilterMng(static_cast<AudioMTKFilterManager *>(mFilterManagerVector[dFltMngindex]));
            mFilterManagerVector[dFltMngindex]->setDevice(stream_attribute_source->output_devices);
        }
    }
endOfCreate:
    // save playback handler object in vector
    ASSERT(pPlaybackHandler != NULL);
    pPlaybackHandler->setIdentity(mPlaybackHandlerIndex);

    AL_LOCK(mPlaybackHandlerVectorLock);
    mPlaybackHandlerVector.add(mPlaybackHandlerIndex, pPlaybackHandler);
    AL_UNLOCK(mPlaybackHandlerVectorLock);

    mPlaybackHandlerIndex++;

    ALOGD("-%s(), mPlaybackHandlerVector.size() = %zu", __FUNCTION__, mPlaybackHandlerVector.size());
    return pPlaybackHandler;
}


status_t AudioALSAStreamManager::destroyPlaybackHandler(AudioALSAPlaybackHandlerBase *pPlaybackHandler) {
    ALOGD("+%s(), mode = %d, pPlaybackHandler = %p", __FUNCTION__, mAudioMode, pPlaybackHandler);
    //AL_AUTOLOCK(mLock); // TODO(Harvey): setparam -> routing -> close -> destroy deadlock

    status_t status = NO_ERROR;

    AL_LOCK(mPlaybackHandlerVectorLock);
    mPlaybackHandlerVector.removeItem(pPlaybackHandler->getIdentity());
    AL_UNLOCK(mPlaybackHandlerVectorLock);

    delete pPlaybackHandler;

    ALOGD("-%s(), mPlaybackHandlerVector.size() = %zu", __FUNCTION__, mPlaybackHandlerVector.size());
    return status;
}


AudioALSACaptureHandlerBase *AudioALSAStreamManager::createCaptureHandler(
    stream_attribute_t *stream_attribute_target) {
    ALOGD("+%s(), mAudioMode = %d, input_source = %d, input_device = 0x%x, mBypassDualMICProcessUL=%d",
          __FUNCTION__, mAudioMode, stream_attribute_target->input_source, stream_attribute_target->input_device, mBypassDualMICProcessUL);
    //AL_AUTOLOCK(mLock);
    status_t retval = AL_LOCK_MS(mLock, 1000);
    if (retval != NO_ERROR) {
        ALOGD("mLock timeout : 1s , return NULL");
        return NULL;
    }

    // use primary stream out device
    const audio_devices_t current_output_devices = (mStreamOutVector.size() > 0)
                                                   ? mStreamOutVector[0]->getStreamAttribute()->output_devices
                                                   : AUDIO_DEVICE_NONE;


    // Init input stream attribute here
    stream_attribute_target->audio_mode = mAudioMode; // set mode to stream attribute for mic gain setting
    stream_attribute_target->output_devices = current_output_devices; // set output devices to stream attribute for mic gain setting and BesRecord parameter
    stream_attribute_target->micmute = mMicMute;

    // BesRecordInfo
    stream_attribute_target->BesRecord_Info.besrecord_enable = false; // default set besrecord off
    stream_attribute_target->BesRecord_Info.besrecord_bypass_dualmicprocess = mBypassDualMICProcessUL;

    // create
    AudioALSACaptureHandlerBase *pCaptureHandler = NULL;
#if defined(MTK_SPEAKER_MONITOR_SUPPORT)
    if (stream_attribute_target->input_device == AUDIO_DEVICE_IN_SPK_FEED) {
        pCaptureHandler = new AudioALSACaptureHandlerSpkFeed(stream_attribute_target);
        goto endOfCreate;
    }
#endif
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
    if (AudioHfpController::getInstance()->getHfpEnable())
    {
        pCaptureHandler = new AudioALSACaptureHandlerNull(stream_attribute_target);
        goto endOfCreate;
    }
#endif
    {
        if (stream_attribute_target->input_source == AUDIO_SOURCE_FM_TUNER) {
            if (isEchoRefUsing() == true) {
                ALOGD("%s(), not support FM record in VoIP mode, return NULL", __FUNCTION__);
                AL_UNLOCK(mLock);
                return NULL;
            }

            pCaptureHandler = new AudioALSACaptureHandlerFMRadio(stream_attribute_target);
        } else if (isModeInPhoneCall() == true) {
            if (SpeechDriverFactory::GetInstance()->GetSpeechDriver()->GetApSideModemStatus(SPEECH_STATUS_MASK) == false) {
                ALOGD("Can not open PhoneCall Record now !! Because speech driver is not ready, return NULL");
                AL_UNLOCK(mLock);
                return NULL;
            }

            pCaptureHandler = new AudioALSACaptureHandlerVoice(stream_attribute_target);
        } else if ((isModeInVoipCall() == true) || (stream_attribute_target->NativePreprocess_Info.PreProcessEffect_AECOn == true)
                   || (stream_attribute_target->input_source == AUDIO_SOURCE_VOICE_COMMUNICATION)
                   || (stream_attribute_target->input_source == AUDIO_SOURCE_CUSTOMIZATION1) //MagiASR enable AEC
                   || (stream_attribute_target->input_source == AUDIO_SOURCE_CUSTOMIZATION2)) { //Normal REC with AEC
            stream_attribute_target->BesRecord_Info.besrecord_enable = EnableBesRecord();
            if (mStreamInVector.size() > 1) {
                for (size_t i = 0; i < mStreamInVector.size(); i++) {
                    if (mStreamInVector[i]->getStreamAttribute()->input_source == AUDIO_SOURCE_FM_TUNER) {
                        mStreamInVector[i]->standby();
                    }
                }
            }
            if (isModeInVoipCall() == true || (stream_attribute_target->input_source == AUDIO_SOURCE_VOICE_COMMUNICATION)) {
                stream_attribute_target->BesRecord_Info.besrecord_voip_enable = true;
                stream_attribute_target->mVoIPEnable = true;
#if 0
                if (current_output_devices == AUDIO_DEVICE_OUT_SPEAKER) {
                    if (stream_attribute_target->input_device == AUDIO_DEVICE_IN_BUILTIN_MIC) {
                        if (USE_REFMIC_IN_LOUDSPK == 1) {
                            ALOGD("%s(), routing changed!! input_device: 0x%x => 0x%x",
                                  __FUNCTION__, stream_attribute_target->input_device, AUDIO_DEVICE_IN_BACK_MIC);
                            stream_attribute_target->input_device = AUDIO_DEVICE_IN_BACK_MIC;
                        }
                    }
                }
#endif
            }

            switch (stream_attribute_target->input_device) {
            case AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET: {
#if 1   //not enable BT AEC
                ALOGD("%s(), BT still use nonAEC handle for temp", __FUNCTION__);
                pCaptureHandler = new AudioALSACaptureHandlerBT(stream_attribute_target);
#else   //enable BT AEC
                ALOGD("%s(), BT use AEC handle", __FUNCTION__);
                pCaptureHandler = new AudioALSACaptureHandlerAEC(stream_attribute_target);
#endif
                break;
            }
            default: {
                pCaptureHandler = new AudioALSACaptureHandlerAEC(stream_attribute_target);
                break;
            }
            }
        } else {
            //enable BesRecord if not these input sources
            if ((stream_attribute_target->input_source != AUDIO_SOURCE_VOICE_UNLOCK) &&
                (stream_attribute_target->input_source != AUDIO_SOURCE_FM_TUNER) && // TODO(Harvey, Yu-Hung): never go through here?
                (stream_attribute_target->input_source != AUDIO_SOURCE_UNPROCESSED)) {
                if ((stream_attribute_target->sample_rate > 48000) || (stream_attribute_target->audio_format != AUDIO_FORMAT_PCM_16_BIT)) { //no uplink preprocess for sample rate higher than 48k
                    stream_attribute_target->BesRecord_Info.besrecord_enable = false;
                } else {
                    stream_attribute_target->BesRecord_Info.besrecord_enable = EnableBesRecord();
                }
            }

            switch (stream_attribute_target->input_device) {
            case AUDIO_DEVICE_IN_BUILTIN_MIC:
            case AUDIO_DEVICE_IN_BACK_MIC:
            case AUDIO_DEVICE_IN_WIRED_HEADSET: {
                pCaptureHandler = new AudioALSACaptureHandlerNormal(stream_attribute_target);
                break;
            }
            case AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET: {
                pCaptureHandler = new AudioALSACaptureHandlerBT(stream_attribute_target);
                break;
            }
            default: {
                ALOGE("%s(), No implement for such input_device(0x%x)", __FUNCTION__, stream_attribute_target->input_device);
                pCaptureHandler = new AudioALSACaptureHandlerNormal(stream_attribute_target);
                break;
            }
            }
        }
    }
endOfCreate:
    // save capture handler object in vector
    ASSERT(pCaptureHandler != NULL);
    pCaptureHandler->setIdentity(mCaptureHandlerIndex);
    mCaptureHandlerVector.add(mCaptureHandlerIndex, pCaptureHandler);
    mCaptureHandlerIndex++;
    AL_UNLOCK(mLock);
    ALOGD("-%s(), mCaptureHandlerVector.size() = %zu", __FUNCTION__, mCaptureHandlerVector.size());
    return pCaptureHandler;
}


status_t AudioALSAStreamManager::destroyCaptureHandler(AudioALSACaptureHandlerBase *pCaptureHandler) {
    ALOGD("+%s(), mode = %d, pCaptureHandler = %p", __FUNCTION__, mAudioMode, pCaptureHandler);
    //AL_AUTOLOCK(mLock); // TODO(Harvey): setparam -> routing -> close -> destroy deadlock

    status_t status = NO_ERROR;

    mCaptureHandlerVector.removeItem(pCaptureHandler->getIdentity());
    delete pCaptureHandler;

    ALOGD("-%s(), mCaptureHandlerVector.size() = %zu", __FUNCTION__, mCaptureHandlerVector.size());
    return status;
}


status_t AudioALSAStreamManager::setVoiceVolume(float volume) {
    ALOGD("%s(), volume = %f", __FUNCTION__, volume);

    if (volume < 0.0 || volume > 1.0) {
        ALOGE("-%s(), strange volume level %f, something wrong!!", __FUNCTION__, volume);
        return BAD_VALUE;
    }

    AL_AUTOLOCK(mLock);

    if (mAudioALSAVolumeController) {
        // use primary stream out device
        const audio_devices_t current_output_devices = (mStreamOutVector.size() > 0)
                                                       ? mStreamOutVector[0]->getStreamAttribute()->output_devices
                                                       : AUDIO_DEVICE_NONE;
        mAudioALSAVolumeController->setVoiceVolume(volume, mAudioMode , current_output_devices);
    }

    return NO_ERROR;
}

#ifdef MTK_AUDIO_GAIN_TABLE
status_t AudioALSAStreamManager::setAnalogVolume(int stream, int device, int index, bool force_incall) {
    ALOGD("%s(),stream=%d, device=%d, index=%d", __FUNCTION__, stream, device, index);

    AL_AUTOLOCK(mLock);

    if (mAudioALSAVolumeController) {
        if (force_incall == 0) {
            mAudioALSAVolumeController->setAnalogVolume(stream, device, index, mAudioMode);
        } else {
            mAudioALSAVolumeController->setAnalogVolume(stream, device, index, AUDIO_MODE_IN_CALL);
        }
    }

    return NO_ERROR;
}

int AudioALSAStreamManager::SetCaptureGain(void) {
    const stream_attribute_t *mStreamAttributeTarget;
    uint32_t i;
    ALOGD("%s(), mStreamInVector.size() = %zu", __FUNCTION__, mStreamInVector.size());

    for (i = 0; i < mStreamInVector.size(); i++) {
        //if(mStreamInVector[i]->getStreamAttribute()->output_devices == output_devices)
        {
            mStreamAttributeTarget = mStreamInVector[i]->getStreamAttribute();
            if (mAudioALSAVolumeController != NULL) {
                mAudioALSAVolumeController->SetCaptureGain(mAudioMode, mStreamAttributeTarget->input_source, mStreamAttributeTarget->input_device, mStreamAttributeTarget->output_devices);
            }
        }
    }
    return 0;
}

#endif
float AudioALSAStreamManager::getMasterVolume(void) {
    return mAudioALSAVolumeController->getMasterVolume();
}

status_t AudioALSAStreamManager::setMasterVolume(float volume, uint32_t iohandle) {
    ALOGD("%s(), volume = %f", __FUNCTION__, volume);

    if (volume < 0.0 || volume > 1.0) {
        ALOGE("-%s(), strange volume level %f, something wrong!!", __FUNCTION__, volume);
        return BAD_VALUE;
    }

    AL_AUTOLOCK(mLock);
    if (mAudioALSAVolumeController) {
        audio_devices_t current_output_devices;
        uint32_t index = mStreamOutVector.indexOfKey(iohandle);
        if (index < mStreamOutVector.size()) {
            current_output_devices = mStreamOutVector[index]->getStreamAttribute()->output_devices;
        } else {
            current_output_devices  = (mStreamOutVector.size() > 0)
                                      ? mStreamOutVector[0]->getStreamAttribute()->output_devices
                                      : AUDIO_DEVICE_NONE;
        }
        mAudioALSAVolumeController->setMasterVolume(volume, mAudioMode , current_output_devices);
    }

    return NO_ERROR;
}


status_t AudioALSAStreamManager::setFmVolume(float volume) {
    ALOGV("+%s(), volume = %f", __FUNCTION__, volume);

    if (volume < 0.0 || volume > 1.0) {
        ALOGE("-%s(), strange volume level %f, something wrong!!", __FUNCTION__, volume);
        return BAD_VALUE;
    }

    AL_AUTOLOCK(mLock);
    mFMController->setFmVolume(volume);

    return NO_ERROR;
}

status_t AudioALSAStreamManager::setMicMute(bool state) {
    ALOGD("%s(), mMicMute: %d => %d", __FUNCTION__, mMicMute, state);
    AL_AUTOLOCK(mLock);
    if (isModeInPhoneCall() == true) {
        AudioALSASpeechPhoneCallController::getInstance()->setMicMute(state);
    } else {
        SetInputMute(state);
    }
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
    AudioHfpController::getInstance()->setHfpMicMute(state);
#endif
    mMicMute = state;
    return NO_ERROR;
}


bool AudioALSAStreamManager::getMicMute() {
    ALOGD("%s(), mMicMute = %d", __FUNCTION__, mMicMute);
    AL_AUTOLOCK(mLock);

    return mMicMute;
}

void AudioALSAStreamManager::SetInputMute(bool bEnable) {
    ALOGD("+%s(), %d", __FUNCTION__, bEnable);
    if (mStreamInVector.size() > 0) {
        for (size_t i = 0; i < mStreamInVector.size(); i++) { // TODO(Harvey): Mic+FM !?
            mStreamInVector[i]->SetInputMute(bEnable);
        }
    }
    ALOGD("-%s()", __FUNCTION__);
}

status_t AudioALSAStreamManager::setVtNeedOn(const bool vt_on) {
    ALOGD("%s(), setVtNeedOn: %d", __FUNCTION__, vt_on);
    AudioALSASpeechPhoneCallController::getInstance()->setVtNeedOn(vt_on);

    return NO_ERROR;
}

status_t AudioALSAStreamManager::setBGSDlMute(const bool mute_on) {
    if (mute_on) {
        mBGSDlGain = 0;
    } else {
        mBGSDlGain = 0xFF;
    }
    ALOGD("%s(), mute_on: %d, mBGSDlGain=0x%x", __FUNCTION__, mute_on, mBGSDlGain);

    return NO_ERROR;
}

status_t AudioALSAStreamManager::setBGSUlMute(const bool mute_on) {
    if (mute_on) {
        mBGSUlGain = 0;
    } else {
        mBGSUlGain = 0xFF;
    }
    ALOGD("%s(), mute_on: %d, mBGSUlGain=0x%x", __FUNCTION__, mute_on, mBGSUlGain);

    return NO_ERROR;
}

status_t AudioALSAStreamManager::setMode(audio_mode_t new_mode) {
    ALOGD("+%s(), mAudioMode: %d => %d", __FUNCTION__, mAudioMode, new_mode);
    bool isNeedResumeAllStreams = false;
    AL_AUTOLOCK(mStreamVectorLock);

    // check value
    if ((new_mode < AUDIO_MODE_NORMAL) || (new_mode > AUDIO_MODE_MAX)) {
        return BAD_VALUE;
    }

    // TODO(Harvey): modem 1 / modem 2 check

    if (new_mode == mAudioMode) {
        ALOGW("-%s(), mAudioMode: %d == %d, return", __FUNCTION__, mAudioMode, new_mode);
        return NO_ERROR;
    }

    // make sure voice wake up is closed before leaving normal mode
    if (new_mode != AUDIO_MODE_NORMAL) {
        mForceDisableVoiceWakeUpForSetMode = true;
        if (mAudioALSAVoiceWakeUpController->getVoiceWakeUpEnable() == true) {
            ALOGD("%s(), force close voice wake up", __FUNCTION__);
            mAudioALSAVoiceWakeUpController->setVoiceWakeUpEnable(false);
        }
    }

    // suspend and standby if needed
    if (isModeInPhoneCall(new_mode) == true || isModeInPhoneCall(mAudioMode) == true ||
        isModeInVoipCall(new_mode)  == true || isModeInVoipCall(mAudioMode) == true) {
        setAllStreamsSuspend(true, true);
        standbyAllStreams(true);
        //Need to reset MicInverse when phone/VOIP call
        AudioALSAHardwareResourceManager::getInstance()->setMicInverse(0);
    }

    // close FM when mode swiching
    if (mFMController->getFmEnable() &&
        (isModeInPhoneCall(new_mode) || isModeInPhoneCall(mAudioMode))) {
        setFmEnable(false);
    }

    // TODO(Harvey): // close mATV when mode swiching

    {
        AL_AUTOLOCK(mLock);
        AL_AUTOLOCK(mAudioModeLock);

        // use primary stream out device // TODO(Harvey): add a function? get from hardware?
#ifdef FORCE_ROUTING_RECEIVER
        const audio_devices_t current_output_devices = AUDIO_DEVICE_OUT_EARPIECE;
#else
        const audio_devices_t current_output_devices = (mStreamOutVector.size() > 0)
                                                       ? mStreamOutVector[0]->getStreamAttribute()->output_devices
                                                       : AUDIO_DEVICE_NONE;
#endif

        // close previous call if needed
        if (isModeInPhoneCall(mAudioMode) == true) {
            mSpeechPhoneCallController->close();
        }
        // open next call if needed
        if (isModeInPhoneCall(new_mode) == true
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
            && !AudioHfpController::getInstance()->getHfpEnable()
#endif
        )
        {
            mPhoneCallSpeechOpen = true;
        }


        // resume if needed
        if (isModeInPhoneCall(new_mode) == true || isModeInPhoneCall(mAudioMode) == true ||
            isModeInVoipCall(new_mode)  == true || isModeInVoipCall(mAudioMode) == true) {
            isNeedResumeAllStreams = true;
        }

        mAudioMode = new_mode;

        if (isModeInPhoneCall() == true
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
            && !AudioHfpController::getInstance()->getHfpEnable()
#endif
        )
        {
            mAudioALSAVolumeController->setVoiceVolume(mAudioALSAVolumeController->getVoiceVolume(), mAudioMode , current_output_devices);
        }
        else
        {
            mAudioALSAVolumeController->setMasterVolume(mAudioALSAVolumeController->getMasterVolume(), mAudioMode , current_output_devices);
        }

        // make sure voice wake up is resume when go back to normal mode
        if (mAudioMode == AUDIO_MODE_NORMAL) {
            mForceDisableVoiceWakeUpForSetMode = false;
            if (mVoiceWakeUpNeedOn == true &&
                mStreamInVector.size() == 0) {
                ALOGD("%s(), resume voice wake up", __FUNCTION__);
                ASSERT(mAudioALSAVoiceWakeUpController->getVoiceWakeUpEnable() == false); // TODO(Harvey): double check, remove it later
                mAudioALSAVoiceWakeUpController->setVoiceWakeUpEnable(true);
            }
        }
    }

    if (isNeedResumeAllStreams == true) {
        setAllStreamsSuspend(false, true);
    }
    ALOGD("-%s(), mAudioMode = %d, mPhoneCallSpeechOpen = %d", __FUNCTION__, mAudioMode, mPhoneCallSpeechOpen);
    return NO_ERROR;
}

audio_mode_t AudioALSAStreamManager::getMode() {
    AL_AUTOLOCK(mAudioModeLock);
    ALOGD("%s(), mAudioMode = %d", __FUNCTION__, mAudioMode);

    return mAudioMode;
}

bool AudioALSAStreamManager::isPhoneCallOpen() {
    return mSpeechPhoneCallController->getCallStatus();
}
status_t AudioALSAStreamManager::DeviceNoneUpdate() {
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(mLock);
    status_t status = NO_ERROR;

    // update the output device info for voice wakeup (even when "routing=0")
    mAudioALSAVoiceWakeUpController->updateDeviceInfoForVoiceWakeUp();
    ALOGD("-%s()", __FUNCTION__);

    return status;
}

status_t AudioALSAStreamManager::routingOutputDevice(AudioALSAStreamOut *pAudioALSAStreamOut, const audio_devices_t current_output_devices, audio_devices_t output_devices) {
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(mLock);

    status_t status = NO_ERROR;
    audio_devices_t streamOutDevice = pAudioALSAStreamOut->getStreamAttribute()->output_devices;

    ALOGD("%s(), output_devices: 0x%x => 0x%x, mPhoneCallSpeechOpen = %d", __FUNCTION__, streamOutDevice, output_devices, mPhoneCallSpeechOpen);

    // TODO(Harvey, Hochi): Sometimes AUDIO_DEVICE_NONE might need to transferred to other device?

    // update the output device info for voice wakeup (even when "routing=0")
    mAudioALSAVoiceWakeUpController->updateDeviceInfoForVoiceWakeUp();

    // update if headset change
    mHeadsetChange = CheckHeadsetChange(streamOutDevice, output_devices);
    if ((mHeadsetChange == true) && (mFMController->getFmEnable() ==  false)) {
        AudioALSAHardwareResourceManager::getInstance()->setHeadPhoneChange(mHeadsetChange);
    }
    ALOGD("mHeadsetChange = %d", mHeadsetChange);

    // When FM + (WFD, A2DP, SCO(44.1K -> 8/16K), ...), Policy will routing to AUDIO_DEVICE_NONE
    // Hence, use other device like AUDIO_DEVICE_OUT_REMOTE_SUBMIX instead to achieve FM routing.
    if (output_devices == AUDIO_DEVICE_NONE && mFMController->getFmEnable() == true) {
        ALOGD("%s(), Replace AUDIO_DEVICE_NONE with AUDIO_DEVICE_OUT_REMOTE_SUBMIX for AP-path FM routing", __FUNCTION__);
        output_devices = AUDIO_DEVICE_OUT_REMOTE_SUBMIX;
    }

    if (output_devices == AUDIO_DEVICE_NONE) {
        ALOGW("-%s(), output_devices == AUDIO_DEVICE_NONE(0x%x), return", __FUNCTION__, AUDIO_DEVICE_NONE);
        return NO_ERROR;
    } else if (output_devices == streamOutDevice) {
        if (isModeInPhoneCall() == true) {
            ALOGW("-%s(), output_devices == current_output_devices(0x%x), but phone call is enabled", __FUNCTION__, current_output_devices);
        } else {
            ALOGW("-%s(), output_devices == current_output_devices(0x%x), return", __FUNCTION__, streamOutDevice);
            return NO_ERROR;
        }
    }

    // do routing
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
    if (AudioHfpController::getInstance()->getHfpEnable())
    {
        for (size_t i = 0; i < mStreamOutVector.size(); i++)
        {
            if (mStreamOutVector[i]->getStreamAttribute()->output_devices == current_output_devices)
            {
                status = mStreamOutVector[i]->routing(output_devices);
                ASSERT(status == NO_ERROR);
            }
        }
    }
    else if (isModeInPhoneCall() == true)
#else
    if (isModeInPhoneCall())
#endif
    {
        if (mPhoneCallSpeechOpen == true) {
            setAllStreamsSuspend(true, true);
            standbyAllStreams(true);
            mSpeechPhoneCallController->open(
                mAudioMode,
                output_devices,
                mSpeechPhoneCallController->getInputDeviceForPhoneCall(current_output_devices));
        }
        mSpeechPhoneCallController->routing(
            output_devices,
            mSpeechPhoneCallController->getInputDeviceForPhoneCall(output_devices));

        if (mPhoneCallSpeechOpen == true) {
            mPhoneCallSpeechOpen = false;
            setAllStreamsSuspend(false, true);
        }
    }

    Vector<AudioALSAStreamOut *> streamOutToRoute;
    AudioALSAHardwareResourceManager *hwResMng = AudioALSAHardwareResourceManager::getInstance();
    bool toSharedOut = hwResMng->isSharedOutDevice(output_devices);

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        audio_devices_t curOutDevice = mStreamOutVector[i]->getStreamAttribute()->output_devices;
        bool curSharedOut = hwResMng->isSharedOutDevice(curOutDevice);

        // check if need routing
        if (curOutDevice != output_devices &&
            (pAudioALSAStreamOut == mStreamOutVector[i] ||   // route ourself
             (toSharedOut && curSharedOut))) { // route shared output device streamout
            // suspend streamout
            mStreamOutVector[i]->setSuspend(true);
            streamOutToRoute.add(mStreamOutVector[i]);
        }
    }

    for (size_t i = 0; i < streamOutToRoute.size(); i++) {
        // stream out routing
        status = streamOutToRoute[i]->routing(output_devices);
        ASSERT(status == NO_ERROR);
    }

    if (!isModeInPhoneCall() && mFMController->getFmEnable() == true  && (output_devices != streamOutDevice)) {
        if (popcount(streamOutDevice) != popcount(output_devices)) {
            mFMController->routing(streamOutDevice, output_devices);    //switch between SPK+HP and HP (ringtone)
        } else {
            mFMController->setFmEnable(false, output_devices, false, false, true);
        }
    }

    for (size_t i = 0; i < streamOutToRoute.size(); i++) {
        // resume streamout
        streamOutToRoute[i]->setSuspend(false);
    }

    if (!isModeInPhoneCall() && mStreamInVector.size() > 0) {
        // update the output device info for input stream (ex:for BesRecord parameters update or mic device change)
        ALOGD("%s(), mStreamInVector.size() = %zu", __FUNCTION__, mStreamInVector.size());
        for (size_t i = 0; i < mStreamInVector.size(); i++) {
            status = mStreamInVector[i]->updateOutputDeviceInfoForInputStream(output_devices);
            ASSERT(status == NO_ERROR);
        }
    }

    // volume control
    if (isModeInPhoneCall() == true
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
        && !AudioHfpController::getInstance()->getHfpEnable()
#endif
    )
    {
        mSpeechPhoneCallController->setMicMute(1);
        mAudioALSAVolumeController->setVoiceVolume(mAudioALSAVolumeController->getVoiceVolume(), mAudioMode , output_devices);
        mSpeechPhoneCallController->setMicMute(mMicMute);
    }
    else
    {
        mAudioALSAVolumeController->setMasterVolume(mAudioALSAVolumeController->getMasterVolume(), mAudioMode , output_devices);
    }

    ALOGD("-%s(), output_devices = 0x%x", __FUNCTION__, output_devices);
    return status;
}


status_t AudioALSAStreamManager::routingInputDevice(AudioALSAStreamIn *pAudioALSAStreamIn, const audio_devices_t current_input_device, audio_devices_t input_device) {
    ALOGD("+%s(), input_device: 0x%x => 0x%x", __FUNCTION__, current_input_device, input_device);
    AL_AUTOLOCK(mLock);

    status_t status = NO_ERROR;

    if (input_device == AUDIO_DEVICE_NONE) {
        ALOGW("-%s(), input_device == AUDIO_DEVICE_NONE(0x%x), return", __FUNCTION__, AUDIO_DEVICE_NONE);
        return NO_ERROR;
    } else if (input_device == current_input_device) {
        ALOGW("-%s(), input_device == current_input_device(0x%x), return", __FUNCTION__, current_input_device);
        return NO_ERROR;
    }


    if (isModeInPhoneCall() == true) {
        ALOGW("-%s(), not route during phone call, return", __FUNCTION__);
        return INVALID_OPERATION;
    } else if (mStreamInVector.size() > 0) {
        for (size_t i = 0; i < mStreamInVector.size(); i++) {
            if (mStreamInVector[i]->getStreamAttribute()->input_device == current_input_device) { // TODO(Harvey): or add group?
                if ((input_device == AUDIO_DEVICE_IN_FM_TUNER) || (current_input_device == AUDIO_DEVICE_IN_FM_TUNER)) {
                    if (pAudioALSAStreamIn == mStreamInVector[i]) {
                        status = mStreamInVector[i]->routing(input_device);
                        ASSERT(status == NO_ERROR);
                    }
                } else {
                    status = mStreamInVector[i]->routing(input_device);
                    ASSERT(status == NO_ERROR);
                }
            }
        }
    }
    return status;
}

// check if headset has changed
bool AudioALSAStreamManager::CheckHeadsetChange(const audio_devices_t current_output_devices, audio_devices_t output_device) {
    ALOGD("+%s(), current_output_devices = %d output_device = %d ", __FUNCTION__, current_output_devices, output_device);
    if (current_output_devices == output_device) {
        return false;
    }
    if (current_output_devices == AUDIO_DEVICE_NONE || output_device == AUDIO_DEVICE_NONE) {
        return true;
    }
    if (current_output_devices == AUDIO_DEVICE_OUT_WIRED_HEADSET || current_output_devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE
        || output_device == AUDIO_DEVICE_OUT_WIRED_HEADSET || output_device == AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
        return true;
    }
    return false;
}

status_t AudioALSAStreamManager::setFmEnable(const bool enable, bool bForceControl, bool bForce2DirectConn, audio_devices_t output_device) { // TODO(Harvey)
    ALOGD("+%s(), enable = %d", __FUNCTION__, enable);
    AL_AUTOLOCK(mLock);

    // Reject set fm enable during phone call mode
    if (isModeInPhoneCall(mAudioMode)) {
        ALOGW("-%s(), mAudioMode(%d) is in phone call mode, return.", __FUNCTION__, mAudioMode);
        return INVALID_OPERATION;
    }

    // use primary stream out device // TODO(Harvey): add a function? get from hardware?
    const audio_devices_t current_output_devices = output_device == AUDIO_DEVICE_NONE ? ((mStreamOutVector.size() > 0)
                                                                                         ? mStreamOutVector[0]->getStreamAttribute()->output_devices
                                                                                         : AUDIO_DEVICE_NONE) : output_device;

    mFMController->setFmEnable(enable, current_output_devices, bForceControl, bForce2DirectConn);

    ALOGD("-%s(), enable = %d", __FUNCTION__, enable);
    return NO_ERROR;
}

status_t AudioALSAStreamManager::setHdmiEnable(const bool enable) {
    ALOGD("+%s(), enable = %d", __FUNCTION__, enable);

    AL_AUTOLOCK(mLock);

    if (enable == mHdmiEnable) {
        return ALREADY_EXISTS;
    }

    mHdmiEnable = enable;

    if (enable == true) {
        int pcmIdx = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmHDMI);
        int cardIdx = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmHDMI);

        memset(&mLoopbackConfig, 0, sizeof(mLoopbackConfig));
        mLoopbackConfig.channels = 2;
        mLoopbackConfig.rate = 44100;
        mLoopbackConfig.period_size = 512;
        mLoopbackConfig.period_count = 4;
        mLoopbackConfig.format = PCM_FORMAT_S16_LE;
        mLoopbackConfig.start_threshold = 0;
        mLoopbackConfig.stop_threshold = 0;
        mLoopbackConfig.silence_threshold = 0;

        if (mHdmiPcm == NULL) {
            mHdmiPcm = pcm_open(cardIdx, pcmIdx, PCM_OUT, &mLoopbackConfig);
            ALOGD("pcm_open mHdmiPcm = %p", mHdmiPcm);
        }

        if (!mHdmiPcm || !pcm_is_ready(mHdmiPcm)) {
            ALOGD("Unable to open mHdmiPcm device %u (%s)", pcmIdx, pcm_get_error(mHdmiPcm));
        }

        pcm_prepare(mHdmiPcm);
    } else {
        ALOGD("pcm_close");

        if (mHdmiPcm != NULL) {
            pcm_close(mHdmiPcm);
            mHdmiPcm = NULL;
        }

        ALOGD("pcm_close done");
    }

    ALOGD("-%s(), enable = %d", __FUNCTION__, enable);
    return NO_ERROR;
}

status_t AudioALSAStreamManager::setLoopbackEnable(const bool enable) { // TODO(Harvey): tmp, add a class to do it
    ALOGD("%s(), enable = %d", __FUNCTION__, enable);
    return NO_ERROR;
}

bool AudioALSAStreamManager::getFmEnable() {
    AL_AUTOLOCK(mLock);
    return mFMController->getFmEnable();
}

status_t AudioALSAStreamManager::setAllOutputStreamsSuspend(const bool suspend_on, const bool setModeRequest __unused) {
    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        ASSERT(mStreamOutVector[i]->setSuspend(suspend_on) == NO_ERROR);
    }

    return NO_ERROR;
}


status_t AudioALSAStreamManager::setAllInputStreamsSuspend(const bool suspend_on, const bool setModeRequest) {
    ALOGD("%s()", __FUNCTION__);

    status_t status = NO_ERROR;

    AudioALSAStreamIn *pAudioALSAStreamIn = NULL;

    for (size_t i = 0; i < mStreamInVector.size(); i++) {
        pAudioALSAStreamIn = mStreamInVector[i];
        if (setModeRequest == true) {
            if (pAudioALSAStreamIn->isSupportConcurrencyInCall()) {
                ALOGD("%s(), mStreamInVector[%zu] support concurrency!!", __FUNCTION__, i);
                continue;
            }
        }
        status = pAudioALSAStreamIn->setSuspend(suspend_on);
        if (status != NO_ERROR) {
            ALOGE("%s(), mStreamInVector[%zu] setSuspend() fail!!", __FUNCTION__, i);
        }
    }

    return status;
}


status_t AudioALSAStreamManager::setAllStreamsSuspend(const bool suspend_on, const bool setModeRequest) {
    ALOGD("%s(), suspend_on = %d", __FUNCTION__, suspend_on);

    status_t status = NO_ERROR;

    status = setAllOutputStreamsSuspend(suspend_on, setModeRequest);
    status = setAllInputStreamsSuspend(suspend_on, setModeRequest);

    return status;
}


status_t AudioALSAStreamManager::standbyAllOutputStreams(const bool setModeRequest __unused) {
    ALOGD("%s()", __FUNCTION__);

    status_t status = NO_ERROR;

    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        status = pAudioALSAStreamOut->standby();
        if (status != NO_ERROR) {
            ALOGE("%s(), mStreamOutVector[%zu] standby() fail!!", __FUNCTION__, i);
        }
    }

    return status;
}


status_t AudioALSAStreamManager::standbyAllInputStreams(const bool setModeRequest) {
    ALOGD("%s()", __FUNCTION__);

    status_t status = NO_ERROR;

    AudioALSAStreamIn *pAudioALSAStreamIn = NULL;

    for (size_t i = 0; i < mStreamInVector.size(); i++) {
        pAudioALSAStreamIn = mStreamInVector[i];
        if (setModeRequest == true) {
            if (pAudioALSAStreamIn->isSupportConcurrencyInCall()) {
                ALOGD("%s(), mStreamInVector[%zu] support concurrency!!", __FUNCTION__, i);
                continue;
            }
        }
        status = pAudioALSAStreamIn->standby();
        if (status != NO_ERROR) {
            ALOGE("%s(), mStreamInVector[%zu] standby() fail!!", __FUNCTION__, i);
        }
    }

    return status;
}


status_t AudioALSAStreamManager::standbyAllStreams(const bool setModeRequest) {
    ALOGD("%s()", __FUNCTION__);

    status_t status = NO_ERROR;

    status = standbyAllOutputStreams(setModeRequest);
    status = standbyAllInputStreams(setModeRequest);

    return status;
}

size_t AudioALSAStreamManager::getInputBufferSize(uint32_t sampleRate, audio_format_t format, uint32_t channelCount) {
    size_t wordSize = 0;
    switch (format) {
    case AUDIO_FORMAT_PCM_8_BIT: {
        wordSize = sizeof(int8_t);
        break;
    }
    case AUDIO_FORMAT_PCM_16_BIT: {
        wordSize = sizeof(int16_t);
        break;
    }
    case AUDIO_FORMAT_PCM_8_24_BIT:
    case AUDIO_FORMAT_PCM_32_BIT: {
        wordSize = sizeof(int32_t);
        break;
    }
    default: {
        ALOGW("%s(), wrong format(0x%x), default use wordSize = %zu", __FUNCTION__, format, sizeof(int16_t));
        wordSize = sizeof(int16_t);
        break;
    }
    }

    size_t bufferSize = ((sampleRate * channelCount * wordSize) * 20) / 1000; // TODO (Harvey): why 20 ms here?

    ALOGD("%s(), sampleRate = %u, format = 0x%x, channelCount = %u, bufferSize = %zu",
          __FUNCTION__, sampleRate, format, channelCount, bufferSize);
    return bufferSize;
}


// set musicplus to streamout
status_t AudioALSAStreamManager::SetMusicPlusStatus(bool bEnable) {

    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        pTempFilter->setParamFixed(bEnable ? true : false);
    }

    return NO_ERROR;
}

bool AudioALSAStreamManager::GetMusicPlusStatus() {

    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        bool musicplus_status = pTempFilter->isParamFixed();
        if (musicplus_status) {
            return true;
        }
    }

    return false;
}

status_t AudioALSAStreamManager::SetBesLoudnessStatus(bool bEnable) {

#ifdef MTK_BESLOUDNESS_SUPPORT
    mBesLoudnessStatus = bEnable;
    property_set(PROPERTY_KEY_BESLOUDNESS_SWITCH_STATE, (mBesLoudnessStatus) ? "1" : "0");
#if 0
    char property_value[PROPERTY_VALUE_MAX];
    property_get(PROPERTY_KEY_BESLOUDNESS_SWITCH_STATE, property_value, "0");
    int besloudness_enable = atoi(property_value);
    ALOGD("SetBesLoudnessStatus After Setting %d, input %d", besloudness_enable, bEnable);
#endif
    if (mBesLoudnessControlCallback != NULL) {
        mBesLoudnessControlCallback((void *)mBesLoudnessStatus);
    }
#else
    ALOGD("Unsupport set mBesLoudnessStatus()");
#endif
    return NO_ERROR;
}

bool AudioALSAStreamManager::GetBesLoudnessStatus() {
    return mBesLoudnessStatus;
}

status_t AudioALSAStreamManager::SetBesLoudnessControlCallback(const BESLOUDNESS_CONTROL_CALLBACK_STRUCT *callback_data) {
    if (callback_data == NULL) {
        mBesLoudnessControlCallback = NULL;
    } else {
        mBesLoudnessControlCallback = callback_data->callback;
        ASSERT(mBesLoudnessControlCallback != NULL);
        mBesLoudnessControlCallback((void *)mBesLoudnessStatus);
    }

    return NO_ERROR;
}

status_t AudioALSAStreamManager::UpdateACFHCF(int value) {
    ALOGD("%s()", __FUNCTION__);

    AUDIO_ACF_CUSTOM_PARAM_STRUCT sACFHCFParam;

    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        if (value == 0) {
            ALOGD("setParameters Update ACF Parames");
            getAudioCompFltCustParam(AUDIO_COMP_FLT_AUDIO, &sACFHCFParam);
            pTempFilter->setParameter(AUDIO_COMP_FLT_AUDIO, &sACFHCFParam);

        } else if (value == 1) {
            ALOGD("setParameters Update HCF Parames");
            getAudioCompFltCustParam(AUDIO_COMP_FLT_HEADPHONE, &sACFHCFParam);
            pTempFilter->setParameter(AUDIO_COMP_FLT_HEADPHONE, &sACFHCFParam);

        } else if (value == 2) {
            ALOGD("setParameters Update ACFSub Parames");
            getAudioCompFltCustParam(AUDIO_COMP_FLT_AUDIO_SUB, &sACFHCFParam);
            pTempFilter->setParameter(AUDIO_COMP_FLT_AUDIO_SUB, &sACFHCFParam);

        }
    }
    return NO_ERROR;
}

// ACF Preview parameter
status_t AudioALSAStreamManager::SetACFPreviewParameter(void *ptr , int len __unused) {
    ALOGD("%s()", __FUNCTION__);

    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        pTempFilter->setParameter(AUDIO_COMP_FLT_AUDIO, (AUDIO_ACF_CUSTOM_PARAM_STRUCT *)ptr);
    }

    return NO_ERROR;
}

status_t AudioALSAStreamManager::SetHCFPreviewParameter(void *ptr , int len __unused) {
    ALOGD("%s()", __FUNCTION__);

    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        pTempFilter->setParameter(AUDIO_COMP_FLT_HEADPHONE, (AUDIO_ACF_CUSTOM_PARAM_STRUCT *)ptr);
    }

    return NO_ERROR;
}

status_t AudioALSAStreamManager::setSpkOutputGain(int32_t gain, uint32_t ramp_sample_cnt) {
    ALOGD("%s(), gain = %d, ramp_sample_cnt = %u", __FUNCTION__, gain, ramp_sample_cnt);

#if 0 //mark temp
    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        pTempFilter->setSpkOutputGain(gain, ramp_sample_cnt);
    }
#endif

    return NO_ERROR;
}

status_t AudioALSAStreamManager::setSpkFilterParam(uint32_t fc, uint32_t bw, int32_t th) {
    ALOGD("%s(), fc %d, bw %d, th %d", __FUNCTION__, fc, bw, th);

#if 0 //mark temp
    for (size_t i = 0; i < mFilterManagerVector.size() ; i++) {
        AudioMTKFilterManager  *pTempFilter = mFilterManagerVector[i];
        pTempFilter->setSpkFilterParam(fc, bw, th);
    }
#endif
    return NO_ERROR;
}

status_t AudioALSAStreamManager::SetEMParameter(AUDIO_CUSTOM_PARAM_STRUCT *pSphParamNB) {
    ALOGD("%s()", __FUNCTION__);

    mAudioCustParamClient->SetNBSpeechParamToNVRam(pSphParamNB);
    SpeechEnhancementController::GetInstance()->SetNBSpeechParametersToAllModem(pSphParamNB);
    // Speech Enhancement, VM, Speech Driver
    // update VM/EPL/TTY record capability & enable if needed
    SpeechVMRecorder::getInstance()->configVm(pSphParamNB);


    return NO_ERROR;
}

status_t AudioALSAStreamManager::updateSpeechNVRAMParam(const int speech_band) {
#if defined(MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT)
    (void) speech_band;
#else
    ALOGD("%s(), speech_band=%d", __FUNCTION__, speech_band);

    //speech_band: 0:Narrow Band, 1: Wide Band, 2: Super Wideband, ..., 8: All
    if (speech_band == 0) { //Narrow Band
        AUDIO_CUSTOM_PARAM_STRUCT eSphParamNB;
        mAudioCustParamClient->GetNBSpeechParamFromNVRam(&eSphParamNB);
        SpeechEnhancementController::GetInstance()->SetNBSpeechParametersToAllModem(&eSphParamNB);
        ALOGD("JT:================================");
        for (int i = 0; i < SPEECH_COMMON_NUM ; i++) {
            ALOGD("JT:speech_common_para[%d] = %d", i, eSphParamNB.speech_common_para[i]);
        }
        for (int i = 0; i < SPEECH_PARA_MODE_NUM; i++) {
            for (int j = 0; j < SPEECH_PARA_NUM; j++) {
                ALOGD("JT:speech_mode_para[%d][%d] = %d", i, j, eSphParamNB.speech_mode_para[i][j]);
            }
        }
        for (int i = 0; i < 4; i++) {
            ALOGD("JT:speech_volume_para[%d] = %d", i, eSphParamNB.speech_volume_para[i]);
        }
    }
#if defined(MTK_WB_SPEECH_SUPPORT)
    else if (speech_band == 1) { //Wide Band
        AUDIO_CUSTOM_WB_PARAM_STRUCT eSphParamWB;
        mAudioCustParamClient->GetWBSpeechParamFromNVRam(&eSphParamWB);
        SpeechEnhancementController::GetInstance()->SetWBSpeechParametersToAllModem(&eSphParamWB);
    }
#endif
    else if (speech_band == 8) { //set all mode parameters
        AUDIO_CUSTOM_PARAM_STRUCT eSphParamNB;
        AUDIO_CUSTOM_WB_PARAM_STRUCT eSphParamWB;
        mAudioCustParamClient->GetNBSpeechParamFromNVRam(&eSphParamNB);
        SpeechEnhancementController::GetInstance()->SetNBSpeechParametersToAllModem(&eSphParamNB);
#if defined(MTK_WB_SPEECH_SUPPORT)
        mAudioCustParamClient->GetWBSpeechParamFromNVRam(&eSphParamWB);
        SpeechEnhancementController::GetInstance()->SetWBSpeechParametersToAllModem(&eSphParamWB);
#endif
    }

    if (isModeInPhoneCall() == true) { // get output device for in_call, and set speech mode
        UpdateSpeechMode();
    }
#endif
    return NO_ERROR;
}

status_t AudioALSAStreamManager::UpdateSpeechLpbkParams() {
    ALOGD("%s()", __FUNCTION__);
#ifdef MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT
    SpeechParamParser::getInstance()->SetParamInfo(String8("ParamSphLpbk=1;"));
#else
    AUDIO_CUSTOM_PARAM_STRUCT eSphParamNB;
    AUDIO_CUSTOM_SPEECH_LPBK_PARAM_STRUCT  eSphParamNBLpbk;
    mAudioCustParamClient->GetNBSpeechParamFromNVRam(&eSphParamNB);
    mAudioCustParamClient->GetNBSpeechLpbkParamFromNVRam(&eSphParamNBLpbk);
    SpeechEnhancementController::GetInstance()->SetNBSpeechLpbkParametersToAllModem(&eSphParamNB, &eSphParamNBLpbk);
    //no need to set speech mode, only for loopback parameters update
#endif

    return NO_ERROR;
}

status_t AudioALSAStreamManager::UpdateMagiConParams() {
    ALOGD("%s()", __FUNCTION__);
#if defined(MTK_MAGICONFERENCE_SUPPORT) && defined(MTK_DUAL_MIC_SUPPORT)
#ifndef MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT
    AUDIO_CUSTOM_MAGI_CONFERENCE_STRUCT eSphParamMagiCon;
    mAudioCustParamClient->GetMagiConSpeechParamFromNVRam(&eSphParamMagiCon);
    SpeechEnhancementController::GetInstance()->SetMagiConSpeechParametersToAllModem(&eSphParamMagiCon);

    if (isModeInPhoneCall() == true) { // get output device for in_call, and set speech mode
        UpdateSpeechMode();
    }

#endif
    return NO_ERROR;
#else
    ALOGW("-%s(), MagiConference Not Support", __FUNCTION__);
    return INVALID_OPERATION;

#endif
}

status_t AudioALSAStreamManager::UpdateHACParams() {
    ALOGD("%s()", __FUNCTION__);
#if defined(MTK_HAC_SUPPORT)
#ifndef MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT
    AUDIO_CUSTOM_HAC_PARAM_STRUCT eSphParamHAC;
    mAudioCustParamClient->GetHACSpeechParamFromNVRam(&eSphParamHAC);
    SpeechEnhancementController::GetInstance()->SetHACSpeechParametersToAllModem(&eSphParamHAC);

    if (isModeInPhoneCall() == true) { // get output device for in_call, and set speech mode
        UpdateSpeechMode();
    }

#endif
    return NO_ERROR;
#else
    ALOGW("-%s(), HAC Not Support", __FUNCTION__);
    return INVALID_OPERATION;

#endif
}

status_t AudioALSAStreamManager::UpdateDualMicParams() {
    ALOGD("%s()", __FUNCTION__);
#ifndef MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT
    AUDIO_CUSTOM_EXTRA_PARAM_STRUCT eSphParamDualMic;
    mAudioCustParamClient->GetDualMicSpeechParamFromNVRam(&eSphParamDualMic);
#if defined(MTK_DUAL_MIC_SUPPORT)
    SpeechEnhancementController::GetInstance()->SetDualMicSpeechParametersToAllModem(&eSphParamDualMic);
#endif

    if (isModeInPhoneCall() == true) { // get output device for in_call, and set speech mode
        UpdateSpeechMode();
    }

#endif
    return NO_ERROR;
}

void AudioALSAStreamManager::updateDeviceConnectionState(audio_devices_t device, bool connect) {
    if ((device & AUDIO_DEVICE_BIT_IN) == false) {
        mAvailableOutputDevices = connect ? mAvailableOutputDevices | device : mAvailableOutputDevices & !device;
    }
}

bool AudioALSAStreamManager::getDeviceConnectionState(audio_devices_t device) {
    if ((device & AUDIO_DEVICE_BIT_IN) == false) {
        return !!(mAvailableOutputDevices & device);
    }
    return false;
}

void AudioALSAStreamManager::setCustScene(const String8 scene) {
    mCustScene = scene;
#if defined(MTK_NEW_VOL_CONTROL)
    AudioMTKGainController::getInstance()->setScene(scene.string());
#endif
}

bool AudioALSAStreamManager::isEchoRefUsing() {
    if (isModeInVoipCall() == true) {
        return true;
    }
    if (mStreamInVector.size() > 1) {
        for (size_t i = 0; i < mStreamInVector.size(); i++) {
            if ((mStreamInVector[i]->getStreamAttribute()->input_source == AUDIO_SOURCE_VOICE_COMMUNICATION)
                || (mStreamInVector[i]->getStreamAttribute()->NativePreprocess_Info.PreProcessEffect_AECOn == true)
                || (mStreamInVector[i]->getStreamAttribute()->input_source == AUDIO_SOURCE_CUSTOMIZATION1) //MagiASR enable AEC
                || (mStreamInVector[i]->getStreamAttribute()->input_source == AUDIO_SOURCE_CUSTOMIZATION2)) {
                return true;
            }
        }
    }
    return false;
}

int AudioALSAStreamManager::setVolumeIndex(int stream, int device, int index) {
    ALOGD("%s() stream= %x, device= %x, index= %x", __FUNCTION__, stream, device, index);
    mVolumeIndex = index;
    if (stream == 0) { //stream voice call/voip call
        if (isModeInPhoneCall() == true) {
            mSpeechDriverFactory->GetSpeechDriver()->setMDVolumeIndex(stream, device, index);
        } else {
            SpeechDriverInterface *pSpeechDriver = NULL;
            for (int mdIndex = MODEM_1; mdIndex < NUM_MODEM; mdIndex++) {
                pSpeechDriver = mSpeechDriverFactory->GetSpeechDriverByIndex((modem_index_t)mdIndex);
                if (pSpeechDriver != NULL) { // Might be single talk and some speech driver is NULL
                    pSpeechDriver->setMDVolumeIndex(stream, device, index);
                }
            }
        }
    }
    return NO_ERROR;
}

status_t AudioALSAStreamManager::UpdateSpeechMode() {
    ALOGD("%s()", __FUNCTION__);
    //tina todo
    const audio_devices_t output_device = (audio_devices_t)AudioALSAHardwareResourceManager::getInstance()->getOutputDevice();
    const audio_devices_t input_device  = (audio_devices_t)AudioALSAHardwareResourceManager::getInstance()->getInputDevice();
    mSpeechDriverFactory->GetSpeechDriver()->SetSpeechMode(input_device, output_device);

    return NO_ERROR;
}

status_t AudioALSAStreamManager::UpdateSpeechVolume() {
    ALOGD("%s()", __FUNCTION__);
    mAudioALSAVolumeController->initVolumeController();

    if (isModeInPhoneCall() == true) {
        //TINA TODO GET DEVICE
        int32_t outputDevice = (audio_devices_t)AudioALSAHardwareResourceManager::getInstance()->getOutputDevice();
        AudioALSASpeechPhoneCallController *pSpeechPhoneCallController = AudioALSASpeechPhoneCallController::getInstance();
#ifndef MTK_AUDIO_GAIN_TABLE
        mAudioALSAVolumeController->setVoiceVolume(mAudioALSAVolumeController->getVoiceVolume(), mAudioMode, (uint32)outputDevice);
#endif
        switch (outputDevice) {
        case AUDIO_DEVICE_OUT_WIRED_HEADSET : {
#ifdef  MTK_TTY_SUPPORT
            if (pSpeechPhoneCallController->getTtyMode() == AUD_TTY_VCO) {
                mAudioALSAVolumeController->ApplyMicGain(Normal_Mic, mAudioMode);
            } else if (pSpeechPhoneCallController->getTtyMode() == AUD_TTY_HCO || pSpeechPhoneCallController->getTtyMode() == AUD_TTY_FULL) {
                mAudioALSAVolumeController->ApplyMicGain(TTY_CTM_Mic, mAudioMode);
            } else {
                mAudioALSAVolumeController->ApplyMicGain(Headset_Mic, mAudioMode);
            }
#else
            mAudioALSAVolumeController->ApplyMicGain(Headset_Mic, mAudioMode);
#endif
            break;
        }
        case AUDIO_DEVICE_OUT_WIRED_HEADPHONE : {
#ifdef  MTK_TTY_SUPPORT
            if (pSpeechPhoneCallController->getTtyMode() == AUD_TTY_VCO) {
                mAudioALSAVolumeController->ApplyMicGain(Normal_Mic, mAudioMode);
            } else if (pSpeechPhoneCallController->getTtyMode() == AUD_TTY_HCO || pSpeechPhoneCallController->getTtyMode() == AUD_TTY_FULL) {
                mAudioALSAVolumeController->ApplyMicGain(TTY_CTM_Mic, mAudioMode);
            } else {
                mAudioALSAVolumeController->ApplyMicGain(Headset_Mic, mAudioMode);
            }
#else
            mAudioALSAVolumeController->ApplyMicGain(Headset_Mic, mAudioMode);
#endif
            break;
        }
        case AUDIO_DEVICE_OUT_SPEAKER: {
#ifdef  MTK_TTY_SUPPORT
            if (pSpeechPhoneCallController->getTtyMode() == AUD_TTY_VCO) {
                mAudioALSAVolumeController->ApplyMicGain(Normal_Mic, mAudioMode);
            } else if (pSpeechPhoneCallController->getTtyMode() == AUD_TTY_HCO || pSpeechPhoneCallController->getTtyMode() == AUD_TTY_FULL) {
                mAudioALSAVolumeController->ApplyMicGain(TTY_CTM_Mic, mAudioMode);
            } else {
                mAudioALSAVolumeController->ApplyMicGain(Handfree_Mic, mAudioMode);
            }
#else
            mAudioALSAVolumeController->ApplyMicGain(Handfree_Mic, mAudioMode);
#endif
            break;
        }
        case AUDIO_DEVICE_OUT_EARPIECE: {
            mAudioALSAVolumeController->ApplyMicGain(Normal_Mic, mAudioMode);
            break;
        }
        default: {
            break;
        }
        }
    } else {
        setMasterVolume(mAudioALSAVolumeController->getMasterVolume());
    }
    return NO_ERROR;

}

status_t AudioALSAStreamManager::SetVCEEnable(bool bEnable) {
    ALOGD("%s()", __FUNCTION__);
    SpeechEnhancementController::GetInstance()->SetDynamicMaskOnToAllModem(SPH_ENH_DYNAMIC_MASK_VCE, bEnable);
    return NO_ERROR;

}

status_t AudioALSAStreamManager::SetMagiConCallEnable(bool bEnable) {
    ALOGD("%s(), bEnable=%d", __FUNCTION__, bEnable);

    // enable/disable flag
    SpeechEnhancementController::GetInstance()->SetMagicConferenceCallOn(bEnable);
    if (isModeInPhoneCall() == true) { // get output device for in_call, and set speech mode
        UpdateSpeechMode();
    }

    return NO_ERROR;

}

bool AudioALSAStreamManager::GetMagiConCallEnable(void) {
    bool bEnable = SpeechEnhancementController::GetInstance()->GetMagicConferenceCallOn();
    ALOGD("-%s(), bEnable=%d", __FUNCTION__, bEnable);

    return bEnable;
}

status_t AudioALSAStreamManager::SetHACEnable(bool bEnable) {
    ALOGD("%s(), bEnable=%d", __FUNCTION__, bEnable);

    // enable/disable flag
    SpeechEnhancementController::GetInstance()->SetHACOn(bEnable);
    if (isModeInPhoneCall() == true) { // get output device for in_call, and set speech mode
        UpdateSpeechMode();
    }

    return NO_ERROR;

}

bool AudioALSAStreamManager::GetHACEnable(void) {
    bool bEnable = SpeechEnhancementController::GetInstance()->GetHACOn();
    ALOGD("-%s(), bEnable=%d", __FUNCTION__, bEnable);

    return bEnable;
}

/**
 * reopen Phone Call audio path according to RIL mapped modem notify
 */
int AudioALSAStreamManager::phoneCallRefreshModem(const char *rilMappedMDName) {
    bool isPhoneCallNeedReopen = false;
    modem_index_t rilMappedMDIdx = MODEM_1;
    audio_mode_t currentAudioMode = getMode();

    if (rilMappedMDName != NULL) {
        if (isModeInPhoneCall(currentAudioMode)) {
            if (strcmp("MD1", rilMappedMDName) == 0) {
                rilMappedMDIdx = MODEM_1;
            } else if (strcmp("MD3", rilMappedMDName) == 0) {
                rilMappedMDIdx = MODEM_EXTERNAL;
            } else {
                ALOGW("%s(), Invalid rilMappedMDName=%s, currentAudioMode(%d)",
                      __FUNCTION__, rilMappedMDName, currentAudioMode);
                return -EINVAL;
            }
            isPhoneCallNeedReopen = mSpeechPhoneCallController->checkReopen(rilMappedMDIdx);
        }
        ALOGV("%s(), rilMappedMDName=%s, currentAudioMode(%d), isPhoneCallNeedReopen(%d)",
              __FUNCTION__, rilMappedMDName, currentAudioMode, isPhoneCallNeedReopen);
    } else {
        ALOGW("%s(), Invalid rilMappedMDName=NULL, currentAudioMode(%d)", __FUNCTION__, currentAudioMode);
        return -EINVAL;
    }
    if (isPhoneCallNeedReopen) {
        ALOGD("%s(), rilMappedMDIdx(%d), currentAudioMode(%d), start to reopen",
              __FUNCTION__, rilMappedMDIdx, currentAudioMode);
        phoneCallReopen();
    } else {
        ALOGD("-%s(), rilMappedMDName=%s, currentAudioMode(%d), no need to reopen",
              __FUNCTION__, rilMappedMDName, currentAudioMode);
    }
    return 0;
}

/**
 * reopen Phone Call audio path
 */
int AudioALSAStreamManager::phoneCallReopen() {
    AL_AUTOLOCK(mStreamVectorLock);
    audio_mode_t currentAudioMode = getMode();
    ALOGD("+%s(), currentAudioMode(%d)", __FUNCTION__, currentAudioMode);

    if (isModeInPhoneCall(currentAudioMode)) {
        setAllStreamsSuspend(true, true);
        standbyAllStreams(true);
        AL_AUTOLOCK(mLock);
        mSpeechPhoneCallController->setMicMute(true);
        const audio_devices_t phonecallOutputDevice = mSpeechPhoneCallController->getOutputDevice();
        const audio_devices_t phonecallInputputDevice = mSpeechPhoneCallController->getAdjustedInputDevice();

        mSpeechPhoneCallController->close();
        mSpeechPhoneCallController->open(currentAudioMode, phonecallOutputDevice, phonecallInputputDevice);

        mAudioALSAVolumeController->setVoiceVolume(mAudioALSAVolumeController->getVoiceVolume(),
                                                   currentAudioMode, phonecallOutputDevice);
        mSpeechPhoneCallController->setMicMute(mMicMute);
        setAllStreamsSuspend(false, true);
        ALOGD("-%s(), currentAudioMode(%d), phonecallOutputDevice(0x%x), reopen end",
              __FUNCTION__, currentAudioMode, phonecallOutputDevice);
        return 0;
    } else {
        ALOGW("-%s(), currentAudioMode(%d) skip reopen.", __FUNCTION__, currentAudioMode);
        return -EINVAL;
    }
}

/**
 * update Phone Call phone id
 */
int AudioALSAStreamManager::phoneCallUpdatePhoneId(const phone_id_t phoneId) {
    if (phoneId != PHONE_ID_0 && phoneId != PHONE_ID_1) {
        return -EINVAL;
    }
    if (isModeInPhoneCall(mAudioMode)) {
        phone_id_t currentPhoneId = mSpeechPhoneCallController->getPhoneId();
        if (phoneId != currentPhoneId) {
            ALOGD("%s(), phoneId(%d->%d), mAudioMode(%d)", __FUNCTION__, currentPhoneId, phoneId, mAudioMode);
            mSpeechPhoneCallController->setPhoneId(phoneId);
            modem_index_t newMDIdx = mSpeechPhoneCallController->getIdxMDByPhoneId(phoneId);
            if (newMDIdx == MODEM_EXTERNAL) {
                phoneCallRefreshModem("MD3");
            } else {
                phoneCallRefreshModem("MD1");
            }
        }
    } else {
        mSpeechPhoneCallController->setPhoneId(phoneId);
    }
    return 0;
}


status_t AudioALSAStreamManager::SetBtHeadsetName(const char *btHeadsetName) {
    if (mBtHeadsetName) {
        free((void *)mBtHeadsetName);
        mBtHeadsetName = NULL;
    }
    if (btHeadsetName) {
        mBtHeadsetName = strdup(btHeadsetName);
        ALOGD("%s(), mBtHeadsetName = %s", __FUNCTION__, mBtHeadsetName);
    }
    return NO_ERROR;
}

const char *AudioALSAStreamManager::GetBtHeadsetName() {
    return mBtHeadsetName;
}

status_t AudioALSAStreamManager::SetBtHeadsetNrec(bool bEnable) {
#if defined(CONFIG_MT_ENG_BUILD)
    // Used for testing the BT_NREC_OFF case
    char property_value[PROPERTY_VALUE_MAX];
    property_get(PROPERTY_KEY_SET_BT_NREC, property_value, "-1");
    int btNrecProp = atoi(property_value);
    if (btNrecProp != -1) {
        bEnable = btNrecProp;
        ALOGD("%s(), force set the BT headset NREC = %d", __FUNCTION__, bEnable);
    }
#endif

    ALOGD("%s(), bEnable=%d", __FUNCTION__, bEnable);

    // enable/disable flag
    if (SpeechEnhancementController::GetInstance()->GetBtHeadsetNrecOn() != bEnable) {
        SpeechEnhancementController::GetInstance()->SetBtHeadsetNrecOnToAllModem(bEnable);
    }

    return NO_ERROR;

}

bool AudioALSAStreamManager::GetBtHeadsetNrecStatus(void) {
    bool bEnable = SpeechEnhancementController::GetInstance()->GetBtHeadsetNrecOn();
    ALOGD("-%s(), bEnable=%d", __FUNCTION__, bEnable);

    return bEnable;
}

status_t AudioALSAStreamManager::Enable_DualMicSettng(sph_enh_dynamic_mask_t sphMask, bool bEnable) {
    ALOGD("%s(), bEnable=%d", __FUNCTION__, bEnable);

    SpeechEnhancementController::GetInstance()->SetDynamicMaskOnToAllModem(sphMask, bEnable);
    return NO_ERROR;

}

status_t AudioALSAStreamManager::Set_LSPK_DlMNR_Enable(sph_enh_dynamic_mask_t sphMask, bool bEnable) {
    ALOGD("%s(), bEnable=%d", __FUNCTION__, bEnable);

    Enable_DualMicSettng(sphMask, bEnable);

    if (SpeechEnhancementController::GetInstance()->GetMagicConferenceCallOn() == true &&
        SpeechEnhancementController::GetInstance()->GetDynamicMask(sphMask) == true) {
        ALOGE("Cannot open MagicConCall & LoudSpeaker DMNR at the same time!!");
    }
    return NO_ERROR;

}


bool AudioALSAStreamManager::getVoiceWakeUpNeedOn() {
    AL_AUTOLOCK(mLock);
    return mVoiceWakeUpNeedOn;
}

status_t AudioALSAStreamManager::setVoiceWakeUpNeedOn(const bool enable) {
    ALOGD("+%s(), mVoiceWakeUpNeedOn: %d => %d ", __FUNCTION__, mVoiceWakeUpNeedOn, enable);
    AL_AUTOLOCK(mLock);

    if (enable == mVoiceWakeUpNeedOn) {
        ALOGW("-%s(), enable(%d) == mVoiceWakeUpNeedOn(%d), return", __FUNCTION__, enable, mVoiceWakeUpNeedOn);
        return INVALID_OPERATION;
    }

    if (enable == true) {
        if (mStreamInVector.size() != 0 || mForceDisableVoiceWakeUpForSetMode == true) {
            ALOGD("-%s(), mStreamInVector.size() = %zu, mForceDisableVoiceWakeUpForSetMode = %d, return", __FUNCTION__, mStreamInVector.size(), mForceDisableVoiceWakeUpForSetMode);
        } else {
            if (mAudioALSAVoiceWakeUpController->getVoiceWakeUpEnable() == false) {
                mAudioALSAVoiceWakeUpController->setVoiceWakeUpEnable(true);
            }
        }
    } else {
        if (mAudioALSAVoiceWakeUpController->getVoiceWakeUpEnable() == true) {
            mAudioALSAVoiceWakeUpController->setVoiceWakeUpEnable(false);
        }
    }


    property_set(PROPERTY_KEY_VOICE_WAKE_UP_NEED_ON, (enable == false) ? "0" : "1");
    mVoiceWakeUpNeedOn = enable;

    ALOGD("-%s(), mVoiceWakeUpNeedOn: %d", __FUNCTION__, mVoiceWakeUpNeedOn);
    return NO_ERROR;
}

void AudioALSAStreamManager::UpdateDynamicFunctionMask(void) {
    ALOGD("+%s()", __FUNCTION__);
    if (mStreamInVector.size() > 0) {
        for (size_t i = 0; i < mStreamInVector.size(); i++) {
            mStreamInVector[i]->UpdateDynamicFunctionMask();
        }
    }
    ALOGD("-%s()", __FUNCTION__);
}

bool AudioALSAStreamManager::EnableBesRecord(void) {
    bool bRet = false;
    if ((mAudioCustParamClient->QueryFeatureSupportInfo()& SUPPORT_HD_RECORD) > 0) {
        bRet = true;
    }
    ALOGD("-%s(), %x", __FUNCTION__, bRet);

    return bRet;
}

status_t AudioALSAStreamManager::setScreenState(bool mode) {
    AL_AUTOLOCK(mStreamVectorLock);
    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        pAudioALSAStreamOut->setScreenState(mode);
    }

    return NO_ERROR;
}

using android::hardware::bluetooth::a2dp::V1_0::IBluetoothAudioHost;
using android::hardware::bluetooth::a2dp::V1_0::CodecConfiguration;

typedef struct {
    uint32_t codec_type; // codec types ex: SBC/AAC/LDAC/APTx
    uint32_t sample_rate;    // sample rate, ex: 44.1/48.88.2/96 KHz
    uint32_t encoded_audio_bitrate;  // encoder audio bitrates
    uint16_t max_latency;    // maximum latency
    uint16_t scms_t_enable;  // content protection enable
    uint16_t acl_hdl;    // connection handle
    uint16_t l2c_rcid;   // l2cap channel id
    uint16_t mtu;        // mtu size
    unsigned char bits_per_sample; // bits per sample, ex: 16/24/32
    unsigned char ch_mode;         // None:0, Left:1, Right:2
    unsigned char codec_info[32];  //codec specific information
} A2DP_CODEC_INFO_T;

static A2DP_CODEC_INFO_T a2dp_codecinfo;

void AudioALSAStreamManager::setBluetoothAudioOffloadParam(const sp<IBluetoothAudioHost> &hostIf,
                                                           const CodecConfiguration &codecConfig,
                                                           bool on) {
    ALOGD("+%s()", __FUNCTION__);
    if (on == true) {
        mBluetoothAudioOffloadHostIf = hostIf;
        a2dp_codecinfo.codec_type = (uint32_t)codecConfig.codecType;
        a2dp_codecinfo.sample_rate = (uint32_t)codecConfig.sampleRate;
        a2dp_codecinfo.encoded_audio_bitrate = codecConfig.encodedAudioBitrate;
        a2dp_codecinfo.mtu = codecConfig.peerMtu;
        a2dp_codecinfo.bits_per_sample = (uint32_t)codecConfig.bitsPerSample;
        a2dp_codecinfo.ch_mode = (uint32_t)codecConfig.channelMode;

        if (a2dp_codecinfo.codec_type == 0x1) { // sbc
            a2dp_codecinfo.codec_info[0] = codecConfig.codecSpecific.sbcData.codecParameters;
            a2dp_codecinfo.codec_info[1] = codecConfig.codecSpecific.sbcData.minBitpool;
            a2dp_codecinfo.codec_info[2] = codecConfig.codecSpecific.sbcData.maxBitpool;
        }

        mBluetoothAudioOffloadSession++;
        ALOGD("codecType=%u, sampleRate=%u, bitsPerSample=%u, channelMode=%u",
              (uint32_t)a2dp_codecinfo.codec_type,
              (uint32_t)a2dp_codecinfo.sample_rate,
              (unsigned char)a2dp_codecinfo.bits_per_sample,
              (unsigned char)a2dp_codecinfo.ch_mode);
        ALOGD("encodedAudioBitrate=%u, peerMtu=%u",
              (uint32_t)a2dp_codecinfo.encoded_audio_bitrate,
              (uint16_t)a2dp_codecinfo.mtu);
        ALOGD("codecParameters=%u, minBitpool=%u, maxBitpool=%u",
              (unsigned char)a2dp_codecinfo.codec_info[0],
              (unsigned char)a2dp_codecinfo.codec_info[1],
              (unsigned char)a2dp_codecinfo.codec_info[2]);
    } else {
        mBluetoothAudioOffloadSession--;
        if (mBluetoothAudioOffloadSession < 0) {
            ALOGW("%s() Error: mBluetoothAudioOffloadSession < 0, reset to 0!", __FUNCTION__);
            mBluetoothAudioOffloadSession = 0;
        }
        if (mBluetoothAudioOffloadSession == 0) {
            mBluetoothAudioOffloadHostIf = NULL;
        }
    }
    ALOGD("-%s() mBluetoothAudioOffloadSession=%d", __FUNCTION__, mBluetoothAudioOffloadSession);
}

void *AudioALSAStreamManager::getBluetoothAudioCodecInfo(void) {
    return (void *)&a2dp_codecinfo;
}

uint32_t AudioALSAStreamManager::getBluetoothAudioCodecType(void) {
    if (mBluetoothAudioOffloadSession > 0) {
        ALOGD("%s(), codec_type = %d", __FUNCTION__, a2dp_codecinfo.codec_type);
        return a2dp_codecinfo.codec_type;
    } else {
        ALOGD("%s(), mBluetoothAudioOffloadSession = %d, set codec_type unknown", __FUNCTION__,
              mBluetoothAudioOffloadSession);
        return 0; // unknown
    }
}

void AudioALSAStreamManager::setA2dpSuspend(bool on) {
#ifdef MTK_AUDIODSP_SUPPORT
    static AudioLock A2dpSuspendLock;
    AL_AUTOLOCK(A2dpSuspendLock);

    int32_t outputDevice = (audio_devices_t)AudioALSAHardwareResourceManager::getInstance()->getOutputDevice();
    ALOGD("%s(), suspend = %d, outputDevice = 0x%x", __FUNCTION__, on, outputDevice);

    if ((mBluetoothAudioOffloadHostIf != NULL) &&
        (mBluetoothAudioOffloadSession > 0)) {
        if (on) {
            if (isModeInPhoneCall() || isModeInVoipCall() || (outputDevice & AUDIO_DEVICE_OUT_ALL_A2DP)) {
                mBluetoothAudioOffloadHostIf->suspendStream();
            }
        } else {
            if (outputDevice & AUDIO_DEVICE_OUT_ALL_A2DP) {
                mBluetoothAudioOffloadHostIf->startStream();
            }
        }
    }
#else
    ALOGV("%s(), suspend = %d", __FUNCTION__, on);
#endif
}

void AudioALSAStreamManager::setA2dpSuspendStatus(int status) {
    AL_AUTOLOCK(mA2dpSuspendStatusLock);
    ALOGD("%s() status=%d", __FUNCTION__, status);
#ifdef MTK_AUDIODSP_SUPPORT
    // 0: Start SUCCESS, 1: Start FAILURE, 3: Start PENDING
    // 4: Suspend SUCCESS, 5: Suspend FAILURE, 7: Suspend PENDING
    mA2dpSuspend = status;

    // send a2dp PAUSE control to DSP
    struct ipi_msg_t ipi_msg;
    if (mA2dpSuspend == 4) { // 4: Suspend SUCCESS
        audio_send_ipi_msg(&ipi_msg,
                           TASK_SCENE_DATAPROVIDER, AUDIO_IPI_LAYER_TO_DSP,
                           AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_BYPASS_ACK,
                           AUDIO_DSP_TASK_DATAPROVIDER_SUSPEND, 1, 0,
                           NULL);
    } else if (mA2dpSuspend == 0) { // 0: Start SUCCESS
        audio_send_ipi_msg(&ipi_msg,
                           TASK_SCENE_DATAPROVIDER, AUDIO_IPI_LAYER_TO_DSP,
                           AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_BYPASS_ACK,
                           AUDIO_DSP_TASK_DATAPROVIDER_SUSPEND, 0, 0,
                           NULL);
    }
#endif
}

status_t AudioALSAStreamManager::setBypassDLProcess(bool flag) {
    AL_AUTOLOCK(mLock);
    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    mBypassPostProcessDL = flag;

    return NO_ERROR;
}
bool AudioALSAStreamManager::getSWVersion(void) {
    const char     kSWVersionFile[128]     = "/proc/chip/sw_ver";
    bool bSW_E1 = false;
    char str[8] = {0};
    char str_SWE1[8] = "0000\n";
    FILE *fp = fopen(kSWVersionFile, "rb");
    int rd_size;

    if (fp == NULL) {
        ALOGE("ERROR: file: %s not found\n", kSWVersionFile);
        return bSW_E1;
    }

    if (4 > (rd_size = fread(str, 1, sizeof(str), fp))) {
        ALOGE("ERROR: read error: %s, size(%d)\n", strerror(errno), rd_size);
    }

    ALOGD("%s(), %s: %s  (str_SWE1=%s)\n", __FUNCTION__, kSWVersionFile, str, str_SWE1);
    if (fp != NULL) {
        fclose(fp);
    }

    if (strncmp(str, str_SWE1, strlen(str_SWE1) + 1) == 0) {
        bSW_E1 = true;
    }
    ALOGD("-%s(), bSW_E1=%d", __FUNCTION__, bSW_E1);

    return bSW_E1;
}


status_t AudioALSAStreamManager::setParametersToStreamOut(const String8 &keyValuePairs) { // TODO(Harvey
    if (mStreamOutVector.size() == 0) {
        return INVALID_OPERATION;
    }

    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;
    for (size_t i = 0; i < mStreamOutVector.size() ; i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        pAudioALSAStreamOut->setParameters(keyValuePairs);
    }

    return NO_ERROR;
}


status_t AudioALSAStreamManager::setParameters(const String8 &keyValuePairs, int IOport) { // TODO(Harvey)
    status_t status = PERMISSION_DENIED;
    ssize_t index = -1;

    ALOGD("+%s(), IOport = %d, keyValuePairs = %s", __FUNCTION__, IOport, keyValuePairs.string());

    index = mStreamOutVector.indexOfKey(IOport);
    if (index >= 0) {
        ALOGD("Send to mStreamOutVector[%zu]", index);
        AudioALSAStreamOut *pAudioALSAStreamOut = mStreamOutVector.valueAt(index);
        status = pAudioALSAStreamOut->setParameters(keyValuePairs);
        ALOGD("-%s()", __FUNCTION__);
        return status;
    }

    index = mStreamInVector.indexOfKey(IOport);
    if (index >= 0) {
        ALOGD("Send to mStreamInVector [%zu]", index);
        AudioALSAStreamIn *pAudioALSAStreamIn = mStreamInVector.valueAt(index);
        status = pAudioALSAStreamIn->setParameters(keyValuePairs);
        ALOGD("-%s()", __FUNCTION__);
        return status;
    }

    ALOGE("-%s(), do nothing, return", __FUNCTION__);
    return status;
}

bool AudioALSAStreamManager::isStreamOutExist(uint32_t type) {
    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        if (pAudioALSAStreamOut->getStreamOutType() == type) {
            return true;
        }
    }
    return false;
}

bool AudioALSAStreamManager::isHdmiStreamOutExist(void) {
    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        if (pAudioALSAStreamOut->getStreamOutType() == STREAM_OUT_HDMI_STEREO ||
            pAudioALSAStreamOut->getStreamOutType() == STREAM_OUT_HDMI_MULTI_CHANNEL) {
            return true;
        }
    }
    return false;
}

bool AudioALSAStreamManager::isStreamOutActive(uint32_t type) {
    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        if (pAudioALSAStreamOut->getStreamOutType() == type &&
            pAudioALSAStreamOut->isOutPutStreamActive()) {
            return true;
        }
    }
    return false;
}

void AudioALSAStreamManager::putStreamOutIntoStandy(uint32_t type) {
    AudioALSAStreamOut *pAudioALSAStreamOut = NULL;

    for (size_t i = 0; i < mStreamOutVector.size(); i++) {
        pAudioALSAStreamOut = mStreamOutVector[i];
        if (pAudioALSAStreamOut->getStreamOutType() == type) {
            pAudioALSAStreamOut->standby();
        }
    }
}


void AudioALSAStreamManager::updateAudioModePolicy(const audio_mode_t new_mode) {
    if (new_mode == AUDIO_MODE_IN_CALL) {
        mPhoneCallControllerStatusPolicy = true;
    } else if (new_mode == AUDIO_MODE_NORMAL) {
        mPhoneCallControllerStatusPolicy = false;
    }

    mPhoneWithVoipPolicy = false;
    if ((mAudioModePolicy == AUDIO_MODE_IN_CALL && new_mode == AUDIO_MODE_IN_COMMUNICATION) ||  // Phone -> VOIP
        (mAudioModePolicy == AUDIO_MODE_IN_COMMUNICATION && new_mode == AUDIO_MODE_IN_CALL) ||  // VOIP -> Phone
        (mVoipToRingTonePolicy == true && new_mode == AUDIO_MODE_IN_CALL) || // VOIP -> Ring Tone -> Phone
        (mVoipToRingTonePolicy == true && new_mode == AUDIO_MODE_IN_COMMUNICATION && mPhoneCallControllerStatusPolicy == true)) { // Phone -> VOIP -> Ring Tone -> VOIP
        mPhoneWithVoipPolicy = true;
        mVoipToRingTonePolicy = false;
    } else if (mAudioModePolicy == AUDIO_MODE_IN_COMMUNICATION && new_mode == AUDIO_MODE_RINGTONE) { // VOIP -> Ring Tone
        mVoipToRingTonePolicy = true;
        if (mPhoneCallControllerStatusPolicy == true) { // Phone -> VOIP -> Ring Tone
            mPhoneWithVoipPolicy = true;
        }
    } else {
        mVoipToRingTonePolicy = false;
    }

#if defined(MTK_AUDIO_DSP_RECOVERY_SUPPORT)
    ALOGD("%s(), mAudioModePolicy: %d => %d, mPhoneCallControllerStatusPolicy: %d , mPhoneWithVoipPolicy: %d , mVoipToRingTonePolicy: %d",
          __FUNCTION__,
          mAudioModePolicy,
          new_mode,
          mPhoneCallControllerStatusPolicy,
          mPhoneWithVoipPolicy,
          mVoipToRingTonePolicy);
#endif

    mAudioModePolicy = new_mode;
}


bool AudioALSAStreamManager::getHiFiStatus() {
    return false;
}


void AudioALSAStreamManager::handleHfpClientState(bool enable)
{
#ifdef MTK_BT_PROFILE_HFP_CLIENT_AUDIO
    ALOGD("+%s enable = %d mode = %d", __FUNCTION__, enable, mAudioMode);

    AudioHfpController *hfpController = AudioHfpController::getInstance();

    if (hfpController->getHfpEnable() == enable) {
        ALOGW("%s state unchanged %d", __FUNCTION__, enable);
        return;
    }

    AL_AUTOLOCK(mStreamVectorLock);

    setAllStreamsSuspend(true);
    standbyAllStreams();

    {
        AL_AUTOLOCK(mLock);

        if (enable)
        {
            if (isModeInPhoneCall()) {
                if (mPhoneCallOpened) {
                    mSpeechPhoneCallController->close();
                    mPhoneCallOpened = false;
                }
            } else {
                ALOGW("%s enable %d in non call mode %d", __FUNCTION__, enable, mAudioMode);
            }

            hfpController->setHfpEnable(true);
            hfpController->enableHfpTask();
        }
        else
        {
            if (hfpController->isHfpTaskRunning())
                hfpController->disableHfpTask();

            hfpController->setHfpEnable(false);
        }
    } // End of AL_AUTOLOCK(mLock);

    setAllStreamsSuspend(false);

    ALOGD("-%s enable = %d mode = %d", __FUNCTION__, enable, mAudioMode);
#endif
}

} // end of namespace android
