/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek
 * Software if you have agreed to and been bound by the applicable license
 * agreement with MediaTek ("License Agreement") and been granted explicit
 * permission to do so within the License Agreement ("Permitted User").
 * If you are not a Permitted User, please cease any access or use of MediaTek
 * Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
 * DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A
 * PARTICULAR STANDARD OR OPEN FORUM.
 * RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 * LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
#if 0
#include <debug.h>
#include <kernel/thread.h>
#include <lk/init.h>
#include <platform.h>
#include <reg.h>
#include "platform/mt_gpt.h"
#include "platform/mt_reg_base.h"
#include "platform/crypto_hal_if.h"
#include "platform/mt_typedefs.h"
#include "platform/pmic.h"
#endif
#include "typedefs.h"
#include "wdt.h"
#include <emi_mpu_mt.h>
#include "custom_emi.c"
#include "blkdev.h"
#include "boot_device.h"
#include "pl_version.h"

unsigned int pin_mux_setting;
static unsigned int emi_conh = 0;
static unsigned int chn_emi_cona[2] = {0, 0};

#ifdef DDR_RESERVE_MODE
extern u32 g_ddr_reserve_enable;
extern u32 g_ddr_reserve_success;
extern u32 g_ddr_reserve_ready;
extern u32 g_ddr_reserve_ta_err;
#endif

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
extern u64 get_part_addr(const char *name);
static int read_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo);
static int write_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo);
static u64 part_dram_data_addr = 0;
static unsigned int dram_offline_data_flags = 0;
#endif

static int dram_main(void)
{
	int ret = 0;
	EMI_SETTINGS *emi_set;
    DRAM_INFO_BY_MRR_T DramInfo;

	if( NUM_EMI_RECORD  ==1)
	emi_setting_index = 0;

	if (emi_setting_index == -1)
		emi_set = &default_emi_setting;
	else
	       emi_set = &emi_settings[emi_setting_index];

	pin_mux_setting = emi_set->PIN_MUX_TYPE;
#ifdef DDR_RESERVE_MODE
	if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==0)
		before_init_dram_while_reserve_mode_fail(emi_set->type & 0xF);
#endif

	*((volatile unsigned *)(0x1d062024)) = 0x8;//for hold DSP runstall when dram k

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
///TODO:
	part_dram_data_addr = get_part_addr("boot_para");
	if (part_dram_data_addr != 0x0)
		print("[dramc] init partition address is 0x%llx\n", part_dram_data_addr);
	else {
		print("[dramc] init partition address is incorrect !!!\n");
	}
	//read_offline_dram_mdl_data(&DramInfo);
	/* create mapping for DRAM access */
	ret = init_dram((emi_set->type & 0xF), emi_set->dram_cbt_mode_extern,
			&DramInfo, NORMAL_USED);
	//write_offline_dram_mdl_data(&DramInfo);

#else
	/* create mapping for DRAM access */
	ret = init_dram((emi_set->type & 0xF), emi_set->dram_cbt_mode_extern,
			NULL, NORMAL_USED);
#endif
	return ret;
}

/* return the start address of rank1 */
static unsigned int set_emi_before_rank1_mem_test(void)
{
	unsigned int emi_cona;

	emi_cona = *(volatile unsigned int*)EMI_CONA;
	emi_conh = *(volatile unsigned int*)EMI_CONH;
	chn_emi_cona[0] = *(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE);
	chn_emi_cona[1] = *(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE);

	if (emi_cona & 0x100) {
		/* set all the rank size of all the channels to the minimum value */
		*(volatile unsigned int*)EMI_CONH = ((*(volatile unsigned int*)EMI_CONH & 0x0000ffff) | (0x22220000));

		/* set all the rank size of all channel-0 to the minimum value */
		*(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE)
			= ((*(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE) & 0xff00ffff ) | (0x00220000));

		/* set all the rank size of all channel-1 to the minimum value */
		*(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE)
			= ((*(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE) & 0xff00ffff ) | (0x00220000));
	} else {
		/* set all the rank size of all the channels to the minimum value */
		*(volatile unsigned int*)EMI_CONH = ((*(volatile unsigned int*)EMI_CONH & 0x0000ffff) | (0x44440000));

		/* set all the rank size of all channel-0 to the minimum value */
		*(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE)
			= ((*(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE) & 0xff00ffff ) | (0x00440000));

		/* set all the rank size of all channel-1 to the minimum value */
		*(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE)
			= ((*(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE) & 0xff00ffff ) | (0x00440000));
	}

	dsb();
	return 0x40000000;
}

