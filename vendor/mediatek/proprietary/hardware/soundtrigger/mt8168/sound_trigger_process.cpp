#include <log/log.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/properties.h>
#include "sound_trigger_process.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "sound_trigger_process"
#define ADSP_PROC_DUMP_PATH "/data/vendor/audiohal/adsp_proc"
#define STDEV_PROCESS_TIME_OUT 2
#ifndef PROPERTY_VALUE_MAX
#define PROPERTY_VALUE_MAX  92
#endif
static const unsigned int ST_PROCESS_PERIOD_SIZE = 160;

static inline int adsp_proc_frames_to_bytes(struct sound_trigger_process *handle, int frames)
{
    return frames * handle->fmt_in.channels * handle->fmt_in.bits / 8;
}

static int adsp_proc_init(struct sound_trigger_process *handle, struct pcm_fmt in, struct pcm_fmt *out)
{
    char value[PROPERTY_VALUE_MAX];

    ALOGD("%s\n", __func__);
    handle->fmt_in = in;
    /* todo init by algo */
    out->bits = 16;
    out->channels = 1;
    out->rate = 16000;
    handle->fmt_out = *out;

    property_get("vendor.soundtrigger.process.dump", value, "0");
    handle->dump_enable = atoi(value);
    ALOGD("-%s done, vendor.soundtrigger.process.dump = %d\n", __func__, handle->dump_enable);

    if (handle->dump_enable) {
        /* open dump file */
        char file_path[256];
        sprintf(file_path, "%s_%p.pcm", ADSP_PROC_DUMP_PATH, handle);
        handle->file_p = fopen(file_path, "wb");
        ALOGD("%s file %s\n", __func__, file_path);
        if (!handle->file_p) {
            ALOGI("Unable to create file '%s'\n", file_path);
        }
    }

    /* timeout count */
    handle->timeout_count = STDEV_PROCESS_TIME_OUT * out->rate;

    /* reset total_count && wwe_passed */
    handle->total_count = 0;
    handle->wwe_passed = false;

#ifdef AIS_STHAL_PROCESS_SUPPORT
    aispeech_st_clear_result(&handle->aisproc);
#endif

    return 0;
}

static int adsp_proc_uninit(struct sound_trigger_process *handle)
{
    ALOGD("%s\n", __func__);

    if (handle->file_p)
        fclose(handle->file_p);

    return 0;
}

static int adsp_proc_process(struct sound_trigger_process *handle, char *in, char *out, int frames_in, int *frames_out)
{
    /* todo algorithm process ,now is just do channel convert for 2ch 16bit*/
#if 0
    int i;
    int16_t * src;
    int16_t * dst;
    src = (int16_t *)in;
    dst = (int16_t *)out;
    for (i = 0; i < frames_in; i++ ) {
        *dst = *src;
        dst += 1;
        src += 2;
    }
    return 0;
#endif

#ifdef AIS_STHAL_PROCESS_SUPPORT
    int size;
    int size_in;
    int size_out;
    int ret;

    *frames_out = 0;
    size_in = adsp_proc_frames_to_bytes(handle, frames_in);
    size_out = size_in * handle->fmt_out.channels / handle->fmt_in.channels;

    aispeech_2nd_stage_process *proc = &handle->aisproc;
    ret = aispeech_st_second_stage_process(proc, in, size_in);
    if (ret != 0) {
        ALOGI("%s AIS 2nd stage process error: %d\n", __func__, ret);
        return ret;
    }

    if (1 == proc->result_cfg_ptr->is_user_verification_passed) {
        ALOGI("%s AIS second stage verification passed\n", __func__);
        aispeech_st_clear_result(proc);
        /* PROC_RET_WW_OK = 1 */
        ret = PROC_RET_WW_OK;
        handle->wwe_passed = true;
    } else if (!handle->wwe_passed && (handle->total_count > handle->timeout_count)) {
        /* PROC_RET_WW_FAIL = 2 */
        ret = PROC_RET_WW_FAIL;
    }

    /* 2ch in, 1ch out */
    if (proc->ring_buf->avail_data >= size_out) {
        size = hotword_read(proc, out, size_out);
        if (size < 0) {
            ALOGI("%s Error hotword_read\n", __func__);
            *frames_out = 0;

            return size;
        } else {
            *frames_out = frames_in;
            handle->total_count += *frames_out;
            if (handle->file_p) {
                /* dump to file */
                if (fwrite(out, 1, size, handle->file_p) != size)
                    ALOGI("%s Error capturing samples\n", __func__);
            }
        }
    } else {
        ALOGI("%s Warning avail_data is not enough, proc->ring_buf->avail_data = %d\n", __func__, proc->ring_buf->avail_data);
    }

    return ret;
#else
    /* todo algorithm process ,now is just do data transfer*/
    int i;
    int16_t * src;
    int16_t * dst;
    src = (int16_t *)in;
    dst = (int16_t *)out;
    for (i = 0; i < frames_in; i++ ) {
        *dst = *src;
        dst += 1;
        src += 1;
    }

    int size_in;
    int size_out;
    size_in = adsp_proc_frames_to_bytes(handle, frames_in);
    size_out = size_in * handle->fmt_out.channels / handle->fmt_in.channels;
	*frames_out = frames_in;
    handle->total_count += *frames_out;
    if (handle->file_p) {
        /* dump to file */
        if (fwrite(out, 1, size_in, handle->file_p) != size_in)
            ALOGI("%s Error capturing samples\n", __func__);
    }

    return 0;
#endif
}

