#include "sound_trigger_out_ctrl.h"
#include <log/log.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "sound_trigger_out_ctrl"
/* remain 8sec */
#define ADSP_VOICE_TIME 10
#define ADSP_OUT_DUMP_PATH "/data/vendor/audiohal/adsp_out"
#ifndef PROPERTY_VALUE_MAX
#define PROPERTY_VALUE_MAX  92
#endif

static inline int adsp_out_frames_to_bytes(struct sound_trigger_out_ctrl *handle, int frames)
{
    return frames * handle->fmt.channels * handle->fmt.bits / 8;
}

static int adsp_out_init(struct sound_trigger_out_ctrl *handle, struct pcm_fmt fmt, int period_size)
{
    int ret = 0;
    char value[PROPERTY_VALUE_MAX];

    /* todo, now fix 2ch 16k 16bit*/
    handle->fmt.bits = fmt.bits ? fmt.bits : 16;
    handle->fmt.channels = fmt.channels ? fmt.channels : 1;
    handle->fmt.rate = fmt.rate ? fmt.rate : 16000;
    handle->period_size = period_size;
    ALOGD("%s bits=%d, ch=%d, rate=%d, period_size=%d\n",
          __func__, handle->fmt.bits, handle->fmt.channels,
          handle->fmt.rate, handle->period_size);

    /* 16k 1ch 16bit 4s 128000 bytes*/
    size_t shm_bytes = ADSP_VOICE_TIME * handle->fmt.rate * handle->fmt.bits / 8 * handle->fmt.channels;

    ALOGE("shm_bytes(%zu) handle->buf_len(%zu) handle->buf(%p)\n", shm_bytes, handle->buf_len, handle->buf);
    /* add cblk header size */
    handle->buf_len = msb_get_alloc_size(shm_bytes);
    handle->buf = malloc(shm_bytes);
    ALOGE(" handle->buf_len(%zu) handle->buf(%p)\n", handle->buf_len, handle->buf);

    if (handle->buf == NULL) {
        ALOGE("msb share buffer(%zu) malloc fail.\n", handle->buf_len);
        return -ENOMEM;
    }

    ret = msb_create(&handle->async_server, handle->buf, handle->buf_len, MSB_SERVER | MSB_THREAD | MSB_INIT);
    if (ret != 0) {
        ALOGE("msb_create client fail(%d).\n", ret);
        return ret;
    }

    ret = msb_create(&handle->async_client, handle->buf, handle->buf_len, MSB_CLIENT | MSB_THREAD);
    if (ret != 0) {
        ALOGE("msb_create server fail(%d).\n", ret);
        return ret;
    }

    property_get("vendor.soundtrigger.out.dump", value, "0");
    handle->dump_enable = atoi(value);
    ALOGD("-%s done, vendor.soundtrigger.out.dump = %d\n", __func__, handle->dump_enable);

    if (handle->dump_enable) {
        /* open dump file */
        char file_path[256];
        sprintf(file_path, "%s_%p.pcm", ADSP_OUT_DUMP_PATH, handle);
        handle->file_o = fopen(file_path, "wb");
        ALOGD("%s file %s\n", __func__, file_path);
        if (!handle->file_o) {
            ALOGI("Unable to create file '%s'.\n", file_path);
        }
    }

    handle->out_status = 1;

    return 0;
}

static int adsp_out_uninit(struct sound_trigger_out_ctrl *handle)
{
    ALOGD("%s\n", __func__);

    if(handle->async_client) {
        msb_destroy(handle->async_client);
        handle->async_client = NULL;
    }

    if(handle->async_server) {
        msb_destroy(handle->async_server);
        handle->async_server = NULL;
    }

    if(handle->file_o)
        fclose(handle->file_o);

    if (handle->buf != NULL)
        free(handle->buf);
    handle->buf = NULL;

    handle->out_status = 0;

    return 0;
}

