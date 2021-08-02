#include <cutils/properties.h>
#include <log/log.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <media/AudioParameter.h>
#include "sound_trigger_in_ctrl.h"
#include "hardware_legacy/AudioMTKHardwareInterface.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "sound_trigger_in_ctrl"

using namespace android;
#define ADSP_IN_DUMP_PATH "/data/vendor/audiohal/adsp_in"
#define HAL_LIBRARY_PATH1 "/system/lib/hw"
#define HAL_LIBRARY_PATH2 "/vendor/lib/hw"
#define AUDIO_HAL_PREFIX "audio.primary"
#define PLATFORM_ID "ro.board.platform"
#define BOARD_PLATFORM_ID "ro.board.platform"

static const String8 keySetDSP = String8("MTK_ADSP_ENABLE");
static const String8 keySetDSPDet = String8("MTK_ADSP_DETECT_ENABLE");
static const String8 keySetStreamPeriod = String8("MTK_ADSP_STREAM_PERIOD");
static const unsigned int CARD = 0;
static const unsigned int DEVICE = 10;
static const unsigned int ST_IN_CTRL_BITS = 16;
static const unsigned int ST_IN_CTRL_CH = 1;
static const unsigned int ST_IN_CTRL_RATE = 16000;
static const unsigned int ST_IN_CTRL_PERIOD_SIZE = 160;
static const unsigned int ST_IN_CTRL_PERIOD_COUNT = 8;

AudioMTKHardwareInterface *gAudioMTKHardware = NULL;
void *AudioHwhndl = NULL;

static int soundtrigger_dlopen()
{
    if (AudioHwhndl == NULL) {
        char prop[PATH_MAX];
        char path[PATH_MAX];
        do {
            if (property_get(PLATFORM_ID, prop, NULL) == 0) {
                snprintf(path, sizeof(path), "%s/%s.default.so",
                         HAL_LIBRARY_PATH1, prop);
                if (access(path, R_OK) == 0) { break; }


                if (access(path, R_OK) == 0) { break; }
            } else {
                snprintf(path, sizeof(path), "%s/%s.%s.so",
                         HAL_LIBRARY_PATH1, AUDIO_HAL_PREFIX, prop);
                if (access(path, R_OK) == 0) { break; }

                snprintf(path, sizeof(path), "%s/%s.%s.so",
                         HAL_LIBRARY_PATH2, AUDIO_HAL_PREFIX, prop);
                if (access(path, R_OK) == 0) { break; }

                if (property_get(BOARD_PLATFORM_ID, prop, NULL) == 0) {
                    snprintf(path, sizeof(path), "%s/%s.default.so",
                             HAL_LIBRARY_PATH1, prop);
                    if (access(path, R_OK) == 0) { break; }

                    snprintf(path, sizeof(path), "%s/%s.default.so",
                             HAL_LIBRARY_PATH2, prop);
                    if (access(path, R_OK) == 0) { break; }
                } else {
                    snprintf(path, sizeof(path), "%s/%s.%s.so",
                             HAL_LIBRARY_PATH1, AUDIO_HAL_PREFIX, prop);
                    if (access(path, R_OK) == 0) { break; }

                    snprintf(path, sizeof(path), "%s/%s.%s.so",
                             HAL_LIBRARY_PATH2, AUDIO_HAL_PREFIX, prop);
                    if (access(path, R_OK) == 0) { break; }
                }
            }
        } while (0);

        ALOGD("Load %s", path);
        AudioHwhndl = dlopen(path, RTLD_NOW);
        const char *dlsym_error1 = dlerror();
        ALOGE("dlerror()=%s", dlsym_error1);
        if (AudioHwhndl == NULL) {
            ALOGE("-DL open AudioHwhndl path [%s] fail", path);
            return -ENOSYS;
        } else {
            create_AudioMTKHw *func1 = (create_AudioMTKHw *)dlsym(AudioHwhndl, "createMTKAudioHardware");
            ALOGD("%s %d func1 %p", __FUNCTION__, __LINE__, func1);
            if (func1 == NULL) {
                ALOGE("-dlsym createMTKAudioHardware fail");
                /* close dlopen */
                dlclose(AudioHwhndl);
                AudioHwhndl = NULL;
                return -ENOSYS;
            }
            gAudioMTKHardware = func1();
            ALOGD("dlopen success gAudioMTKHardware");
        }
    }
    return 0;
}

