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

/** @file dramc_actiming_api.c
 *  Basic DRAMC API implementation
 */

/* Include files */
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_api.h"



/*
 * JESD209-4B: tRFCab has 4 settings for 7 density settings
 * (130, 180, 280, 380)
 * tRFCAB_NUM: Used to indicate tRFCab group
 * (since some densities share the same tRFCab)
 */
enum tRFCABIdx {
	tRFCAB_130 = 0,
	tRFCAB_180,
	tRFCAB_280,
	tRFCAB_380,
	tRFCAB_NUM
};

enum ACTimeIdx {
	GRP_DDR1600_ACTIM = 0,
	GRP_DDR2400_ACTIM,
	GRP_DDR2666_ACTIM,
	GRP_DDR3200_ACTIM,
	GRP_DDR3733_ACTIM,
	GRP_DDR4266_ACTIM,
	GRP_ACTIM_NUM
};

enum ACTimeIdxDiv4 {
	GRP_DDR800_DIV4_ACTIM = 0,
	GRP_DDR1200_DIV4_ACTIM,
	GRP_DDR1333_DIV4_ACTIM,
	GRP_DDR1600_DIV4_ACTIM,
	GRP_ACTIM_NUM_DIV4
};

/*
 * ACTiming struct declaration
 * (declared here due fld_wid for each register type)
 * Should include all fields from ACTiming excel file
 * (And update the correct values in UpdateACTimingReg()
 * Note: DQSINCTL, DATLAT aren't in ACTiming excel file
 * (internal delay parameters)
 */

typedef struct _ACTime_T {
	unsigned char dramType, cbtMode, readDBI;
	unsigned short freqGroup;
	unsigned char readLat, writeLat;
	unsigned char divmode;
	/* DQSINCTL, DATLAT aren't in ACTiming excel file */
	unsigned char dqsinctl, datlat;

	/* DRAMC_REG_SHU_ACTIM0 =================================== */
	unsigned short trcd;
	unsigned short trrd;
	unsigned short twr;
	unsigned short twtr;

	/* DRAMC_REG_SHU_ACTIM1 =================================== */
	unsigned short trc;
	unsigned short tras;
	unsigned short trp;
	unsigned short trpab;

	/* DRAMC_REG_SHU_ACTIM2 =================================== */
	unsigned short tfaw;

	unsigned short trtw_ODT_off;
	unsigned short trtw_ODT_on;

	unsigned short trtp;
	unsigned short txp;

	/* DRAMC_REG_SHU_ACTIM3 =================================== */
	unsigned short refcnt;
	unsigned short trfc;
	unsigned short trfcpb;

	/* DRAMC_REG_SHU_ACTIM4 =================================== */
	unsigned short tzqcs;
	unsigned short refcnt_fr_clk;
	unsigned short txrefcnt;

	/* DRAMC_REG_SHU_ACTIM5 =================================== */
	unsigned short tmrr2w_ODT_off;
	unsigned short tmrr2w_ODT_on;

	unsigned short twtpd;
	unsigned short trtpd;

	/* DRAMC_REG_SHU_ACTIM_XRT ================================= */
	unsigned short xrtw2w;
	unsigned short xrtw2r;
	unsigned short xrtr2w;
	unsigned short xrtr2r;

	/* DRAMC_REG_SHU_AC_TIME_05T ================================ */
	unsigned short twtr_05T : 1;
	unsigned short trtw_ODT_off_05T : 1;
	unsigned short trtw_ODT_on_05T : 1;

	unsigned short twtpd_05T : 1;
	unsigned short trtpd_05T : 1;
	unsigned short tfaw_05T : 1;
	unsigned short trrd_05T : 1;
	unsigned short twr_05T : 1;
	unsigned short tras_05T : 1;
	unsigned short trpab_05T : 1;
	unsigned short trp_05T : 1;
	unsigned short trcd_05T : 1;
	unsigned short trtp_05T : 1;
	unsigned short txp_05T : 1;
	unsigned short trfc_05T : 1;
	unsigned short trfcpb_05T : 1;
	unsigned short trc_05T : 1;

	/* cc add for DDR4 BG timing */
	unsigned short tbg_wtr;
	unsigned short tbg_ccd;
	unsigned short tbg_rrd;
	unsigned short tbg_wtr_05T : 1;
	unsigned short tbg_ccd_05T : 1;
	unsigned short tbg_rrd_05T : 1;

	/* Other ACTiming reg fields =================================== */
	unsigned short r_dmcatrain_intv;
	unsigned short r_dmmrw_intv;
	unsigned short r_dmfspchg_prdcnt;
	unsigned short ckeprd;
	unsigned short ckelckcnt;
	unsigned short zqlat2;
} ACTime_T;

/* ac_timing_tbl[] forward declaration */
const ACTime_T ac_timing_tbl[TOTAL_AC_TIMING_NUMBER];

/* ac_timing_tbl: All freq's ACTiming from ACTiming excel file
 * Note: !!All ACTiming adjustments should not be set in -
 * table should be moved into UpdateACTimingReg()!!
 *       Or else preloader's highest freq ACTimings may be set to different
 * values than expected.
 */
const ACTime_T ac_timing_tbl[TOTAL_AC_TIMING_NUMBER] = {

#if SUPPORT_TYPE_LPDDR4
#if (LPDDR4_DIV4_MODE_EN == 0)
#if SUPPORT_LP4_DDR3200_ACTIM
    //LP4_DDR3200 ACTiming---------------------------------
#if (ENABLE_READ_DBI == 1)
    //AC_TIME_LP4_BYTE_DDR3200_DBI_ON
    // LP4-3200, 1600MHz, RDBI_ON, byte mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR3200_FREQ,
	.cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_ON,
        .readLat = 36,	.writeLat = 14,
	.divmode = DIV8_MODE,

        .tras = 8,	.tras_05T = 1,
        .trp = 5,	.trp_05T = 1,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 16,	.trc_05T = 1,
        .trfc = 100,	.trfc_05T = 0,
        .trfcpb = 44,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 2,	.trtp_05T = 1,
        .trcd = 6,	.trcd_05T = 1,
        .twr = 13,	.twr_05T = 1,
        .twtr = 8,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 7,	.tfaw_05T = 0,
        .trtw_ODT_off = 7,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 9,	.trtw_ODT_on_05T = 0,
        .refcnt = 97,
        .refcnt_fr_clk = 101,
        .txrefcnt = 119,
        .tzqcs = 34,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 7,
        .xrtr2r = 7,
        .r_dmcatrain_intv = 11,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 100,
        .trtpd = 13,	.trtpd_05T = 0,
        .twtpd = 12,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 10,
        .tmrr2w_ODT_on = 12,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 12,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 5,	 .datlat = 18
    },

    //AC_TIME_LP4_NORM_DDR3200_DBI_ON
    // LP4-3200, 1600MHz, RDBI_ON, normal mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR3200_FREQ,
	.cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_ON,
        .readLat = 32,	.writeLat = 14,
	.divmode = DIV8_MODE,

        .tras = 8,	.tras_05T = 1,
        .trp = 5,	.trp_05T = 1,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 16,	.trc_05T = 1,
        .trfc = 100,	.trfc_05T = 0,
        .trfcpb = 44,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 2,	.trtp_05T = 1,
        .trcd = 6,	.trcd_05T = 1,
        .twr = 12,	.twr_05T = 1,
        .twtr = 7,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 7,	.tfaw_05T = 0,
        .trtw_ODT_off = 6,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 8,	.trtw_ODT_on_05T = 0,
        .refcnt = 97,
        .refcnt_fr_clk = 101,
        .txrefcnt = 119,
        .tzqcs = 34,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 6,
        .xrtr2r = 7,
        .r_dmcatrain_intv = 11,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 100,
        .trtpd = 12,	.trtpd_05T = 0,
        .twtpd = 12,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 9,
        .tmrr2w_ODT_on = 11,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 12,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 4,	 .datlat = 17
    },
#else //ENABLE_READ_DBI == 0)
    //AC_TIME_LP4_BYTE_DDR3200_DBI_OFF
    // LP4-3200, 1600MHz, RDBI_OFF, byte mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR3200_FREQ,
	.cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 32,	.writeLat = 14,
	.divmode = DIV8_MODE,

        .tras = 8,	.tras_05T = 1,
        .trp = 5,	.trp_05T = 1,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 16,	.trc_05T = 1,
        .trfc = 100,	.trfc_05T = 0,
        .trfcpb = 44,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 2,	.trtp_05T = 1,
        .trcd = 6,	.trcd_05T = 1,
        .twr = 13,	.twr_05T = 1,
        .twtr = 8,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 7,	.tfaw_05T = 0,
        .trtw_ODT_off = 6,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 8,	.trtw_ODT_on_05T = 0,
        .refcnt = 97,
        .refcnt_fr_clk = 101,
        .txrefcnt = 119,
        .tzqcs = 34,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 7,
        .xrtr2r = 7,
        .r_dmcatrain_intv = 11,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 100,
        .trtpd = 12,	.trtpd_05T = 0,
        .twtpd = 12,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 9,
        .tmrr2w_ODT_on = 11,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 12,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 5,	 .datlat = 18
    },

    //AC_TIME_LP4_NORM_DDR3200_DBI_OFF
    // LP4-3200, 1600MHz, RDBI_OFF, normal mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR3200_FREQ,
	.cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 28,	.writeLat = 14,
	.divmode = DIV8_MODE,

        .tras = 8,	.tras_05T = 1,
        .trp = 5,	.trp_05T = 1,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 16,	.trc_05T = 1,
        .trfc = 100,	.trfc_05T = 0,
        .trfcpb = 44,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 2,	.trtp_05T = 1,
        .trcd = 6,	.trcd_05T = 1,
        .twr = 12,	.twr_05T = 1,
        .twtr = 7,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 7,	.tfaw_05T = 0,
        .trtw_ODT_off = 5,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 7,	.trtw_ODT_on_05T = 0,
        .refcnt = 97,
        .refcnt_fr_clk = 101,
        .txrefcnt = 119,
        .tzqcs = 34,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 6,
        .xrtr2r = 7,
        .r_dmcatrain_intv = 11,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 100,
        .trtpd = 11,	.trtpd_05T = 0,
        .twtpd = 12,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 8,
        .tmrr2w_ODT_on = 10,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 12,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 4,	 .datlat = 17
    },
#endif //(ENABLE_READ_DBI)
#endif //SUPPORT_LP4_DDR3200_ACTIM

#if SUPPORT_LP4_DDR2666_ACTIM
    //LP4_DDR2667 ACTiming---------------------------------
