#ifndef SOUND_TRIGGER_COMMON
#define SOUND_TRIGGER_COMMON

#ifdef __cplusplus
extern "C" {
#endif

struct pcm_fmt {
    int bits;
    int channels;
    int rate;
};

enum {
    CMD_IN_CTRL_SET_STREAM_PERIOD = 0,
    CMD_IN_CTRL_SET_ADSP,
    CMD_IN_CTRL_SET_ADSP_DETECT,
};

enum {
    PROC_RET_OK = 0,
    PROC_RET_WW_OK,
    PROC_RET_WW_FAIL,
};

enum {
    CMD_PROC_GET_PERIOD = 0x1000,
    CMD_PROC_GET_WW_SUPPORT,
    CMD_PROC_GET_BEAMFORMING,
};

enum {
    RESET_OUT_CTRL_READ_POS = 0x0,
    RESET_OUT_CTRL_WRITE_POS = 0x1,
};

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif
