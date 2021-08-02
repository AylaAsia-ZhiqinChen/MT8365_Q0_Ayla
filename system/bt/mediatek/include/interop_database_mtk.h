/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly
 * * prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER
 * * AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING
 * * THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE
 * * RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED
 * * IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY
 * * MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM
 * * TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE
 * * REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE
 * * MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO
 * * REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE
 * * LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH
 * * MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek
 * * Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any
 * * receiver's applicable license agreements with MediaTek Inc.
 * */

#pragma once

/******************************************************************************
 *
 *  This file contains  MTK added interop devices
 *
 ******************************************************************************/

#include "interop_mtk.h"

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)

typedef struct {
  RawAddress addr;
  size_t length;
  mtk_interop_feature_t feature;
} mtk_interop_addr_entry_t;

static const mtk_interop_addr_entry_t mtk_interop_addr_db[] = {
// Added for HOGP/GATT/LE, @start {
    //BSM mouse
    {{{0x00, 0x1b, 0xdc, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_LATENCY_ADJUST},

    // CASIO watch
    {{{0xff, 0x40, 0x3a, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xda, 0x58, 0x98, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xc2, 0x80, 0x29, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xff, 0x74, 0xe1, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {{{0xd9, 0xe6, 0xea, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},

    //{0xd0, 0x5f, 0xb8}. /*ELECOM Laser Mouse*/
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_LE_DISABLE_FAST_CONNECTION},
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS},
    {{{0xd0, 0x5f, 0xb8, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},

    //Designer Mouse
    {{{0xcf, 0x18, 0xe0, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ADJUST},

    //Mad Catz M.O.U.S.9
    {{{0x00, 0x15, 0x9e, 0, 0, 0}}, 3, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ADJUST},

    //Huitong BLE Remote
    {{{0x7c, 0x66, 0x9d, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},

    //XiaoYi_RC
    {{{0x04, 0xe6, 0x76, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
// } @end

// Added for A2DP, @start {
    // INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS
    // {0x00, 0x21, 0x3c},   /* Jawbone ICON*/
    {{{0x00, 0x21, 0x3c, 0, 0, 0}},
     3,
     INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS},

    // INTEROP_MTK_A2DP_DELAY_START_CMD
    // {0x00, 0x17, 0x53}   /* Tiggo5 */
    // {0x00, 0x13, 0x04}   /* CASKA */
    // {0x00, 0x0d, 0x3c}   /* Clip Music 801 */
    // {0x0c, 0xa6, 0x94}   /* Philips Fidelio M2BT */
    // {0x33, 0x60, 0x7c}   /* DVD */
    // {0x00, 0x0a, 0x08}   /* CDE-153Series */
    // {0xbc, 0x6e, 0x64}   /* SBH54 */
    // {0x50, 0x32, 0x37}   /* Airpod */
    // {0xd0, 0xeb, 0x03}   /* Sounder */
    // {0x00, 0x18, 0x09}   /* MDR-XB80BS */
    // {0x00, 0x1e, 0x45}   /* MBS-100 */
    // {0x94, 0x16, 0x25}   /* Airpods2 */
    // {0x00, 0x16, 0x94}   /* Sennheiser MM550-X */
    {{{0x00, 0x17, 0x53, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x13, 0x04, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x0d, 0x3c, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x0c, 0xa6, 0x94, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x33, 0x60, 0x7c, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x0a, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0xbc, 0x6e, 0x64, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x50, 0x32, 0x37, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0xd0, 0xeb, 0x03, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x18, 0x09, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x1e, 0x45, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x94, 0x16, 0x25, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {{{0x00, 0x16, 0x94, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DELAY_START_CMD},

    // INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG
    // {0xd4, 0xca, 0x6e},   /* Audi MMI 6610 */
    //{0x74, 0x95, 0xec},   /* MY-CAR */
    {{{0xd4, 0xca, 0x6e, 0, 0, 0}}, 3, INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG},
    {{{0x74, 0x95, 0xec, 0, 0, 0}}, 3, INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG},

    // INTEROP_MTK_ADVANCED_A2DP_CONNECT
    // {0x00, 0x0d, 0x18, 0, 0, 0}, /* Alpine */
    // {0x40, 0xef, 0x4c, 0, 0, 0}, /* UFO */
    // {0x00, 0x00, 0x46, 0, 0, 0}, /* Media Center */
    {{{0x00, 0x0d, 0x18, 0, 0, 0}}, 3, INTEROP_MTK_ADVANCED_A2DP_CONNECT},
    {{{0x40, 0xef, 0x4c, 0, 0, 0}}, 3, INTEROP_MTK_ADVANCED_A2DP_CONNECT},
    {{{0x00, 0x00, 0x46, 0, 0, 0}}, 3, INTEROP_MTK_ADVANCED_A2DP_CONNECT},

     //INTEROP_MTK_ACCEPT_L2C_WHEN_CONNECTING
     // {0x30, 0xc3, 0xd9}, /**/
     {{{0x30, 0xc3, 0xd9, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_L2C_WHEN_CONNECTING},

     //INTEROP_MTK_A2DP_DISABLE_DELAY_REPORT
     // {0x28, 0x3F, 0x69}, /SBH24/
     {{{0x28, 0x3f, 0x69, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DISABLE_DELAY_REPORT},

     //INTEROP_MTK_A2DP_DISABLE_AAC_CODEC
     // {0x34, 0xdf, 0x2a}, /AKG Y45BT
     // {0xe0, 0xd1, 0xe6}, /MINIJAMBOX by Jawbone/
     {{{0x34, 0xdf, 0x2a, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DISABLE_AAC_CODEC},
     {{{0xe0, 0xd1, 0xe6, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_DISABLE_AAC_CODEC},

     //Some BT device has pop sound when doing music pause
     // {0x9c, 0x8d, 0x7c}, /LEXUS/
     // {0x00, 0x09, 0x93}, /FORD AUDIO/
     // {0xbc, 0xf2, 0x92}, /PLT BBFIT2100 Series/
     {{{0x9c, 0x8d, 0x7c, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_SET_STANDBY_TIME},
     {{{0x00, 0x09, 0x93, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_SET_STANDBY_TIME},
     {{{0xbc, 0xf2, 0x92, 0, 0, 0}}, 3, INTEROP_MTK_A2DP_SET_STANDBY_TIME},

     //INTEROP_MTK_SKIP_REMOTE_START_REQ
     // {0x0c, 0xa6, 0x94}, /* SONY:SRS-X5 */
     {{{0x0c, 0xa6, 0x94, 0, 0, 0}}, 3, INTEROP_MTK_SKIP_REMOTE_START_REQ},

     //INTEROP_MTK_DISABLE_SNIFF_MODE_WHEN_A2DP_START
     // {0x28, 0x37, 0x13}, /NIT-X3/
     // {0xac, 0x10, 0x5a}, /H-02/
     {{{0x28, 0x37, 0x13, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_SNIFF_MODE_WHEN_A2DP_START},
     {{{0xac, 0x10, 0x5a, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_SNIFF_MODE_WHEN_A2DP_START},

// } @end

// Added for HFP, @start {

    // INTEROP_MTK_HFP_DEALY_OPEN_SCO
    // {0x00, 0x1E, 0xDC}   /* DS205 */
    // {0x00, 0x58, 0x50}   /* BELKIN */
    // {0xFC, 0x58, 0xFA}   /*MEIZU speaker*/
    // {0x00, 0x13, 0x04}   /*BTHFD*/
    // {0xA0, 0x10, 0x81}   /* Gear IconX R (783B) */
    // {0x24, 0xA8, 0x7D}   /*CAR AUDIO*/swift dzire
    // {0x61, 0x63, 0x83}   /*baojun730*/
    // {0x38, 0xc0, 0x96}   /*benz R320*/
    // {0xA4, 0xD5, 0x78}   /*Mazda CX4*/
    // {0xfc, 0xdb, 0xb3}   /*Toyota Altis*/
    // {0x30, 0x23, 0x8a}   /*G067*/
    // {0xf4, 0x5e, 0xab}   /*MB Bluetooth*/
    {{{0x00, 0x1E, 0xDC, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x00, 0x58, 0x50, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0xFC, 0x58, 0xFA, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x00, 0x13, 0x04, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0xA0, 0x10, 0x81, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x24, 0xA8, 0x7D, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x61, 0x63, 0x83, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x38, 0xc0, 0x96, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0xA4, 0xD5, 0x78, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0xfc, 0xdb, 0xb3, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0x30, 0x23, 0x8a, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},
    {{{0xf4, 0x5e, 0xab, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DEALY_OPEN_SCO},

    // INTEROP_MTK_HFP_DELAY_DISC_SCO
    // {0x24, 0xA8, 0x7D}   /*CAR AUDIO*/
    {{{0x24, 0xA8, 0x7D, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_DISC_SCO},

    // INTEROP_MTK_HFP_DELAY_SCO_IN_ACT
    // {0xB0, 0x72, 0xBF}   /*KIA MOTORS*/
    // {0xD8, 0xC4, 0x6A}   /*KIA MOTORS*/
    {{{0xB0, 0x72, 0xBF, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_IN_ACT},
    {{{0xD8, 0xC4, 0x6A, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_IN_ACT},

    // INTEROP_MTK_HFP_17_TO_16
    // {0x00, 0x17, 0x53}   /* Tiggo5 */
    // {0x04, 0xf8, 0xc2}   /* VW Radio */
    // {0x00, 0x13, 0x04}   /* JAC */
    {{{0x00, 0x17, 0x53,       0,0,0}} ,3, INTEROP_MTK_HFP_17_TO_16},
    {{{0x04, 0xf8, 0xc2,       0,0,0}} ,3, INTEROP_MTK_HFP_17_TO_16},
    {{{0x00, 0x13, 0x04,       0,0,0}} ,3, INTEROP_MTK_HFP_17_TO_16},

    // INTEROP_MTK_HFP_FORCE_TO_USE_SCO
    // {0x00, 0x58, 0x76},  /* BT800 */
    // {0xD0, 0x8A, 0x55},  /* Skull Candy XTFree */
    // {0xA0, 0xE9, 0xDB},    /* ODYS-XOUND cube */
    // {0x00, 0x58, 0x50},    /* SeeCom */
    // {0x00, 0x58, 0x75},    /* Mifa F1 */
    // {0x59, 0x90, 0xa1},    /* TRACTOR-7462 */
    // {0x00, 0x58, 0x50},    /* AMETHYST MD3 */
    // {0x00, 0x0D, 0x18},    /* CARDVD-BT; CarKit10BF; NXO-3 */
    {{{0x00, 0x58, 0x76, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0xD0, 0x8A, 0x55, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0xA0, 0xE9, 0xDB, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0x00, 0x58, 0x75, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0x59, 0x90, 0xa1, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {{{0x00, 0x0D, 0x18, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},


    // INTEROP_MTK_HFP_DELAY_SCO_AFTER_ACTIVE
    // {0x00, 0x13, 0x7b},    /* HMC */
    // {0x24, 0xA8, 0x7D}   /*CAR AUDIO*/swift dzire
    {{{0x00, 0x13, 0x7b, 0,0,0}} ,3, INTEROP_MTK_HFP_DELAY_SCO_AFTER_ACTIVE},
    {{{0x24, 0xA8, 0x7D, 0,0,0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_AFTER_ACTIVE},

    // {0x7c, 0x1c, 0x4e}   /*Tesla Model S*/
    // {0x74, 0x6f, 0xf7}   /*benz NTG 6 */
    // {0x33, 0x60, 0xb0}   /* Honda HFT_B960 */
    {{{0x7c, 0x1c, 0x4e, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_FOR_MO_CALL},
    {{{0x74, 0x6f, 0xf7, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_FOR_MO_CALL},
    {{{0x33, 0x60, 0xb0, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_FOR_MO_CALL},

    // {0x00, 0x0a, 0x08}   /* Alpine (CDE-153Series) */
    {{{0x00, 0x0a, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_HFP_DELAY_SCO_FOR_MT_CALL},

    // INTEROP_MTK_HFP_NO_USE_CIND
    // {0x61, 0x63, 0x83}   /*baojun730*/
    // {0x38, 0xc0, 0x96}   /*benz R320*/
    // {0xE8, 0xf2, 0xe2}   /* MISTRA * /
    {{{0x61, 0x63, 0x83, 0, 0, 0}}, 3, INTEROP_MTK_HFP_NO_USE_CIND},
    {{{0x38, 0xc0, 0x96, 0, 0, 0}}, 3, INTEROP_MTK_HFP_NO_USE_CIND},
    {{{0x00, 0x58, 0x76, 0, 0, 0}}, 3, INTEROP_MTK_HFP_NO_USE_CIND},
    {{{0xe8, 0xf2, 0xe2, 0, 0, 0}} ,3, INTEROP_MTK_HFP_NO_USE_CIND},

    // INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT
    // {0xE0, 0xEE, 0x1B}   /*MY TIIDA*/
    {{{0xE0, 0xEE, 0x1B, 0, 0, 0}}, 3, INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT},

    // INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT
    // {0xE0, 0xEE, 0x1B}   /*MY TIIDA*/
    {{{0xE0, 0xEE, 0x1B, 0, 0, 0}}, 3, INTEROP_MTK_HFP_SCO_RECOVERY},

    // INTEROP_MTK_HFP_FORCE_TO_USE_CVSD
    // {} /* */
    //{{{0x00, 0x00, 0x00, 0,0,0}} ,3, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},

    // INTEROP_MTK_HFP_UPDATE_SECOND_CALLSTATE
    // {0xfc, 0x58, 0xfa}   /* Meidong */
    // {0x00, 0x0d, 0x18}   /* DVD (feige) */
    {{{0xfc, 0x58, 0xfa,       0,0,0}} ,3, INTEROP_MTK_HFP_UPDATE_SECOND_CALLSTATE},
    {{{0x00, 0x0d, 0x18,       0,0,0}} ,3, INTEROP_MTK_HFP_UPDATE_SECOND_CALLSTATE},

    // INTEROP_MTK_HFP_CREATE_SCO_AFTER_ATA
    // {0xE0, 0xEE, 0x1B}   /*MY TIIDA*/
    {{{0xE0, 0xEE, 0x1B, 0, 0, 0}}, 3, INTEROP_MTK_HFP_CREATE_SCO_AFTER_ATA},

    // INTEROP_MTK_HFP_NO_REPORT_CIEV_7_2
    // {0x40, 0xEF, 0x4C} /* PASSWAY */
    {{{0x40, 0xEF, 0x4C, 0,0,0}} ,3, INTEROP_MTK_HFP_NO_REPORT_CIEV_7_2},

// } @end

// Added for HID, @start {
    // INTEROP_MTK_HID_DISABLE_SDP
    // {0x04, 0x0C, 0xCE},  /* Apple Magic Mouse */
    // {0x00, 0x07, 0x61},  /* Bluetooth Laser Travel Mouse */
    // {0x00, 0x1d, 0xd8},  /* Microsoft Bluetooth Notebook Mouse 5000 */
    // {0x00, 0x1f, 0x20},  /* Logitech MX Revolution Mouse */
    // {0x6c, 0x5d, 0x63},  /* Rapoo 6080 mouse */
    // {0x28, 0x18, 0x78},  /* Microsoft Sculpt Touch Mouse */
    // {0x30, 0x59, 0xb7},  /* Microsoft Sculpt Comfort Mouse */
    // {0x34, 0x88, 0x5d},  /* Bluetooth Mouse M557*/
    {{{0x04, 0x0C, 0xCE, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x00, 0x07, 0x61, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x00, 0x1d, 0xd8, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x00, 0x1f, 0x20, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x6c, 0x5d, 0x63, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x28, 0x18, 0x78, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x30, 0x59, 0xb7, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},
    {{{0x34, 0x88, 0x5d, 0, 0, 0}}, 3, INTEROP_MTK_HID_DISABLE_SDP},

    // INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING
    // {0x54, 0x46, 0x6b},  /* JW MT002 Bluetooth Mouse */
    // /*LMP version&subversion  - 5, 8721 & LMP Manufacturer - 15*/
    // {0x6c, 0x5d, 0x63}, /* Rapoo 6610 Bluetooth Mouse */
    // {0x60, 0x8c, 0x2b},  /* FelTouch Magic */
    {{{0x54, 0x46, 0x6b, 0, 0, 0}}, 3, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},
    {{{0x6c, 0x5d, 0x63, 0, 0, 0}}, 3, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},
    {{{0x60, 0x8c, 0x2b, 0, 0, 0}}, 3, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},

    // INTEROP_MTK_HID_PEER_NOT_USE_HID
    // {0x04, 0xf8, 0xc2},  /* GS4 BT 1285 Carkit */
    {{{0x04, 0xf8, 0xc2, 0, 0, 0}}, 3, INTEROP_MTK_HID_PEER_NOT_USE_HID},
// } @end

// Added for PAN/NAP, @start {
    // INTEROP_MTK_PAN_NAP_IGNORE_PEER_NAP_CAPABILITY
    // {0x40, 0x45, 0xDA} /*carkit Magotan*/
    {{{0x40, 0x45, 0xDA, 0, 0, 0}}, 3, INTEROP_MTK_PAN_NAP_IGNORE_PEER_NAP_CAPABILITY},
// } @end

// Added for OPP/RFCOMM, @start {
    // INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA
    // {0x00, 0x0A, 0x08},   /*BMW94506*/
    // {0xfc, 0xdb, 0xb3}   /*Toyota Altis*/
    // {0x28, 0x56, 0xc1},  /*TATA CIBBECTBEXT*/
    // {0xe4, 0x22, 0xa5}   /*PLT BB GO 3,*/
    {{{0x00, 0x0A, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA},
    {{{0xfc, 0xdb, 0xb3, 0, 0, 0}}, 3, INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA},
    {{{0x28, 0x56, 0xc1, 0, 0, 0}}, 3, INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA},
    {{{0xe4, 0x22, 0xa5, 0, 0, 0}}, 3, INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA},
// } @end

// Added for L2CAP, @start {
    // INTEROP_MTK_ACCEPT_CONN_AS_MASTER
    // {0x6c, 0x5d, 0x63},  /* Rapoo 6610 mouse */
    // {0x00, 0x02, 0xc7},  /* HANDS FREE carkit of Infineon Technologies AG */
    // {0x00, 0x58, 0x75},  /* Mifa_F1 */
    // {0x00, 0x58, 0x76},  /* BT800, this device does not send detach when power off */
    // {0x20, 0x16, 0x03},  /* 56s */
    // {0x50, 0x65, 0x83},  /* Honda crown road */
    {{{0x6c, 0x5d, 0x63, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x00, 0x02, 0xc7, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x00, 0x58, 0x75, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x00, 0x58, 0x76, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x20, 0x16, 0x03, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},
    {{{0x50, 0x65, 0x83, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_MASTER},


    // INTEROP_MTK_ACCEPT_CONN_AS_SLAVE
    // {0x00, 0x1e, 0xae},  /* SYNC, FORD carkit */
    // {0x00, 0x26, 0xb4},  /* NAC ford, 2013 Lincoln */
    // {0x00, 0x26, 0xe8},  /* Nissan Murano */
    // {0x00, 0x37, 0x6d},  /* Lexus ES300h */
    // {0x9c, 0x3a, 0xaf},  /* SAMSUNG HM1900 */
    // {0xb0, 0xe1, 0x7e},  /* Mate 10 */
    {{{0x00, 0x1e, 0xae, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x00, 0x26, 0xb4, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x00, 0x26, 0xe8, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x00, 0x37, 0x6d, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0x9c, 0x3a, 0xaf, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},
    {{{0xb0, 0xe1, 0x7e, 0, 0, 0}}, 3, INTEROP_MTK_ACCEPT_CONN_AS_SLAVE},

    // INTEROP_MTK_NOT_RESTART_ACL
    // {0x00, 0x06, 0xf7},  /* MB Bluetooth, carkit */
    {{{0x00, 0x06, 0xf7,       0,0,0}}, 3, INTEROP_MTK_NOT_RESTART_ACL},
// } @end

// Added for GAP, @start {
  // TEMIC SDS (Porsche,Audi exp(PCM)) - auto-pairing fails
  {{{0x00, 0x0e, 0x9f, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_AUTO_PAIRING},
    // Jabra STONE2 - reconnect without sercurity procedure
    {{{0x50, 0xc9, 0x71, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK},
    {{{0x00, 0x23, 0x78, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK},
    // INTEROP_MTK_GET_UUIDS_FROM_STORAGE
    // {0x0c, 0xfc, 0x83},  /* ADYSS-A6 */
    {{{0x0c, 0xfc, 0x83, 0, 0, 0}}, 3, INTEROP_MTK_GET_UUIDS_FROM_STORAGE},
    // {0x30, 0xa9, 0xde},  /* Media Nav Evolution  */
    {{{0x30, 0xa9, 0xde, 0, 0, 0}}, 3, INTEROP_MTK_GET_UUIDS_FROM_STORAGE},
    // {0xfc, 0x58, 0xfa},  /* Honor xSport AM61   */
    {{{0xfc, 0x58, 0xfa, 0, 0, 0}}, 3, INTEROP_MTK_GET_UUIDS_FROM_STORAGE},
    // INTEROP_MTK_FORCED_SSP
    // {0x34, 0x88, 0x5d},   /*Bluetooth Mouse M557*/
    {{{0x34, 0x88, 0x5d, 0, 0, 0}}, 3, INTEROP_MTK_FORCED_SSP},
    {{{0x00, 0x1f, 0x20, 0xaa, 0, 0}}, 4, INTEROP_MTK_FORCED_SSP},

    // {0x70, 0x66, 0x1b},  /* PPR RT */
    {{{0x70, 0x66, 0x1b, 0, 0, 0}}, 3, INTEROP_MTK_ROLE_SWITCH},
// } @end

// Added for SDP, @start {
// } @end

// Added for AVRCP, @start {
    // INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME
    // {0x00, 0x08, 0x7A}   /* APS-BH80 */
    // {0x00, 0x08, 0x3e}   /* aigo X6 */
    // {0x00, 0x00, 0x1a}   /* Gionee */
    // {0xad, 0xcd, 0xee}   /* PISEN-LE005 */
    // {0xe8, 0x07, 0xbf}   /* Mi Bluetooth Speaker */
    // {0xb8, 0x07, 0xbf}   /* MI Portable Bluetooth Speaker */
    // {0xb4, 0x52, 0x7d}   /* SBH50 */
    // {0x00, 0x1a, 0x7d}   /* BT016 */
    // {0x68, 0xc9, 0x0b}   /* SYNC */
    // {0xe9, 0x08, 0xef}   /* lingdong */
    // {0x50, 0x32, 0x37}   /* AirPods */
    // {0xac, 0xfd, 0xce}   /* Range Rover Sport */
    // {0xf0, 0x0f, 0xec}   /* HUAWEI FreeBuds */
    // {0x00, 0x21, 0x3e}   /* Renault R-Link */
    // {0xdc, 0xef, 0xca}   /* Porsche BT 1706 */
    // {0x98, 0x5d, 0xad}   /* Ford Ecosport */
    // {0x60, 0x64, 0x05}   /* Ford Mondeo */
    // {0xcc, 0xf9, 0x57}   /* Audi MMI 3958 */
    // {0x00, 0x1b, 0x66}   /* IE 80S BT */
    // {0xa4, 0xd5, 0x78}   /* Mazda */
    {{{0x00, 0x08, 0x7A, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x08, 0x3e, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x00, 0x1a, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xad, 0xcd, 0xee, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xe8, 0x07, 0xbf, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xb8, 0x07, 0xbf, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xb4, 0x52, 0x7d, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x1a, 0x7d, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x68, 0xc9, 0x0b, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xe9, 0x08, 0xef, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x50, 0x32, 0x37, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xac, 0xfd, 0xce, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xf0, 0x0f, 0xec, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x21, 0x3e, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xdc, 0xef, 0xca, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x98, 0x5d, 0xad, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x60, 0x64, 0x05, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xcc, 0xf9, 0x57, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0x00, 0x1b, 0x66, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},
    {{{0xa4, 0xd5, 0x78, 0, 0, 0}}, 3, INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME},

    // INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE
    // {0xA0, 0xE6, 0xF8},   /*Blaze (Classic)*/
    // {0x6b, 0x1f, 0x88},   /*Versa (Classic)*/
    // {0x28, 0xa1, 0x83},   /*CAR MULTIMEDIA*/
    {{{0xA0, 0xE6, 0xF8, 0,0,0}}, 3, INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE},
    {{{0x6b, 0x1f, 0x88, 0,0,0}}, 3, INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE},
    {{{0x28, 0xa1, 0x83, 0,0,0}}, 3, INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE},

    // INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP
    // {0xA0, 0xE6, 0xF8},   /*Fitbit Blaze*/
    {{{0xA0, 0xE6, 0xF8, 0,0,0}}, 3, INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP},

    // INTEROP_MTK_AVRCP13_USE
    // {0xe0, 0x75, 0x0a},    /* Honda HFT */
    // {0x00, 0x09, 0x93},    /* My Car */
    {{{0xe0, 0x75, 0x0a, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP13_USE},
    {{{0x00, 0x09, 0x93, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP13_USE},

    // INTEROP_MTK_AVRCP14_USE
    // {0x50, 0x33, 0x8b},    /* Car Display Audio */
    // {0x94, 0xe3, 0x6d},    /* Car Display Audio */
    {{{0x50, 0x33, 0x8b, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP14_USE},
    {{{0x94, 0xe3, 0x6d, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP14_USE},

    // INTEROP_MTK_START_AVRCP_SDP_IOT_DEVICE
    // {0x00, 0x0a, 0x08},  /* CDE-153Series */
    {{{0x00, 0x0a, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_START_AVRCP_SDP_IOT_DEVICE},

    // INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL
    // {0xd4, 0xca, 0x6e},  /* Audi MMI 2407 */
    // {0xa0, 0xc9, 0xa0},  /* Audi MMI 1130 */
    // {0xb8, 0xd7, 0xaf},  /* Audi MMI 1420 */
    // {0x30, 0x14, 0x4a},  /* PSJ1 */
    {{{0xd4, 0xca, 0x6e, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL},
    {{{0xa0, 0xc9, 0xa0, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL},
    {{{0xb8, 0xd7, 0xaf, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL},
    {{{0x30, 0x14, 0x4a, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL},

    // INTEROP_MTK_AVRCP_SEND_EXTRA_TRACK_CHANGE
    // {0x33, 0x60, 0x40},   /* HZ Audio 9825 */
    {{{0x33, 0x60, 0x40, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_SEND_EXTRA_TRACK_CHANGE},

    // INTEROP_MTK_AVRCP_TRACK_ID_NO_CHANGE
    // {0x00, 0x17, 0x53},  /* Honda HFT */
    {{{0x00, 0x17, 0x53, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_TRACK_ID_NO_CHANGE},

    // INTEROP_MTK_AVRCP_FORCE_TRACK_NUMBER_TO_ONE
    //{0x00, 0x0a, 0x08},   /* BMW 80957 */
    {{{0x00, 0x0a, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_FORCE_TRACK_NUMBER_TO_ONE},

    // INTEROP_MTK_AVRCP_SEND_TRACK_WHEN_PLAY
    // {0x00, 0x22, 0xa0},   /* CHEVY */
    {{{0x00, 0x22, 0xa0, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_SEND_TRACK_WHEN_PLAY},

    // INTEROP_MTK_START_AVRCP_100_MS
    // {0x00, 0x0a, 0x08},  /* CDE-153Series */
    // {0x00, 0x0d, 0x18},  /* TUCSON */
    // {0x2c, 0xdc, 0xad},  /* Audi A4 */
    // {0x85, 0xb1, 0xaa},  /* M11 */
    {{{0x00, 0x0a, 0x08, 0, 0, 0}}, 3, INTEROP_MTK_START_AVRCP_100_MS},
    {{{0x00, 0x0d, 0x18, 0, 0, 0}}, 3, INTEROP_MTK_START_AVRCP_100_MS},
    {{{0x2c, 0xdc, 0xad, 0, 0, 0}}, 3, INTEROP_MTK_START_AVRCP_100_MS},
    {{{0x85, 0xb1, 0xaa, 0, 0, 0}}, 3, INTEROP_MTK_START_AVRCP_100_MS},

    // INTEROP_MTK_AVRCP_PLAYER_ID_NO_ZERO
    // {0x64, 0xd4, 0xbd},  /* CAR MULTIMEDIA */
    {{{0x64, 0xd4, 0xbd, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_PLAYER_ID_NO_ZERO},

    // INTEROP_MTK_AVRCP_FOR_QQ_PLAYER
    // {0xa0, 0x14, 0x3d},  /* AVH-X8850BT */
    {{{0xa0, 0x14, 0x3d, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_FOR_QQ_PLAYER},

    // INTEROP_MTK_AVRCP_USE_MUSIC_ONLY
    // {0x00, 0x22, 0xa0}, /HAVAL/
    {{{0x00, 0x22, 0xa0, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_USE_MUSIC_ONLY},

    // INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS
    //{0x00, 0x00, 0x2b},  /* Discovery Sport */
    //{0x00, 0x00, 0x64},  /* Discovery Sport */
    {{{0x00, 0x00, 0x2b, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS},
    {{{0x00, 0x00, 0x64, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS},

    // INTEROP_MTK_AVRCP_USE_A2DP_ONLY
    //{0xfa, 0x6a, 0xda},  /* Chevy MyLink */
    {{{0xfa, 0x6a, 0xda, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_USE_A2DP_ONLY},

    // INTEROP_MTK_AVRCP_USE_FAKE_ID
    // {0x33, 0x60, 0x40},   /* HZ Audio 9825 */
    {{{0x33, 0x60, 0x40, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_USE_FAKE_ID},

    // INTEROP_MTK_AVRCP_NO_APP_SETTINGS
    // {0x88, 0xc2, 0x55},  /* SEMI */
    // {0xd0, 0xb5, 0xc2},  /* SEMI */
    {{{0x88, 0xc2, 0x55, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_NO_APP_SETTINGS},
    {{{0xd0, 0xb5, 0xc2, 0, 0, 0}}, 3, INTEROP_MTK_AVRCP_NO_APP_SETTINGS},
// } @end

// Added for Common, @start {
// } @end
};

typedef struct {
  char name[30];
  size_t length;
  mtk_interop_feature_t feature;
} mtk_interop_name_entry_t;

static const mtk_interop_name_entry_t mtk_interop_name_db[] = {
// Added for HOGP/GATT/LE, @start {
    {"honor zero-", 11, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"小米蓝牙遥控", 18, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"Arc Touch BT Mouse", 18, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},
    {"BSMBB09DS", 9, INTEROP_MTK_LE_CONN_LATENCY_ADJUST},
    {"CASIO GB-6900A*", 15, INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST},
    {"Huitong BLE Remote", 18, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_LE_DISABLE_FAST_CONNECTION},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS},
    {"ELECOM Laser Mouse", 18, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},
    {"FeiZhi", 6, INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS},
    {"XiaoYi_RC", 9, INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS},
    {"Mad Catz M.O.U.S.9", 18, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},
    {"Mad Catz M.O.U.S.9", 18, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ADJUST},
    {"Designer Mouse", 14, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT},
    {"Designer Mouse", 14, INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ADJUST},
// } @end

// Added for A2DP, @start {
    // RDA Microelectronics HBS device will a2dp no sound after reject the call
    // {0x00, 0x0d, 0x18}   /* RDA HBS device*/
    {"HBS8864", 7, INTEROP_MTK_A2DP_DELAY_START_CMD},
    {"SBH50", 5, INTEROP_MTK_A2DP_DELAY_START_CMD},

    // Tiguan RNS315 will no sound after play wechat voice message.
    // {0x00, 0x07, 0x04}   /* Tiguan RNS carkit*/
    {"Tiguan RNS315 Bluetooth", 13, INTEROP_MTK_A2DP_DELAY_START_CMD},
// } @end

// Added for AVRCP, @start {
    {"CAR MULTIMEDIA", 14, INTEROP_MTK_AVRCP_PLAYER_ID_NO_ZERO},
    {"SEMI", 4, INTEROP_MTK_AVRCP_NO_APP_SETTINGS},
    {"CITROEN CONNECT", 15, INTEROP_MTK_AVRCP_USE_A2DP_ONLY},
    {"CONNECTNE><T", 12, INTEROP_MTK_AVRCP_USE_A2DP_ONLY},
    {"CIVIA3C15", 9, INTEROP_MTK_AVRCP_USE_A2DP_ONLY},
    {"Uconnect 1G4AH59H45G118341", 26, INTEROP_MTK_AVRCP13_USE},
    {"Uconnect 1G4AH59H45G118341", 26, INTEROP_MTK_AVRCP_USE_MUSIC_ONLY},
// } @end

// Added for HFP, @start {
    {"CARDVD-BT", 9, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"CarKit10BF", 10, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"NXO-3", 5, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"SeeCom", 6, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"AMETHYST MD3", 12, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"RAVIS", 5, INTEROP_MTK_HFP_FORCE_TO_USE_SCO},
    {"Parrot ASTEROID Smart", 21, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},
    {"Jabra Classic v0.5.3", 20, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},
    {"T1S", 3, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},
    {"H8T", 3, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},
    {"E613", 3, INTEROP_MTK_HFP_FORCE_TO_USE_CVSD},
    // INTEROP_MTK_HFP_SCO_OPEN_NOT_DO_SNIFF
    // {0x40, 0xEF, 0x4C} /* PASSWAY */
    {"WI-C400" , 7, INTEROP_MTK_HFP_SCO_OPEN_NOT_DO_SNIFF},
// } @end

// Added for HID, @start {
    {"Wireless Controller", 19, INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING},
// } @end

// Added for OPP/RFCOMM, @start {
// } @end

// Added for L2CAP, @start {
// } @end

// Added for GAP, @start {
    {"Bluetooth Mouse M557", 20, INTEROP_MTK_FORCED_SSP},
// } @end

// Added for SDP, @start {
// } @end

// Added for Common, @start {
// } @end
};
#endif
