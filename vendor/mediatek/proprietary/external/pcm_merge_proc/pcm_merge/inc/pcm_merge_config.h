#ifndef PCM_MERGE_CONFIG_H
#define PCM_MERGE_CONFIG_H

#define PCM_HAL_TINY_ALSA_SUPPORT		1
#define PCM_HAL_ALSA_LIB_SUPPORT		0

#define PCM_POINT_HW_TOLERANCE_BITS		64
#define SYNC_TOLERANCE_MIN		(150000LL)
#define SYNC_TOERANCE_UNIT		(50000LL)
#define TSTAMP_PER_SECOND		(1000000000LL)


#define PCM_MERGE_SRC_TYPE		1   /* 0: simple  1: blisrc 2: customer*/

#define PCM_MERGE_LOG_TYPE		1   /* 0: printf  1: android */

#endif
