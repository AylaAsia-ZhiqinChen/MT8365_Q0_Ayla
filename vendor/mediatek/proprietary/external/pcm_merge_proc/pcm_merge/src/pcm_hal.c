#include <stdio.h>
#include <errno.h>
#include "pcm_hal.h"
#include "pcm_hal_alsa_lib.h"
#include "pcm_hal_tiny_alsa.h"

int pcm_hal_create(struct pcm_hal_mgr *mgr, pcm_hw_type_t type)
{
	int ret = 0;
	switch (type) {
	case PCM_HW_ALSA_LIB:
		ret = pcm_hal_alsa_lib_create(mgr);
		break;
	case PCM_HW_TINY_ALSA:
		ret = pcm_hal_tiny_alsa_create(mgr);
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}

int pcm_hal_destroy(struct pcm_hal_mgr *mgr)
{
	int ret = 0;
	switch (mgr->type) {
	case PCM_HW_ALSA_LIB:
		ret = pcm_hal_alsa_lib_destroy(mgr);
		break;
	case PCM_HW_TINY_ALSA:
		ret = pcm_hal_tiny_alsa_destroy(mgr);
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}