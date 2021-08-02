/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#define LOG_TAG "sound_trigger_hw_mtk"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cutils/uevent.h>
#include <log/log.h>

#include <hardware/hardware.h>
#include <system/sound_trigger.h>
#include <hardware/sound_trigger.h>
#include <hardware_legacy/power.h>
#include "sound_trigger_in_ctrl.h"
#include "sound_trigger_out_ctrl.h"
#include "sound_trigger_process.h"

#define CMD_EXIT       0x00000001
#define CMD_2NDWWE_OK  0x00001000
#define CMD_BYTES_LEN  4
#define STDEV_STREAM_TIME_OUT 300
#define STDEV_PROCESS_INIT_TIME_OUT 1000

#define STDEV_STREAM_UPLOAD_JUMP_WAKEWORD     1
#define STDEV_MIPS_CALC
#define DEBUG_FOR_DEMO //just for ces demo

static const char *st_proc_wake_lock = "st_proc_wake_lock";
static const char *st_cb_wake_lock = "st_cb_wake_lock";
static const char *st_close_wake_lock = "st_close_wake_lock";

enum {
    ST_CMD_GET_STREAM_STATUS = 0x0,
};

static const struct sound_trigger_properties hw_properties = {
        "The Android Open Source Project", // implementor
        "Sound Trigger MTK HAL", // description
        1, // version
        { 0xed7a7d60, 0xc65e, 0x11e3, 0x9be4, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // uuid
        1, // max_sound_models
        1, // max_key_phrases
        1, // max_users
        RECOGNITION_MODE_VOICE_TRIGGER, // recognition_modes
        true, // capture_transition
        0, // max_buffer_ms
        false, // concurrent_capture
        false, // trigger_in_event
        0 // power_consumption_mw
};

struct adsp_sound_trigger_device {
    struct sound_trigger_hw_device device;
    sound_model_handle_t model_handle;
    recognition_callback_t recognition_callback;
    void *recognition_cookie;
    sound_model_callback_t sound_model_callback;
    void *sound_model_cookie;
    pthread_t callback_thread;
    pthread_mutex_t lock;
    pthread_mutex_t proc_lock;
    int cmd_send;
    int cmd_recv;
    struct sound_trigger_recognition_config *config;
    int opened;

    pthread_t process_thread;
    int proc_enable;
    int proc_status;
    int period_size;
    struct sound_trigger_in_ctrl *in_ctrl;
    struct sound_trigger_process *proc;
    struct sound_trigger_out_ctrl *out_ctrl;
};

static struct adsp_sound_trigger_device g_stdev = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .proc_lock = PTHREAD_MUTEX_INITIALIZER,
};

struct adsp_ioctl_model_data {
    long addr_from_user;
    long size_from_user;
};

#define MT8168_ADSP_IOC_MAGIC 'C'
#define ADSP_IOCTL_DYNAMIC_LOAD_MODEL        _IOWR(MT8168_ADSP_IOC_MAGIC, 0x00, unsigned int)

static inline long long stdev_get_systimer(void)
{
     struct timespec time;
     clock_gettime(CLOCK_MONOTONIC, &time);
     return (long long)(time.tv_sec) * 1000000 + (long long)(time.tv_nsec) / 1000;
}

static int stdev_cmd_open(struct adsp_sound_trigger_device *stdev)
{
    int ret;
    int cmd_fds[2];

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, cmd_fds);
    if (ret != 0)
        return ret;

    stdev->cmd_send = cmd_fds[0];
    stdev->cmd_recv = cmd_fds[1];
    return 0;
}

static int stdev_cmd_close(struct adsp_sound_trigger_device *stdev)
{
    if (stdev->cmd_send >= 0) {
        close(stdev->cmd_send);
        stdev->cmd_send = -1;
    }
    if (stdev->cmd_recv >= 0) {
        close(stdev->cmd_recv);
        stdev->cmd_recv = -1;
    }
    return 0;
}

