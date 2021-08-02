#ifndef _SOUND_TRIGGER_OUT_CTRL_
#define _SOUND_TRIGGER_OUT_CTRL_
#include <pthread.h>
#include "sound_trigger_common.h"
#include "msb_inc.h"
#ifdef __cplusplus
extern "C" {
#endif

struct sound_trigger_out_ctrl {
    struct pcm_fmt fmt;
    int period_size;
    int out_status;
    int dump_enable;
    size_t  buf_len;
    void *buf;
    void *priv;
    FILE *file_o;
    msb_handle_t *async_server; /* for read thread */
    msb_handle_t *async_client; /* for write thread */
    int (*init)(struct sound_trigger_out_ctrl *handle, struct pcm_fmt fmt, int period_size);
    int (*uninit)(struct sound_trigger_out_ctrl *handle);
    int (*reset)(struct sound_trigger_out_ctrl *handle, int flag);
    int (*read)(struct sound_trigger_out_ctrl *handle, char *buf, int frames);  /* block, non-periods */
    int (*write)(struct sound_trigger_out_ctrl *handle, char *buf, int frames); /* non-block, periods */
};
int sound_trigger_out_ctrl_create(struct sound_trigger_out_ctrl **handle); //init =>, memory allocate
int sound_trigger_out_ctrl_destroy(struct sound_trigger_out_ctrl *handle); //uninit =>, memory free

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif
