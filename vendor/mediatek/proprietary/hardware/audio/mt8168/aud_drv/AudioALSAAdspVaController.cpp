#include "AudioALSAAdspVaController.h"

#include "AudioLock.h"
#include "AudioALSADriverUtility.h"

#include "AudioMTKHeadsetMessager.h"

#include "AudioCustParamClient.h"
#include "AudioUtility.h"
#include "AudioALSAHardwareResourceManager.h"
#include "AudioALSAStreamManager.h"
#include <linux/vow.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AudioALSAAdspVaController"
#define SOUND_TRIGGER_HAL_LIBRARY_PATH "/system/vendor/lib/hw/sound_trigger.primary.mt8168.so"

namespace android {

AudioALSAAdspVaController *AudioALSAAdspVaController::mAudioALSAAdspVaController = NULL;
AudioALSAAdspVaController *AudioALSAAdspVaController::getInstance() {
    static AudioLock mGetInstanceLock;
    AL_AUTOLOCK(mGetInstanceLock);

    if (mAudioALSAAdspVaController == NULL) {
        mAudioALSAAdspVaController = new AudioALSAAdspVaController();
    }
    ASSERT(mAudioALSAAdspVaController != NULL);
    return mAudioALSAAdspVaController;
}

/*==============================================================================
 *                     Callback Function
 *============================================================================*/

AudioALSAAdspVaController::AudioALSAAdspVaController() :
    mMixer(AudioALSADriverUtility::getInstance()->getMixer()),
    mDetectEnable(false),
    mAdspEnable(false),
    mIsUseHeadsetMic(false)
 {
    ALOGD("%s()", __FUNCTION__);

    memset(&mStDev, 0, sizeof(mStDev));
    if (access(SOUND_TRIGGER_HAL_LIBRARY_PATH, R_OK) == 0) {
        mStDev.sound_trigger_lib = dlopen(SOUND_TRIGGER_HAL_LIBRARY_PATH, RTLD_NOW);
        if (mStDev.sound_trigger_lib == NULL) {
             ALOGE("%s(), %s open fail", __FUNCTION__, SOUND_TRIGGER_HAL_LIBRARY_PATH);
        } else {
             mStDev.open_for_streaming =
                (int (* )(void))dlsym(mStDev.sound_trigger_lib,
                                             "sound_trigger_open_for_streaming");
             mStDev.close_for_streaming =
                (int (* )(int))dlsym(mStDev.sound_trigger_lib,
                                             "sound_trigger_close_for_streaming");
             mStDev.read_samples =
                (int (* )(int, void *, size_t))dlsym(mStDev.sound_trigger_lib,
                                             "sound_trigger_read_samples");
             mStDev.get_parameters =
                (int (* )(int, void *))dlsym(mStDev.sound_trigger_lib,
                                             "sound_trigger_get_parameters");

             if (!mStDev.open_for_streaming ||
                 !mStDev.close_for_streaming ||
                 !mStDev.read_samples ||
                 !mStDev.get_parameters) {
                 dlclose(mStDev.sound_trigger_lib);
                 mStDev.sound_trigger_lib = NULL;
                 mStDev.open_for_streaming = NULL;
                 mStDev.close_for_streaming = NULL;
                 mStDev.read_samples = NULL;
                 mStDev.get_parameters = NULL;
                 ALOGI("%s(), soundtrigger device open fail\n", __FUNCTION__);
             } else {
                 ALOGI("%s(), soundtrigger device open succuss\n", __FUNCTION__);
             }
        }

    } else {
        ALOGE("%s(), %s can not access", __FUNCTION__, SOUND_TRIGGER_HAL_LIBRARY_PATH);
    }


    /* sync kernel status */
    int ret;
    ret = mixer_ctl_get_value(mixer_get_ctl_by_name(mMixer, "VA Hostless Enable"), 0);
    if (ret == 1)
        mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "VA Hostless Enable"), 0, 0);
    ret = mixer_ctl_get_value(mixer_get_ctl_by_name(mMixer, "ADSP Enable"), 0);
    if (ret == 1)
        mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "ADSP Enable"), 0, 0);

    stream_attribute_target = new stream_attribute_t;
    memset(stream_attribute_target, 0, sizeof(stream_attribute_t));
    stream_attribute_target->input_source = AUDIO_SOURCE_HOTWORD;
    // Init input stream attribute here
    stream_attribute_target->audio_mode = AUDIO_MODE_NORMAL; // set mode to stream attribute for mic gain setting
    stream_attribute_target->output_devices = AUDIO_DEVICE_NONE; // set output devices to stream attribute for mic gain setting and BesRecord parameter

    besrecord_info_struct_t besrecord;
    //native_preprocess_info_struct_t nativePreprocess_Info;
    memset(&besrecord, 0, sizeof(besrecord_info_struct_t));
    stream_attribute_target->BesRecord_Info = besrecord;

    stream_attribute_target->BesRecord_Info.besrecord_enable = false; // default set besrecord off
    stream_attribute_target->BesRecord_Info.besrecord_bypass_dualmicprocess = false;  // bypass dual MIC preprocess
    stream_attribute_target->NativePreprocess_Info.PreProcessEffect_Update = false;
    stream_attribute_target->sample_rate = 16000;
    stream_attribute_target->audio_format = AUDIO_FORMAT_PCM_16_BIT;
    ALOGD("%s() , stream_attribute_target->BesRecord_Info.besrecord_enable %d", __FUNCTION__, stream_attribute_target->BesRecord_Info.besrecord_enable);

}