#if (ENABLE_READ_DBI == 1)
    //AC_TIME_LP4_BYTE_DDR2667_DBI_ON
    // LP4-2667, 1333MHz, RDBI_ON, byte mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR2666_FREQ,
	.cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_ON,
        .readLat = 30,	.writeLat = 12,
	.divmode = DIV8_MODE,

        .tras = 5,	.tras_05T = 1,
        .trp = 4,	.trp_05T = 0,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 12,	.trc_05T = 0,
        .trfc = 81,	.trfc_05T = 1,
        .trfcpb = 35,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 1,
        .trtp = 1,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 11,	.twr_05T = 1,
        .twtr = 7,	.twtr_05T = 1,
        .trrd = 1,	.trrd_05T = 1,
        .tfaw = 4,	.tfaw_05T = 1,
        .trtw_ODT_off = 6,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 8,	.trtw_ODT_on_05T = 0,
        .refcnt = 81,
        .refcnt_fr_clk = 101,
        .txrefcnt = 100,
        .tzqcs = 28,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 6,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 10,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 84,
        .trtpd = 11,	.trtpd_05T = 0,
        .twtpd = 11,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 8,
        .tmrr2w_ODT_on = 10,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 10,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 4,	 .datlat = 16
    },

    //AC_TIME_LP4_NORM_DDR2667_DBI_ON
    // LP4-2667, 1333MHz, RDBI_ON, normal mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR2666_FREQ,
	.cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_ON,
        .readLat = 28,	.writeLat = 12,
	.divmode = DIV8_MODE,

        .tras = 5,	.tras_05T = 1,
        .trp = 4,	.trp_05T = 0,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 12,	.trc_05T = 0,
        .trfc = 81,	.trfc_05T = 1,
        .trfcpb = 35,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 1,
        .trtp = 1,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 10,	.twr_05T = 0,
        .twtr = 6,	.twtr_05T = 0,
        .trrd = 1,	.trrd_05T = 1,
        .tfaw = 4,	.tfaw_05T = 1,
        .trtw_ODT_off = 6,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 8,	.trtw_ODT_on_05T = 0,
        .refcnt = 81,
        .refcnt_fr_clk = 101,
        .txrefcnt = 100,
        .tzqcs = 28,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 6,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 10,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 84,
        .trtpd = 10,	.trtpd_05T = 0,
        .twtpd = 10,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 7,
        .tmrr2w_ODT_on = 9,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 10,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 3,	 .datlat = 15
    },
#else //ENABLE_READ_DBI == 0)
    //AC_TIME_LP4_BYTE_DDR2667_DBI_OFF
    // LP4-2667, 1333MHz, RDBI_OFF, byte mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR2666_FREQ,
	.cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 26,	.writeLat = 12,
	.divmode = DIV8_MODE,

        .tras = 5,	.tras_05T = 1,
        .trp = 4,	.trp_05T = 0,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 12,	.trc_05T = 0,
        .trfc = 81,	.trfc_05T = 1,
        .trfcpb = 35,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 1,
        .trtp = 1,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 11,	.twr_05T = 1,
        .twtr = 7,	.twtr_05T = 1,
        .trrd = 1,	.trrd_05T = 1,
        .tfaw = 4,	.tfaw_05T = 1,
        .trtw_ODT_off = 5,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 7,	.trtw_ODT_on_05T = 0,
        .refcnt = 81,
        .refcnt_fr_clk = 101,
        .txrefcnt = 100,
        .tzqcs = 28,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 6,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 10,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 84,
        .trtpd = 10,	.trtpd_05T = 0,
        .twtpd = 11,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 7,
        .tmrr2w_ODT_on = 9,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 10,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 4,	 .datlat = 16
    },

    //AC_TIME_LP4_NORM_DDR2667_DBI_OFF
    // LP4-2667, 1333MHz, RDBI_OFF, normal mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR2666_FREQ,
	.cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 24,	.writeLat = 12,
	.divmode = DIV8_MODE,

        .tras = 5,	.tras_05T = 1,
        .trp = 4,	.trp_05T = 0,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 12,	.trc_05T = 0,
        .trfc = 81,	.trfc_05T = 1,
        .trfcpb = 35,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 1,
        .trtp = 1,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 10,	.twr_05T = 0,
        .twtr = 6,	.twtr_05T = 0,
        .trrd = 1,	.trrd_05T = 1,
        .tfaw = 4,	.tfaw_05T = 1,
        .trtw_ODT_off = 5,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 7,	.trtw_ODT_on_05T = 0,
        .refcnt = 81,
        .refcnt_fr_clk = 101,
        .txrefcnt = 100,
        .tzqcs = 28,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 6,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 10,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 84,
        .trtpd = 9,	.trtpd_05T = 0,
        .twtpd = 10,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 6,
        .tmrr2w_ODT_on = 8,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 10,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 3,	 .datlat = 15
    },
#endif //(ENABLE_READ_DBI)
#endif //SUPPORT_LP4_DDR2667_ACTIM

#if SUPPORT_LP4_DDR2400_ACTIM
    //LP4_DDR2400 ACTiming---------------------------------
    //AC_TIME_LP4_BYTE_DDR2400_DBI_OFF
    // LP4-2400, 1200MHz, RDBI_OFF, byte mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR2400_FREQ, .cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 26,	.writeLat = 12,
	.divmode = DIV8_MODE,

        .tras = 4,	.tras_05T = 1,
        .trp = 3,	.trp_05T = 1,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 10,	.trc_05T = 0,
        .trfc = 72,	.trfc_05T = 0,
        .trfcpb = 30,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 1,
        .trtp = 1,	.trtp_05T = 0,
        .trcd = 4,	.trcd_05T = 1,
        .twr = 10,	.twr_05T = 0,
        .twtr = 6,	.twtr_05T = 0,
        .trrd = 1,	.trrd_05T = 0,
        .tfaw = 3,	.tfaw_05T = 0,
        .trtw_ODT_off = 5,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 7,	.trtw_ODT_on_05T = 0,
        .refcnt = 73,
        .refcnt_fr_clk = 101,
        .txrefcnt = 91,
        .tzqcs = 25,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 5,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 9,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 75,
        .trtpd = 10,	.trtpd_05T = 0,
        .twtpd = 10,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 7,
        .tmrr2w_ODT_on = 9,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 9,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 4,	 .datlat = 14
    },

    //AC_TIME_LP4_NORM_DDR2400_DBI_OFF
    // LP4-2400, 1200MHz, RDBI_OFF, normal mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR2400_FREQ, .cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 24,	.writeLat = 12,
	.divmode = DIV8_MODE,

        .tras = 4,	.tras_05T = 1,
        .trp = 3,	.trp_05T = 1,
        .trpab = 1,	.trpab_05T = 0,
        .trc = 10,	.trc_05T = 0,
        .trfc = 72,	.trfc_05T = 0,
        .trfcpb = 30,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 1,
        .trtp = 1,	.trtp_05T = 0,
        .trcd = 4,	.trcd_05T = 1,
        .twr = 10,	.twr_05T = 1,
        .twtr = 6,	.twtr_05T = 1,
        .trrd = 1,	.trrd_05T = 0,
        .tfaw = 3,	.tfaw_05T = 0,
        .trtw_ODT_off = 5,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 7,	.trtw_ODT_on_05T = 0,
        .refcnt = 73,
        .refcnt_fr_clk = 101,
        .txrefcnt = 91,
        .tzqcs = 25,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 5,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 9,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 75,
        .trtpd = 9,	.trtpd_05T = 0,
        .twtpd = 9,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 6,
        .tmrr2w_ODT_on = 8,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 9,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 3,	 .datlat = 13
    },
#endif

#if SUPPORT_LP4_DDR1600_ACTIM
    //LP4_DDR1600 ACTiming---------------------------------
    //AC_TIME_LP4_BYTE_DDR1600_DBI_OFF
    // LP4-1600, 800MHz, RDBI_OFF, byte mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1600_FREQ, .cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 16,	.writeLat = 8,
	.divmode = DIV8_MODE,

        .tras = 0,	.tras_05T = 0,
        .trp = 2,	.trp_05T = 0,
        .trpab = 0,	.trpab_05T = 1,
        .trc = 4,	.trc_05T = 0,
        .trfc = 44,	.trfc_05T = 0,
        .trfcpb = 16,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 0,
        .trtp = 1,	.trtp_05T = 1,
        .trcd = 3,	.trcd_05T = 0,
        .twr = 7,	.twr_05T = 0,
        .twtr = 4,	.twtr_05T = 0,
        .trrd = 0,	.trrd_05T = 0,
        .tfaw = 0,	.tfaw_05T = 0,
        .trtw_ODT_off = 3,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 4,	.trtw_ODT_on_05T = 0,
        .refcnt = 48,
        .refcnt_fr_clk = 101,
        .txrefcnt = 62,
        .tzqcs = 16,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 3,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 50,
        .trtpd = 7,	.trtpd_05T = 0,
        .twtpd = 7,	.twtpd_05T = 1,
        .tmrr2w_ODT_off = 4,
        .tmrr2w_ODT_on = 6,
        .ckeprd = 1,
        .ckelckcnt = 3,
        .zqlat2 = 6,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 2,	 .datlat = 13
    },

    //AC_TIME_LP4_NORM_DDR1600_DBI_OFF
    // LP4-1600, 800MHz, RDBI_OFF, normal mode
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1600_FREQ, .cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 14,	.writeLat = 8,
	.divmode = DIV8_MODE,

        .tras = 0,	.tras_05T = 0,
        .trp = 2,	.trp_05T = 0,
        .trpab = 0,	.trpab_05T = 1,
        .trc = 4,	.trc_05T = 0,
        .trfc = 44,	.trfc_05T = 0,
        .trfcpb = 16,	.trfcpb_05T = 0,
        .txp = 0,	.txp_05T = 0,
        .trtp = 1,	.trtp_05T = 1,
        .trcd = 3,	.trcd_05T = 0,
        .twr = 7,	.twr_05T = 1,
        .twtr = 4,	.twtr_05T = 1,
        .trrd = 0,	.trrd_05T = 0,
        .tfaw = 0,	.tfaw_05T = 0,
        .trtw_ODT_off = 3,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 4,	.trtw_ODT_on_05T = 0,
        .refcnt = 48,
        .refcnt_fr_clk = 101,
        .txrefcnt = 62,
        .tzqcs = 16,
        .xrtw2w = 6,
        .xrtw2r = 3,
        .xrtr2w = 3,
        .xrtr2r = 6,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 50,
        .trtpd = 6,	.trtpd_05T = 0,
        .twtpd = 6,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 3,
        .tmrr2w_ODT_on = 5,
        .ckeprd = 1,
        .ckelckcnt = 3, //LP3 doesn't use this field
        .zqlat2 = 6, //LP3 doesn't use this field

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 1,	 .datlat = 12
    },
#endif //SUPPORT_LP4_DDR1600_ACTIM
#else /* DIV4 mode */

