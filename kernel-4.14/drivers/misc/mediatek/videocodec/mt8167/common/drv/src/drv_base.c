/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include "drv_api.h"
/*#include "val_log.h"*/
#include "val_types_private.h"
#include <linux/dma-mapping.h>
#include <linux/sched.h>

/* #define MODULE_MFV_PR_DEBUG */
#ifdef MODULE_MFV_PR_DEBUG
#undef MODULE_MFV_PR_DEBUG
#define MODULE_MFV_PR_DEBUG pr_debug
#undef pr_debug
#define pr_debug pr_debug
#else
#define MODULE_MFV_PR_DEBUG(...)
#undef pr_debug
#define pr_debug(...)
#endif

/* ============================================================== */
/* For Hybrid HW */
struct VAL_VCODEC_OAL_HW_CONTEXT_T oal_hw_context[VCODEC_MULTIPLE_INSTANCE_NUM];
/* mutex : NonCacheMemoryListLock */
struct VAL_NON_CACHE_MEMORY_LIST_T
	grNonCacheMemoryList[VCODEC_MULTIPLE_INSTANCE_NUM_x_10];

/* For both hybrid and pure HW */
struct VAL_VCODEC_HW_LOCK_T grVcodecDecHWLock; /* mutex : VdecHWLock */
struct VAL_VCODEC_HW_LOCK_T grVcodecEncHWLock; /* mutex : VencHWLock */

unsigned int gu4LockDecHWCount; /* spinlock : LockDecHWCountLock */
unsigned int gu4LockEncHWCount; /* spinlock : LockEncHWCountLock */
unsigned int gu4DecISRCount;    /* spinlock : DecISRCountLock */
unsigned int gu4EncISRCount;    /* spinlock : EncISRCountLock */

signed int search_HWLockSlot_ByTID(unsigned long ulpa, unsigned int curr_tid)
{
	int i;
	int j;

	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].u4ThreadNum !=
		    VCODEC_THREAD_MAX_NUM) {
			for (j = 0; j < oal_hw_context[i].u4ThreadNum;
			     j++) {
				if (oal_hw_context[i].u4ThreadID[j] ==
				    curr_tid) {
					MODULE_MFV_PR_DEBUG(
						"%s, Lookup curr HW Locker is ObjId %d in index%d\n",
						__func__, curr_tid, i);
					return i;
				}
			}
		}
	}

	return -1;
}

signed int search_HWLockSlot_ByHandle(unsigned long ulpa, unsigned long handle)
{
	signed int i;

	if (handle == (unsigned long)NULL) {
		pr_debug("%s, [VCODEC] Get NULL Handle\n", __func__);
		return -1;
	}
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].pvHandle == handle)
			return i;
	}

	return -1;
}

struct VAL_VCODEC_OAL_HW_CONTEXT_T *setCurr_HWLockSlot(unsigned long ulpa,
						unsigned int tid)
{

	int i, j;

	/* Dump current ObjId */
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++)
		MODULE_MFV_PR_DEBUG("Dump curr slot %d ObjId %lu\n", i,
				    oal_hw_context[i].ObjId);

	/* check if current ObjId exist in oal_hw_context[i].ObjId */
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].ObjId == ulpa) {
			MODULE_MFV_PR_DEBUG(
				"[VCODEC] Curr Already exist in %d Slot\n", i);
			return &oal_hw_context[i];
		}
	}

	/* if not exist in table,  find a new free slot and put it */
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].u4ThreadNum !=
		    VCODEC_THREAD_MAX_NUM) {
			for (j = 0; j < oal_hw_context[i].u4ThreadNum;
			     j++) {
				if (oal_hw_context[i].u4ThreadID[j] ==
				    current->pid) {
					oal_hw_context[i].ObjId = ulpa;
					MODULE_MFV_PR_DEBUG(
						"[VCODEC][%s] setCurr %d Slot\n",
						__func__, i);
					return &oal_hw_context[i];
				}
			}
		}
	}

	pr_debug(
		"[VCODEC][ERROR] %s All %d Slots unavaliable\n",
		__func__, VCODEC_MULTIPLE_INSTANCE_NUM);
	oal_hw_context[0].u4ThreadNum = VCODEC_THREAD_MAX_NUM - 1;
	for (i = 0; i < oal_hw_context[0].u4ThreadNum; i++)
		oal_hw_context[0].u4ThreadID[i] = current->pid;

	return &oal_hw_context[0];
}

