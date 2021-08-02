#ifndef PCM_HAL_ALSA_LIB_H
#define PCM_HAL_ALSA_LIB_H
#include "pcm_hal.h"

int pcm_hal_alsa_lib_create(struct pcm_hal_mgr *mgr);
int pcm_hal_alsa_lib_destroy(struct pcm_hal_mgr *mgr);

#endif