static void restore_emi_after_rank1_mem_test(void)
{
	*(volatile unsigned int*)EMI_CONH = emi_conh;
	*(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE) = chn_emi_cona[0];
	*(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE) = chn_emi_cona[1];
	dsb();
}


static int simple_mem_test(unsigned int start, unsigned int len)
{
	unsigned int *MEM32_BASE = (unsigned int *) start;
	unsigned int i, orig_val, new_val;

	for (i = 0; i < (len >> 2); ++i) {
		orig_val = MEM32_BASE[i];
		dsb();
		MEM32_BASE[i] = PATTERN1;
		dsb();
		new_val = MEM32_BASE[i];
		if (new_val != PATTERN1)
			return -1;
		dsb();
		MEM32_BASE[i] = orig_val;
	}

	return 0;
}
void mt_mem_init(void)
{
	unsigned int emi_cona;
	int region;

#if 0
	thread_t *thr_main;

#if 0	/* TODO: skip PowerKey temporarily and will enable in the future */
	if (is_new_p1_mode() || get_powerkey_status() == PWRKEY_RELEASE)
		dprintf(INFO, "Skip DRAM calibration\n");
		return;
#endif
	thr_main = thread_create("dram_main", dram_main, NULL,
				 DEFAULT_PRIORITY, 4096);
	thread_resume(thr_main);
	thread_join(thr_main, NULL, INFINITE_TIME);
#endif
	/* clear EMI MPU protect setting */
	for (region = 0; region < EMI_MPU_REGION_NUM; region++) {
		DRV_WriteReg32(EMI_MPU_SA(region), 0x0);
		DRV_WriteReg32(EMI_MPU_EA(region), 0x0);
		DRV_WriteReg32(EMI_MPU_APC(region, 0), 0x0);
	}

#ifdef DDR_RESERVE_MODE
	if((g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1)) {
		/* EMI register dummy read: give clock to EMI APB register to avoid DRAM access hang */
		emi_cona = *(volatile unsigned int *)(EMI_CONA);
		print("[DDR Reserve mode] EMI dummy read CONA = 0x%x\n", emi_cona);

		/* disable transaction mask */
		*(volatile unsigned int *) (CHN0_EMI_BASE + 0x3FC) &= 0xFFFFFFFE;
		*(volatile unsigned int *) (CHN1_EMI_BASE + 0x3FC) &= 0xFFFFFFFE;

		/* disable EMI APB protect */
		DRV_WriteReg32(EMI_MPU_CTRL, DRV_Reg32(EMI_MPU_CTRL)&0xFFFFFFFE);

		if (g_ddr_reserve_ta_err != 0) {
			print("[DDR Reserve mode] TA2 ERR = %d -> overwrite tag.ddr_reserve_success to 0\n", g_ddr_reserve_ta_err);
		}
	} else /* normal boot */
#endif
	{
		/* disable EMI APB protect */
		print("EMI_MPU_CTRL=%x 1st\n",DRV_Reg32(EMI_MPU_CTRL));
		DRV_WriteReg32(EMI_MPU_CTRL, DRV_Reg32(EMI_MPU_CTRL)&0xFFFFFFFE);
		print("EMI_MPU_CTRL=%x 2nd\n",DRV_Reg32(EMI_MPU_CTRL));

		/* force clear RGU control for DRAMC before calibration */
		rgu_release_rg_dramc_conf_iso();//Release DRAMC/PHY conf ISO
		rgu_release_rg_dramc_iso();//Release PHY IO ISO
		rgu_release_rg_dramc_sref();//Let DRAM Leave SR

		dram_main();
	}

#ifdef DDR_RESERVE_MODE
	unsigned int rank1_start_address, i;
	g_ddr_reserve_ready = 0x9502;
	/* Disable DDR-reserve mode in pre-loader stage then enable it again in kernel stage */
	rgu_dram_reserved(1);

	if((g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1)) {

		i = simple_mem_test(DDR_BASE, 0x2000);
		if (i == 0) {
			print("simple R/W mem test pass (start addr:0x%x, @Rank0)\n", (unsigned int)DDR_BASE);
		} else {
			print("simple R/W mem test fail :%x (start addr:0x%x, @Rank0)\n", i, (unsigned int)DDR_BASE);
#ifdef LAST_DRAMC
			rk0_err = 1;
#else
			ASSERT(0);
#endif
		}

		if (get_dram_rank_nr() >= 2){
			/* need to test rank1 */
			rank1_start_address = DDR_BASE + set_emi_before_rank1_mem_test();
			/* simple mem test to avoid memory corruption in DDR reserve mode */
			i = simple_mem_test(rank1_start_address, 0x2000);
			restore_emi_after_rank1_mem_test();

			if (i == 0) {
				print("simple R/W mem test pass (start addr:0x%x, @Rank1)\n", rank1_start_address);
			} else {
				print("simple R/W mem test fail :%x (start addr:0x%x, @Rank1)\n", i, rank1_start_address);
#ifdef LAST_DRAMC
				rk1_err = 1;
#else
				ASSERT(0);
#endif
			}
		}
	}
#endif
}

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#include "emi.h"
#include "mmc_core.h"
u32 g_dram_storage_api_err_code;
#ifndef DRAM_ETT
static u16 crc16(const u8* data, u32 length){
	u8 x;
	u16 crc = 0xFFFF;

	while (length--) {
		x = crc >> 8 ^ *data++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((u8)(x << 12)) ^ ((u8)(x <<5)) ^ ((u8)x);
	}
	return crc;
}
static void assign_checksum_for_dram_data(DRAM_CALIBRATION_SHU_DATA_T *shu_data)
{
	/* need to initialize checksum to 0 before calculation */
	shu_data->checksum = 0;
	shu_data->checksum = crc16((u8*)shu_data, sizeof(*shu_data));
}

