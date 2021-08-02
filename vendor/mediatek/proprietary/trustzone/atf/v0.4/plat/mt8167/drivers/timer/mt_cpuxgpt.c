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
 * MediaTek Inc. (C) 2010. All rights reserved.
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
#include <stdint.h>
#include <arch_helpers.h>
#include "typedefs.h"
#include "platform.h"
#include "mt_cpuxgpt.h"

#define CPUXGPT_BASE 	0x10200000
#define INDEX_BASE  	(CPUXGPT_BASE+0x0674)
#define CTL_BASE    	(CPUXGPT_BASE+0x0670)

__u64	normal_time_base;
__u64	atf_time_base;

#if 0
static unsigned int __read_cpuxgpt(unsigned int reg_index )
{
  	unsigned int value = 0;
  	DRV_WriteReg32(INDEX_BASE,reg_index);
 
  	value = DRV_Reg32(CTL_BASE);
  	return value;
}
#endif

static void __write_cpuxgpt(unsigned int reg_index,unsigned int value )
{

  	DRV_WriteReg32(INDEX_BASE,reg_index);
  	DRV_WriteReg32(CTL_BASE,value);
}

static void __cpuxgpt_set_init_cnt(unsigned int countH,unsigned int  countL)
{
   	__write_cpuxgpt(INDEX_CNT_H_INIT,countH);
   	__write_cpuxgpt(INDEX_CNT_L_INIT,countL); // update count when countL programmed
}

static __u64 read_apgpt_counter(void)
{
	__u32 counter_H, counter_L;

	do {
		counter_H = DRV_Reg32(GPT6_CNTH);
		counter_L = DRV_Reg32(GPT6_CNTL);
	} while (counter_H != DRV_Reg32(GPT6_CNTH));

	return ((__u64)counter_H << 32) | counter_L;
}

static __u64 diff_counter;
void generic_timer_backup(void)
{
	if (diff_counter == 0)
		diff_counter = read_cntpct_el0() - read_apgpt_counter();
}

void generic_timer_restore(void)
{
	__u64 cval = read_apgpt_counter() + diff_counter;
	__cpuxgpt_set_init_cnt((__u32)(cval >> 32), (__u32)(cval & 0xffffffff));
}

void atf_sched_clock_init(unsigned long long normal_base, unsigned long long atf_base)
{
	normal_time_base = normal_base;
	atf_time_base = atf_base;
	return;
}

unsigned long long atf_sched_clock(void)
{
	__u64 cval;
	
	cval = (((read_cntpct_el0() - atf_time_base)*1000)/13) + normal_time_base; 
	return cval;
}