static int adsp_out_reset(struct sound_trigger_out_ctrl *handle, int flag)
{
    ALOGD("%s\n", __func__);
    int err;
    size_t size = 0;
    msb_buffer_t buffer;
    memset(&buffer, 0, sizeof(msb_buffer_t));

    if (flag == RESET_OUT_CTRL_WRITE_POS) {
        size = msb_get_remain_size(handle->async_server);
        ALOGD("%s remain size = %zu\n", __func__, size);
        while (size > 0) {
            err = msb_obtain(handle->async_server, &buffer, size, MSB_OBTAIN_BLOCK | MSB_OBTAIN_FULL);
            if (err < 0) {
                ALOGE("msb_obtain fail(%d)", err);
                return err;
            }
            size -= buffer.buf_size;
            msb_release(handle->async_server, &buffer);
            ALOGD("%s buffer.buf_size = %zu\n", __func__, buffer.buf_size);
        }
    } else if (flag == RESET_OUT_CTRL_READ_POS) {
        /* todo */
    }
    return 0;
}

static int adsp_out_read(struct sound_trigger_out_ctrl *handle, char *buf, int frames)
{
    int err;
    msb_buffer_t buffer;
    memset(&buffer, 0, sizeof(msb_buffer_t));
    int read_process_bytes = adsp_out_frames_to_bytes(handle, frames);

    if(!handle->out_status) {
        ALOGD("%s out_ctrl has been uninit!\n", __func__);
        return -1;
    }

    /* ALOGD("%s read_process_bytes %d.\n", __func__, read_process_bytes); */
    while (read_process_bytes > 0) {
        err = msb_obtain(handle->async_server, &buffer, read_process_bytes, MSB_OBTAIN_BLOCK | MSB_OBTAIN_FULL);
        if (err < 0) {
            ALOGE("msb_obtain fail(%d)", err);
            return err;
        }
        memcpy(buf, buffer.buf, buffer.buf_size);
        read_process_bytes -= buffer.buf_size;
        msb_release(handle->async_server, &buffer);
        /* ALOGD("%s buffer.buf_size = %zu\n", __func__, buffer.buf_size); */
    }
    return frames;
}

static int adsp_out_write(struct sound_trigger_out_ctrl *handle, char *buf, int frames)
{
    int err;
    msb_buffer_t buffer;
    memset(&buffer, 0, sizeof(msb_buffer_t));
    const int write_process_bytes = adsp_out_frames_to_bytes(handle, frames);

    if(!handle->out_status) {
        ALOGD("%s out_ctrl has been uninit!\n", __func__);
        return -1;
    }

    if (msb_get_remain_size(handle->async_client) < (size_t)write_process_bytes) {
        ALOGE("%s msb_get_remain_size not enough\n", __func__);
        return 0;
    }

    err = msb_obtain(handle->async_client, &buffer, write_process_bytes, MSB_OBTAIN_NON_BLOCK | MSB_OBTAIN_FULL);
    /*ALOGE("%s after  msb_obtain, err %d, buffer.buf_size %zu\n", __func__, err, buffer.buf_size); */
    if (err < 0) {
        ALOGE("%s msb_obtain fail(%d)\n", __func__, err);
        return err;
    }

    if (buffer.buf_size != (size_t)write_process_bytes) {
        ALOGE("%s buffer.buf_size(%zu) != output_process_bytes(%d)", __func__, buffer.buf_size, write_process_bytes);
    } else {
        memcpy(buffer.buf, buf, buffer.buf_size);
        /* ALOGE("%s buffer.buf_size = %zu\n", __func__, buffer.buf_size); */
    }

    msb_release(handle->async_client, &buffer);

    if (handle->file_o) {
        /* dump to file */
        if (fwrite(buffer.buf, 1, write_process_bytes, handle->file_o) != (size_t)write_process_bytes) {
            ALOGI("Error capturing samples\n");
        }
    }

    return frames;
}

int sound_trigger_out_ctrl_create(struct sound_trigger_out_ctrl **handle)
{
    struct sound_trigger_out_ctrl *adsp_out = NULL;
    adsp_out = (struct sound_trigger_out_ctrl *)malloc(sizeof(struct sound_trigger_out_ctrl));

    if (!adsp_out)
        return -ENOMEM;

    memset(adsp_out, 0, sizeof(struct sound_trigger_out_ctrl));
    adsp_out->init = adsp_out_init;
    adsp_out->uninit = adsp_out_uninit;
    adsp_out->reset = adsp_out_reset;
    adsp_out->write = adsp_out_write;
    adsp_out->read = adsp_out_read;
    *handle = adsp_out;

    return 0;
}

int sound_trigger_out_ctrl_destroy(struct sound_trigger_out_ctrl *handle)
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
