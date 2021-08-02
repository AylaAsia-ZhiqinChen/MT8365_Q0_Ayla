#ifndef PCM_HAL_H
#define PCM_HAL_H
#include <stdint.h>
#include "pcm_merge.h"
#include "pcm_merge_debug.h"
#include "pcm_merge_config.h"

struct pcm_hal_config {
	char name[32];
	int chnum;
	int rate;
	int bitdepth;
	int period_size;
	int buffer_size;
	int start_threshold;
};

struct pcm_hal_mgr {
	pcm_hw_type_t type;
	void *priv;
	int (*pcm_open)(struct pcm_hal_mgr *mgr, struct pcm_hal_config config);
	int (*pcm_start)(struct pcm_hal_mgr *mgr);
	int (*pcm_stop)(struct pcm_hal_mgr *mgr);
	int (*pcm_close)(struct pcm_hal_mgr *mgr);
	int (*pcm_read)(struct pcm_hal_mgr *mgr, void *buffer, int frames, int64_t *tstamp);
};

int pcm_hal_create(struct pcm_hal_mgr *mgr, pcm_hw_type_t type);
int pcm_hal_destroy(struct pcm_hal_mgr *mgr);
#endif