static int check_checksum_for_dram_data(DRAM_CALIBRATION_SHU_DATA_T *shu_data)
{
	u16 checksum_in_storage = shu_data->checksum;

	assign_checksum_for_dram_data(shu_data);
	return (shu_data->checksum == checksum_in_storage) ? 1 : 0;
}

#ifdef COMBO_MCP
static void assign_checksum_for_mdl_data(DRAM_CALIBRATION_MRR_DATA_T *mrr_info)
{
	/* need to initialize checksum to 0 before calculation */
	mrr_info->checksum = 0;
	mrr_info->checksum = crc16((u8*)mrr_info, sizeof(*mrr_info));
}

static int check_checksum_for_mdl_data(DRAM_CALIBRATION_MRR_DATA_T *mrr_info)
{
	u16 checksum_in_storage = mrr_info->checksum;

	assign_checksum_for_mdl_data(mrr_info);

	return (mrr_info->checksum == checksum_in_storage) ? 1 : 0;
}
static int read_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo)
{
	int i, ret;
	u16 emi_checksum;
	blkdev_t *bootdev = NULL;
	DRAM_CALIBRATION_HEADER_T hdr;
	DRAM_CALIBRATION_MRR_DATA_T mrr_info;
	DRAM_CALIBRATION_SHU_DATA_T shu_data;
	DRAM_CALIBRATION_DATA_T *datap = NULL;

	if (dram_offline_data_flags)
		goto exit;

	if (DramInfo == NULL) {
		printf("[dramc] DramInfo == NULL, skip\n");
		dram_offline_data_flags = ERR_NULL_POINTER;
		goto exit;
	}

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		printf("[dramc] can't find boot device(%d)\n", CFG_BOOT_DEV);
		dram_offline_data_flags = ERR_BLKDEV_NOT_FOUND;
		goto exit;
	}

	if (!part_dram_data_addr) {
		dram_offline_data_flags = ERR_BLKDEV_NO_PART;
		goto exit;
	}

	ret = blkdev_read(bootdev, part_dram_data_addr, sizeof(hdr), (u8*)&hdr, PART_ID_DRAM_DATA);
	if (ret != 0) {
		printf("[dramc] blkdev_read %s failed\n", "hdr");
		dram_offline_data_flags = ERR_BLKDEV_READ_FAIL;
		goto exit;
	}

	if (hdr.pl_version != DRAMK_VERSION) {
		/* current preloader version does not match the calibration hdr in storage -> erase the partition */
		printf("[dramc] PL_VERSION is updated, erase the DRAM shu_data\n");

		shu_data.checksum = 0;

		/* clear each shuffle */
		for (i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
			ret = blkdev_write(bootdev, part_dram_data_addr + ((unsigned long) &datap->data[i]), sizeof(shu_data), (u8*)&shu_data, PART_ID_DRAM_DATA);
			if (ret != 0) {
				printf("[dramc] blkdev_write failed\n");
				dram_offline_data_flags = ERR_BLKDEV_WRITE_FAIL;
				goto exit;
			}
		}

		dram_offline_data_flags = ERR_PL_UPDATED;
		goto exit;
	}

	/* check magic number */
	if (hdr.magic_number != DRAM_CALIBRATION_DATA_MAGIC) {
		printf("[dramc] magic number mismatch\n");
		dram_offline_data_flags = ERR_MAGIC_NUMBER;
		goto exit;
	}

	ret = blkdev_read(bootdev, part_dram_data_addr + ((unsigned long) &datap->mrr_info), sizeof(mrr_info), (u8*)&mrr_info, PART_ID_DRAM_DATA);
	if (ret != 0) {
		printf("[dramc] blkdev_read %s failed\n", "data");
		dram_offline_data_flags = ERR_BLKDEV_READ_FAIL;
		goto exit;
	}

	/* check checksum */
	if (check_checksum_for_mdl_data(&mrr_info) != 1) {
		printf("[dramc] checksum failed\n");
		dram_offline_data_flags = ERR_CHECKSUM;

		goto exit;
	}

	emi_checksum = crc16((u8*)emi_settings, sizeof(emi_settings));

	if (emi_checksum != mrr_info.emi_checksum) {
		printf("[dramc] emi checksum failed\n");
		dram_offline_data_flags = ERR_CHECKSUM;

		goto exit;
	}

	/* copy the data stored in storage to the data structure for calibration */
	memcpy(DramInfo, &(mrr_info.DramInfo), sizeof(*DramInfo));

