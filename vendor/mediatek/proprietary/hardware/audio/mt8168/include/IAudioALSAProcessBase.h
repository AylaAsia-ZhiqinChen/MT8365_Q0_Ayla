#ifndef ANDROID_IAUDIO_ALSA_PROCESS_BASE_H
#define ANDROID_IAUDIO_ALSA_PROCESS_BASE_H

#include <pthread.h>


#include "AudioType.h"
#include "AudioUtility.h"

#include <audio_ringbuf.h>

#include <AudioLock.h>

namespace android {

typedef struct proc_fmt_desc {
    audio_format_t format;
    uint32_t       channels;
    uint32_t       rates;
} proc_fmt_desc_t;

class IAudioALSAProcessBase {
public:
    virtual ~IAudioALSAProcessBase() = 0;

    virtual int init(proc_fmt_desc_t in, proc_fmt_desc_t *out) = 0;

    virtual int uninit(void) = 0;

    virtual int process(char *in, char *out, uint32_t in_size) = 0;

    virtual int setParameters(int cmd, void *data) = 0;

    virtual int getParameters(int cmd, void *data) = 0;

    static const int CMD_GET_IN_PERIOD_MS = 0x0;
    static const int CMD_GET_IN_RATES     = 0x1;

protected:
    bool mInit;
    proc_fmt_desc_t inFmt;
    proc_fmt_desc_t outFmt;
    uint32_t frameCount;
    void *procHandle;
};

} // end namespace android

#endif // end of ANDROID_IAUDIO_ALSA_PROCESS_BASE_H