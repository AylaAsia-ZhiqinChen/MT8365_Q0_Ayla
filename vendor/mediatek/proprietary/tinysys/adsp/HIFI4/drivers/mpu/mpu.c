/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"

#include <xtensa/hal.h>

#include <adsp_excep.h>
#include <cli.h>
#include <main.h>
#include <mpu.h>
#include <mt_reg_base.h>


#ifdef CFG_MPU_DEBUG
static const char* mpu_get_access_right(uint8_t access_right)
{
	switch (access_right)
	{
	case XTHAL_AR_NONE:
		return "NONE";
	case XTHAL_AR_R:
		return "R";
	case XTHAL_AR_RX:
		return "RX";
	case XTHAL_AR_RW:
		return "RW";
	case XTHAL_AR_RWX:
		return "RWX";
	case XTHAL_AR_Ww:
		return "Ww";
	case XTHAL_AR_RWrwx:
		return "RWrwx";
	case XTHAL_AR_RWr:
		return "RWr";
	case XTHAL_AR_RWXrx:
		return "RWXrx";
	case XTHAL_AR_Rr:
		return "Rr";
	case XTHAL_AR_RXrx:
		return "RXrx";
	case XTHAL_AR_RWrw:
		return "RWrw";
	case XTHAL_AR_RWXrwx:
		return "RWXrwx";
	default:
		return "UKN";
	}
}

static const char* mpu_get_memory_type(uint32_t encode_mem_type)
{
	uint32_t encode_device = xthal_encode_memory_type(XTHAL_MEM_DEVICE);
	uint32_t encode_nc = xthal_encode_memory_type(XTHAL_MEM_NON_CACHEABLE);
	uint32_t encode_wt_na = xthal_encode_memory_type(XTHAL_MEM_WRITETHRU_NOALLOC);
	uint32_t encode_wt = xthal_encode_memory_type(XTHAL_MEM_WRITETHRU);
	uint32_t encode_wt_wa = xthal_encode_memory_type(XTHAL_MEM_WRITETHRU_WRITEALLOC);
	uint32_t encode_wb_na = xthal_encode_memory_type(XTHAL_MEM_WRITEBACK_NOALLOC);
	uint32_t encode_wb = xthal_encode_memory_type(XTHAL_MEM_WRITEBACK);

	if (encode_mem_type == encode_device)
		return "D";
	else if (encode_mem_type == encode_nc)
		return "NC";
	else if (encode_mem_type == encode_wt_na)
		return "WT_NA";
	else if (encode_mem_type == encode_wt)
		return "WT";
	else if (encode_mem_type == encode_wt_wa)
		return "WT_WA";
	else if (encode_mem_type == encode_wb_na)
		return "WB_NA";
	else if (encode_mem_type == encode_wb)
		return "WB";
	else
		return "UKN";
}

void dump_mpu_status(void)
{
	int32_t i = 0;
	struct xthal_MPU_entry fg_entry[MAX_NUM_REGIONS];

	xthal_read_map(fg_entry);
	PRINTF_I("REGION\tVADDR\tACCESS RIGHT\tMEMORY TYPE\tVALID\n");
	PRINTF_I("============================================================\n");
	for (i = 0; i < MAX_NUM_REGIONS; i++) {
		PRINTF_I("%4d\t0x%08x\t%s\t%s\t%8d\n", i,
			XTHAL_MPU_ENTRY_GET_VSTARTADDR(fg_entry[i]),
			mpu_get_access_right(XTHAL_MPU_ENTRY_GET_ACCESS(fg_entry[i])),
			mpu_get_memory_type(XTHAL_MPU_ENTRY_GET_MEMORY_TYPE(fg_entry[i])),
			XTHAL_MPU_ENTRY_GET_VALID(fg_entry[i]));
	}
}
#endif

