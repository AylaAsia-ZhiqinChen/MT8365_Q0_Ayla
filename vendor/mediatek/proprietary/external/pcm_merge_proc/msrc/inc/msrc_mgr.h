#ifndef MSRC_MGR_H
#define MSRC_MGR_H

#include "msrc.h"
#include "msrc_config.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef int   bool;

#define false   0
#define true    1

#define MSRC_RATIO_Q_NUM 24
#define MSRC_RATIO_1_VAL 0x1000000
/* ratio 1/64 ~ 64 */
#define MSRC_RATIO_MAX_VAL (0x40000000)
#define MSRC_RATIO_MIN_VAL (0x40000)
#define MSRC_RATION_MIN_DIFF 0

typedef enum {
	MSRC_PROCESS_NORMAL,
	MSRC_PROCESS_CALLBACK,
}msrc_process_mode_t;

typedef struct msrc_mgr {
	msrc_type_t src_type;
	msrc_process_mode_t proc_mode;
	int32_t chnum;
	int32_t bitdepth;          //now only support 32bit sign-integer
	int32_t samplerate_in;
	int32_t samplerate_out;
	int32_t ratio;
	int32_t ratio_mod_idx;
	int32_t ratio_mod;
	int32_t ratio_mod_period;
	int32_t ratio_update;
	int32_t ratio_mod_update;
	int32_t ratio_mod_period_update;
	int (*process)(struct msrc_mgr *src_mgr, msrc_data_t *src_data);
	int (*reset)(struct msrc_mgr *src_mgr);
	msrc_callback_t cb_obtain_in;
	msrc_callback_t cb_release_in;
	void *cb_data;
	void *private_data;
}msrc_mgr_t;
#endif