static int adsp_detect_enable(struct sound_trigger_in_ctrl *obj, bool enable)
{
    int ret = 0;
    int count = 0;
    AudioParameter param = AudioParameter();

    param.addInt(keySetDSPDet, (int)enable);
    pthread_mutex_lock(&obj->in_ctrl_lock);
    count = obj->detect_cnt;
    ALOGD("%s detect_cnt(%d) enable(%d) param(%s)\n", __func__, obj->detect_cnt, enable, param.toString().string());

    if (enable) {
        obj->detect_cnt++;
    } else {
        obj->detect_cnt--;
    }

    assert(obj->detect_cnt >= 0);

    if (!obj->adsp_enable && enable) {
        ALOGI("%s(), If ADSP is disable, can not set detect\n", __FUNCTION__);
        return INVALID_OPERATION;
    }

    if (gAudioMTKHardware == NULL) {
        ret = soundtrigger_dlopen();
        if (ret == -ENOSYS) {
            ALOGI("-%s, dlopen error\n", __func__);
            return ret;
        }
    }

    if ((count == 0 && enable) || (count == 1 && !enable))
        ret = gAudioMTKHardware->setParameters(param.toString());

    pthread_mutex_unlock(&obj->in_ctrl_lock);

    return ret;
}

static int adsp_enable(struct sound_trigger_in_ctrl *obj, bool enable)
{
    int ret = 0;
    AudioParameter param = AudioParameter();

    param.addInt(keySetDSP, (int)enable);
    ALOGD("%s adsp_enable(%d) enable(%d) param(%s)\n", __func__, obj->adsp_enable, enable, param.toString().string());

    pthread_mutex_lock(&obj->in_ctrl_lock);
    if (obj->adsp_enable == enable) {
        ALOGD("-%s no need to set the same setting\n", __func__);
        return 0;
    }

    if (obj->adsp_enable && !enable && obj->detect_cnt) {
        ALOGI("%s(), If ADSP DETECT is enable, can not disable ADSP\n", __FUNCTION__);
        assert(0);
    }

    if (gAudioMTKHardware == NULL) {
        ret = soundtrigger_dlopen();
        if (ret == -ENOSYS) {
            ALOGI("-%s, dlopen error\n", __func__);
            return ret;
        }
    }

    ret = gAudioMTKHardware->setParameters(param.toString());

    if (ret != 0) {
        ALOGE("%s(), ADSP Enable setting error:%d\n", __FUNCTION__, ret);
    } else if (enable) {
        obj->adsp_enable = 1;
    } else {
        obj->adsp_enable = 0;
    }
    pthread_mutex_unlock(&obj->in_ctrl_lock);

    return ret;
}

static int adsp_in_init(struct sound_trigger_in_ctrl *obj)
{
    int ret = 0;

    ALOGD("%s\n", __func__);
    /* now fix 1ch 16k 16bit*/
    obj->fmt.bits = ST_IN_CTRL_BITS;
    obj->fmt.channels = ST_IN_CTRL_CH;
    obj->fmt.rate = ST_IN_CTRL_RATE;

    /* release adsp detect */
    if (obj->detect_cnt) {
        /* force set to 1, so we can disable adsp detect finally */
        obj->detect_cnt = 1;
        ret = adsp_detect_enable(obj, false);
        if (ret < 0)
            ALOGE("%s(), Failed to disable adsp_detect_enable, ret = %d\n!", __FUNCTION__, ret);
    }

    /* release adsp */
    if (obj->adsp_enable) {
        ret = adsp_enable(obj, false);
        if (ret < 0)
            ALOGE("%s(), Failed to disable adsp_detect_enable, ret = %d\n!", __FUNCTION__, ret);
    }

    obj->detect_cnt = 0;
    obj->adsp_enable = 0;

    ret = pthread_mutex_init(&obj->in_ctrl_lock, NULL);
    if (ret != 0) {
        ALOGE("%s(), Failed to initialize in_ctrl_lock, ret = %d\n!", __FUNCTION__, ret);
    }

    return ret;
}

static int adsp_in_stream_open(struct sound_trigger_in_ctrl *obj)
{
    struct pcm_config config;
    char value[PROPERTY_VALUE_MAX];

    ALOGD("%s\n", __func__);
    adsp_detect_enable(obj, true);

    property_get("vendor.soundtrigger.in.dump", value, "0");
    obj->dump_enable = atoi(value);
    ALOGD("-%s done, vendor.soundtrigger.in.dump = %d\n", __func__, obj->dump_enable);

    if (obj->dump_enable) {
        /* open dump file */
        char file_path[256];
        sprintf(file_path, "%s_%p.pcm", ADSP_IN_DUMP_PATH, obj);
        obj->file = fopen(file_path, "wb");
        ALOGD("%s file %s\n", __func__, file_path);
        if (!obj->file) {
            ALOGI("Unable to create file '%s'\n", file_path);
        }
    }

    memset(&config, 0, sizeof(config));
    config.channels = obj->fmt.channels;
    config.rate = obj->fmt.rate;
    config.period_size = obj->period_size;
    config.period_count = obj->period_count;
    config.format = (obj->fmt.bits == 16) ? PCM_FORMAT_S16_LE : PCM_FORMAT_S32_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
    ALOGD("%s channels %d, rate %d, period_size %d, period_count %d, format %d \n",
          __func__, config.channels, config.rate, config.period_size, config.period_count, config.format);

    obj->pcm = pcm_open(CARD, DEVICE, PCM_IN, &config);
    if (!obj->pcm || !pcm_is_ready(obj->pcm)) {
        ALOGD("%s Unable to open PCM device (%s)\n", __func__, pcm_get_error(obj->pcm));
        return -1;
    }

    ALOGD("%s done\n", __func__);

    return 0;
}

