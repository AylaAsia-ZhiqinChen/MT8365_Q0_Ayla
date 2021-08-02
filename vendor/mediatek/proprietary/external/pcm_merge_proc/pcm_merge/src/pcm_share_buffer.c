#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "pcm_share_buffer.h"

int sharebuffer_tstamp_init(struct sharebuffer_tstamp_mgr *mgr, uint32_t unit_frames, uint32_t uint_num, uint32_t frame_size, uint32_t rate)
{
	int ret;
	int i;
	struct sharebuffer_tstamp *unit;

	mgr->unit_frames = unit_frames;
	mgr->frame_size = frame_size;
	mgr->unit_size = mgr->unit_frames * mgr->frame_size;
	mgr->unit_num = uint_num;
	mgr->total_size = mgr->unit_size * mgr->unit_num;
	mgr->rate = rate;

	mgr->start = (uint8_t *)malloc(mgr->total_size);
	if (mgr->start == NULL)
		return -ENOMEM;

	mgr->shm_size = sizeof(struct sharebuffer_tstamp) * mgr->unit_num;
	mgr->shm_size = msb_get_alloc_size((size_t)mgr->shm_size);
	mgr->shm = (void *)malloc(mgr->shm_size);
	if (mgr->shm == NULL) {
		free(mgr->start);
		return -ENOMEM;
	}

	ret = msb_create(&(mgr->server), mgr->shm, mgr->shm_size, MSB_SERVER|MSB_INIT);
	if (ret < 0) {
		free(mgr->start);
		free(mgr->shm);
		return ret;
	}

	unit = (struct sharebuffer_tstamp *)msb_get_start_address(mgr->server);
	for (i = 0; i < mgr->unit_num; i++) {
		unit->buf = mgr->start + (i * mgr->unit_size);
		unit->buf_size = mgr->unit_size;
		unit->data_size = 0;
		unit->frame_count = 0;
		unit->tstamp = 0;
		unit->flag = 0;
		unit++;
	}

	ret = msb_create(&(mgr->client), mgr->shm, mgr->shm_size, MSB_CLIENT);
	if (ret < 0) {
		free(mgr->start);
		free(mgr->shm);
		msb_destroy(mgr->server);
		return ret;
	}

	mgr->inited = 1;
	return 0;
}

int sharebuffer_tstamp_uninit(struct sharebuffer_tstamp_mgr *mgr)
{
	if (!mgr->inited)
		return 0;
	msb_destroy(mgr->client);
	msb_destroy(mgr->server);
	free(mgr->shm);
	free(mgr->start);
	return 0;
}

int sharebuffer_tstamp_reset(struct sharebuffer_tstamp_mgr *mgr)
{
	int ret;
	int i, total, remain_size;
	struct sharebuffer_tstamp *unit;
	msb_buffer_t unit_buf;

	if (!mgr->inited)
		return 0;

	msb_reset_status(mgr->client);
	msb_reset_status(mgr->server);

	remain_size = msb_get_remain_size(mgr->server);
	total = remain_size / sizeof(struct sharebuffer_tstamp);

	for (i = 0; i < total; i++) {
		msb_obtain(mgr->server, &unit_buf, sizeof(struct sharebuffer_tstamp), MSB_OBTAIN_NON_BLOCK|MSB_OBTAIN_FULL);
		unit = (struct sharebuffer_tstamp *)(unit_buf.buf);
		if (unit == NULL)
			continue;
		unit->data_size = 0;
		unit->frame_count = 0;
		unit->flag = 0;
		unit->tstamp = 0;
		msb_release(mgr->server, &unit_buf);
	}

	return 0;
}

int sharebuffer_tstamp_get_remain_size(struct sharebuffer_tstamp_mgr *mgr)
{
	int remain_size;

	if (!mgr->inited)
		return -EPERM;

	remain_size = msb_get_remain_size(mgr->server);
	if (remain_size < 0)
		return remain_size;

	remain_size = remain_size / sizeof(struct sharebuffer_tstamp);
	remain_size *= (int)(mgr->unit_frames);

	return remain_size;
}

void sharebuffer_tstamp_interrupt(struct sharebuffer_tstamp_mgr *mgr)
{
	msb_interrupt(mgr->client);
	msb_interrupt(mgr->server);
}