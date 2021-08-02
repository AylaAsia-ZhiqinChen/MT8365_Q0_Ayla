/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef _MT6392_PMIC_LK_SW_H_
#define _MT6392_PMIC_LK_SW_H_

#include <platform/mt_typedefs.h>

//==============================================================================
// PMIC6392 Define
//==============================================================================
#define VBAT_CHANNEL_NUMBER     0
#define ISENSE_CHANNEL_NUMBER       1
#define VCHARGER_CHANNEL_NUMBER     2
#define VBATTEMP_CHANNEL_NUMBER     4

#define VOLTAGE_FULL_RANGE     1800
#define ADC_PRECISE         32768 // 10 bits


typedef enum {
	CHARGER_UNKNOWN = 0,
	STANDARD_HOST,          // USB : 450mA
	CHARGING_HOST,
	NONSTANDARD_CHARGER,    // AC : 450mA~1A
	STANDARD_CHARGER,       // AC : ~1A
	APPLE_2_1A_CHARGER,     // 2.1A apple charger
	APPLE_1_0A_CHARGER,     // 1A apple charger
	APPLE_0_5A_CHARGER,     // 0.5A apple charger
} CHARGER_TYPE;


//==============================================================================
// PMIC6392 Exported Function
//==============================================================================
extern U32 pmic_read_interface (U32 RegNum, U32 *val, U32 MASK, U32 SHIFT);
extern U32 pmic_config_interface (U32 RegNum, U32 val, U32 MASK, U32 SHIFT);
extern U32 pmic_IsUsbCableIn (void);
extern kal_bool upmu_is_chr_det(void);
extern int pmic_detect_powerkey(void);
extern int pmic_detect_powerkey(void);
extern kal_uint32 upmu_get_reg_value(kal_uint32 reg);
extern void PMIC_DUMP_ALL_Register(void);
extern U32 pmic_init (void);
extern int PMIC_IMM_GetOneChannelValue(int dwChannel, int deCount, int trimd);
extern int get_bat_sense_volt(int times);
extern int get_i_sense_volt(int times);
extern int get_charger_volt(int times);
extern int get_tbat_volt(int times);
extern CHARGER_TYPE mt_charger_type_detection(void);

//==============================================================================
// PMIC6392 Status Code
//==============================================================================
#define PMIC_TEST_PASS               0x0000
#define PMIC_TEST_FAIL               0xB001
#define PMIC_EXCEED_I2C_FIFO_LENGTH  0xB002
#define PMIC_CHRDET_EXIST            0xB003
#define PMIC_CHRDET_NOT_EXIST        0xB004

//==============================================================================
// PMIC6392 Register Index
//==============================================================================
//register number
#include "upmu_hw.h"

//==============================================================================
// MT6392 APIs
//==============================================================================
#include "upmu_common.h"

#endif // _MT_PMIC_LK_SW_H_

