#ifndef _SOUND_TRIGGER_IN_CTRL_
#define _SOUND_TRIGGER_IN_CTRL_
#include "sound_trigger_common.h"
#include <pthread.h>
#include <tinyalsa/asoundlib.h>
#ifdef __cplusplus
extern "C" {
#endif

struct sound_trigger_in_ctrl {
    struct pcm_fmt fmt;
    struct pcm *pcm;
    int period_size;
    int period_count;
    int detect_cnt;
    int dump_enable;
    bool adsp_enable;
    void *priv;
    FILE *file;
    pthread_mutex_t in_ctrl_lock;
    int (*init)(struct sound_trigger_in_ctrl *handle);         //count =>0, adsp, detect => off
    int (*stream_open)(struct sound_trigger_in_ctrl *handle);  //open va record, adsp, detect count++
    int (*stream_close)(struct sound_trigger_in_ctrl *handle); //close va record, adsp, detect count--
    int (*stream_read)(struct sound_trigger_in_ctrl *handle, char *buf, int frames);  //pcm read
    int (*set_parameters)(struct sound_trigger_in_ctrl *handle, int cmd, void *data); // CMD_IN_CTRL_SET_STREAM_PERIOD, CMD_IN_CTRL
};
int sound_trigger_in_ctrl_create(struct sound_trigger_in_ctrl **handle); //init =>, memory allocate to handle, sym link to audio hal
int sound_trigger_in_ctrl_destroy(struct sound_trigger_in_ctrl *handle); //uninit =>, memory free, sym link close

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif
