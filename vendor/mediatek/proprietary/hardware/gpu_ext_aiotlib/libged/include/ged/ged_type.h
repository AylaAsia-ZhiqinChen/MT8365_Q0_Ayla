/*
 * Copyright (C) 2011-2014 MediaTek Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __GED_TYPE_H__
#define __GED_TYPE_H__

typedef enum GED_ERROR_TAG {
	GED_OK,
	GED_ERROR_FAIL,
	GED_ERROR_OOM ,
	GED_ERROR_OUT_OF_FD,
	GED_ERROR_FAIL_WITH_LIMIT,
	GED_ERROR_TIMEOUT,
	GED_ERROR_CMD_NOT_PROCESSED,
	GED_ERROR_INVALID_PARAMS,
	GED_ERROR_INTENTIONAL_BLOCK,
} GED_ERROR;

typedef void *GED_HANDLE;

typedef void *GED_FRR_HANDLE;

typedef void *GED_GLES_HANDLE;

typedef void *GED_SWD_HANDLE;

typedef void *GED_LOG_HANDLE;

typedef void *GED_KPI_HANDLE;

typedef struct kpi_info
{
	void* win_addr;
	int fence_fd;
	int frameID;
}KPI_INFO;

typedef struct GED_KPI_TAG
{
	int isSF;	/* if it is created by SurfaceFlinger or not */
	int queue_frameID;
	int acquire_frameID;
	uint64_t BBQ_ID;
	int BBQ_api_type;
}GED_KPI;

typedef unsigned int GED_LOG_BUF_HANDLE;

typedef enum GED_BOOL_TAG {
	GED_FALSE,
	GED_TRUE
} GED_BOOL;

typedef enum GED_INFO_TAG {
	GED_LOADING = 0,
	GED_IDLE = 1,
	GED_BLOCKING = 2,
	GED_PRE_FREQ = 3,
	GED_PRE_FREQ_IDX = 4,
	GED_CUR_FREQ = 5,
	GED_CUR_FREQ_IDX = 6,
	GED_MAX_FREQ_IDX = 7 ,
	GED_MAX_FREQ_IDX_FREQ = 8,
	GED_MIN_FREQ_IDX = 9,
	GED_MIN_FREQ_IDX_FREQ = 10,
	GED_3D_FENCE_DONE_TIME = 11,
	GED_VSYNC_OFFSET = 12,
	GED_EVENT_STATUS = 13,
	GED_EVENT_DEBUG_STATUS = 14,
	GED_EVENT_GAS_MODE = 15,
	GED_SRV_SUICIDE = 16,
	GED_PRE_HALF_PERIOD = 17,
	GED_LATEST_START = 18,
	GED_FPS = 19,

	GED_CPU0_LOADING = 100,
	GED_CPU1_LOADING = 101,
	GED_CPU2_LOADING = 102,
	GED_CPU3_LOADING = 103,
	GED_CPU4_LOADING = 104,
	GED_CPU5_LOADING = 105,
	GED_CPU6_LOADING = 106,
	GED_CPU7_LOADING = 107,
	GED_CPU8_LOADING = 108,
	GED_CPU9_LOADING = 109,

	GED_INFO_SIZE
} GED_INFO;

typedef enum {
	GED_DVFS_VSYNC_OFFSET_DEBUG_CLEAR_EVENT = 0,
	GED_DVFS_VSYNC_OFFSET_FORCE_ON = 1,
	GED_DVFS_VSYNC_OFFSET_FORCE_OFF = 2,
	GED_DVFS_VSYNC_OFFSET_TOUCH_EVENT = 3,
	GED_DVFS_VSYNC_OFFSET_THERMAL_EVENT = 4,
	GED_DVFS_VSYNC_OFFSET_WFD_EVENT = 5,
	GED_DVFS_VSYNC_OFFSET_MHL_EVENT = 6,
	GED_DVFS_VSYNC_OFFSET_GAS_EVENT = 7,
	GED_DVFS_VSYNC_OFFSET_LOW_POWER_MODE_EVENT = 8,
	GED_DVFS_VSYNC_OFFSET_MHL4K_VID_EVENT = 9,
	GED_DVFS_VSYNC_OFFSET_VR_EVENT = 10,
	GED_DVFS_BOOST_HOST_EVENT = 11,
	GED_DVFS_VSYNC_OFFSET_VILTE_VID_EVENT = 12,
	GED_DVFS_VSYNC_OFFSET_LOW_LATENCY_MODE_EVENT = 13,
	GED_DVFS_VSYNC_OFFSET_DHWC_EVENT = 14,
} GED_DVFS_VSYNC_OFFSET_SWITCH_CMD;

typedef enum {
	GED_VSYNC_SW_EVENT,
	GED_VSYNC_HW_EVENT
} GED_VSYNC_TYPE;

enum {
	GAS_CATEGORY_GAME,
	GAS_CATEGORY_OTHERS,
};

typedef enum {
	GED_BOOST_GPU_FREQ_LEVEL_MAX = 100
} GED_BOOST_GPU_FREQ_LEVEL;

#define GED_BRIDGE_IN_LOGBUF_SIZE 320
enum {
	/* bit 0~7 reserved for internal used */
	GED_RESERVED                = 0xFF,

	/* log with a prefix kernel time */
	GED_LOG_ATTR_TIME           = 0x100,

	/* log with a prefix user time, pid, tid */
	GED_LOG_ATTR_TIME_TPT       = 0x200,
};

typedef struct GED_DVFS_UM_QUERY_PACK_TAG {
	char bFirstBorn;
	unsigned int ui32GPULoading;
	unsigned int ui32GPUFreqID;
	unsigned int gpu_cur_freq;
	unsigned int gpu_pre_freq;
	long long usT;
	long long nsOffset;
	unsigned long long ul3DFenceDoneTime;
	unsigned long long ulPreCalResetTS_us;
	unsigned long long ulWorkingPeriod_us;
	unsigned int ui32TargetPeriod_us;
	unsigned int ui32BoostValue;
} GED_DVFS_UM_QUERY_PACK;

typedef enum GED_BOOST_ORIGIN_TAG {
    GED_BOOST_ORIGIN_GPU, // Inner GPU driver
    GED_BOOST_ORIGIN_NN, // GPU NN Hal
    GED_BOOST_ORIGIN_HINT, // hint by white list
    GED_BOOST_ORIGIN_NUM
} GED_BOOST_ORIGIN;

#endif