static int adsp_in_stream_close(struct sound_trigger_in_ctrl *obj)
{
    struct timespec time_st, time_mid, time_ed;

    ALOGD("%s\n", __func__);
    adsp_detect_enable(obj, false);
    if (!obj->pcm) {
        ALOGD("%s pcm handler is null.\n", __func__);
        return 0;
    }

    if (obj->file)
        fclose(obj->file);

    clock_gettime(CLOCK_MONOTONIC, &time_st);
    pcm_stop(obj->pcm);
    clock_gettime(CLOCK_MONOTONIC, &time_mid);
    pcm_close(obj->pcm);
    clock_gettime(CLOCK_MONOTONIC, &time_ed);
    ALOGD("va_record stop time:%d\n", ((int)(time_ed.tv_sec - time_st.tv_sec)* 1000000 + (int)(time_ed.tv_nsec - time_st.tv_nsec)/1000));
    ALOGD("va_record pcm_stop time:%d\n", ((int)(time_mid.tv_sec - time_st.tv_sec)* 1000000 + (int)(time_mid.tv_nsec - time_st.tv_nsec)/1000));

    return 0;
}

static int adsp_in_stream_read(struct sound_trigger_in_ctrl *obj, char *buf, int frames)
{
    unsigned int size;
    int ret = 0;

    if(obj->period_size != frames)
        ALOGI(" The read frames is not equal period_size!\n");

    size = pcm_frames_to_bytes(obj->pcm, frames);

    /* read from adsp in*/
    ret = pcm_read(obj->pcm, buf, size);
    if (ret != 0) {
        ALOGD("%s pcm_read error!\n", __func__);
        return ret;
    }

    if (obj->file) {
        /* dump to file */
        if (fwrite(buf, 1, size, obj->file) != size) {
            ALOGI("Error capturing samples\n");
        }
    }

    return frames;
}

static int adsp_in_set_params(struct sound_trigger_in_ctrl *obj, int cmd, void *data)
{
    int ret = 0;
    int value = *(int *)data;
    AudioParameter param = AudioParameter();

    ALOGD("%s\n", __func__);

    switch(cmd) {
    case CMD_IN_CTRL_SET_ADSP:
        param.addInt(keySetDSP, value);
        ret = adsp_enable(obj, value);
        if (ret != 0)
            ALOGE("%s(), MTK_ADSP_ENABLE setting error:%d\n", __FUNCTION__, ret);
        break;
    case CMD_IN_CTRL_SET_STREAM_PERIOD:
        param.addInt(keySetStreamPeriod, value);

        if (value) {
            obj->period_count = ST_IN_CTRL_PERIOD_COUNT;
            obj->period_size = value;
        } else {
            /* default setting */
            obj->period_count = ST_IN_CTRL_PERIOD_COUNT;
            obj->period_size = ST_IN_CTRL_PERIOD_SIZE;
        }
        break;
    case CMD_IN_CTRL_SET_ADSP_DETECT:
        param.addInt(keySetDSPDet, value);
        ret = adsp_detect_enable(obj, value);
        if (ret != 0)
            ALOGE("%s(), MTK_ADSP_DETECT_ENABLE setting error:%d\n", __FUNCTION__, ret);
        break;
    default:
        break;
    }
    ALOGD("-%s setParameter %s ret = %d\n", __func__, param.toString().string(), ret);

    return ret;
}

int sound_trigger_in_ctrl_create(struct sound_trigger_in_ctrl **handle)
{
    struct sound_trigger_in_ctrl *adsp_in = NULL;
    adsp_in = (struct sound_trigger_in_ctrl *)malloc(sizeof(struct sound_trigger_in_ctrl));

    if (!adsp_in)
        return -ENOMEM;

    memset(adsp_in, 0, sizeof(struct sound_trigger_in_ctrl));
    adsp_in->init = adsp_in_init;
    adsp_in->stream_open = adsp_in_stream_open;
    adsp_in->stream_close = adsp_in_stream_close;
    adsp_in->stream_read = adsp_in_stream_read;
    adsp_in->set_parameters = adsp_in_set_params;
    *handle = adsp_in;

    return 0;
}

int sound_trigger_in_ctrl_destroy(struct sound_trigger_in_ctrl *handle)
{
    ALOGD("%s\n", __func__);
    if (!handle)
        return 0;

    free(handle);
    handle = NULL;

    return 0;
}

#ifdef __cplusplus
}  /* extern "C" */
#endif