struct VAL_VCODEC_OAL_HW_CONTEXT_T *
setCurr_HWLockSlot_Thread_ID(struct VAL_VCODEC_THREAD_ID_T a_prVcodecThreadID,
			     unsigned int *a_prIndex)
{
	int i;
	int j;
	int k;

	/* Dump current tids */
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].u4ThreadNum !=
		    VCODEC_THREAD_MAX_NUM) {
			for (j = 0; j < oal_hw_context[i].u4ThreadNum;
			     j++) {
				MODULE_MFV_PR_DEBUG(
					"[%s] Dump curr slot %d, ThreadID[%d] = %d\n",
					__func__, i, j,
					oal_hw_context[i].u4ThreadID[j]);
			}
		}
	}

	for (i = 0; i < a_prVcodecThreadID.u4ThreadNum; i++) {
		MODULE_MFV_PR_DEBUG(
			"[%s] VCodecThreadNum = %d, VCodecThreadID = %d\n",
			__func__,
			a_prVcodecThreadID.u4ThreadNum,
			a_prVcodecThreadID.u4ThreadID[i]);
	}

	/* check if current tids exist in oal_hw_context[i].ObjId */
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].u4ThreadNum !=
		    VCODEC_THREAD_MAX_NUM) {
			for (j = 0; j < oal_hw_context[i].u4ThreadNum;
			     j++) {
				for (k = 0;
				     k < a_prVcodecThreadID.u4ThreadNum;
				     k++) {
					if (oal_hw_context[i]
						    .u4ThreadID[j] ==
					    a_prVcodecThreadID
						    .u4ThreadID[k]) {
						MODULE_MFV_PR_DEBUG(
							"[%s] Curr Already exist in %d Slot\n",
							__func__, i);
						*a_prIndex = i;
						return &oal_hw_context[i];
					}
				}
			}
		}
	}

	/* if not exist in table,  find a new free slot and put it */
	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].u4ThreadNum ==
		    VCODEC_THREAD_MAX_NUM) {
			oal_hw_context[i].u4ThreadNum =
				a_prVcodecThreadID.u4ThreadNum;
			for (j = 0; j < a_prVcodecThreadID.u4ThreadNum;
			     j++) {
				oal_hw_context[i].u4ThreadID[j] =
					a_prVcodecThreadID.u4ThreadID[j];
				MODULE_MFV_PR_DEBUG(
					"[%s] setCurr %d Slot, %d\n",
					__func__, i,
					oal_hw_context[i].u4ThreadID[j]);
			}
			*a_prIndex = i;
			return &oal_hw_context[i];
		}
	}

	{
		pr_debug(
			"[VCodec Error][ERROR] %s All %d Slots unavaliable\n",
			__func__, VCODEC_MULTIPLE_INSTANCE_NUM);
		oal_hw_context[0].u4ThreadNum =
			a_prVcodecThreadID.u4ThreadNum;
		for (i = 0; i < oal_hw_context[0].u4ThreadNum; i++) {
			oal_hw_context[0].u4ThreadID[i] =
				a_prVcodecThreadID.u4ThreadID[i];
		}
		*a_prIndex = 0;
		return &oal_hw_context[0];
	}
}

struct VAL_VCODEC_OAL_HW_CONTEXT_T *freeCurr_HWLockSlot(unsigned long ulpa)
{
	int i;
	int j;

	/* check if current ObjId exist in oal_hw_context[i].ObjId */