#if SUPPORT_LP4_DDR1600_ACTIM
    //LP4_DDR1600_DIV4 ACTiming---------------------------------
    //AC_TIME_LP4_BYTE_DDR1600_DBI_OFF
    // LP4-1600, 800MHz, RDBI_OFF, byte mode (1:4 mode)
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1600_FREQ, .cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 16,	.writeLat =  8,
        .divmode = DIV4_MODE,

        .tras = 9,	.tras_05T = 0,
        .trp = 6,	.trp_05T = 0,
        .trpab = 2,	.trpab_05T = 0,
        .trc = 17,	.trc_05T = 0,
        .trfc = 100,	.trfc_05T = 0,
        .trfcpb = 44,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 3,	.trtp_05T = 0,
        .trcd = 7,	.trcd_05T = 0,
        .twr = 16,	.twr_05T = 0,
        .twtr = 11,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 7,	.tfaw_05T = 0,
        .trtw_ODT_off = 9,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 11,	.trtw_ODT_on_05T = 0,
        .refcnt = 195,
        .refcnt_fr_clk = 101,
        .txrefcnt = 119,
        .tzqcs = 34,
        .xrtw2w = 9,
        .xrtw2r = 7,
        .xrtr2w = 11,
        .xrtr2r = 10,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 100,
        .trtpd = 16,	.trtpd_05T = 0,
        .twtpd = 16,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 11,
        .tmrr2w_ODT_on = 13,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 12,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 6,
    #if RX_PIPE_BYPASS_EN
        .datlat = 17
    #else
        .datlat = 18
    #endif
    },

    //AC_TIME_LP4_NORM_DDR1600_DBI_OFF
    // LP4-1600, 800MHz, RDBI_OFF, normal mode (1:4 mode)
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1600_FREQ, .cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 14,	.writeLat =  8,
        .divmode = DIV4_MODE,

        .tras = 9,	.tras_05T = 0,
        .trp = 6,	.trp_05T = 0,
        .trpab = 2,	.trpab_05T = 0,
        .trc = 17,	.trc_05T = 0,
        .trfc = 100,	.trfc_05T = 0,
        .trfcpb = 44,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 3,	.trtp_05T = 0,
        .trcd = 7,	.trcd_05T = 0,
        .twr = 15,	.twr_05T = 0,
        .twtr = 10,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 7,	.tfaw_05T = 0,
        .trtw_ODT_off = 8,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 10,	.trtw_ODT_on_05T = 0,
        .refcnt = 195,
        .refcnt_fr_clk = 101,
        .txrefcnt = 119,
        .tzqcs = 34,
        .xrtw2w = 9,
        .xrtw2r = 8,
        .xrtr2w = 10,
        .xrtr2r = 9,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 100,
        .trtpd = 15,	.trtpd_05T = 0,
        .twtpd = 15,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 10,
        .tmrr2w_ODT_on = 12,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 12,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 5,
    #if RX_PIPE_BYPASS_EN
        .datlat = 16
    #else
        .datlat = 17
    #endif
    },
#endif //SUPPORT_LP4_DDR1600_ACTIM

#if SUPPORT_LP4_DDR1333_ACTIM
    //LP4_DDR1600_DIV4 ACTiming---------------------------------
    //AC_TIME_LP4_BYTE_DDR1600_DBI_OFF
    // LP4-1600, 800MHz, RDBI_OFF, byte mode (1:4 mode)
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1333_FREQ, .cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 16,	.writeLat =  8,
        .divmode = DIV4_MODE,

        .tras = 6,	.tras_05T = 0,
        .trp = 4,	.trp_05T = 0,
        .trpab = 2,	.trpab_05T = 0,
        .trc = 12,	.trc_05T = 0,
        .trfc = 82,	.trfc_05T = 0,
        .trfcpb = 35,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 3,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 15,	.twr_05T = 0,
        .twtr = 10,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 5,	.tfaw_05T = 0,
        .trtw_ODT_off = 9,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 11,	.trtw_ODT_on_05T = 0,
        .refcnt = 162,
        .refcnt_fr_clk = 101,
        .txrefcnt = 100,
        .tzqcs = 28,
        .xrtw2w = 9,
        .xrtw2r = 7,
        .xrtr2w = 11,
        .xrtr2r = 9,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 84,
        .trtpd = 16,	.trtpd_05T = 0,
        .twtpd = 15,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 11,
        .tmrr2w_ODT_on = 13,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 10,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 6,
    #if RX_PIPE_BYPASS_EN
        .datlat = 17
    #else
        .datlat = 18
    #endif
    },

    //AC_TIME_LP4_NORM_DDR1600_DBI_OFF
    // LP4-1600, 800MHz, RDBI_OFF, normal mode (1:4 mode)
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1333_FREQ, .cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 14,	.writeLat =  8,
        .divmode = DIV4_MODE,

        .tras = 6,	.tras_05T = 0,
        .trp = 4,	.trp_05T = 0,
        .trpab = 2,	.trpab_05T = 0,
        .trc = 12,	.trc_05T = 0,
        .trfc = 82,	.trfc_05T = 0,
        .trfcpb = 35,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 3,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 14,	.twr_05T = 0,
        .twtr = 10,	.twtr_05T = 0,
        .trrd = 2,	.trrd_05T = 0,
        .tfaw = 5,	.tfaw_05T = 0,
        .trtw_ODT_off = 8,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 10,	.trtw_ODT_on_05T = 0,
        .refcnt = 162,
        .refcnt_fr_clk = 101,
        .txrefcnt = 100,
        .tzqcs = 28,
        .xrtw2w = 9,
        .xrtw2r = 8,
        .xrtr2w = 10,
        .xrtr2r = 9,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 84,
        .trtpd = 15,	.trtpd_05T = 0,
        .twtpd = 14,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 10,
        .tmrr2w_ODT_on = 12,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 10,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 5,
    #if RX_PIPE_BYPASS_EN
        .datlat = 16
    #else
        .datlat = 17
    #endif
    },
#endif //SUPPORT_LP4_DDR1600_ACTIM


#if SUPPORT_LP4_DDR1200_ACTIM
    //LP4_DDR1200 ACTiming---------------------------------
    //AC_TIME_LP4_BYTE_DDR1200_DBI_OFF
    // LP4-1200, 600MHz, RDBI_OFF, byte mode (1:4 mode)
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1200_FREQ, .cbtMode = CBT_BYTE_MODE1, .readDBI = DBI_OFF,
        .readLat = 16,	.writeLat =  8,
        .divmode = DIV4_MODE,

        .tras = 5,	.tras_05T = 0,
        .trp = 4,	.trp_05T = 0,
        .trpab = 2,	.trpab_05T = 0,
        .trc = 10,	.trc_05T = 0,
        .trfc = 72,	.trfc_05T = 0,
        .trfcpb = 30,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 3,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 14,	.twr_05T = 0,
        .twtr = 10,	.twtr_05T = 0,
        .trrd = 1,	.trrd_05T = 0,
        .tfaw = 3,	.tfaw_05T = 0,
        .trtw_ODT_off = 8,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 11,	.trtw_ODT_on_05T = 0,
        .refcnt = 146,
        .refcnt_fr_clk = 101,
        .txrefcnt = 91,
        .tzqcs = 25,
        .xrtw2w = 8,
        .xrtw2r = 6,
        .xrtr2w = 11,
        .xrtr2r = 9,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 75,
        .trtpd = 16,	.trtpd_05T = 0,
        .twtpd = 14,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 11,
        .tmrr2w_ODT_on = 13,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 9,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 6,
    #if RX_PIPE_BYPASS_EN
        .datlat = 16
    #else
        .datlat = 17
    #endif
    },

    //AC_TIME_LP4_NORM_DDR1200_DBI_OFF
    // LP4-1200, 600MHz, RDBI_OFF, normal mode (1:4 mode)
    {
        .dramType = TYPE_LPDDR4, .freqGroup = DDR1200_FREQ, .cbtMode = CBT_NORMAL_MODE, .readDBI = DBI_OFF,
        .readLat = 14,	.writeLat =  8,
        .divmode = DIV4_MODE,

        .tras = 5,	.tras_05T = 0,
        .trp = 4,	.trp_05T = 0,
        .trpab = 2,	.trpab_05T = 0,
        .trc = 10,	.trc_05T = 0,
        .trfc = 72,	.trfc_05T = 0,
        .trfcpb = 30,	.trfcpb_05T = 0,
        .txp = 1,	.txp_05T = 0,
        .trtp = 3,	.trtp_05T = 0,
        .trcd = 5,	.trcd_05T = 0,
        .twr = 13,	.twr_05T = 0,
        .twtr = 10,	.twtr_05T = 0,
        .trrd = 1,	.trrd_05T = 0,
        .tfaw = 3,	.tfaw_05T = 0,
        .trtw_ODT_off = 7,	.trtw_ODT_off_05T = 0,
        .trtw_ODT_on = 10,	.trtw_ODT_on_05T = 0,
        .refcnt = 146,
        .refcnt_fr_clk = 101,
        .txrefcnt = 91,
        .tzqcs = 25,
        .xrtw2w = 8,
        .xrtw2r = 7,
        .xrtr2w = 10,
        .xrtr2r = 9,
        .r_dmcatrain_intv = 8,
        .r_dmmrw_intv = 0xf, //Berson: LP3/4 both use this field -> Formula may change, set to 0xF for now
        .r_dmfspchg_prdcnt = 75,
        .trtpd = 15,	.trtpd_05T = 0,
        .twtpd = 13,	.twtpd_05T = 0,
        .tmrr2w_ODT_off = 10,
        .tmrr2w_ODT_on = 12,
        .ckeprd = 2,
        .ckelckcnt = 3,
        .zqlat2 = 9,

        //DQSINCTL, DATLAT aren't in ACTiming excel file
        .dqsinctl = 5,
    #if RX_PIPE_BYPASS_EN
        .datlat = 15
    #else
        .datlat = 16
    #endif
    },
#endif //SUPPORT_LP4_DDR1200_ACTIM
#endif /* LPDDR4_DIV4_MODE_EN */
#endif /* SUPPORT_TYPE_LPDDR4 */

#if SUPPORT_TYPE_PCDDR4
#if SUPPORT_PC4_DDR3200_ACTIM /* From DE sim waveform */
	{
		.dramType = TYPE_PCDDR4, .freqGroup = DDR3200_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 24, .writeLat = 20,
		.divmode = DIV8_MODE,

		.tras = 0x6,
		.tras_05T = 1,
		.trp = 0x4,
		.trp_05T = 0,
		.trpab = 0x4,
		.trpab_05T = 0,
		.trc = 0xc,
		.trc_05T = 0,
		.trfc = 0xd0,
		.trfc_05T = 0,
		.trfcpb = 0x4,
		.trfcpb_05T = 0,
		.txp = 0x1,
		.txp_05T = 1,
		.trtp = 0x3,
		.trtp_05T = 1,
		.trcd = 0x5,
		.trcd_05T = 0,
		.twr = 0xc,
		.twr_05T = 1,
		.twtr = 0x4,
		.twtr_05T = 1,
		.trrd = 0x1,
		.trrd_05T = 1,
		.tfaw = 0x6,
		.tfaw_05T = 0,
		.trtw_ODT_off = 0x5,
		.trtw_ODT_off_05T = 0,
		.trtw_ODT_on = 0xb,
		.trtw_ODT_on_05T = 0,
		.refcnt = 0xc3,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0x100,
		.tzqcs = 0x3e,
		.xrtw2w = 0x5,
		.xrtw2r = 0x2,
		.xrtr2w = 0x7,
		.xrtr2r = 0x7,
		.r_dmcatrain_intv = 0x4,

		.tbg_wtr = 0x8,
		.tbg_wtr_05T = 0x1,
		.tbg_ccd = 0x1,
		.tbg_ccd_05T = 0x0,
		.tbg_rrd = 0x1,
		.tbg_rrd_05T = 0x0,

		.r_dmmrw_intv = 0x15,
		.r_dmfspchg_prdcnt = 0x64,
		.trtpd = 0xc,
		.trtpd_05T = 0,
		.twtpd = 0xc,
		.twtpd_05T = 0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 0x1,
		.ckelckcnt = 0x4,
		.zqlat2 = 0x0,

		.dqsinctl = 2,
		.datlat = 0xf,
	},
