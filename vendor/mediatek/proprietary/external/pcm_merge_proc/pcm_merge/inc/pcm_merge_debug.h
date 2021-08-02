#ifndef PCM_MERGE_DEBUG_H
#define PCM_MERGE_DEBUG_H
#include "pcm_merge_config.h"

#if (PCM_MERGE_LOG_TYPE == 0)
#define print_err		printf
#define print_info		printf
#define print_warn		printf
#define print_dbg		printf
#else
#include <utils/Log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PCM_MERGE"

#define print_err		ALOGE
#define print_info		ALOGD
#define print_warn		ALOGD
#define print_dbg		ALOGD
#endif

#endif