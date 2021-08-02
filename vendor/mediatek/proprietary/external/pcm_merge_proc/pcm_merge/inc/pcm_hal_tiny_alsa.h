#ifndef PCM_HAL_TINY_ALSA_H
#define PCM_HAL_TINY_ALSA_H
#include "pcm_hal.h"

int pcm_hal_tiny_alsa_create(struct pcm_hal_mgr *mgr);
int pcm_hal_tiny_alsa_destroy(struct pcm_hal_mgr *mgr);

#endif