static int stdev_cmd_send(struct adsp_sound_trigger_device *stdev, uint32_t cmd)
{
    int ret;

    if (stdev->cmd_send < 0)
        return -ENODEV;

    ret = write(stdev->cmd_send, (void *)&cmd, CMD_BYTES_LEN);
    return ret;
}

static inline int stdev_cmd_valid(struct adsp_sound_trigger_device *stdev)
{
    return (int)(stdev->cmd_send >= 0 && stdev->cmd_recv >= 0);
}

static char *stdev_sound_trigger_event_alloc(struct adsp_sound_trigger_device *stdev)
{
    char *data;
    struct sound_trigger_phrase_recognition_event *event;

    data = (char *)calloc(1, sizeof(struct sound_trigger_phrase_recognition_event));
    if (!data)
        return NULL;

    event = (struct sound_trigger_phrase_recognition_event *)data;
    event->common.status = RECOGNITION_STATUS_SUCCESS;
    event->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
    event->common.model = stdev->model_handle;

    event->num_phrases = 1;
    if (stdev->config) {
        event->phrase_extras[0].id = stdev->config->phrases[0].id;
        event->phrase_extras[0].recognition_modes =
            stdev->config->phrases[0].recognition_modes;
    }
    event->phrase_extras[0].confidence_level = 100;
    event->phrase_extras[0].num_levels = 1;
    event->phrase_extras[0].levels[0].level = 100;
    event->phrase_extras[0].levels[0].user_id = 0;

    // Signify that all the data is comming through streaming, not through the buffer.
    event->common.capture_available = true;
    event->common.audio_config = AUDIO_CONFIG_INITIALIZER;
    event->common.audio_config.sample_rate = 16000;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;
    return data;
}