exit:
	if (dram_offline_data_flags)
		SET_DRAM_STORAGE_API_ERR(dram_offline_data_flags, DRAM_STORAGE_API_READ);

	return 0 - dram_offline_data_flags;
}

static int write_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo)
{
	int ret;
	blkdev_t *bootdev = NULL;
	DRAM_CALIBRATION_HEADER_T hdr;
	DRAM_CALIBRATION_MRR_DATA_T mrr_info;
	DRAM_CALIBRATION_DATA_T *datap = NULL;

	if (DramInfo == NULL) {
		printf("[dramc] DramInfo == NULL, skip\n");
		SET_DRAM_STORAGE_API_ERR(ERR_NULL_POINTER, DRAM_STORAGE_API_WRITE);
		return -ERR_NULL_POINTER;
	}

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		printf("[dramc] can't find boot device(%d)\n", CFG_BOOT_DEV);
		SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_NOT_FOUND, DRAM_STORAGE_API_WRITE);
		return -ERR_BLKDEV_NOT_FOUND;
	}

	if (!part_dram_data_addr) {
		return -ERR_BLKDEV_NO_PART;
	}

	memcpy(&(mrr_info.DramInfo), DramInfo, sizeof(*DramInfo));

#if 0
    /* assign PL version */
    hdr.pl_version = PL_VERSION;

    /* assign magic number */
    hdr.magic_number = DRAM_CALIBRATION_DATA_MAGIC;

    /* assign api error code */
    hdr.calib_err_code = g_dram_storage_api_err_code;

    ret = blkdev_write(bootdev, part_dram_data_addr, sizeof(hdr), (u8*)&hdr, PART_ID_DRAM_DATA);
    if (ret != 0) {
        printf("[dramc] blkdev_write failed\n");
        SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_WRITE_FAIL, DRAM_STORAGE_API_WRITE);
        return -ERR_BLKDEV_WRITE_FAIL;
    }
