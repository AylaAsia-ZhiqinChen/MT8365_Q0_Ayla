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

#include "dramc_pi_api.h"
#include "dramc_common.h"
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

//#define DRAM_HQA
#ifdef DRAM_HQA
//#define HVCORE_HVDRAM
#define NVCORE_NVDRAM
//#define LVCORE_LVDRAM
//#define HVCORE_LVDRAM
//#define LVCORE_HVDRAM
//#define NVCORE_LVDRAM
//#define NVCORE_HVDRAM
#endif //end #ifdef DRAM_HQA

//#define DDR_RESERVE_MODE
#define COMBO_MCP
#define DRAM_BASE 0x40000000ULL
#define DDR_BASE DRAM_BASE
//#define DRAM_CALIB_LOG

int get_dram_rank_nr (void);
void get_dram_rank_size(unsigned int dram_rank_size[]);

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
    U32 AC_TIME_EMI_TRTPD            :8; // LP4/LP3, // Olympus new 
    U32 AC_TIME_EMI_TWTPD            :8; // LP4/LP3, // Olympus new 
    U32 AC_TIME_EMI_TMRR2W_ODT_OFF   :8; // LP4      // Olympus new 
    U32 AC_TIME_EMI_TMRR2W_ODT_ON    :8; // LP4      // Olympus new 

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
    U32 AC_TIME_EMI_TWTR_05T          :2; // Olympus modified
    U32 AC_TIME_EMI_TRRD_05T          :2;
    // Byte3	
    U32 AC_TIME_EMI_TFAW_05T          :2;
    U32 AC_TIME_EMI_TRTW_ODT_OFF_05T  :2;
    U32 AC_TIME_EMI_TRTW_ODT_ON_05T   :2;
    U32 AC_TIME_EMI_TRTPD_05T         :2; // LP4/LP3 // Olympus new 

    // U 07
    // Byte0
    U32 AC_TIME_EMI_TWTPD_05T           :2; // LP4/LP3 // Olympus new
    U32 AC_TIME_EMI_TMRR2W_ODT_OFF_05T  :2; // Useless, no 0.5T in Olympus and Elbrus
    U32 AC_TIME_EMI_TMRR2W_ODT_ON_05T   :2; // Useless, no 0.5T in Olympus and Elbrus

   
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
    unsigned int   dram_cbt_mode;
    unsigned int   reserved[6];
    unsigned int   iLPDDR3_MODE_REG_5;
    unsigned int   PIN_MUX_TYPE;   
} EMI_SETTINGS;

typedef struct
{               
	unsigned int  EMI_CONA_VAL;
	unsigned int  EMI_CONH_VAL;
	unsigned int  DRAM_RANK_SIZE[4];
	unsigned int  DRAM_tRFC;
	unsigned int  DRAM_tREFI;
} DRAM_COM_SETTINGS;

uint32 mt_set_emis(uint8* emi, uint32 len, bool use_default); //array of emi setting.

extern int num_of_emi_records;
extern int emi_setting_index;
extern EMI_SETTINGS emi_settings[];
extern EMI_SETTINGS default_emi_setting;
extern void mt_set_emisettings(int index);
extern EMI_SETTINGS *mt_get_emisettings(void);
extern int mt_get_dram_type (void);
extern U8 mt_get_dram_comsetting_type(EMI_SETTINGS *emi_set);

//================================================
//=============pmic related api for ETT HQA test ==============
//================================================

#if defined(DRAM_HQA) || defined(DRAM_ETT)
//===========Vcore=====================================
#define VCORE_LV 0x3F // 0.7+0.00625*63=1.09375
#define VCORE_NV 0x48 // 0.7+0.00625*72=1.15
#define VCORE_HV 0x61 // 0.7+0.00625*97=1.30625

//===========Vdram=====================================
#define VDRAM_LV_LPDDR3 0x3 // 1.2-0.02*3=1.14
#define VDRAM_NV_LPDDR3 0x0 // 1.2+0.02*0=1.20
#define VDRAM_HV_LPDDR3 0xB // 1.2+0.02*5=1.30

#define VDRAM_LV_PCDDR3 0x3 // 1.35-0.02*3=1.29
#define VDRAM_NV_PCDDR3 0x0 // 1.35+0.02*0=1.35
#define VDRAM_HV_PCDDR3 0xB // 1.35+0.02*5=1.45

#define VDRAM_LV_PCDDR4 0x3 // 1.2-0.02*3=1.14
#define VDRAM_NV_PCDDR4 0x0 // 1.2+0.02*0=1.20
#define VDRAM_HV_PCDDR4 0xD // 1.2+0.02*3=1.26

enum 
{
	ETT_HVCORE_HVDRAM = 1,
	ETT_NVCORE_NVDRAM,
	ETT_LVCORE_LVDRAM,
	ETT_HVCORE_LVDRAM,
	ETT_LVCORE_HVDRAM,
	ETT_NVCORE_LVDRAM,
	ETT_NVCORE_HVDRAM,
	ETT_VCORE_INC,
	ETT_VCORE_DEC,
	ETT_VDRAM_INC,
	ETT_VDRAM_DEC,
	ETT_VCORE_VDRAM_MAX 
};

typedef struct _VOLTAGE_VALUE_T
{
	U8 HV;
	U8 NV;
	U8 LV;
}VOLTAGE_VALUE_T;

typedef struct _VOLTAGE_LIST_T
{
	VOLTAGE_VALUE_T vcore;
	VOLTAGE_VALUE_T vdram;
}VOLTAGE_LIST_T;

typedef struct _CRITERIA_SIGNAL_T
{
    U32 CA;
    U32 RX;
    U32 TX;
}CRITERIA_SIGNAL_T;

typedef struct _CRITERIA_LIST_T
{
    CRITERIA_SIGNAL_T HTLV;
    CRITERIA_SIGNAL_T NTNV;
    CRITERIA_SIGNAL_T LTHV;
    CRITERIA_SIGNAL_T BUFF;
}CRITERIA_LIST_T;

typedef enum
{
    ETT_FAIL = 0,
    ETT_WARN,
    ETT_PASS,
    ETT_RET_MAX
}ETT_RESULT_T;
#endif //defined(DRAM_HQA) || defined(DRAM_ETT)

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

#endif