#endif

#if SUPPORT_PC4_DDR2667_ACTIM /* From DE sim waveform */
	{
		.dramType = TYPE_PCDDR4, .freqGroup = DDR2666_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 20, .writeLat = 18,
		.divmode = DIV8_MODE,

		.tras = 0x6,
		.tras_05T = 1,
		.trp = 0x4,
		.trp_05T = 1,
		.trpab = 0x0,
		.trpab_05T = 0,
		.trc = 0xc,
		.trc_05T = 0,
		.trfc = 0xac,
		.trfc_05T = 1,
		.trfcpb = 0x18,
		.trfcpb_05T = 0,
		.txp = 0x1,
		.txp_05T = 1,
		.trtp = 0x3,
		.trtp_05T = 1,
		.trcd = 0x4,
		.trcd_05T = 0,
		.twr = 0xa,
		.twr_05T = 0,
		.twtr = 0x5,
		.twtr_05T = 1,
		.trrd = 0x1,
		.trrd_05T = 0,
		.tfaw = 0x4,
		.tfaw_05T = 1,
		.trtw_ODT_off = 0x4,
		.trtw_ODT_off_05T = 0,
		.trtw_ODT_on = 0xb,
		.trtw_ODT_on_05T = 0,
		.refcnt = 0xa2,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0x100,
		.tzqcs = 0x3e,
		.xrtw2w = 0x5,
		.xrtw2r = 0x2,
		.xrtr2w = 0x6,
		.xrtr2r = 0x7,
		.r_dmcatrain_intv = 0x4,

		.tbg_wtr = 0x7,
		.tbg_wtr_05T = 0x1,
		.tbg_ccd = 0x1,
		.tbg_ccd_05T = 0x0,
		.tbg_rrd = 0x1,
		.tbg_rrd_05T = 0x0,

		.r_dmmrw_intv = 0x15,
		.r_dmfspchg_prdcnt = 0x54,
		.trtpd = 0xc,
		.trtpd_05T = 0,
		.twtpd = 0xc,
		.twtpd_05T = 0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 0x1,
		.ckelckcnt = 0x4,
		.zqlat2 = 0x0,

		.dqsinctl = 2,
		.datlat = 0xf,
	},
#endif

#if SUPPORT_PC4_DDR1600_ACTIM
	{
		.dramType = TYPE_PCDDR4, .freqGroup = DDR1600_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 11, .writeLat = 9,
		.divmode = DIV4_MODE,

		.tras = 0x6,
		.tras_05T = 0x0,
		.trp = 0x4,
		.trp_05T = 0x0,
		.trpab = 0x4,
		.trpab_05T = 0x0,
		.trc = 0xc,
		.trc_05T = 0,
		.trfc = 0xd0,
		.trfc_05T = 0x0,
		.trfcpb = 0x2c,
		.trfcpb_05T = 0x0,
		.txp = 0x0,
		.txp_05T = 0x0,
		.trtp = 0x1,
		.trtp_05T = 0x0,
		.trcd = 0x5,
		.trcd_05T = 0x0,
		.twr = 0xb,
		.twr_05T = 0x0,
		.twtr = 0x5,
		.twtr_05T = 0x0,
		.trrd = 0x2,
		.trrd_05T = 0x0,
		.tfaw = 0xc,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x2,
		.trtw_ODT_off_05T = 0x0,
		.trtw_ODT_on = 0xc,
		.trtw_ODT_on_05T = 0x0,
		.refcnt = 0x186,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0x200,
		.tzqcs = 0x3e,
		.xrtw2w = 0x0,
		.xrtw2r = 0x1,
		.xrtr2w = 0x0,
		.xrtr2r = 0x4,
		.r_dmcatrain_intv = 8, /* NOT used */

		/* DDR4 specific */
		.tbg_wtr = 0x7,
		.tbg_wtr_05T = 0x0,
		.tbg_ccd = 0x2,
		.tbg_ccd_05T = 0x1,
		.tbg_rrd = 0x2,
		.tbg_rrd_05T = 0x0,

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x64,  /* NOT used */
		.trtpd = 0xc,
		.trtpd_05T = 0x0,
		.twtpd = 0x10,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x2,
		.tmrr2w_ODT_on = 0x5,
		.ckeprd = 1,  /* NOT used */
		.ckelckcnt = 4,  /* NOT used */
		.zqlat2 = 0x0,  /* NOT used */
		.dqsinctl = 0x1,
		.datlat = 0xf,
	},
#endif

#if SUPPORT_PC4_DDR1333_ACTIM
	{
		.dramType = TYPE_PCDDR4, .freqGroup = DDR1333_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 11, .writeLat = 9,
		.divmode = DIV4_MODE,

		.tras = 0x3,
		.tras_05T = 0x0,
		.trp = 0x3,
		.trp_05T = 0x0,
		.trpab = 0x4,
		.trpab_05T = 0x0,
		.trc = 0x9,
		.trc_05T = 0,
		.trfc = 0xac,
		.trfc_05T = 0x0,
		.trfcpb = 0x2c,
		.trfcpb_05T = 0x0,
		.txp = 0x0,
		.txp_05T = 0x0,
		.trtp = 0x1,
		.trtp_05T = 0x0,
		.trcd = 0x4,
		.trcd_05T = 0x0,
		.twr = 0xa,
		.twr_05T = 0x0,
		.twtr = 0x5,
		.twtr_05T = 0x0,
		.trrd = 0x2,
		.trrd_05T = 0x0,
		.tfaw = 0xa,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x2,
		.trtw_ODT_off_05T = 0x0,
		.trtw_ODT_on = 0xc,
		.trtw_ODT_on_05T = 0x0,
		.refcnt = 0x145,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0x200,
		.tzqcs = 0x3e,
		.xrtw2w = 0x0,
		.xrtw2r = 0x1,
		.xrtr2w = 0x0,
		.xrtr2r = 0x4,
		.r_dmcatrain_intv = 8, /* NOT used */

		/* DDR4 specific */
		.tbg_wtr = 0x7,
		.tbg_wtr_05T = 0x0,
		.tbg_ccd = 0x2,
		.tbg_ccd_05T = 0x1,
		.tbg_rrd = 0x2,
		.tbg_rrd_05T = 0x0,

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x54,  /* NOT used */
		.trtpd = 0xc,
		.trtpd_05T = 0x0,
		.twtpd = 0x10,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x2,
		.tmrr2w_ODT_on = 0x5,
		.ckeprd = 1,  /* NOT used */
		.ckelckcnt = 4,  /* NOT used */
		.zqlat2 = 0x0,  /* NOT used */

		.dqsinctl = 0x1,
		.datlat = 0xf,
	},
#endif

#endif /* SUPPORT_TYPE_PCDDR4 */

#if SUPPORT_TYPE_LPDDR3
#if SUPPORT_LP3_DDR1866_ACTIM
	{
		.dramType = TYPE_LPDDR3, .freqGroup = DDR1866_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 14, .writeLat = 8,
		.divmode = DIV4_MODE,

		.tras = 0xb,
		.tras_05T = 0,
		.trp = 0x9,
		.trp_05T = 0x0,
		.trpab = 0x1,
		.trpab_05T = 0x0,
		.trc = 0x14,
		.trc_05T = 0x0,
		.trfc = 0x49,
		.trfc_05T = 0x0,
		.trfcpb = 0x19,
		.trfcpb_05T = 0x0,
		.txp = 0x0,
		.txp_05T = 0x0,
		.trtp = 0x2,
		.trtp_05T = 0x0,
		.trcd = 0x9,
		.trcd_05T = 0x0,
		.twr = 0xd,
		.twr_05T = 0x1,
		.twtr = 0x8,
		.twtr_05T = 0x1,
		.trrd = 0x3,
		.trrd_05T = 0x0,
		.tfaw = 0xc,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x5,
		.trtw_ODT_off_05T = 0x1,
		.trtw_ODT_on = 0x7,
		.trtw_ODT_on_05T = 0x1,
		.refcnt = 0x0,
		.refcnt_fr_clk = 0x65,
		.txrefcnt = 0x60,
		.tzqcs = 0x23,
		.xrtw2w = 0x8,
		.xrtw2r = 0x1,
		.xrtr2w = 0x4,
		.xrtr2r = 0xc,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 50,  /* NOT used */
		.trtpd = 0x8,
		.trtpd_05T = 0x0,
		.twtpd = 0x9,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 2,
		.ckelckcnt = 0,  /* NOT used */
		.zqlat2 = 6,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0x13,
	},
#endif

#if SUPPORT_LP3_DDR1600_ACTIM
	{
		.dramType = TYPE_LPDDR3, .freqGroup = DDR1600_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		//.readDBI = DBI_OFF, .readLat = 14, .writeLat = 8,
		.readDBI = DBI_OFF, .readLat = 12, .writeLat = 6,
		.divmode = DIV4_MODE,

		.tras = 0x8,
		.tras_05T = 0,
		.trp = 0x8,
		.trp_05T = 0x0,
		.trpab = 0x1,
		.trpab_05T = 0x0,
		.trc = 0x13,
		.trc_05T = 0x0,
		.trfc = 0x4a,
		.trfc_05T = 0x0,
		.trfcpb = 0x18,
		.trfcpb_05T = 0x0,
		.txp = 0x2,
		.txp_05T = 0x1,
		.trtp = 0x2,
		.trtp_05T = 0x0,
		.trcd = 0x9,
		.trcd_05T = 0x0,
		.twr = 0xb,
		.twr_05T = 0x1,
		.twtr = 0x7,
		.twtr_05T = 0x1,
		.trrd = 0x2,
		.trrd_05T = 0x0,
		.tfaw = 0xb,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x6,
		.trtw_ODT_off_05T = 0x1,
		.trtw_ODT_on = 0x7,
		.trtw_ODT_on_05T = 0x1,
		.refcnt = 0x60,
		.refcnt_fr_clk = 0x65,
		.txrefcnt = 0x56,
		.tzqcs = 0x23,
		.xrtw2w = 0x6,
		.xrtw2r = 0x2,
		.xrtr2w = 0x4,
		.xrtr2r = 0xa,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x0,  /* NOT used */
		.trtpd = 0x8,
		.trtpd_05T = 0x0,
		.twtpd = 0x9,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 2,
		.ckelckcnt = 0,  /* NOT used */
		.zqlat2 = 0,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0x12,
	},