#endif

    /* calculate and assign checksum */
	mrr_info.emi_checksum = crc16((u8*)emi_settings, sizeof(emi_settings));
    assign_checksum_for_mdl_data(&mrr_info);

    ret = blkdev_write(bootdev, part_dram_data_addr + ((unsigned long) &datap->mrr_info), sizeof(mrr_info), (u8*)&mrr_info, PART_ID_DRAM_DATA);
    if (ret != 0) {
        printf("[dramc] blkdev_write failed\n");
        SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_WRITE_FAIL, DRAM_STORAGE_API_WRITE);
        return -ERR_BLKDEV_WRITE_FAIL;
    }

    return 0;
}
#endif
void get_offline_dram_calibration_data(SAVE_TIME_FOR_CALIBRATION_T *pSavetimeData)
{
	unsigned int channel_idx, rank_idx, bit_idx, index;

	print("[dramc offline_dram_calibration_data] ucnum_dlycell_perT = %d\n",pSavetimeData->ucnum_dlycell_perT);

	print("[dramc offline_dram_calibration_data] u2DelayCellTimex100 = %d\n",pSavetimeData->u2DelayCellTimex100);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; s1ClockDuty_clk_delay_cell = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->s1ClockDuty_clk_delay_cell[channel_idx][rank_idx]);

	print("[dramc offline_dram_calibration_data] u1clk_use_rev_bit = %d\n",pSavetimeData->u1clk_use_rev_bit);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(index = 0; index < DQS_NUMBER_LP4; index++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; s1DQSDuty_clk_delay_cell = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->s1DQSDuty_clk_delay_cell[channel_idx][index]);

	print("[dramc offline_dram_calibration_data] u1dqs_use_rev_bit = %d\n",pSavetimeData->u1dqs_use_rev_bit);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; u1CBTVref_Save = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->u1CBTVref_Save[channel_idx][rank_idx]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; u1CBTClkDelay_Save = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->u1CBTClkDelay_Save[channel_idx][rank_idx]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; u1CBTCmdDelay_Save = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->u1CBTCmdDelay_Save[channel_idx][rank_idx]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; u1CBTCsDelay_Save = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->u1CBTCsDelay_Save[channel_idx][rank_idx]);

#if CA_PER_BIT_DELAY_CELL
	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_BIT_NUMBER; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1CBTCA_PerBit_DelayLine_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1CBTCA_PerBit_DelayLine_Save[channel_idx][rank_idx][index]);