AudioALSAAdspVaController::~AudioALSAAdspVaController() {

    ALOGD("%s()", __FUNCTION__);
    if (mStDev.sound_trigger_lib)
        dlclose(mStDev.sound_trigger_lib);
}

bool AudioALSAAdspVaController::getVoiceDetectEnable() {
    AL_AUTOLOCK(mLock);
    return mDetectEnable;
}


status_t AudioALSAAdspVaController::setVoiceDetectEnable(const bool enable) {
    int ret = -1;

    ALOGD("+%s(), mAdspEnable:%d mEnable: %d => %d, mIsUseHeadsetMic = %d",
          __FUNCTION__, mAdspEnable, mDetectEnable, enable, mIsUseHeadsetMic);
    AL_AUTOLOCK(mLock);

    if (mAdspEnable == 0) {
        ALOGE("If ADSP is disable, can not set detect", __FUNCTION__);
        return INVALID_OPERATION;
    }

    if (mDetectEnable == enable) {
        ALOGE("-%s(), enable(%d) == mEnable(%d), return", __FUNCTION__, enable, mDetectEnable);
        return INVALID_OPERATION;
    }

    if (enable) {
        //Set Detect format to 2ch 16Khz 16bit 10ms * 8 periods
        //mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Hostless_Pcm_Format Select"), 0, 16000);
        //mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Hostless_Pcm_Format Select"), 1, 2);
        //mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Hostless_Pcm_Format Select"), 2, 16);
        //mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Hostless_Pcm_Format Select"), 3, 160);
        //mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Hostless_Pcm_Format Select"), 4, 8);
        AudioALSAHardwareResourceManager::getInstance()->startInputDevice(AUDIO_DEVICE_IN_BUILTIN_MIC);
    } else {
        AudioALSAHardwareResourceManager::getInstance()->stopInputDevice(AUDIO_DEVICE_IN_BUILTIN_MIC);
    }

    ret = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "VA Hostless Enable"), 0, enable);
    if (ret != 0) {
        ALOGE("-%s(), ADSP Hostless Detect setting error:%d", __FUNCTION__, ret);
    }

    mDetectEnable = enable;

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAAdspVaController::setAdspEnable(int enable)
{
    int ret;
    AL_AUTOLOCK(mLock);

    ALOGD("+%s(), mAdspEnable:%d => %d, mDetectEnable:%d", __FUNCTION__, mAdspEnable, enable, mDetectEnable);

    if (mAdspEnable == enable)
        return 0;

    if (mDetectEnable && enable == 0) {
        ALOGE("If ADSP detect is enable, can not set adsp off directly", __FUNCTION__);
        return INVALID_OPERATION;
    }

    if (mAdspEnable != 0 && enable != 0)
        return BAD_VALUE;

    ret = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "ADSP Enable"), 0, enable);
    if (ret != 0) {
        ALOGE("-%s(), ADSP Enable setting error:%d", __FUNCTION__, ret);
        return ret;
    }
    if (enable > 0) {
        int count = 0;
        do {
            ret = mixer_ctl_get_value(mixer_get_ctl_by_name(mMixer, "ADSP Enable"), 0);
            if (ret != enable)
                usleep(10000);
            count++;
        } while (ret != enable && count < 50);
        if (count >= 50) {
            ALOGE("-%s(), ADSP status check timeout 300ms", __FUNCTION__);
        }
    }
    mAdspEnable = enable;
    ALOGD("-%s()", __FUNCTION__);

    return NO_ERROR;
}

bool AudioALSAAdspVaController::getAdspEnable()
{
    AL_AUTOLOCK(mLock);
    return mAdspEnable;
}


status_t AudioALSAAdspVaController::updateDeviceInfoForVoiceWakeUp() {
    ALOGV("+%s(), mIsUseHeadsetMic = %d", __FUNCTION__, mIsUseHeadsetMic);

    bool bIsUseHeadsetMic = AudioALSAStreamManager::getInstance()->getDeviceConnectionState(AUDIO_DEVICE_OUT_WIRED_HEADSET);
#if 0 //TODO
    if (bIsUseHeadsetMic != mIsUseHeadsetMic) {
        if (mDetectEnable == false) {
            mIsUseHeadsetMic = bIsUseHeadsetMic;
        } else {
            //TODO Device Route Path Update;
            setVoiceDetectEnable(false);
            mIsUseHeadsetMic = bIsUseHeadsetMic;
            setVoiceDetectEnable(true);
        }
    }
#endif
    ALOGV("-%s(), mIsUseHeadsetMic = %d, bIsUseHeadsetMic = %d", __FUNCTION__, mIsUseHeadsetMic, bIsUseHeadsetMic);
    return NO_ERROR;
}

} // end of namespace android
