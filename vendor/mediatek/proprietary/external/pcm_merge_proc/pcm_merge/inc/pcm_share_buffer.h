#ifndef PCM_SHARE_BUFFER_H
#define PCM_SHARE_BUFFER_H
#include <stdint.h>
#include "msb_inc.h"

struct sharebuffer_tstamp {
	uint8_t  *buf;
	uint32_t data_size;
	uint32_t buf_size;
	uint32_t frame_count;
	int32_t  flag;
	int64_t  tstamp;
};

struct sharebuffer_tstamp_mgr {
	uint8_t  *start;
	uint32_t total_size;
	uint32_t unit_size;
	uint32_t unit_num;
	uint32_t unit_frames;
	uint32_t frame_size;
	uint32_t rate;

	size_t shm_size;
	void *shm;
	msb_handle_t *server;
	msb_handle_t *client;

	int inited;
};

int sharebuffer_tstamp_init(struct sharebuffer_tstamp_mgr *mgr, uint32_t unit_frames, uint32_t uint_num, uint32_t frame_size, uint32_t rate);
int sharebuffer_tstamp_uninit(struct sharebuffer_tstamp_mgr *mgr);
int sharebuffer_tstamp_reset(struct sharebuffer_tstamp_mgr *mgr);
int sharebuffer_tstamp_get_remain_size(struct sharebuffer_tstamp_mgr *mgr);
void sharebuffer_tstamp_interrupt(struct sharebuffer_tstamp_mgr *mgr);

#endif