#endif
	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1WriteLeveling_bypass_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1WriteLeveling_bypass_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1Gating2T_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1Gating2T_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1Gating05T_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1Gating05T_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1Gatingfine_tune_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1Gatingfine_tune_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1Gatingucpass_count_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1Gatingucpass_count_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; u1TxWindowPerbitVref_Save = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->u1TxWindowPerbitVref_Save[channel_idx][rank_idx]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1TxCenter_min_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1TxCenter_min_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1TxCenter_max_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1TxCenter_max_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQ_DATA_WIDTH_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1Txwin_center_Save = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1Txwin_center_Save[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; u1RxDatlat_Save = %d\n",
				channel_idx, rank_idx,
				pSavetimeData->u1RxDatlat_Save[channel_idx][rank_idx]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		print("[dramc offline_dram_calibration_data] channel_idx: %d; u1RxWinPerbitVref_Save = %d\n",
			channel_idx,
			pSavetimeData->u1RxWinPerbitVref_Save[channel_idx]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1RxWinPerbit_DQS = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1RxWinPerbit_DQS[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1RxWinPerbit_DQM = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1RxWinPerbit_DQM[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQ_DATA_WIDTH_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1RxWinPerbit_DQ = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1RxWinPerbit_DQ[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1TX_OE_DQ_MCK = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1TX_OE_DQ_MCK[channel_idx][rank_idx][index]);

	for(channel_idx = 0; channel_idx < CHANNEL_NUM; channel_idx++)
		for(rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
			for(index = 0; index < DQS_NUMBER_LP4; index++)
				print("[dramc offline_dram_calibration_data] channel_idx: %d; rank_idx = %d; dqs_idx = %d; u1TX_OE_DQ_UI = %d\n",
					channel_idx, rank_idx, index,
					pSavetimeData->u1TX_OE_DQ_UI[channel_idx][rank_idx][index]);

}
int read_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	int i, ret;
	blkdev_t *bootdev = NULL;
	DRAM_CALIBRATION_HEADER_T hdr;
	DRAM_CALIBRATION_SHU_DATA_T shu_data;
	DRAM_CALIBRATION_DATA_T *datap = NULL;
	unsigned char *ch;
	unsigned long addr = 0;

	if (dram_offline_data_flags)
		goto exit;

	if (offLine_SaveData == NULL) {
		printf("[dramc] offLine_SaveData == NULL, skip\n");
		dram_offline_data_flags = ERR_NULL_POINTER;
		goto exit;
	}

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		printf("[dramc] can't find boot device(%d)\n", CFG_BOOT_DEV);
		dram_offline_data_flags = ERR_BLKDEV_NOT_FOUND;
		goto exit;
	}

	if (!part_dram_data_addr) {
		dram_offline_data_flags = ERR_BLKDEV_NO_PART;
		goto exit;
	}

	ret = blkdev_read(bootdev, part_dram_data_addr, sizeof(hdr), (u8*)&hdr, PART_ID_DRAM_DATA);
	if (ret != 0) {
		printf("[dramc] blkdev_read %s failed\n", "hdr");
		dram_offline_data_flags = ERR_BLKDEV_READ_FAIL;
		goto exit;
	}

	/* check preloader version */
	if ((hdr.pl_version != DRAMK_VERSION) || strncmp(hdr.pl_build_time, BUILD_TIME, sizeof(hdr.pl_build_time))){
		/* current preloader version does not match the calibration hdr in storage -> erase the partition */
		printf("[dramc]cached dramk version: %d, build time:%s\n", hdr.pl_version, hdr.pl_build_time);
		printf("[dramc]fw dramk version: %d, build time:%s\n", DRAMK_VERSION, BUILD_TIME);

		shu_data.checksum = 0;

		/* clear each shuffle */
		for (i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
			addr = (((unsigned long) &datap->data[i]) + 512)/512;
			printf("[dramc] erase off[%d] = %d %d\n", i, addr*512, sizeof(shu_data));
			ret = blkdev_write(bootdev, part_dram_data_addr + addr * 512, sizeof(shu_data), (u8*)&shu_data, PART_ID_DRAM_DATA);
			if (ret != 0) {
				printf("[dramc] blkdev_write failed\n");
				dram_offline_data_flags = ERR_BLKDEV_WRITE_FAIL;
				goto exit;
			}
		}

		dram_offline_data_flags = ERR_PL_UPDATED;
		goto exit;
	}

	/* check magic number */
	if (hdr.magic_number != DRAM_CALIBRATION_DATA_MAGIC) {
		printf("[dramc] magic number mismatch\n");
		dram_offline_data_flags = ERR_MAGIC_NUMBER;
		goto exit;
	}

	addr = (((unsigned long) &datap->data[shuffle]) + 512)/512;
	printf("[dramc] read off[%d] = %d %d\n", shuffle, addr, sizeof(shu_data));

	ret = blkdev_read(bootdev, part_dram_data_addr + addr * 512, sizeof(shu_data), (u8*)&shu_data, PART_ID_DRAM_DATA);
	if (ret != 0) {
		printf("[dramc] blkdev_read %s failed\n", "data");
		dram_offline_data_flags = ERR_BLKDEV_READ_FAIL;
		goto exit;
	}

	/* check checksum */
	if (check_checksum_for_dram_data(&shu_data) != 1) {
		printf("[dramc] checksum failed\n");
		dram_offline_data_flags = ERR_CHECKSUM;
		goto exit;
	}

	/* copy the data stored in storage to the data structure for calibration */
	memcpy(offLine_SaveData, &(shu_data.calibration_data), sizeof(*offLine_SaveData));

exit:
	if (dram_offline_data_flags)
		SET_DRAM_STORAGE_API_ERR(dram_offline_data_flags, DRAM_STORAGE_API_READ);

	return 0 - dram_offline_data_flags;
}

int write_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	int ret;
	blkdev_t *bootdev = NULL;
	DRAM_CALIBRATION_HEADER_T hdr,t_hdr;
	DRAM_CALIBRATION_SHU_DATA_T shu_data;
	DRAM_CALIBRATION_DATA_T *datap = NULL;
	unsigned long addr = 0;

	if (offLine_SaveData == NULL) {
		printf("[dramc] offLine_SaveData == NULL, skip\n");
		SET_DRAM_STORAGE_API_ERR(ERR_NULL_POINTER, DRAM_STORAGE_API_WRITE);
		return -ERR_NULL_POINTER;
	}

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		printf("[dramc] can't find boot device(%d)\n", CFG_BOOT_DEV);
		SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_NOT_FOUND, DRAM_STORAGE_API_WRITE);
		return -ERR_BLKDEV_NOT_FOUND;
	}

	if (!part_dram_data_addr) {
		return -ERR_BLKDEV_NO_PART;
	}



	memset(&shu_data, 0, sizeof(shu_data));
	memcpy(&(shu_data.calibration_data), offLine_SaveData, sizeof(*offLine_SaveData));

    /* assign PL version */
    hdr.pl_version = DRAMK_VERSION;

    /* assign magic number */
    hdr.magic_number = DRAM_CALIBRATION_DATA_MAGIC;

	/*assign magic build time*/
	memset(hdr.pl_build_time, 0, sizeof(hdr.pl_build_time));
	memcpy(hdr.pl_build_time, BUILD_TIME, sizeof(hdr.pl_build_time));

    /* assign api error code */
    hdr.calib_err_code = g_dram_storage_api_err_code;

    ret = blkdev_write(bootdev, part_dram_data_addr, sizeof(hdr), (u8*)&hdr, PART_ID_DRAM_DATA);
    if (ret != 0) {
        printf("[dramc] blkdev_write failed\n");
        SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_WRITE_FAIL, DRAM_STORAGE_API_WRITE);
        return -ERR_BLKDEV_WRITE_FAIL;
    }
	ret = blkdev_read(bootdev, part_dram_data_addr, sizeof(t_hdr), (u8*)&t_hdr, PART_ID_DRAM_DATA);

    /* calculate and assign checksum */
    assign_checksum_for_dram_data(&shu_data);

	addr = (((unsigned long) &datap->data[shuffle]) + 512)/512;
	printf("[dramc] write off[%d] = %d %d\n", shuffle, addr, sizeof(shu_data));

    ret = blkdev_write(bootdev, part_dram_data_addr + addr * 512, sizeof(shu_data), (u8*)&shu_data, PART_ID_DRAM_DATA);
    if (ret != 0) {
        printf("[dramc] blkdev_write failed\n");
        SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_WRITE_FAIL, DRAM_STORAGE_API_WRITE);
        return -ERR_BLKDEV_WRITE_FAIL;
    }

    return 0;
}