static int mpu_create_map(const mpu_region_t* mpu_regions, int num_regions, xthal_MPU_entry *new_entry)
{
	int32_t idx = MAX_NUM_REGIONS - 1, id;
	uint32_t entry_end;

	for (id = num_regions - 1; id >= 0; id--)
	{
		if (idx < 0)
			return MPU_ENTRIES_EXCEED;

		if (mpu_regions[id].enable == MPU_ENTRY_ENABLE)
		{
			entry_end = mpu_regions[id].vaddr + mpu_regions[id].size;
			if (id == num_regions - 1)
			{
				MPU_ENTRY_SET_AS(new_entry[idx], entry_end, MPU_ENTRY_ENABLE);
				MPU_ENTRY_SET_AT(new_entry[idx], XTHAL_AR_NONE, XTHAL_MEM_DEVICE);
				idx--;
			}
			else if (entry_end < mpu_regions[id + 1].vaddr)
			{
				MPU_ENTRY_SET_AS(new_entry[idx], entry_end, MPU_ENTRY_ENABLE);
				MPU_ENTRY_SET_AT(new_entry[idx], XTHAL_AR_NONE, XTHAL_MEM_DEVICE);
				idx--;
			}
			MPU_ENTRY_SET_AS(new_entry[idx], mpu_regions[id].vaddr,
							 mpu_regions[id].enable);
			MPU_ENTRY_SET_AT(new_entry[idx], mpu_regions[id].access_right,
							 mpu_regions[id].memory_type);
			idx--;
		}
	}

	if (mpu_regions[0].vaddr != 0x0)
	{
		if (idx < 0)
			return MPU_ENTRIES_EXCEED;
		MPU_ENTRY_SET_AS(new_entry[idx], 0x0, MPU_ENTRY_ENABLE);
		MPU_ENTRY_SET_AT(new_entry[idx], XTHAL_AR_NONE, XTHAL_MEM_DEVICE);
	}
	return MPU_SUCCESS;
}

/**
 * Initialize MPU entry
 * Notice:
 *  1. interrupt must be disabled before calling xthal_write_map()
 *  2. PROHIBIT printing log
 */
int mpu_init(const mpu_region_t *mpu_regions, int num_regions)
{
	int ret;
	UBaseType_t uxSavedInterruptStatus;

	struct xthal_MPU_entry fg_entry[MAX_NUM_REGIONS];

	memset(fg_entry, 0x0, sizeof(fg_entry));
	ret = mpu_create_map(mpu_regions, num_regions, fg_entry);
	if (ret)
		return ret;
	ret = xthal_check_map(fg_entry, MAX_NUM_REGIONS);
	if (ret != XTHAL_SUCCESS)
	{
		switch (ret)
		{
		case XTHAL_MAP_NOT_ALIGNED:
			return MPU_MAP_NOT_ALIGNED;
		case XTHAL_BAD_ACCESS_RIGHTS:
			return MPU_BAD_ACCESS_RIGHTS;
		case XTHAL_OUT_OF_ENTRIES:
			return MPU_OUT_OF_ENTRIES;
		case XTHAL_OUT_OF_ORDER_MAP:
			return MPU_OUT_OF_ORDER;
		case XTHAL_UNSUPPORTED:
			return MPU_UNSUPPORTED;
		default:
			return MPU_UNKNOWN_ERROR;
		}
	}

	/* Note: Before calling this funciton, to avoid asynchronous use of the MPU during the call,
	 * 1. All interrupts must be disabled.
	 * 2. The integrated DMA engine(iDMA), if configured, must be idle. //XCHAL_HAVE_IDMA is not configured at core-isa.h
	 * 3. All previous block prefetch requests must be complete.
	 */
	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
#if XCHAL_HAVE_CACHE_BLOCKOPS
    __asm__ volatile ("pfwait.a");
#endif
	xthal_write_map(fg_entry, MAX_NUM_REGIONS);
	portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);

	return MPU_SUCCESS;
}

#if defined(CFG_CLI_SUPPORT) && defined(CFG_MPU_DEBUG)
static BaseType_t prvMpuDump( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	dump_mpu_status();
	return pdFALSE;
}

static const CLI_Command_Definition_t xMpuDump =
{
	"mpudump",
	"\r\nmpudump:\r\n dump mpu regions\r\n",
	prvMpuDump,
	0
};

void vRegisterMpuCommands( void )
{
	FreeRTOS_CLIRegisterCommand( &xMpuDump );
}
#endif