#endif

#if SUPPORT_LP3_DDR1333_ACTIM
	{
		.dramType = TYPE_LPDDR3, .freqGroup = DDR1333_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		//.readDBI = DBI_OFF, .readLat = 14, .writeLat = 8,
		.readDBI = DBI_OFF, .readLat = 10, .writeLat = 6,
		.divmode = DIV4_MODE,

		.tras = 0x5,
		.tras_05T = 0,
		.trp = 0x6,
		.trp_05T = 0x0,
		.trpab = 0x1,
		.trpab_05T = 0x0,
		.trc = 0xe,
		.trc_05T = 0x0,
		.trfc = 0x3a,
		.trfc_05T = 0x0,
		.trfcpb = 0x12,
		.trfcpb_05T = 0x0,
		.txp = 0x2,
		.txp_05T = 0x1,
		.trtp = 0x2,
		.trtp_05T = 0x1,
		.trcd = 0x7,
		.trcd_05T = 0x0,
		.twr = 0xa,
		.twr_05T = 0x1,
		.twtr = 0x5,
		.twtr_05T = 0x0,
		.trrd = 0x2,
		.trrd_05T = 0x1,
		.tfaw = 0x8,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x5,
		.trtw_ODT_off_05T = 0x1,
		.trtw_ODT_on = 0x7,
		.trtw_ODT_on_05T = 0x1,
		.refcnt = 0x50,
		.refcnt_fr_clk = 0x65,
		.txrefcnt = 0x48,
		.tzqcs = 0x1d,
		.xrtw2w = 0x8,
		.xrtw2r = 0x4,
		.xrtr2w = 0x5,
		.xrtr2r = 0xb,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x0,  /* NOT used */
		.trtpd = 0x7,
		.trtpd_05T = 0x0,
		.twtpd = 0x8,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x0,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 2,
		.ckelckcnt = 0,  /* NOT used */
		.zqlat2 = 0,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0x12,
	},
#endif

#if SUPPORT_LP3_DDR1200_ACTIM
	{
		.dramType = TYPE_LPDDR3, .freqGroup = DDR1200_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		//.readDBI = DBI_OFF, .readLat = 14, .writeLat = 8,
		.readDBI = DBI_OFF, .readLat = 9, .writeLat = 5,
		.divmode = DIV4_MODE,

		.tras = 0x4,
		.tras_05T = 0,
		.trp = 0x6,
		.trp_05T = 0x1,
		.trpab = 0x1,
		.trpab_05T = 0x0,
		.trc = 0xc,
		.trc_05T = 0x0,
		.trfc = 0x35,
		.trfc_05T = 0x0,
		.trfcpb = 0xf,
		.trfcpb_05T = 0x0,
		.txp = 0x2,
		.txp_05T = 0x1,
		.trtp = 0x2,
		.trtp_05T = 0x1,
		.trcd = 0x7,
		.trcd_05T = 0x1,
		.twr = 0x8,
		.twr_05T = 0x0,
		.twtr = 0x5,
		.twtr_05T = 0x0,
		.trrd = 0x1,
		.trrd_05T = 0x0,
		.tfaw = 0x6,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x5,
		.trtw_ODT_off_05T = 0x1,
		.trtw_ODT_on = 0x7,
		.trtw_ODT_on_05T = 0x1,
		.refcnt = 0x48,
		.refcnt_fr_clk = 0x65,
		.txrefcnt = 0x40,
		.tzqcs = 0x1a,
		.xrtw2w = 0x8,
		.xrtw2r = 0x4,
		.xrtr2w = 0x5,
		.xrtr2r = 0xb,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x0,  /* NOT used */
		.trtpd = 0x7,
		.trtpd_05T = 0x0,
		.twtpd = 0x7,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 2,
		.ckelckcnt = 0,  /* NOT used */
		.zqlat2 = 0,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0x12,
	},
#endif
#endif /* SUPPORT_TYPE_LPDDR3 */

#if SUPPORT_TYPE_PCDDR3
#if SUPPORT_PC3_DDR1866_ACTIM
	{ /* From 7580 */
		.dramType = TYPE_PCDDR3, .freqGroup = DDR1866_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 13, .writeLat = 9,
		.divmode = DIV4_MODE,

		.tras = 0x8,
		.tras_05T = 0,
		.trp = 0x5,
		.trp_05T = 0x0,
		.trpab = 0x0,
		.trpab_05T = 0x0,
		.trc = 0x11,
		.trc_05T = 0x0,
		.trfc = 0x98,
		.trfc_05T = 0x0,
		.trfcpb = 0x18,
		.trfcpb_05T = 0x0,
		.txp = 0x1,
		.txp_05T = 0x0,
		.trtp = 0x1,
		.trtp_05T = 0x0,
		.trcd = 0x6,
		.trcd_05T = 0x0,
		.twr = 0xd,
		.twr_05T = 0x0,
		.twtr = 0x8,
		.twtr_05T = 0x0,
		.trrd = 0x5,
		.trrd_05T = 0x0,
		.tfaw = 0x9,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x4,
		.trtw_ODT_off_05T = 0x0,
		.trtw_ODT_on = 0xc,
		.trtw_ODT_on_05T = 0x0,
		.refcnt = 0x1c6,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0xac,
		.tzqcs = 0x3e,
		.xrtw2w = 0x7,
		.xrtw2r = 0x4,
		.xrtr2w = 0xa,
		.xrtr2r = 0x7,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 50,  /* NOT used */
		.trtpd = 0x8,
		.trtpd_05T = 0x0,
		.twtpd = 0x9,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 2,  /* NOT used */
		.ckelckcnt = 5,  /* NOT used */
		.zqlat2 = 6,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0xf,
	},
#endif

#if SUPPORT_PC3_DDR1600_ACTIM
	{ /* From 7580 */
		.dramType = TYPE_PCDDR3, .freqGroup = DDR1600_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 11, .writeLat = 8,
		.divmode = DIV4_MODE,

		.tras = 0x6,
		.tras_05T = 0,
		.trp = 0x5,
		.trp_05T = 0x0,
		.trpab = 0x0,
		.trpab_05T = 0x0,
		.trc = 0xe,
		.trc_05T = 0x0,
		.trfc = 0x80,
		.trfc_05T = 0x0,
		.trfcpb = 0x18,
		.trfcpb_05T = 0x0,
		.txp = 0x1,
		.txp_05T = 0x0,
		.trtp = 0x2,
		.trtp_05T = 0x0,
		.trcd = 0x5,
		.trcd_05T = 0x0,
		.twr = 0xc,
		.twr_05T = 0x0,
		.twtr = 0x8,
		.twtr_05T = 0x0,
		.trrd = 0x4,
		.trrd_05T = 0x0,
		.tfaw = 0x8,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x4,
		.trtw_ODT_off_05T = 0x0,
		.trtw_ODT_on = 0xc,
		.trtw_ODT_on_05T = 0x0,
		.refcnt = 0x186,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0x94,
		.tzqcs = 0x3e,
		.xrtw2w = 0x7,
		.xrtw2r = 0x6,
		.xrtr2w = 0xa,
		.xrtr2r = 0x7,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x64,  /* NOT used */
		.trtpd = 0x8,
		.trtpd_05T = 0x0,
		.twtpd = 0x9,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 1,  /* NOT used */
		.ckelckcnt = 3,  /* NOT used */
		.zqlat2 = 6,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0xf,
	},
#endif

#if SUPPORT_PC3_DDR1333_ACTIM
	{
		.dramType = TYPE_PCDDR3, .freqGroup = DDR1333_FREQ,
		.cbtMode = CBT_NORMAL_MODE,
		.readDBI = DBI_OFF, .readLat = 11, .writeLat = 7,
		.divmode = DIV4_MODE,

		.tras = 0x4,
		.tras_05T = 0,
		.trp = 0x4,
		.trp_05T = 0x0,
		.trpab = 0x0,
		.trpab_05T = 0x0,
		.trc = 0xa,
		.trc_05T = 0x0,
		.trfc = 0x69,
		.trfc_05T = 0x0,
		.trfcpb = 0x18,
		.trfcpb_05T = 0x0,
		.txp = 0x0,
		.txp_05T = 0x0,
		.trtp = 0x2,
		.trtp_05T = 0x0,
		.trcd = 0x4,
		.trcd_05T = 0x0,
		.twr = 0xb,
		.twr_05T = 0x0,
		.twtr = 0x7,
		.twtr_05T = 0x0,
		.trrd = 0x3,
		.trrd_05T = 0x0,
		.tfaw = 0x8,
		.tfaw_05T = 0x0,
		.trtw_ODT_off = 0x4,
		.trtw_ODT_off_05T = 0x0,
		.trtw_ODT_on = 0xc,
		.trtw_ODT_on_05T = 0x0,
		.refcnt = 0x145,
		.refcnt_fr_clk = 0xca,
		.txrefcnt = 0x7c,
		.tzqcs = 0x3e,
		.xrtw2w = 0x7,
		.xrtw2r = 0x6,
		.xrtr2w = 0xa,
		.xrtr2r = 0x7,
		.r_dmcatrain_intv = 8, /* NOT used */

		.r_dmmrw_intv = 0xf,  /* NOT used */
		.r_dmfspchg_prdcnt = 0x64,  /* NOT used */
		.trtpd = 0x8,
		.trtpd_05T = 0x0,
		.twtpd = 0x9,
		.twtpd_05T = 0x0,
		.tmrr2w_ODT_off = 0x1,
		.tmrr2w_ODT_on = 0x3,
		.ckeprd = 1,  /* NOT used */
		.ckelckcnt = 3,  /* NOT used */
		.zqlat2 = 6,  /* NOT used */

		.dqsinctl = 0x2,
		.datlat = 0xf,
	},
#endif
#endif /* SUPPORT_TYPE_PCDDR3 */
};

