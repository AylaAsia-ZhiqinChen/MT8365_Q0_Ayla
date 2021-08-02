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
 * The following software/firmware and/or related documentation ("Media Tek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef MT_EMI_H
#define MT_EMI_H

#include "dramc_api.h"
#include "dramc_common.h"
#include "ufs_aio_core.h"
#include "mmc_core.h"

#if __FLASH_TOOL_DA__
  #include "sys/types.h"
  #include "driver/hw_types.h"

  #define RAMSIZE_BEGIN_PATTERN	"<<<RAM_BEGIN>>>"
  #define RAMSIZE_END_PATTERN		"<<<RAM_END>>>"
  #define RAMRW_PATTERN           "<<<RAM_RW>>>"

  #define RAM_STEP_UINT	16*1024		//16M
  #define DRAM_MAX_SIZE	0x80000000		//8G
#else
  #include "typedefs.h"
#endif

#if defined(TARGET_BUILD_VARIANT_ENG) || defined(TARGET_BUILD_VARIANT_USERDEBUG)
#define DDR_RESERVE_MODE
#endif
#define COMBO_MCP
#define DRAM_BASE 0x40000000ULL
#define DDR_BASE DRAM_BASE
//#define DRAM_CALIB_LOG

// it is NVCORE_NVDRAM by default
//#define HVCORE_HVDRAM
//#define LVCORE_LVDRAM

int get_dram_rank_nr (void);
void get_dram_rank_size(unsigned long long dram_rank_size[]);

typedef struct _AC_TIMING_EXTERNAL_T
{
    // U 00
    U32 AC_TIME_EMI_FREQUENCY      :16;
    U32 AC_TIME_EMI_TRAS           :8;
    U32 AC_TIME_EMI_TRP            :8;

    // U 01
    U32 AC_TIME_EMI_TRPAB          :8;
    U32 AC_TIME_EMI_TRC            :8;
    U32 AC_TIME_EMI_TRFC           :8;
    U32 AC_TIME_EMI_TRFCPB         :8;

    // U 02
    U32 AC_TIME_EMI_TXP            :8;
    U32 AC_TIME_EMI_TRTP           :8;
    U32 AC_TIME_EMI_TRCD           :8;
    U32 AC_TIME_EMI_TWR            :8;

    // U 03
    U32 AC_TIME_EMI_TWTR           :8;
    U32 AC_TIME_EMI_TRRD           :8;
    U32 AC_TIME_EMI_TFAW           :8;
    U32 AC_TIME_EMI_TRTW_ODT_OFF   :4;
    U32 AC_TIME_EMI_TRTW_ODT_ON    :4;

    // U 04
    U32 AC_TIME_EMI_REFCNT         :8; //(REFFRERUN = 0)
    U32 AC_TIME_EMI_REFCNT_FR_CLK  :8; //(REFFRERUN = 1)
    U32 AC_TIME_EMI_TXREFCNT       :8;
    U32 AC_TIME_EMI_TZQCS          :8;

    // U 05
    U32 AC_TIME_EMI_TRTPD            :8; // LP4/LP3
    U32 AC_TIME_EMI_TWTPD            :8; // LP4/LP3
    U32 AC_TIME_EMI_TMRR2W_ODT_OFF   :8; // LP4
    U32 AC_TIME_EMI_TMRR2W_ODT_ON    :8; // LP4

    // U 06
    // Byte0
    U32 AC_TIME_EMI_TRAS_05T          :2;
    U32 AC_TIME_EMI_TRP_05T           :2;
    U32 AC_TIME_EMI_TRPAB_05T         :2;
    U32 AC_TIME_EMI_TRC_05T           :2;
    // Byte1
    U32 AC_TIME_EMI_TRFC_05T          :2;
    U32 AC_TIME_EMI_TRFCPB_05T        :2;
    U32 AC_TIME_EMI_TXP_05T           :2;
    U32 AC_TIME_EMI_TRTP_05T          :2;
    // Byte2
    U32 AC_TIME_EMI_TRCD_05T          :2;
    U32 AC_TIME_EMI_TWR_05T           :2;
    U32 AC_TIME_EMI_TWTR_05T          :2;
    U32 AC_TIME_EMI_TRRD_05T          :2;
    // Byte3
    U32 AC_TIME_EMI_TFAW_05T          :2;
    U32 AC_TIME_EMI_TRTW_ODT_OFF_05T  :2;
    U32 AC_TIME_EMI_TRTW_ODT_ON_05T   :2;
    U32 AC_TIME_EMI_TRTPD_05T         :2; // LP4/LP3

    // U 07
    // Byte0
    U32 AC_TIME_EMI_TWTPD_05T           :2; // LP4/LP3
    U32 AC_TIME_EMI_TMRR2W_ODT_OFF_05T  :2; // Useless
    U32 AC_TIME_EMI_TMRR2W_ODT_ON_05T   :2; // Useless


}AC_TIMING_EXTERNAL_T;


typedef struct
{
    int   sub_version;            // sub_version: 0x1 for new version
    int  type;                /* 0x0000 : Invalid
                                 0x0001 : Discrete DDR1
                                 0x0002 : Discrete LPDDR2
                                 0x0003 : Discrete LPDDR3
                                 0x0004 : Discrete PCDDR3
                                 0x0005 : Discrete PCDDR4
                                 0x0101 : MCP(NAND+DDR1)
                                 0x0102 : MCP(NAND+LPDDR2)
                                 0x0103 : MCP(NAND+LPDDR3)
                                 0x0104 : MCP(NAND+PCDDR3)
                                 0x0105 : MCP(NAND+PCDDR4)
                                 0x0201 : MCP(eMMC+DDR1)
                                 0x0202 : MCP(eMMC+LPDDR2)
                                 0x0203 : MCP(eMMC+LPDDR3)
                                 0x0204 : MCP(eMMC+PCDDR3)
                                 0x0205 : MCP(eMMC+PCDDR4)
                              */
    int   id_length;              // EMMC and NAND ID checking length
    int   fw_id_length;              // FW ID checking length
    char  ID[16];
    char  fw_id[8];               // To save fw id
    unsigned int   EMI_CONA_VAL;           //@0x3000
    unsigned int   EMI_CONH_VAL;

    union   {
        unsigned int DRAMC_ACTIME_UNION[8];
        AC_TIMING_EXTERNAL_T AcTimeEMI;
    };

    unsigned int   DRAM_RANK_SIZE[4];
    unsigned int   EMI_CONF_VAL;
    unsigned int   CHN0_EMI_CONA_VAL;
    unsigned int   CHN1_EMI_CONA_VAL;
    unsigned int   dram_cbt_mode_extern;
    unsigned int   reserved[6];
    unsigned int   iLPDDR3_MODE_REG_5;
    unsigned int   PIN_MUX_TYPE;
} EMI_SETTINGS;

typedef struct
{
	unsigned int  EMI_CONA_VAL;
	unsigned int  EMI_CHN0_CONA_VAL;
	unsigned int  EMI_CHN1_CONA_VAL;
	unsigned int  EMI_CONF_VAL;
	unsigned int  EMI_CONH_VAL;
} DRAM_COM_SETTINGS;

typedef struct {
	unsigned long long full_sys_addr;
	unsigned int addr;
	unsigned int row;
	unsigned int col;
	unsigned char ch;
	unsigned char rk;
	unsigned char bk;
	unsigned char dummy;
} dram_addr_t;


uint32 mt_set_emis(uint8* emi, uint32 len, bool use_default); //array of emi setting.

extern int num_of_emi_records;
extern int emi_setting_index;
extern EMI_SETTINGS emi_settings[];
extern EMI_SETTINGS default_emi_setting;
extern void mt_set_emisettings(int index);
extern EMI_SETTINGS *mt_get_emisettings(void);
extern int mt_get_dram_type (void);
extern U8 mt_get_dram_comsetting_type(EMI_SETTINGS *emi_set);

#ifdef DRAM_CALIB_LOG
#include <mt_rtc_hw.h>
#include <rtc.h>
#include <pmic_wrap_init.h>
#define CALIB_LOG_BASE 0x0011E000
#define CALIB_LOG_SIZE 8192
#define CALIB_LOG_MAGIC 0x19870611
void dram_klog_clean(void);
void dram_klog_init(void);
int i4WriteSramLog(unsigned int u4Offset, unsigned int *pu4Src, unsigned int u4WordCnt);
typedef struct {
    unsigned int guard;
    unsigned int rtc_dom_hou;
    unsigned int rtc_min_sec;
    unsigned int data_count;
    unsigned int rtc_yea_mth;
} DRAM_KLOG_HEAD;
typedef struct {
    unsigned int dram_type;
    unsigned int check;
    unsigned int guard;
} DRAM_KLOG_TAIL;
#endif //end #ifdef DRAM_CALIB_LOG

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION

#define PART_DRAM_DATA_SIZE	0x100000

#if !DRAM_ETT
#if !defined(BOOTDEV_UFS) && !defined(BOOTDEV_SDMMC)
///TODO:
//#error "BOOTDEV_UFS & BOOTDEV_SDMMC not defined"
#endif
#endif

#if (CFG_BOOT_DEV == BOOTDEV_UFS)
#define	PART_ID_DRAM_DATA	UFS_LU_USER
#elif (CFG_BOOT_DEV == BOOTDEV_SDMMC)
#define	PART_ID_DRAM_DATA	EMMC_PART_USER
#endif

#define	DRAM_CALIBRATION_DATA_MAGIC	0x9502
#define PL_BUILD_TIME_LEN			16

typedef struct _DRAM_CALIBRATION_HEADER_T
{
	u32	pl_version;
	u16	magic_number;
	u32	calib_err_code;
	u8	pl_build_time[PL_BUILD_TIME_LEN];
	u32 size_unused[121];
} DRAM_CALIBRATION_HEADER_T;

typedef struct _DRAM_CALIBRATION_MRR_DATA_T
{
	u16	checksum;
	u16 emi_checksum;
	DRAM_INFO_BY_MRR_T DramInfo;
} DRAM_CALIBRATION_MRR_DATA_T;

typedef struct _DRAM_CALIBRATION_SHU_DATA_T
{
	u16	checksum;
	SAVE_TIME_FOR_CALIBRATION_T	calibration_data;
	u8	checksum_unused[53];
} DRAM_CALIBRATION_SHU_DATA_T;

typedef struct _DRAM_CALIBRATION_DATA_T
{
	DRAM_CALIBRATION_HEADER_T header;
	DRAM_CALIBRATION_MRR_DATA_T mrr_info;
	DRAM_CALIBRATION_SHU_DATA_T	data[DRAM_DFS_SHUFFLE_MAX];
} DRAM_CALIBRATION_DATA_T;


/*
 * g_dram_storage_api_err_code:
 * 	bit[0:3] -> read api
 * 	bit[4:7] -> write api
 * 	bit[8:11] -> clean api
 * 	bit[12:12] -> data formatted due to fatal exception
 */
#define ERR_NULL_POINTER	(0x1)
#define ERR_MAGIC_NUMBER	(0x2)
#define ERR_CHECKSUM		(0x3)
#define ERR_PL_UPDATED		(0x4)
#define ERR_BLKDEV_NOT_FOUND	(0x5)
#define ERR_BLKDEV_READ_FAIL	(0x6)
#define ERR_BLKDEV_WRITE_FAIL	(0x7)
#define ERR_BLKDEV_NO_PART	(0x8)

#define ERR_DATA_FORMATTED_OFFSET	(12)

typedef enum {
	DRAM_STORAGE_API_READ = 0,
	DRAM_STORAGE_API_WRITE,
	DRAM_STORAGE_API_CLEAN,
} DRAM_STORAGE_API_TPYE;

extern u32 g_dram_storage_api_err_code;
#define SET_DRAM_STORAGE_API_ERR(err_type, api_type) \
do {\
	g_dram_storage_api_err_code |= (err_type << (api_type * 4));\
} while(0)

#define SET_DATA_FORMATTED_STORAGE_API_ERR() \
do {\
	g_dram_storage_api_err_code |= (1 << ERR_DATA_FORMATTED_OFFSET);\
} while(0)

int read_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData);
int write_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData);
int clean_dram_calibration_data(void);

void dram_fatal_exception_detection_start(void);
void dram_fatal_exception_detection_end(void);

#define CBT_VREF_OFFSET			2
#define WRITE_LEVELING_OFFSET		5
#define GATING_START_OFFSET		0
#define GATING_PASS_WIN_OFFSET		3
#define RX_WIN_PERBIT_OFFSET		5
#define RX_WIN_PERBIT_VREF_OFFSET	4
#define TX_WIN_PERBIT_OFFSET		5
#define TX_WIN_PERBIT_VREF_OFFSET	4
#define RX_DATLAT_OFFSET		1
#define RX_WIN_HIGH_SPEED_TH		10
#define RX_WIN_LOW_SPEED_TH		100
#define TX_WIN_TH			12

#endif

#endif