	for (i = 0; i < VCODEC_MULTIPLE_INSTANCE_NUM; i++) {
		if (oal_hw_context[i].ObjId == ulpa) {
			oal_hw_context[i].ObjId = -1;
			for (j = 0; j < oal_hw_context[i].u4ThreadNum;
			     j++)
				oal_hw_context[i].u4ThreadID[j] = -1;

			oal_hw_context[i].u4ThreadNum =
				VCODEC_THREAD_MAX_NUM;
			oal_hw_context[i].Oal_HW_reg =
				(struct VAL_VCODEC_OAL_HW_REGISTER_T *)0;
			MODULE_MFV_PR_DEBUG("%s %d Slot\n", __func__, i);
			return &oal_hw_context[i];
		}
	}

	pr_debug(
		"[VCodec Error][ERROR] %s can't find pid in HWLockSlot\n",
		__func__);
	return 0;
}

void Add_NonCacheMemoryList(unsigned long a_ulKVA, unsigned long a_ulKPA,
			    unsigned long a_ulSize,
			    unsigned int a_u4ThreadNum,
			    unsigned int *a_pu4ThreadID)
{
	unsigned int u4I = 0;
	unsigned int u4J = 0;

	MODULE_MFV_PR_DEBUG(
		"%s +, KVA = 0x%lx, KPA = 0x%lx, Size = 0x%lx\n",
		__func__, a_ulKVA, a_ulKPA, a_ulSize);

	for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {
		if ((grNonCacheMemoryList[u4I].ulKVA == 0xffffffff) &&
		    (grNonCacheMemoryList[u4I].ulKPA == 0xffffffff)) {
			MODULE_MFV_PR_DEBUG(
				"ADD %s index = %d, VCodecThreadNum = %d, curr_tid = %d\n",
				__func__, u4I,
				a_u4ThreadNum, current->pid);

			grNonCacheMemoryList[u4I].u4ThreadNum =
				a_u4ThreadNum;
			for (u4J = 0;
			     u4J < grNonCacheMemoryList[u4I].u4ThreadNum;
			     u4J++) {
				grNonCacheMemoryList[u4I]
					.u4ThreadID[u4J] =
					*(a_pu4ThreadID + u4J);
				MODULE_MFV_PR_DEBUG(
					"[%s] VCodecThreadNum = %d, VCodecThreadID = %d\n",
					__func__,
					grNonCacheMemoryList[u4I]
						.u4ThreadNum,
					grNonCacheMemoryList[u4I]
						.u4ThreadID[u4J]);
			}

			grNonCacheMemoryList[u4I].ulKVA = a_ulKVA;
			grNonCacheMemoryList[u4I].ulKPA = a_ulKPA;
			grNonCacheMemoryList[u4I].ulSize = a_ulSize;
			break;
		}
	}

	if (u4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10) {
		pr_debug(
			"[VCODEC][ERROR] CAN'T ADD %s, List is FULL!!\n",
			__func__);
	}

	MODULE_MFV_PR_DEBUG("[VCODEC] %s -\n", __func__);
}

void Free_NonCacheMemoryList(unsigned long a_ulKVA, unsigned long a_ulKPA)
{
	unsigned int u4I = 0;
	unsigned int u4J = 0;

	MODULE_MFV_PR_DEBUG(
		"[VCODEC] %s +, KVA = 0x%lx, KPA = 0x%lx\n",
		__func__, a_ulKVA, a_ulKPA);

	for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {
		if ((grNonCacheMemoryList[u4I].ulKVA == a_ulKVA) &&
		    (grNonCacheMemoryList[u4I].ulKPA == a_ulKPA)) {
			MODULE_MFV_PR_DEBUG(
				"[VCODEC] Free %s index = %d\n",
				__func__, u4I);
			grNonCacheMemoryList[u4I].u4ThreadNum =
				VCODEC_THREAD_MAX_NUM;
			for (u4J = 0; u4J < VCODEC_THREAD_MAX_NUM; u4J++)
				grNonCacheMemoryList[u4I]
					.u4ThreadID[u4J] = 0xffffffff;

			grNonCacheMemoryList[u4I].ulKVA = -1L;
			grNonCacheMemoryList[u4I].ulKPA = -1L;
			grNonCacheMemoryList[u4I].ulSize = -1L;
			break;
		}
	}

	if (u4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10) {
		pr_debug(
		"[VCODEC][ERROR]CAN'T Free %s, Address is not find!!\n",
		__func__);
	}
	MODULE_MFV_PR_DEBUG("[VCODEC]%s -\n", __func__);
}