typedef struct {
	unsigned char trfc:8;
	unsigned char trfrc_05t:1;
	unsigned short txrefcnt_val:10;
} optimizeACTime;
const optimizeACTime tRFCab_Opt[GRP_ACTIM_NUM][tRFCAB_NUM] = {
	/* For freqGroup DDR1600 */
	{
		/* tRFCab = 130 */
		{.trfc = 14, .trfrc_05t = 0, .txrefcnt_val = 32},
		/* tRFCab = 180 */
		{.trfc = 24, .trfrc_05t = 0, .txrefcnt_val = 42},
		/* tRFCab = 280 */
		{.trfc = 44, .trfrc_05t = 0, .txrefcnt_val = 62},
		/* tRFCab = 380 */
		{.trfc = 64, .trfrc_05t = 0, .txrefcnt_val = 82}
	},
	/* For freqGroup DDR2400 */
	{
		{.trfc = 27, .trfrc_05t = 0, .txrefcnt_val = 46},
		{.trfc = 42, .trfrc_05t = 0, .txrefcnt_val = 61},
		{.trfc = 72, .trfrc_05t = 0, .txrefcnt_val = 91},
		{.trfc = 114, .trfrc_05t = 0, .txrefcnt_val = 133}
	},
	/* For freqGroup DDR2667 */
	{
		{.trfc = 31, .trfrc_05t = 1, .txrefcnt_val = 50},
		{.trfc = 48, .trfrc_05t = 0, .txrefcnt_val = 67},
		{.trfc = 81, .trfrc_05t = 1, .txrefcnt_val = 100},
		{.trfc = 128, .trfrc_05t = 0, .txrefcnt_val = 147}
	},
	/* For freqGroup DDR3200 */
	{
		/* tRFCab = 130 */
		{.trfc = 40, .trfrc_05t = 0, .txrefcnt_val = 59},
		/* tRFCab = 180 */
		{.trfc = 60, .trfrc_05t = 0, .txrefcnt_val = 79},
		/* tRFCab = 280 */
		{.trfc = 100, .trfrc_05t = 0, .txrefcnt_val = 119},
		/* tRFCab = 380 */
		{.trfc = 140, .trfrc_05t = 0, .txrefcnt_val = 159}
	},
	/* For freqGroup DDR3733 */
	{
		/* tRFCab = 130 */
		{.trfc = 48, .trfrc_05t = 1, .txrefcnt_val = 68},
		/* tRFCab = 180 */
		{.trfc = 72, .trfrc_05t = 0, .txrefcnt_val = 92},
		/* tRFCab = 280 */
		{.trfc = 118, .trfrc_05t = 1, .txrefcnt_val = 138},
		/* tRFCab = 380 */
		{.trfc = 165, .trfrc_05t = 0, .txrefcnt_val = 185}
	},
	/* For freqGroup DDR4266 */
	{
		/* tRFCab = 130 */
		{.trfc = 57, .trfrc_05t = 1, .txrefcnt_val = 78},
		/* tRFCab = 180 */
		{.trfc = 84, .trfrc_05t = 0, .txrefcnt_val = 104},
		/* tRFCab = 280 */
		{.trfc = 137, .trfrc_05t = 1, .txrefcnt_val = 158}
		/* tRFCab = 380 */,
		{.trfc = 191, .trfrc_05t = 0, .txrefcnt_val = 211}
	}
};

#if (SUPPORT_TYPE_LPDDR4 && LPDDR4_DIV4_MODE_EN)
optimizeACTime tRFCab_Opt_Div4 [GRP_ACTIM_NUM_DIV4][tRFCAB_NUM] =
{
	/* For freqGroup DDR800 */
	{
		{.trfc = 14, .trfrc_05t = 0, .txrefcnt_val = 32},
		{.trfc = 24, .trfrc_05t = 0, .txrefcnt_val = 42},
		{.trfc = 44, .trfrc_05t = 0, .txrefcnt_val = 62},
		{.trfc = 72, .trfrc_05t = 0, .txrefcnt_val = 90}
	},
	/* For freqGroup DDR1200 */
	{
		{.trfc = 27, .trfrc_05t = 0, .txrefcnt_val = 46},
		{.trfc = 42, .trfrc_05t = 0, .txrefcnt_val = 61},
		{.trfc = 72, .trfrc_05t = 0, .txrefcnt_val = 91},
		{.trfc = 114, .trfrc_05t = 0, .txrefcnt_val = 133}
	},
	/* For freqGroup DDR1333 */
	{
		{.trfc = 27, .trfrc_05t = 0, .txrefcnt_val = 46},
		{.trfc = 42, .trfrc_05t = 0, .txrefcnt_val = 61},
		{.trfc = 72, .trfrc_05t = 0, .txrefcnt_val = 91},
		{.trfc = 114, .trfrc_05t = 0, .txrefcnt_val = 133}
	},
	/* For freqGroup DDR1600 */
	{
		{.trfc = 40, .trfrc_05t = 0, .txrefcnt_val = 59},
		{.trfc = 60, .trfrc_05t = 0, .txrefcnt_val = 79},
		{.trfc = 100, .trfrc_05t = 0, .txrefcnt_val = 119},
		{.trfc = 156, .trfrc_05t = 0, .txrefcnt_val = 175}
	}
};
#endif

/* Optimize all-bank refresh parameters (by density) for LP4 */
void dramc_ac_timing_optimize(DRAMC_CTX_T *p)
{
	/* TRFC: tRFCab
	 *       Refresh Cycle Time (All Banks)
	 * TXREFCNT: tXSR max((tRFCab + 7.5ns), 2nCK)
	 *           Min self refresh time (Entry to Exit)
	 * execute_optimize: Indicate if ACTimings are updated
	 * at the end of this function
	 */
	unsigned char rf_cab_grp_idx = 0, freq_grp_idx = 0,
		execute_optimize = ENABLE;
	unsigned char trfc = 101, trfc_05_t = 0, tx_ref_cnt = 118;
#if ENABLE_RANK_NUMBER_AUTO_DETECTION
	unsigned int cona, conh, cha_a, chb_a;
	static unsigned char emi_update_flag = 0;
#endif
	/*
	* Values retrieved from 1. Alaska ACTiming excel file
	* 2. JESD209-4B Refresh requirement table
	*/

	show_msg((INFO, "[ACTimingOptimize]"));

	/* already read MR8 for density update */
	/* Set tRFCab group idx p->density = MR8 OP[5:2] */
	switch (p->density) {
	case 0x0:	/* 4Gb per die  (2Gb per channel),  tRFCab=130 */
		rf_cab_grp_idx = tRFCAB_130;
		break;
	case 0x1:	/* 6Gb per die  (3Gb per channel),  tRFCab=180 */
	case 0x2:	/* 8Gb per die  (4Gb per channel),  tRFCab=180 */
		rf_cab_grp_idx = tRFCAB_180;
		break;
	case 0x3:	/* 12Gb per die (6Gb per channel),  tRFCab=280 */
	case 0x4:	/* 16Gb per die (8Gb per channel),  tRFCab=280 */
		rf_cab_grp_idx = tRFCAB_280;
		break;
	case 0x5:	/* 24Gb per die (12Gb per channel), tRFCab=380 */
	case 0x6:	/* 32Gb per die (16Gb per channel), tRFCab=380 */
		rf_cab_grp_idx = tRFCAB_380;
		break;
	default:
		execute_optimize = DISABLE;
		show_err("MR8 density err!\n");
	}
#if ENABLE_RANK_NUMBER_AUTO_DETECTION
	if (emi_update_flag == 0)
	{
		emi_update_flag = 1;
		if (p->support_rank_num == RANK_DUAL)
		{
			switch (p->density) {
			case 0x0:	/* 4Gb per die  (2Gb per channel),  tRFCab=130 */
				cona = 0x50535154;
				conh = 0x11110003;
				cha_a = 0x04115051;
				chb_a = 0x04115051;
				break;
			case 0x2:	/* 8Gb per die  (4Gb per channel),  tRFCab=180 */
				cona = 0xa053a154;
				conh = 0x22220003;
				cha_a = 0x0422a051;
				chb_a = 0x0422a051;
				break;
			case 0x3:	/* 12Gb per die (6Gb per channel),  tRFCab=280 */
				cona = 0xf053f154;
				conh = 0x33330003;
				cha_a = 0x0433f051;
				chb_a = 0x0433f051;
				break;
			case 0x4:	/* 16Gb per die (8Gb per channel),	tRFCab=280 */
				cona = 0xf053f154;
				conh = 0x44440003;
				cha_a = 0x0444f051;
				chb_a = 0x0444f051;
				break;
			case 0x5:	/* 24Gb per die (12Gb per channel), tRFCab=380 */
				cona = 0x03530154;
				conh = 0x66660003;
				cha_a = 0x0466005d;
				chb_a = 0x0466005d;
				break;
			case 0x6:	/* 32Gb per die (16Gb per channel), tRFCab=380 */
				cona = 0x03530154;
				conh = 0x88880003;
				cha_a = 0x0488005d;
				chb_a = 0x0488005d;
				break;
			default:
				cona = 0xf053f154;
				conh = 0x44440003;
				cha_a = 0x0444f051;
				chb_a = 0x0444f051;
				show_err("MR8 density err!\n");
			}
		}
		else
		{
			switch (p->density) {
			case 0x0:	/* 4Gb per die  (2Gb per channel),  tRFCab=130 */
				cona = 0x50505154;
				conh = 0x01010003;
				cha_a = 0x04015050;
				chb_a = 0x04015050;
				break;
			case 0x2:	/* 8Gb per die  (4Gb per channel),  tRFCab=180 */
				cona = 0xa050a154;
				conh = 0x02020003;
				cha_a = 0x04021050;
				chb_a = 0x04021050;
				break;
			case 0x3:	/* 12Gb per die (6Gb per channel),  tRFCab=280 */
				cona = 0xf050f154;
				conh = 0x03030003;
				cha_a = 0x0403f050;
				chb_a = 0x0403f050;
				break;
			case 0x4:	/* 16Gb per die (8Gb per channel),	tRFCab=280 */
				cona = 0xf050f154;
				conh = 0x04040003;
				cha_a = 0x0404f050;
				chb_a = 0x0404f050;
				break;
			case 0x5:	/* 24Gb per die (12Gb per channel), tRFCab=380 */
				cona = 0x03500154;
				conh = 0x06060003;
				cha_a = 0x0406005c;
				chb_a = 0x0406005c;
				break;
			case 0x6:	/* 32Gb per die (16Gb per channel), tRFCab=380 */
				cona = 0x03500154;
				conh = 0x08080003;
				cha_a = 0x0408005c;
				chb_a = 0x0408005c;
				break;
			default:
				cona = 0xf050f154;
				conh = 0x04040003;
				cha_a = 0x0404f050;
				chb_a = 0x0404f050;
				show_err("MR8 density err!\n");
			}
		}
		*(volatile unsigned int*)EMI_CONA = cona;
		*(volatile unsigned int*)EMI_CONH = conh;
		*(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE) = cha_a;
		*(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE) = chb_a;
		show_msg((INFO, "[AUTO DETECT] EMI CONA: %x\n", *(volatile unsigned int*)EMI_CONA));
		show_msg((INFO, "[AUTO DETECT] EMI CONH: %x\n", *(volatile unsigned int*)EMI_CONH));
		show_msg((INFO, "[AUTO DETECT] EMI CHA CONA: %x\n", *(volatile unsigned int*)CHN_EMI_CONA(CHN0_EMI_BASE)));
		show_msg((INFO, "[AUTO DETECT] EMI CHB CONA: %x\n", *(volatile unsigned int*)CHN_EMI_CONA(CHN1_EMI_BASE)));
	}
#endif

	switch (p->freqGroup) {
	case DDR800_FREQ:
		if (get_div_mode(p) == DIV4_MODE)
			freq_grp_idx = GRP_DDR800_DIV4_ACTIM;
		else
			execute_optimize = DISABLE;
		break;
	case DDR1200_FREQ:
		if (get_div_mode(p) == DIV4_MODE)
			freq_grp_idx = GRP_DDR1200_DIV4_ACTIM;
		else
			execute_optimize = DISABLE;
		break;
	case DDR1333_FREQ:
		if (get_div_mode(p) == DIV4_MODE)
			freq_grp_idx = GRP_DDR1333_DIV4_ACTIM;
		else
			execute_optimize = DISABLE;
		break;
	case DDR1600_FREQ:
		if (get_div_mode(p) == DIV4_MODE)
			freq_grp_idx = GRP_DDR1600_DIV4_ACTIM;
		else
			freq_grp_idx = GRP_DDR1600_ACTIM;
		break;
	case DDR2400_FREQ:
		freq_grp_idx = GRP_DDR2400_ACTIM;
		break;
	case DDR2666_FREQ:
		freq_grp_idx = GRP_DDR2666_ACTIM;
		break;
	case DDR3200_FREQ:
		freq_grp_idx = GRP_DDR3200_ACTIM;
		break;
	default:
		execute_optimize = DISABLE;
		show_err("freqGroup err!\n");
	}

	trfc = tRFCab_Opt[freq_grp_idx][rf_cab_grp_idx].trfc;
	trfc_05_t = tRFCab_Opt[freq_grp_idx][rf_cab_grp_idx].trfrc_05t;
	tx_ref_cnt = tRFCab_Opt[freq_grp_idx][rf_cab_grp_idx].txrefcnt_val;

	/*
	 * Only execute ACTimingOptimize(write to regs)
	 * when corresponding values have been found
	 */
	if (execute_optimize == ENABLE) {
		io_32_write_fld_align_all(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM3),
			trfc, SHU_ACTIM3_TRFC);
		io_32_write_fld_align_all(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_AC_TIME_05T), trfc_05_t,
			SHU_AC_TIME_05T_TRFC_05T);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4),
			tx_ref_cnt, SHU_ACTIM4_TXREFCNT);

		show_msg((INFO,
			"%s %u, TRFC %u, TRFC_05T %u, TXREFCNT %u\n",
			"Density (MR8 OP[5:2])",
			p->density, trfc, trfc_05_t, tx_ref_cnt));
	}
}