static void *process_thread_loop(void *context)
{
    struct adsp_sound_trigger_device *stdev =
            (struct adsp_sound_trigger_device *)context;
    char *read_buf = NULL;
    char *write_buf = NULL;
    struct pcm_fmt fmt;
    int frame_count = 0;
    int frame_out = 0;
    int jump_wakeword = 0;
    int detected_count = 0;
    int total_count = 0;
    int timeout_count;
    int ret;
#ifdef STDEV_MIPS_CALC
    struct timespec time_st, time_ed;
    int cal_enable = 1;
    int total_time = 0;
    int cur_time = 0;
#endif

    /* wake lock */
    acquire_wake_lock(PARTIAL_WAKE_LOCK, st_proc_wake_lock);
    ALOGI("%s start\n", __func__);

    prctl(PR_SET_NAME, "adsp_st_process");

    pthread_mutex_lock(&stdev->proc_lock);

    /* TODO how to process the process lock */
    stdev->proc->get_parameters(stdev->proc, CMD_PROC_GET_PERIOD, &frame_count);
    /* in_ctrl stream open */
    stdev->in_ctrl->set_parameters(stdev->in_ctrl, CMD_IN_CTRL_SET_STREAM_PERIOD, &frame_count);
    ret = stdev->in_ctrl->stream_open(stdev->in_ctrl);
    if (ret != 0)
        goto exit;

    /* process init */
    ret = stdev->proc->init(stdev->proc, stdev->in_ctrl->fmt, &fmt);
    if (ret != 0)
        goto exit1;
    /* out ctrl init */
    ret = stdev->out_ctrl->init(stdev->out_ctrl, fmt, frame_count);
    if (ret != 0)
        goto exit2;

    /* timeout count */
    timeout_count = STDEV_STREAM_TIME_OUT * stdev->out_ctrl->fmt.rate;

    read_buf = (char *)malloc(frame_count * stdev->in_ctrl->fmt.bits / 8 * stdev->in_ctrl->fmt.channels);
    if (read_buf == NULL)
        goto exit3;
    write_buf = (char *)malloc(frame_count * stdev->out_ctrl->fmt.bits / 8 * stdev->out_ctrl->fmt.channels);
    if (write_buf == NULL)
        goto exit3;

    stdev->proc_status = 1;
    pthread_mutex_unlock(&stdev->proc_lock);
    while (stdev->proc_enable) {
        /* Read input data */
        ret = stdev->in_ctrl->stream_read(stdev->in_ctrl, read_buf, frame_count);
        if (ret != frame_count) {
            ALOGI("%s in_ctl read error:%d\n", __func__, ret);
            break;
        }
#ifdef STDEV_MIPS_CALC
        if(cal_enable)
            clock_gettime(CLOCK_MONOTONIC, &time_st);
#endif
        /* do process */
        ret = stdev->proc->process(stdev->proc, read_buf, write_buf, frame_count, &frame_out);
#ifdef STDEV_MIPS_CALC
        if(cal_enable)
            clock_gettime(CLOCK_MONOTONIC, &time_ed);
        cur_time = (int)(time_ed.tv_sec - time_st.tv_sec)* 1000000 + (int)(time_ed.tv_nsec - time_st.tv_nsec)/1000;
        total_time += cur_time;
#endif

        /* check process event */
        if (ret == PROC_RET_WW_OK && detected_count == 0) {
            /* case1, detect ok, send event */
            stdev_cmd_send(stdev, CMD_2NDWWE_OK);
            /* if only need voice command, need update read point to current position */
#ifndef DEBUG_FOR_DEMO
            jump_wakeword = STDEV_STREAM_UPLOAD_JUMP_WAKEWORD;
#endif
            detected_count++;
#ifdef STDEV_MIPS_CALC
            if(cal_enable) {
                ALOGI("%s total_time:%d\n", __func__, total_time);
                cal_enable =0;
            }
#endif
            ALOGI("%s Wakeword detect ok(index:%d)\n", __func__, total_count);
        } else if (ret == PROC_RET_WW_OK && detected_count > 0) {
            /* Do nothing for 2nd or later detect */
            ALOGI("%s Wakeword detect ok(index:%d, detected_count:%d)\n", __func__, total_count, detected_count);
        } else if (ret == PROC_RET_WW_FAIL) {
            /* case2, detect fail, stop the event */
            ALOGI("%s wakeword detect fail, stop the process\n", __func__);
            break;
        } else if (ret < 0) {
            /* now not consider stop the process */
            ALOGI("%s process error(%d)\n", __func__, ret);
        }

        /* out of process data not enough */
        if (frame_out != frame_count)
            continue;

        /* out ctrl write */
        /* out buffer overrun, just stop process flow */
        ret = stdev->out_ctrl->write(stdev->out_ctrl, write_buf, frame_count);
        if (ret != frame_count) {
             ALOGI("%s write error(%d), stop the process\n", __func__, ret);
             break;
        }

        if (jump_wakeword) {
            stdev->out_ctrl->reset(stdev->out_ctrl, RESET_OUT_CTRL_WRITE_POS);
            jump_wakeword = 0;
        }

        total_count+= frame_count;
        /* Timeout process */
        if (total_count >= timeout_count) {
            ALOGI("%s process timeout\n", __func__);
            break;
        }
    }
    pthread_mutex_lock(&stdev->proc_lock);
    stdev->proc_status = 0;

exit3:
    /* out ctrl uninit */
    stdev->out_ctrl->uninit(stdev->out_ctrl);
exit2:
    /* process uninit */
    stdev->proc->uninit(stdev->proc);
exit1:
    /* in_ctrl stream close */
    stdev->in_ctrl->stream_close(stdev->in_ctrl);
exit:
    if (read_buf != NULL)
        free(read_buf);
    if (write_buf != NULL)
        free(write_buf);

    pthread_mutex_unlock(&stdev->proc_lock);
    /* wake lock release */
    release_wake_lock(st_proc_wake_lock);
    ALOGI("%s stop\n", __func__);
    return NULL;
}