void Force_Free_NonCacheMemoryList(unsigned int a_u4Tid)
{
	unsigned int u4I = 0;
	unsigned int u4J = 0;
	unsigned int u4K = 0;

	MODULE_MFV_PR_DEBUG(
		"[VCODEC] %s +, curr_id = %d",
		__func__, a_u4Tid);

	for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {
		if (grNonCacheMemoryList[u4I].u4ThreadNum !=
		    VCODEC_THREAD_MAX_NUM) {
			for (u4J = 0;
			     u4J < grNonCacheMemoryList[u4I].u4ThreadNum;
			     u4J++) {
				if (grNonCacheMemoryList[u4I]
					    .u4ThreadID[u4J] == a_u4Tid) {
					MODULE_MFV_PR_DEBUG(
						"[VCODEC][WARNING] %s index = %d, tid = %d, KVA = 0x%lx, KPA = 0x%lx, Size = %lu\n",
						__func__, u4I, a_u4Tid,
						grNonCacheMemoryList[u4I].ulKVA,
						grNonCacheMemoryList[u4I].ulKPA,
						grNonCacheMemoryList[u4I]
							.ulSize);

					dma_free_coherent(
						0, grNonCacheMemoryList[u4I]
							   .ulSize,
						(void *)grNonCacheMemoryList
							[u4I].ulKVA,
						(dma_addr_t)grNonCacheMemoryList
							[u4I].ulKPA);

					grNonCacheMemoryList[u4I]
						.u4ThreadNum =
						VCODEC_THREAD_MAX_NUM;
					for (u4K = 0;
					     u4K < VCODEC_THREAD_MAX_NUM;
					     u4K++) {
						grNonCacheMemoryList[u4I]
							.u4ThreadID[u4K] =
							0xffffffff;
					}
					grNonCacheMemoryList[u4I].ulKVA = -1L;
					grNonCacheMemoryList[u4I].ulKPA = -1L;
					grNonCacheMemoryList[u4I].ulSize = -1L;
					break;
				}
			}
		}
	}

	MODULE_MFV_PR_DEBUG("%s -, curr_id = %d",
			    __func__, a_u4Tid);
}

unsigned int Search_NonCacheMemoryList_By_KPA(unsigned long a_ulKPA)
{
	unsigned int u4I = 0;
	unsigned long ulVA_Offset = 0;

	ulVA_Offset = a_ulKPA & 0x00000fff;

	MODULE_MFV_PR_DEBUG(
		"%s +, KPA = 0x%lx, ulVA_Offset = 0x%lx\n",
		__func__, a_ulKPA, ulVA_Offset);

	for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {
		if (grNonCacheMemoryList[u4I].ulKPA ==
		    (a_ulKPA - ulVA_Offset)) {
			MODULE_MFV_PR_DEBUG(
				"Find %s index = %d\n",
				__func__, u4I);
			break;
		}
	}

	if (u4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10) {
		pr_debug(
			"[ERROR] CAN'T Find %s, Address is not find!!\n",
			__func__);
		return (grNonCacheMemoryList[0].ulKVA + ulVA_Offset);
	}

	MODULE_MFV_PR_DEBUG(
		"%s, ulVA = 0x%lx -\n",
		__func__, (grNonCacheMemoryList[u4I].ulKVA + ulVA_Offset));

	return (grNonCacheMemoryList[u4I].ulKVA + ulVA_Offset);
}