/*
 *  UpdateACTimingReg()
 *  ACTiming related register field update
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  ACTbl           Pointer to correct ACTiming table struct
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
DRAM_STATUS_T ddr_update_ac_timing_reg(DRAMC_CTX_T *p, const ACTime_T *ac_tbl)
{
	/*
	 * ac_tbl_final: Use to set correct ACTiming values and
	 * write into registers
	 * Variable used in step 1 (decide to use odt on or off ACTiming)
	 * ACTiming regs that have ODT on/off values ->
	 * declare variables to save the wanted value ->
	 * Used to retrieve correct SHU_ACTIM2_TR2W value and
	 * write into final register field
	 */
	ACTime_T ac_tbl_final;
	DRAM_ODT_MODE_T r2w_odt_onoff = p->odt_onoff;
	unsigned char trtw, trtw_05t, tmrr2w;
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	unsigned char rankinctl = 0;
#endif
	/* Used to store tmp ACTiming values */
	unsigned char rodt_tracking_saveing_mck = 0, root = 0,
		tx_rank_in_ctl = 0, tx_dly = 0, datlat_dsel = 0;

#if SAMSUNG_LP4_NWR_WORKAROUND
	/* Port from MT8183, special workaround for Samsung LPDDR4 */
	U8 twtr = 0, twtr_05t = 0, twtr_tmp = 0;
#endif
	/* ACTiming regs that aren't currently in ACTime_T struct */
	unsigned char trefbw = 0;	/* REFBW_FR (tREFBW) for LP3 */

	if (ac_tbl == NULL)
		return DRAM_FAIL;
	ac_tbl_final = *ac_tbl;

	/*
	 * Step 1: Perform ACTiming table adjustments according to
	 * different usage/scenarios--
	 */
	r2w_odt_onoff = p->odt_onoff;

	/*
	 * ACTimings that have different values for odt on/off,
	 * retrieve the correct one and store in local variable
	 */
	if (r2w_odt_onoff == ODT_ON)	/* odt_on */ {
		trtw = ac_tbl_final.trtw_ODT_on;
		trtw_05t = ac_tbl_final.trtw_ODT_on_05T;
		tmrr2w = ac_tbl_final.tmrr2w_ODT_on;
	} else {	/* odt_off */
		trtw = ac_tbl_final.trtw_ODT_off;
		trtw_05t = ac_tbl_final.trtw_ODT_off_05T;
		tmrr2w = ac_tbl_final.tmrr2w_ODT_off;
	}

#if 0
	/* Override the above tRTW & tRTW_05T selection for Hynix LPDDR4P dram
	 * (always use odt_on's value for tRTW)
	 * (temp solution, need to discuss with SY)
	 */
	if (p->dram_type == TYPE_LPDDR4P) {
		trtw = ac_tbl_final.trtw_ODT_on;
		trtw_05t = ac_tbl_final.trtw_ODT_on_05T;
	}
#endif

	/*
	 * REFBW_FR in LP3 ACTiming excel file (value == 176 for all freqs),
	 * LP4 doesn't use this register -> set to 0
	 */
	trefbw = 0;

#if (ENABLE_RODT_TRACKING)
	/*
	 * set to 0, let TRTW & XRTR2W setting values
	 * are the smae with DV-sim's value that DE provided
	 */
	rodt_tracking_saveing_mck = 0;

#endif

	/* Update values that are used by rodt_tracking_saveing_mck */
	trtw = trtw - rodt_tracking_saveing_mck;
	ac_tbl_final.xrtr2w = ac_tbl_final.xrtr2w - rodt_tracking_saveing_mck;

#if SAMSUNG_LP4_NWR_WORKAROUND
    // If nWR is fixed to 30 for all freqs, tWTR@800Mhz should add 2tCK gap, allowing sufficient Samsung DRAM internal IO precharge time
    // As we are using 1st generation LP4 Samsung chip, it seems we have the similar problem on all frequency.
    // So we should apply the +2tCK patch to all freq but not only 800MHz
    if (is_lp4_family(p) && (p->vendor_id == DDR_VENDOR_SAMSUNG)) //LP4X, Samsung
    {
        twtr_tmp = (ac_tbl_final.twtr * 4 - ac_tbl_final.twtr_05T * 2) + 2; //Convert TWTR to tCK, and add 2tCK
        if ((twtr_tmp % 4) == 0) //TWTR can be transferred to TWTR directly
        {
            twtr = twtr_tmp >> 2;
            twtr_05t = 0;
        }
        else //Can't be transfered to TWTR directly
        {
            twtr = (twtr_tmp + 2) >> 2; //Add 2 tCK and set TWTR value (Then minus 2tCK using 05T)
            twtr_05t = 1;  //05T means minus 2tCK
        }

        ac_tbl_final.twtr = twtr;
        ac_tbl_final.twtr_05T = twtr_05t;
    }
#endif
	/* DATLAT related */
	datlat_dsel = ac_tbl_final.datlat - 4;