int clean_dram_calibration_data(void)
{
	int i, ret;
	blkdev_t *bootdev = NULL;
	DRAM_CALIBRATION_HEADER_T hdr;
	DRAM_CALIBRATION_SHU_DATA_T shu_data;
	DRAM_CALIBRATION_DATA_T *datap = NULL;
	unsigned addr = 0;

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		printf("[dramc] can't find block device(%d)\n", CFG_BOOT_DEV);
		SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_NOT_FOUND, DRAM_STORAGE_API_CLEAN);
		return -ERR_BLKDEV_NOT_FOUND;
	}

	if (!part_dram_data_addr) {
		return -ERR_BLKDEV_NO_PART;
	}

	memset(&hdr, 0, sizeof(hdr));

	ret = blkdev_write(bootdev, part_dram_data_addr, sizeof(hdr), (u8*)&hdr, PART_ID_DRAM_DATA);
	if (ret != 0) {
		printf("[dramc] blkdev_write failed\n");
		SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_WRITE_FAIL, DRAM_STORAGE_API_CLEAN);
		return -ERR_BLKDEV_WRITE_FAIL;
	}

    shu_data.checksum = 0;

    /* clear each shuffle */
    for (i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
		addr = (((unsigned long) &datap->data[i]) + 512)/512;
        ret = blkdev_write(bootdev, part_dram_data_addr + addr * 512, sizeof(shu_data), (u8*)&shu_data, PART_ID_DRAM_DATA);
        if (ret != 0) {
            printf("[dramc] blkdev_write failed\n");
            SET_DRAM_STORAGE_API_ERR(ERR_BLKDEV_WRITE_FAIL, DRAM_STORAGE_API_WRITE);
            return -ERR_BLKDEV_WRITE_FAIL;
        }
    }

    return 0;
}

#else

DRAM_CALIBRATION_DATA_T dram_data; // using global variable to avoid stack overflow

int read_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	return 0;
}

int write_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	return 0;
}

int clean_dram_calibration_data(void)
{
	return;
}
#endif

#endif

//LK_INIT_HOOK(mt_mem_init, mt_mem_init, LK_INIT_LEVEL_PLATFORM + 1);