static int process_start(struct adsp_sound_trigger_device *stdev)
{
    uint32_t msec;

    pthread_mutex_lock(&stdev->proc_lock);
    /* if process is going, do not create again. */
    if (stdev->proc_enable) {
        pthread_mutex_unlock(&stdev->proc_lock);
        return 0;
    }

    stdev->proc_enable = 1;
    stdev->proc_status = 0;
    pthread_mutex_unlock(&stdev->proc_lock);

    pthread_create(&stdev->process_thread, (const pthread_attr_t *)NULL,
             process_thread_loop, stdev);

    while((stdev->proc_status == 0) && (stdev->proc_enable == 1)) {
        usleep(20 * 1000);
        msec += 20;
        if (msec == STDEV_PROCESS_INIT_TIME_OUT) {
            ALOGI("%s process_thread_loop init timeout %d ms\n", __func__, msec);
            return 0;
        }
        ALOGI("%s usleep %d ms wait process_thread_loop init done\n", __func__, msec);
    }

    return 0;
}

static int process_stop(struct adsp_sound_trigger_device *stdev)
{
    int ret;

    pthread_mutex_lock(&stdev->proc_lock);
    if (!stdev->proc_enable) {
        pthread_mutex_unlock(&stdev->proc_lock);
        return 0;
    }
    stdev->proc_enable = 0;
    stdev->proc_status = 0;
    pthread_mutex_unlock(&stdev->proc_lock);
    pthread_join(stdev->process_thread, (void **)NULL);

    return 0;
}

static void *callback_thread_loop(void *context)
{
    struct adsp_sound_trigger_device *stdev =
            (struct adsp_sound_trigger_device *)context;
    struct sound_trigger_phrase_recognition_event *event;
    uint32_t cmd;
    char uevent_buf[1024];
    struct pollfd fds[2];
    int ret, i;
    int detect_enable = 1;

    acquire_wake_lock(PARTIAL_WAKE_LOCK, st_cb_wake_lock);

    prctl(PR_SET_NAME, "adsp_st_callback");

    pthread_mutex_lock(&stdev->lock);
    if (stdev->recognition_callback == NULL)
        goto exit1;

    process_stop(stdev);

    /* enable ADSP detect */
    ret = stdev->in_ctrl->set_parameters(stdev->in_ctrl, CMD_IN_CTRL_SET_ADSP_DETECT, &detect_enable);
    if (ret != 0) {
        ALOGI("%s adsp detect start error(%d)\n", __func__, ret);
        goto exit1;
    }

    ret = stdev_cmd_open(stdev);
    if (ret != 0)
        goto exit1;

    fds[0].fd = uevent_open_socket(64*1024, true);
    if (fds[0].fd == -1)
        goto exit1;

    fds[0].events = POLLIN;
    fds[1].fd = stdev->cmd_recv;
    fds[1].events = POLLIN;
    pthread_mutex_unlock(&stdev->lock);

    while(1) {
        release_wake_lock(st_cb_wake_lock);
        ret = poll(fds, 2, -1);
        acquire_wake_lock(PARTIAL_WAKE_LOCK, st_cb_wake_lock);
        pthread_mutex_lock(&stdev->lock);
        if (ret < 0) {
            break;
        }
        if (stdev->recognition_callback == NULL) {
            break;
        }
        if (fds[0].revents & POLLIN) {
            ret = uevent_kernel_multicast_recv(fds[0].fd, uevent_buf, 1024);
            if (ret <= 0) {
                ALOGI("%s invalid uevent return value :%d\n", __func__, ret);
                pthread_mutex_unlock(&stdev->lock);
                continue;
            }
            for (i = 0; i < ret; ) {
                if (strstr(uevent_buf + i, "ACTION=HOTWORD")) {
                    int ww_support = 0;
                    ALOGI("%s uevent ACTION=HOTWORD got(%lld)\n", __func__, stdev_get_systimer());
                    if (stdev->proc != NULL) {
                        stdev->proc->get_parameters(stdev->proc, CMD_PROC_GET_WW_SUPPORT, &ww_support);
                        process_stop(stdev);
                        process_start(stdev);
                        usleep(20 * 1000);
                        ALOGI("%s usleep 20ms\n", __func__);
                    }
                    if (!ww_support) {
                         event =
                         (struct sound_trigger_phrase_recognition_event *)stdev_sound_trigger_event_alloc(stdev);
                         if (event) {
                            stdev->recognition_callback(&event->common,
                                            stdev->recognition_cookie);
                            free(event);
                         }
                         goto exit0;
                    }
                }
                i += strlen(uevent_buf + i) + 1;
            }
        }

        if (fds[1].revents & POLLIN) {
            read(fds[1].fd, &cmd, CMD_BYTES_LEN);
            if (cmd == CMD_EXIT) {
                ALOGI("%s CMD_EXIT !\n", __func__);
                break;
            } else if (cmd == CMD_2NDWWE_OK) {
                ALOGI("%s CMD_2NDWWE_OK !\n", __func__);
                /* notify recognition event */
                event =
                (struct sound_trigger_phrase_recognition_event *)stdev_sound_trigger_event_alloc(stdev);
                if (event) {
                    stdev->recognition_callback(&event->common,
                                                stdev->recognition_cookie);
                    free(event);
                }
                /* hotword detect, stop the callback loop */
                break;
            }
        }
        pthread_mutex_unlock(&stdev->lock);
    }
exit0:
    close(fds[0].fd);
exit1:
    stdev_cmd_close(stdev);
    /* dsiable ADSP detect */
    detect_enable = 0;
    ret = stdev->in_ctrl->set_parameters(stdev->in_ctrl, CMD_IN_CTRL_SET_ADSP_DETECT, &detect_enable);
    if (ret != 0) {
        ALOGI("%s adsp detect start error(%d)\n", __func__, ret);
    }

#if 0  //TODO this depends on application setting, may needed for normal app
    if (stdev->config && !stdev->config->capture_requested)
        process_stop(stdev);
#endif
    pthread_mutex_unlock(&stdev->lock);
    release_wake_lock(st_cb_wake_lock);
    return NULL;
}