#ifndef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	ac_tbl_final.xrtr2r = 12;
#endif

	/* ----Step 2: Perform register writes for entries in ac_tbl_final
	 * struct & ACTiming excel file
	 * (all actiming adjustments should be done in Step 1)-------
	 */
	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM0,
		p_fld(ac_tbl_final.trcd, SHU_ACTIM0_TRCD) |
		p_fld(ac_tbl_final.trrd, SHU_ACTIM0_TRRD) |
		p_fld(ac_tbl_final.twr, SHU_ACTIM0_TWR) |
		p_fld(ac_tbl_final.twtr, SHU_ACTIM0_TWTR));

	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM1,
		p_fld(ac_tbl_final.trc, SHU_ACTIM1_TRC) |
		p_fld(ac_tbl_final.tras, SHU_ACTIM1_TRAS) |
		p_fld(ac_tbl_final.trp, SHU_ACTIM1_TRP) |
		p_fld(ac_tbl_final.trpab, SHU_ACTIM1_TRPAB));

	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM2,
		p_fld(ac_tbl_final.tfaw, SHU_ACTIM2_TFAW) |
		p_fld(trtw, SHU_ACTIM2_TR2W) |
		p_fld(ac_tbl_final.trtp, SHU_ACTIM2_TRTP) |
		p_fld(ac_tbl_final.txp, SHU_ACTIM2_TXP));

	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM3,
		p_fld(ac_tbl_final.trfc, SHU_ACTIM3_TRFC) |
		p_fld(ac_tbl_final.refcnt, SHU_ACTIM3_REFCNT) |
		p_fld(0x4, SHU_ACTIM3_MANTMRR) |
		p_fld(ac_tbl_final.trfcpb, SHU_ACTIM3_TRFCPB));

	if (get_div_mode(p) == DIV8_MODE) {
		io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM3,
			p_fld(0x4, SHU_ACTIM3_MANTMRR) |
			p_fld(0x4, SHU_ACTIM3_TR2MRR));
	} else {
		io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM3,
			p_fld(0x8, SHU_ACTIM3_MANTMRR) |
			p_fld(0x8, SHU_ACTIM3_TR2MRR));
	}
	if (p->dram_type == TYPE_PCDDR3 || p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM3,
			p_fld(0x0, SHU_ACTIM3_MANTMRR) |
			p_fld(0x0, SHU_ACTIM3_TR2MRR));
	}
	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM4,
		p_fld(ac_tbl_final.tzqcs, SHU_ACTIM4_TZQCS) |
		p_fld(ac_tbl_final.refcnt_fr_clk, SHU_ACTIM4_REFCNT_FR_CLK) |
		p_fld(ac_tbl_final.txrefcnt, SHU_ACTIM4_TXREFCNT));

	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM5,
		p_fld(tmrr2w, SHU_ACTIM5_TMRR2W) |
		p_fld(ac_tbl_final.twtpd, SHU_ACTIM5_TWTPD) |
		p_fld(ac_tbl_final.trtpd, SHU_ACTIM5_TR2PD));

	if (p->dram_type == TYPE_PCDDR4) {
		io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM6,
			p_fld(ac_tbl_final.tbg_ccd, SHU_ACTIM6_BGTCCD) |
			p_fld(ac_tbl_final.tbg_wtr, SHU_ACTIM6_BGTWTR) |
			p_fld(ac_tbl_final.tbg_rrd, SHU_ACTIM6_BGTRRD));
	}

	io_32_write_fld_multi_all(DRAMC_REG_SHU_ACTIM_XRT,
		p_fld(ac_tbl_final.xrtw2w, SHU_ACTIM_XRT_XRTW2W) |
		p_fld(ac_tbl_final.xrtw2r, SHU_ACTIM_XRT_XRTW2R) |
		p_fld(ac_tbl_final.xrtr2w, SHU_ACTIM_XRT_XRTR2W) |
		p_fld(ac_tbl_final.xrtr2r, SHU_ACTIM_XRT_XRTR2R));

	/* AC timing 0.5T */
	io_32_write_fld_multi_all(DRAMC_REG_SHU_AC_TIME_05T,
		p_fld(ac_tbl_final.twtr_05T, SHU_AC_TIME_05T_TWTR_M05T) |
		p_fld(trtw_05t, SHU_AC_TIME_05T_TR2W_05T) |
		p_fld(ac_tbl_final.twtpd_05T, SHU_AC_TIME_05T_TWTPD_M05T) |
		p_fld(ac_tbl_final.trtpd_05T, SHU_AC_TIME_05T_TR2PD_05T) |
		p_fld(ac_tbl_final.tfaw_05T, SHU_AC_TIME_05T_TFAW_05T) |
		p_fld(ac_tbl_final.trrd_05T, SHU_AC_TIME_05T_TRRD_05T) |
		p_fld(ac_tbl_final.twr_05T, SHU_AC_TIME_05T_TWR_M05T) |
		p_fld(ac_tbl_final.tras_05T, SHU_AC_TIME_05T_TRAS_05T) |
		p_fld(ac_tbl_final.trpab_05T, SHU_AC_TIME_05T_TRPAB_05T) |
		p_fld(ac_tbl_final.trp_05T, SHU_AC_TIME_05T_TRP_05T) |
		p_fld(ac_tbl_final.trcd_05T, SHU_AC_TIME_05T_TRCD_05T) |
		p_fld(ac_tbl_final.trtp_05T, SHU_AC_TIME_05T_TRTP_05T) |
		p_fld(ac_tbl_final.txp_05T, SHU_AC_TIME_05T_TXP_05T) |
		p_fld(ac_tbl_final.trfc_05T, SHU_AC_TIME_05T_TRFC_05T) |
		p_fld(ac_tbl_final.trfcpb_05T, SHU_AC_TIME_05T_TRFCPB_05T) |
		p_fld(ac_tbl_final.tbg_ccd_05T, SHU_AC_TIME_05T_BGTCCD_05T) |
		p_fld(ac_tbl_final.tbg_wtr_05T, SHU_AC_TIME_05T_BGTWTR_05T) |
		p_fld(ac_tbl_final.trc_05T, SHU_AC_TIME_05T_TRC_05T));

	/*
	 * CATRAIN_INTV isn't a shuffle register,
	 * but only affects LP4 CBT timing intv. during calibration
	 */
	/* CATRAIN_INTV is calculated based on CATRAINLAT = 0 */
	io_32_write_fld_multi_all(DRAMC_REG_CATRAINING1,
		p_fld(ac_tbl_final.r_dmcatrain_intv, CATRAINING1_CATRAIN_INTV) |
		p_fld(CLEAR_FLD, CATRAINING1_CATRAINLAT));

	/* DQSINCTL related */
	io_32_write_fld_align_all(DRAMC_REG_SHURK0_DQSCTL,
		ac_tbl_final.dqsinctl, SHURK0_DQSCTL_DQSINCTL);
	io_32_write_fld_align_all(DRAMC_REG_SHURK1_DQSCTL,
		ac_tbl_final.dqsinctl, SHURK1_DQSCTL_R1DQSINCTL);

	io_32_write_fld_align_all(DRAMC_REG_SHU_ODTCTRL,
		ac_tbl_final.dqsinctl, SHU_ODTCTRL_RODT);

	/* DATLAT related, tREFBW */
	io_32_write_fld_multi_all(DRAMC_REG_SHU_CONF1,
		p_fld(ac_tbl_final.datlat, SHU_CONF1_DATLAT) |
		p_fld(datlat_dsel, SHU_CONF1_DATLAT_DSEL) |
		p_fld(datlat_dsel, SHU_CONF1_DATLAT_DSEL_PHY) |
		p_fld(trefbw, SHU_CONF1_REFBW_FR));

	/*
	 * FSPCHG_PRDCNT: LPDDR4 tFC constraint
	 */
	io_32_write_fld_align_all(DRAMC_REG_SHU_CONF2,
		ac_tbl_final.r_dmfspchg_prdcnt,
		SHU_CONF2_FSPCHG_PRDCNT);

	/*
	 * TODO: MRW_INTV can be set to different values for each freq,
	 * request new forumula/values from Berson
	 */
	io_32_write_fld_multi_all(DRAMC_REG_SHU_SCINTV,
		p_fld(ac_tbl_final.r_dmmrw_intv, SHU_SCINTV_MRW_INTV) |
		p_fld(ac_tbl_final.zqlat2, SHU_SCINTV_TZQLAT2)); /* cc change. REVIEW?? */

	/* CKEPRD - CKE pulse width */
	io_32_write_fld_align_all(DRAMC_REG_SHU_CKECTRL, ac_tbl_final.ckeprd,
		SHU_CKECTRL_CKEPRD);

	io_32_write_fld_align_all(DRAMC_REG_CKECTRL, ac_tbl_final.ckelckcnt,
		CKECTRL_CKELCKCNT);
	/*
	 * Step 3: Perform register writes/calculation for other regs
	 * (That aren't in ac_tbl_final struct)
	 */
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	/*
	 * Ininital setting values are the same,
	 * RANKINCTL_RXDLY = RANKINCTL = RANKINCTL_ROOT1
	 * XRTR2R setting will be updated in RxdqsGatingPostProcess
	 */
	rankinctl =
		io_32_read_fld_align(DRAMC_REG_SHU_RANKCTL,
		SHU_RANKCTL_RANKINCTL);
	io_32_write_fld_align_all(DRAMC_REG_SHU_RANKCTL, rankinctl,
		SHU_RANKCTL_RANKINCTL_RXDLY);
#endif

	/* Update related RG of XRTW2W */
	if (p->frequency <= DDR1600_FREQ) {
		root = 0;
		if (get_div_mode(p) == DIV8_MODE) {
			tx_rank_in_ctl = 0;
			tx_dly = 1;
		} else {
			tx_rank_in_ctl = 1;
			tx_dly = 2;
		}
	} else if (p->frequency <= DDR3200_FREQ) {
		root = 0;
		tx_rank_in_ctl = 1;
		tx_dly = 2;
	} else if (p->frequency <= DDR3733_FREQ) {
		root = 1;
		tx_rank_in_ctl = 1;
		tx_dly = 2;
	} else { /* (p->frequency == DDR4266_FREQ) */
		root = 1;
		tx_rank_in_ctl = 2;
		tx_dly = 3;
	}
	io_32_write_fld_multi_all(DRAMC_REG_SHU_RANKCTL,
		p_fld(root, SHU_RANKCTL_TXRANKINCTL_ROOT) |
		p_fld(tx_dly, SHU_RANKCTL_TXRANKINCTL_TXDLY) |
		p_fld(tx_rank_in_ctl, SHU_RANKCTL_TXRANKINCTL));

	return DRAM_OK;
}

/*
 * get_ac_timing_idx()
 *  Retrieve internal ac_timing_tbl's index according to dram type, freqGroup,
 * Read DBI status
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval timing_idx     Return ac_timing_tbl entry's index
 */
static unsigned char get_ac_timing_idx(DRAMC_CTX_T *p)
{
	unsigned char timing_idx = 0xff, tmp_idx;
	unsigned char tmp_dram_type = p->dram_type;

	/* LP4/LP4P/LP4X use same table */
	if ((tmp_dram_type == TYPE_LPDDR4X) || (tmp_dram_type == TYPE_LPDDR4P))
		tmp_dram_type = TYPE_LPDDR4;

	/* p->frequency may not be in ACTimingTable, use p->freqGroup */
	/* LP4 byte/mixed mode dram both use byte mode ACTiming */
	for (tmp_idx = 0; tmp_idx < TOTAL_AC_TIMING_NUMBER; tmp_idx++) {
		if ((ac_timing_tbl[tmp_idx].dramType == tmp_dram_type) &&
			(ac_timing_tbl[tmp_idx].freqGroup == p->freqGroup) &&
			(ac_timing_tbl[tmp_idx].readDBI == p->dbi_r_onoff[p->dram_fsp]) &&
			(ac_timing_tbl[tmp_idx].cbtMode == get_dram_cbt_mode(p)) &&
			(ac_timing_tbl[tmp_idx].divmode == get_div_mode(p))) {
			timing_idx = tmp_idx;
			show_msg((INFO, "match AC timing %d\n", timing_idx));
			break;
		}
	}
	return timing_idx;
}

DRAM_STATUS_T ddr_update_ac_timing(DRAMC_CTX_T *p)
{
	unsigned char timing_idx = 0;

	show_msg3((INFO, "[UpdateACTiming] "));

	/* Retrieve ACTimingTable's corresponding index */
	timing_idx = get_ac_timing_idx(p);

	if (timing_idx == 0xff) {
		show_err("Error, no match AC timing, not apply table\n");
		return DRAM_FAIL;
	}
	/* Set ACTiming registers */
	show_msg3((INFO, "timing_idx = %d\n", timing_idx));
	ddr_update_ac_timing_reg(p, &ac_timing_tbl[timing_idx]);

	return DRAM_OK;
}

#if (FOR_DV_SIMULATION_USED == 0)
DRAM_STATUS_T ddr_update_ac_timing_emi(DRAMC_CTX_T *p,
		AC_TIMING_EXTERNAL_T *ac_reg_from_emi)
{
	unsigned char timing_idx = 0;
	ACTime_T ACTime;

	show_msg3((INFO, "[ddr_update_ac_timing_emi]\n"));

	if (ac_reg_from_emi == NULL)
		return DRAM_FAIL;

	/* Retrieve ACTimingTable's corresponding index */
	timing_idx = get_ac_timing_idx(p);
	ACTime = ac_timing_tbl[timing_idx];

	/* Overwrite AC timing from emi settings */
	ACTime.dramType = p->dram_type;
	/*Will use MDL ac timing, Others from internal ac timing*/
	ACTime.trp = ac_reg_from_emi->AC_TIME_EMI_TRP;
	ACTime.trpab = ac_reg_from_emi->AC_TIME_EMI_TRPAB;
	ACTime.trc = ac_reg_from_emi->AC_TIME_EMI_TRC;
	ACTime.trcd = ac_reg_from_emi->AC_TIME_EMI_TRCD;

	ACTime.trp_05T = ac_reg_from_emi->AC_TIME_EMI_TRP_05T;
	ACTime.trpab_05T = ac_reg_from_emi->AC_TIME_EMI_TRPAB_05T;
	ACTime.trc_05T = ac_reg_from_emi->AC_TIME_EMI_TRC_05T;
	ACTime.trcd_05T = ac_reg_from_emi->AC_TIME_EMI_TRCD_05T;

	/* Set ACTiming registers */
	ddr_update_ac_timing_reg(p, &ACTime);

	return DRAM_OK;
}
#endif