static int adsp_proc_set_parameters(struct sound_trigger_process *handle, int cmd, void *data)
{
    ALOGD("%s\n", __func__);

    return 0;
}

static int adsp_proc_get_parameters(struct sound_trigger_process *handle, int cmd, void *data)
{
    ALOGD("%s\n", __func__);

    switch(cmd){
    case CMD_PROC_GET_PERIOD:
        /* todo now default setting 160 frames*/
        *(int *)data = ST_PROCESS_PERIOD_SIZE;
        break;
    case CMD_PROC_GET_WW_SUPPORT:
#ifdef AIS_STHAL_PROCESS_SUPPORT
        *(int *)data = 1;
#else
        /* todo now default not support WWE */
        *(int *)data = 0;
#endif
        break;
    case CMD_PROC_GET_BEAMFORMING:
        /* todo */
        break;
    default:
        break;
    }

    return 0;
}



int sound_trigger_process_create(struct sound_trigger_process **handle)
{
    struct sound_trigger_process *adsp_proc = NULL;
    adsp_proc = (struct sound_trigger_process *)malloc(sizeof(struct sound_trigger_process));

    if (!adsp_proc)
        return -ENOMEM;

    memset(adsp_proc, 0, sizeof(struct sound_trigger_process));
    adsp_proc->init = adsp_proc_init;
    adsp_proc->uninit = adsp_proc_uninit;
    adsp_proc->process = adsp_proc_process;
    adsp_proc->set_parameters = adsp_proc_set_parameters;
    adsp_proc->get_parameters = adsp_proc_get_parameters;
    *handle = adsp_proc;

#ifdef AIS_STHAL_PROCESS_SUPPORT
    /* algorithm init */
    memset(&(*handle)->aisproc, 0, sizeof((*handle)->aisproc));
    aispeech_st_second_stage_init(&(*handle)->aisproc);
    hot_word_buffer_init(&(*handle)->aisproc);
#endif

    return 0;
}

int sound_trigger_process_destroy(struct sound_trigger_process *handle)
{
    ALOGD("%s\n", __func__);
    if (!handle)
        return 0;

#ifdef AIS_STHAL_PROCESS_SUPPORT
    /* algorithm uninit */
    aispeech_st_second_stage_deinit(&handle->aisproc);
    hot_word_buffer_deinit(&handle->aisproc);

    free(handle);
        handle = NULL;
#endif

    return 0;
}

#ifdef __cplusplus
}  /* extern "C" */
#endif