static int stdev_get_properties(const struct sound_trigger_hw_device *dev,
                                struct sound_trigger_properties *properties)
{
    struct adsp_sound_trigger_device *stdev = (struct adsp_sound_trigger_device *)dev;

    ALOGI("%s", __func__);
    if (properties == NULL)
        return -EINVAL;
    memcpy(properties, &hw_properties, sizeof(struct sound_trigger_properties));
    return 0;
}

static int stdev_load_sound_model(const struct sound_trigger_hw_device *dev,
                                  struct sound_trigger_sound_model *sound_model,
                                  sound_model_callback_t callback,
                                  void *cookie,
                                  sound_model_handle_t *handle)
{
    struct adsp_sound_trigger_device *stdev = (struct adsp_sound_trigger_device *)dev;
    struct adsp_ioctl_model_data model_data;
    int ret = 0;
    int fd = 0;
    int adsp_mode = 1;

    ALOGI("%s stdev %p\n", __func__, stdev);
    pthread_mutex_lock(&stdev->lock);

    if (handle == NULL || sound_model == NULL) {
        pthread_mutex_unlock(&stdev->lock);
        return -EINVAL;
    }

    /* model data size check */
    if (sound_model->data_size == 0 ||
        sound_model->data_offset < sizeof(struct sound_trigger_sound_model)) {
        pthread_mutex_unlock(&stdev->lock);
        return -EINVAL;
    }

    if (stdev->model_handle == 1) {
        pthread_mutex_unlock(&stdev->lock);
        return -ENOSYS;
    }

    /* if process is active, release it */
    process_stop(stdev);

    /* if callback task is active, release it */
    if (stdev->recognition_callback != NULL) {
        stdev_cmd_send(stdev, CMD_EXIT);
        pthread_mutex_unlock(&stdev->lock);
        pthread_join(stdev->callback_thread, (void **)NULL);
        pthread_mutex_lock(&stdev->lock);
        stdev->recognition_callback = NULL;
    }

    /* in_ctrl init */
    ret = stdev->in_ctrl->init(stdev->in_ctrl);
    if (ret != 0) {
        ALOGI("%s in_ctrl init error(%d)\n", __func__, ret);
        pthread_mutex_unlock(&stdev->lock);
        return ret;
    }

    /* adsp enable */
    ret = stdev->in_ctrl->set_parameters(stdev->in_ctrl, CMD_IN_CTRL_SET_ADSP, &adsp_mode);
    if (ret != 0) {
        ALOGI("%s adsp enable error(%d)\n", __func__, ret);
        pthread_mutex_unlock(&stdev->lock);
        return ret;
    }

    /* model load */
    char *data = (char *)sound_model + sound_model->data_offset;
    ALOGI("%s data size %d data %d - %d\n", __func__,
          sound_model->data_size, data[0], data[sound_model->data_size - 1]);
    ALOGI("%s sizeof(adsp_ioctl_model_data) = %d\n", __func__, sizeof(model_data));
    model_data.addr_from_user = (long)data;
    model_data.size_from_user = (long)sound_model->data_size;
    fd = open("/dev/adsp_misc", O_RDWR);
    if (fd < 0) {
        ALOGI("%s open fail:%s, fd:%d\n", __func__, strerror(errno), fd);
        return -1;
    } else {
        ALOGI("%s open ok, fd:%d\n", __func__, fd);
    }
    if ((ret = ioctl(fd, ADSP_IOCTL_DYNAMIC_LOAD_MODEL, (unsigned long)&model_data)) < 0) {
        ALOGI("%s ioctl set fail:%s\n", __func__, strerror(errno));
    }
    close(fd);

    stdev->model_handle = 1;
    stdev->sound_model_callback = callback;
    stdev->sound_model_cookie = cookie;
    *handle = stdev->model_handle;
    ALOGI("%s Sound model loaded: Handle %d\n", __func__, *handle);

    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_unload_sound_model(const struct sound_trigger_hw_device *dev,
                                    sound_model_handle_t handle)
{
    struct adsp_sound_trigger_device *stdev = (struct adsp_sound_trigger_device *)dev;
    int ret = 0;
    int adsp_mode = 0;

    ALOGI("unload_sound_model:%d\n", handle);
    pthread_mutex_lock(&stdev->lock);
    if (handle != 1) {
        pthread_mutex_unlock(&stdev->lock);
        return -EINVAL;
    }
    if (stdev->model_handle == 0) {
        pthread_mutex_unlock(&stdev->lock);
        return -ENOSYS;
    }
    stdev->model_handle = 0;

    /* if process is active, release it */
    process_stop(stdev);

    /* if callback task is active, release it */
    if (stdev->recognition_callback != NULL) {
        stdev_cmd_send(stdev, CMD_EXIT);
        pthread_mutex_unlock(&stdev->lock);
        pthread_join(stdev->callback_thread, (void **)NULL);
        pthread_mutex_lock(&stdev->lock);
        stdev->recognition_callback = NULL;
    }

    if (stdev->config)
        free(stdev->config);
    stdev->config = NULL;

    /* model resourc release */

    /* adsp disable */
    ret = stdev->in_ctrl->set_parameters(stdev->in_ctrl, CMD_IN_CTRL_SET_ADSP, &adsp_mode);
    if (ret != 0) {
        ALOGI("%s adsp disable error(%d)\n", __func__, ret);
    }

    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_start_recognition(const struct sound_trigger_hw_device *dev,
                                   sound_model_handle_t handle,
                                   const struct sound_trigger_recognition_config *config,
                                   recognition_callback_t callback,
                                   void *cookie)
{
    struct adsp_sound_trigger_device *stdev = (struct adsp_sound_trigger_device *)dev;
    int ret = 0;

    ALOGI("%s for model %d\n", __func__, handle);
    pthread_mutex_lock(&stdev->lock);
    if (stdev->model_handle != handle) {
        pthread_mutex_unlock(&stdev->lock);
        return -ENOSYS;
    }

    /* if callback thread is active and running */
    if ((stdev->recognition_callback != NULL) &&
         stdev_cmd_valid(stdev)) {
        pthread_mutex_unlock(&stdev->lock);
        return -ENOSYS;
    }

    /* if process is active, release it */
    process_stop(stdev);

    /* if callback task is active, release it */
    if (stdev->recognition_callback != NULL) {
        pthread_mutex_unlock(&stdev->lock);
        pthread_join(stdev->callback_thread, (void **)NULL);
        pthread_mutex_lock(&stdev->lock);
        stdev->recognition_callback = NULL;
    }

    if (stdev->config)
        free(stdev->config);
    stdev->config = NULL;

    if (config) {
        stdev->config = (struct sound_trigger_recognition_config *)malloc(sizeof(*config));
        if (!stdev->config) {
            pthread_mutex_unlock(&stdev->lock);
            return -ENOMEM;
        }
        memcpy(stdev->config, config, sizeof(*config));
    }

    stdev->recognition_callback = callback;
    stdev->recognition_cookie = cookie;
    pthread_create(&stdev->callback_thread, (const pthread_attr_t *)NULL,
                   callback_thread_loop, stdev);
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_stop_recognition(const struct sound_trigger_hw_device *dev,
            sound_model_handle_t handle)
{
    struct adsp_sound_trigger_device *stdev = (struct adsp_sound_trigger_device *)dev;
    int ret = 0;

    ALOGI("%s for model %d\n", __func__, handle);
    pthread_mutex_lock(&stdev->lock);
    if (stdev->model_handle != handle) {
        pthread_mutex_unlock(&stdev->lock);
        return -ENOSYS;
    }
    if (stdev->recognition_callback == NULL) {
        pthread_mutex_unlock(&stdev->lock);
        return -ENOSYS;
    }

    stdev_cmd_send(stdev, CMD_EXIT);
    pthread_mutex_unlock(&stdev->lock);
    pthread_join(stdev->callback_thread, (void **)NULL);
    pthread_mutex_lock(&stdev->lock);
    stdev->recognition_callback = NULL;
    stdev->recognition_cookie = NULL;
    if (stdev->config)
        free(stdev->config);
    stdev->config = NULL;
    ALOGI("%s pthread_join done\n", __func__);

    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((visibility ("default")))
int sound_trigger_open_for_streaming()
{
    struct adsp_sound_trigger_device *stdev = &g_stdev;
    int ret = 1;

    pthread_mutex_lock(&stdev->proc_lock);
    if (!stdev->proc_status) {
        ret = -ENODEV;
        goto exit;
    }
    ALOGI("%s()\n", __func__);
exit:
    pthread_mutex_unlock(&stdev->proc_lock);
    return ret;
}

__attribute__ ((visibility ("default")))
size_t sound_trigger_read_samples(int audio_handle, void *buffer, size_t  buffer_len)
{
    struct adsp_sound_trigger_device *stdev = &g_stdev;
    size_t ret = 0;

    /* TODO how to do resource protect */
    if (audio_handle <= 0)
        return (size_t)(-EINVAL);

    pthread_mutex_lock(&stdev->proc_lock);
    if (!stdev->proc_status) {
        ret = (size_t)(-ENODEV);
        goto exit;
    }
    /* data read, need protect?, if so this step should not block too much time */
    pthread_mutex_unlock(&stdev->proc_lock);
    ret = stdev->out_ctrl->read(stdev->out_ctrl, (char *)buffer, (int)buffer_len);
    pthread_mutex_lock(&stdev->proc_lock);
    if ((ret != buffer_len) || (!stdev->proc_status))
        ret = -ENODEV;

exit:
    pthread_mutex_unlock(&stdev->proc_lock);
    return ret;
}

static void *stream_thread_loop(void *context)
{
    struct adsp_sound_trigger_device *stdev =
        (struct adsp_sound_trigger_device *)context;

    acquire_wake_lock(PARTIAL_WAKE_LOCK, st_close_wake_lock);
    usleep(500 * 1000);
    release_wake_lock(st_close_wake_lock);
    ALOGI("%s()\n", __func__);

    return NULL;
}

__attribute__ ((visibility ("default")))
int sound_trigger_close_for_streaming(int audio_handle __unused)
{

    struct adsp_sound_trigger_device *stdev = &g_stdev;
    ALOGI("+%s()\n", __func__);

    if (audio_handle <= 0)
        return -EINVAL;

    process_stop(stdev);

    pthread_t stream_thread;
    pthread_create(&stream_thread, (const pthread_attr_t *)NULL,
                   stream_thread_loop, stdev);
    pthread_detach(stream_thread);

    ALOGI("-%s()\n", __func__);
    return 0;
}

__attribute__ ((visibility ("default")))
int sound_trigger_get_parameters(int cmd, void *data)
{
    struct adsp_sound_trigger_device *stdev = &g_stdev;
    int ret = 0;

    pthread_mutex_lock(&stdev->lock);
    if (!stdev->opened) {
        ret = -ENODEV;
        goto exit;
    }

    switch (cmd) {
    case ST_CMD_GET_STREAM_STATUS:
        {
            *((int *)data) = (int)(stdev->proc_status);
        }
        break;
    default:
        break;
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

#ifdef __cplusplus
}  /* extern "C" */
#endif

static int stdev_close(hw_device_t *device)
{
    struct adsp_sound_trigger_device *stdev = (struct adsp_sound_trigger_device *)device;

    pthread_mutex_lock(&stdev->lock);
    stdev->opened = 0;

    if (stdev->recognition_callback != NULL) {
        stdev_cmd_send(stdev, CMD_EXIT);
        pthread_mutex_unlock(&stdev->lock);
        pthread_join(stdev->callback_thread, (void **)NULL);
        pthread_mutex_lock(&stdev->lock);
        stdev->recognition_callback = NULL;
    }

    if (stdev->config)
        free(stdev->config);
    stdev->config = NULL;

    process_stop(stdev);

    /* out_ctrl destroy */
    sound_trigger_out_ctrl_destroy(stdev->out_ctrl);
    stdev->out_ctrl = NULL;
    /* process detroy */
    sound_trigger_process_destroy(stdev->proc);
    stdev->proc = NULL;
    /* in_ctrl detroy */
    sound_trigger_in_ctrl_destroy(stdev->in_ctrl);
    stdev->in_ctrl = NULL;
    pthread_mutex_unlock(&stdev->lock);

    return 0;
}

static int stdev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device) {
    struct adsp_sound_trigger_device *stdev = &g_stdev;
    int ret;

    if (strcmp(name, SOUND_TRIGGER_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    pthread_mutex_lock(&stdev->lock);
    /* in_ctrl create */
    ret = sound_trigger_in_ctrl_create(&stdev->in_ctrl);
    if ((ret != 0) || (stdev->in_ctrl == NULL)) {
        pthread_mutex_unlock(&stdev->lock);
        return ret;
    }
    /* process create */
    sound_trigger_process_create(&stdev->proc);

    /* out_ctrl create */
    sound_trigger_out_ctrl_create(&stdev->out_ctrl);

    stdev->device.common.tag = HARDWARE_DEVICE_TAG;
    stdev->device.common.version = SOUND_TRIGGER_DEVICE_API_VERSION_1_0;
    stdev->device.common.module = (struct hw_module_t *) module;
    stdev->device.common.close = stdev_close;
    stdev->device.get_properties = stdev_get_properties;
    stdev->device.load_sound_model = stdev_load_sound_model;
    stdev->device.unload_sound_model = stdev_unload_sound_model;
    stdev->device.start_recognition = stdev_start_recognition;
    stdev->device.stop_recognition = stdev_stop_recognition;

    stdev->cmd_send = -1;
    stdev->cmd_recv = -1;
    stdev->opened = 1;

    *device = &stdev->device.common;
    pthread_mutex_unlock(&stdev->lock);
    ALOGI("Starting control thread for the mtk hal.");

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = stdev_open,
};

struct sound_trigger_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SOUND_TRIGGER_DEVICE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SOUND_TRIGGER_HARDWARE_MODULE_ID,
        .name = "MTK Audio HW HAL",
        .author = "MTK",
        .methods = &hal_module_methods,
        .dso = NULL,
        .reserved = {0},
    },
};

