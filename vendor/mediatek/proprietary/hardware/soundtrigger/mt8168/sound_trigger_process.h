#ifndef _SOUND_TRIGGER_PROCESS_
#define _SOUND_TRIGGER_PROCESS_

#include "sound_trigger_common.h"

#ifdef AIS_STHAL_PROCESS_SUPPORT
#include "ais_2nd_stage.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sound_trigger_process {
    struct pcm_fmt fmt_in;
    struct pcm_fmt fmt_out;
#ifdef AIS_STHAL_PROCESS_SUPPORT
    aispeech_2nd_stage_process aisproc;
#endif
    int period_size;
    int dump_enable;
    int timeout_count;
    int total_count;
    bool wwe_passed;
    void *priv;
    FILE *file_p;
    int (*init)(struct sound_trigger_process *handle, struct pcm_fmt in, struct pcm_fmt *out); // algorithm init, intenal buffer inti
    int (*uninit)(struct sound_trigger_process *handle);  // algorithm uninit, intenal buffer uninit
    int (*process)(struct sound_trigger_process *handle, char *in, char *out, int frames_in, int *frames_out); // return value will include EVENT
    int (*set_parameters)(struct sound_trigger_process *handle, int cmd, void *data);
    int (*get_parameters)(struct sound_trigger_process *handle, int cmd, void *data);
};

int sound_trigger_process_create(struct sound_trigger_process **handle); //init =>, memory allocate to handle
int sound_trigger_process_destroy(struct sound_trigger_process *handle); //uninit =>, memory free

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif
