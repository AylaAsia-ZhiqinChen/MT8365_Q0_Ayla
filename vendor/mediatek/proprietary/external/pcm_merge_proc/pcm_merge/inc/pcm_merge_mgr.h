#ifndef PCM_MERGE_MGR_H
#define PCM_MERGE_MGR_H
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "msrc.h"
#include "pcm_merge_config.h"
#include "pcm_merge.h"
#include "pcm_merge_debug.h"
#include "pcm_bits_convert.h"
#include "pcm_share_buffer.h"

#define MAJOR_VERSION  (0x01)
#define MINOR_VERSION  (0x01)
#define PCM_MERGE_VERSION  (MAJOR_VERSION << 8 | MINOR_VERSION)

typedef enum {
	PCM_MERGE_ST_START,
	PCM_MERGE_ST_STOP,
} pcm_merge_state_t;

enum {
	MERGE_ST_PREPARE = 0,
	MERGE_ST_RUNNING,
};

struct pcm_bits_convert {
	int use;
	int bits_in;
	int bits_out;
	pcm_bits_convert_func process;
};

struct pcm_format {
	int chnum;
	int rate;
	int bitdepth;
	int period_size;
	int period_count;
};

struct sub_read_request {
	uint8_t *buf;
	int size;
	int flag;
	int64_t tstamp;
	int64_t tolerance;
};

struct pcm_merge_mgr {
	pcm_merge_state_t state;
	int config;

	char main_name[32];
	pcm_hw_type_t main_type;
	struct pcm_format main_hal_fmt;
	struct pcm_format main_out_fmt;
	struct pcm_hal_mgr main_hal;
	struct pcm_bits_convert main_bitcvt;

	char sub_name[32];
	pcm_hw_type_t sub_type;
	struct pcm_format sub_hal_fmt;
	struct pcm_format sub_out_fmt;
	msrc_handle_t* sub_src;
	int src_type;
	struct pcm_bits_convert sub_bitcvt;
	struct pcm_hal_mgr sub_hal;
	struct sharebuffer_tstamp_mgr sub_out;

	struct pcm_format out_merge_fmt;
	struct pcm_format out_fmt;
	struct pcm_bits_convert out_bitcvt;
	struct sharebuffer_tstamp_mgr merge_out;
	merge_read_callback cb;
	void *cb_priv;
	int64_t ts_offset;
	int use_main_chnum;
	int use_sub_chnum;
	int chnum_idx[PCM_MERGE_MAX_CHANNEL];

	pthread_t h_main_thread;
	pthread_t h_sub_thread;
	pthread_t h_out_thread;

	int is_main_use;
	int is_main_task_enable;
	int is_sub_use;
	int is_sub_task_enable;
	int is_out_use;
	int is_out_task_enable;
};

#endif
