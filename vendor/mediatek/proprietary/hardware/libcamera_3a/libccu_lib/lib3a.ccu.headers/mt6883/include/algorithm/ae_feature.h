/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef _LIB3A_AE_FEATURE_H
#define _LIB3A_AE_FEATURE_H

// AE mode definition
typedef enum                           
{
    LIB3A_AE_MODE_UNSUPPORTED                    = -1,
    LIB3A_AE_MODE_OFF                                     = 0,
    LIB3A_AE_MODE_ON                                       = 1,    
    LIB3A_AE_MODE_ON_AUTO_FLASH                = 2,    
    LIB3A_AE_MODE_ON_ALWAYS_FLASH            = 3,        
    LIB3A_AE_MODE_ON_AUTO_FLASH_REDEYE  = 4,        
    LIB3A_AE_MODE_MAX
} LIB3A_AE_MODE_T;

// Cam mode definition
typedef enum                           
{
    LIB3A_AECAM_MODE_UNSUPPORTED      = -1,
    LIB3A_AECAM_MODE_PHOTO                       = 0,	
    LIB3A_AECAM_MODE_VIDEO                   = 1,    
    LIB3A_AECAM_MODE_ZSD                       = 2,    
    LIB3A_AECAM_MODE_S3D                       = 3,        
    LIB3A_AECAM_MODE_MAX
} LIB3A_AECAM_MODE_T;

// Sensor index definition
typedef enum                           
{
    LIB3A_SENSOR_MODE_UNSUPPORTED      = -1,
    LIB3A_SENSOR_MODE_PRVIEW                = 0,
    LIB3A_SENSOR_MODE_CAPTURE              = 1,    
    LIB3A_SENSOR_MODE_VIDEO                   = 2,    
    LIB3A_SENSOR_MODE_VIDEO1                 = 3,        
    LIB3A_SENSOR_MODE_VIDEO2                 = 4,      
    LIB3A_SENSOR_MODE_CUSTOM1              = 5,    
    LIB3A_SENSOR_MODE_CUSTOM2              = 6,        
    LIB3A_SENSOR_MODE_CUSTOM3              = 7,        
    LIB3A_SENSOR_MODE_CUSTOM4              = 8,    
    LIB3A_SENSOR_MODE_CUSTOM5              = 9,            
    LIB3A_SENSOR_MODE_CAPTURE_ZSD      = 10,            
    LIB3A_SENSOR_MODE_MAX
} LIB3A_SENSOR_MODE_T;

// AE EV compensation
typedef enum                            // enum  for evcompensate
{
    LIB3A_AE_EV_COMP_UNSUPPORTED = -1,
    LIB3A_AE_EV_COMP_00          =  0,           // Disable EV compenate
    LIB3A_AE_EV_COMP_01          =  1,           // EV compensate 0.1
    LIB3A_AE_EV_COMP_02          =  2,           // EV compensate 0.2
    LIB3A_AE_EV_COMP_03          =  3,           // EV compensate 0.3
    LIB3A_AE_EV_COMP_04          =  4,           // EV compensate 0.4
    LIB3A_AE_EV_COMP_05          =  5,           // EV compensate 0.5
    LIB3A_AE_EV_COMP_06          =  6,           // EV compensate 0.6
    LIB3A_AE_EV_COMP_07          =  7,           // EV compensate 0.7
    LIB3A_AE_EV_COMP_08          =  8,           // EV compensate 0.8
    LIB3A_AE_EV_COMP_09          =  9,           // EV compensate 0.9
    LIB3A_AE_EV_COMP_10          =  10,          // EV compensate 1.0
    LIB3A_AE_EV_COMP_11          =  11,          // EV compensate 1.1
    LIB3A_AE_EV_COMP_12          =  12,          // EV compensate 1.2
    LIB3A_AE_EV_COMP_13          =  13,          // EV compensate 1.3
    LIB3A_AE_EV_COMP_14          =  14,          // EV compensate 1.4
    LIB3A_AE_EV_COMP_15          =  15,          // EV compensate 1.5
    LIB3A_AE_EV_COMP_16          =  16,          // EV compensate 1.6
    LIB3A_AE_EV_COMP_17          =  17,          // EV compensate 1.7
    LIB3A_AE_EV_COMP_18          =  18,          // EV compensate 1.8
    LIB3A_AE_EV_COMP_19          =  19,          // EV compensate 1.9
    LIB3A_AE_EV_COMP_20          =  20,          // EV compensate 2.0
    LIB3A_AE_EV_COMP_21          =  21,          // EV compensate 2.1
    LIB3A_AE_EV_COMP_22          =  22,          // EV compensate 2.2
    LIB3A_AE_EV_COMP_23          =  23,          // EV compensate 2.3
    LIB3A_AE_EV_COMP_24          =  24,          // EV compensate 2.4
    LIB3A_AE_EV_COMP_25          =  25,          // EV compensate 2.5
    LIB3A_AE_EV_COMP_26          =  26,          // EV compensate 2.6
    LIB3A_AE_EV_COMP_27          =  27,          // EV compensate 2.7
    LIB3A_AE_EV_COMP_28          =  28,          // EV compensate 2.8
    LIB3A_AE_EV_COMP_29          =  29,          // EV compensate 2.9
    LIB3A_AE_EV_COMP_30          =  30,          // EV compensate 3.0
    LIB3A_AE_EV_COMP_31          =  31,          // EV compensate 3.1
    LIB3A_AE_EV_COMP_32          =  32,          // EV compensate 3.2
    LIB3A_AE_EV_COMP_33          =  33,          // EV compensate 3.3
    LIB3A_AE_EV_COMP_34          =  34,          // EV compensate 3.4
    LIB3A_AE_EV_COMP_35          =  35,          // EV compensate 3.5
    LIB3A_AE_EV_COMP_36          =  36,          // EV compensate 3.6
    LIB3A_AE_EV_COMP_37          =  37,          // EV compensate 3.7
    LIB3A_AE_EV_COMP_38          =  38,          // EV compensate 3.8
    LIB3A_AE_EV_COMP_39          =  39,          // EV compensate 3.9
    LIB3A_AE_EV_COMP_40          =  40,          // EV compensate 4.0
    LIB3A_AE_EV_COMP_n01         =  41,          // EV compensate -0.1
    LIB3A_AE_EV_COMP_n02         =  42,          // EV compensate -0.2
    LIB3A_AE_EV_COMP_n03         =  43,          // EV compensate -0.3
    LIB3A_AE_EV_COMP_n04         =  44,          // EV compensate -0.4
    LIB3A_AE_EV_COMP_n05         =  45,          // EV compensate -0.5
    LIB3A_AE_EV_COMP_n06         =  46,          // EV compensate -0.6
    LIB3A_AE_EV_COMP_n07         =  47,          // EV compensate -0.7
    LIB3A_AE_EV_COMP_n08         =  48,          // EV compensate -0.8
    LIB3A_AE_EV_COMP_n09         =  49,          // EV compensate -0.9
    LIB3A_AE_EV_COMP_n10         =  50,          // EV compensate -1.0
    LIB3A_AE_EV_COMP_n11         =  51,          // EV compensate -1.1
    LIB3A_AE_EV_COMP_n12         =  52,          // EV compensate -1.2
    LIB3A_AE_EV_COMP_n13         =  53,          // EV compensate -1.3
    LIB3A_AE_EV_COMP_n14         =  54,          // EV compensate -1.4
    LIB3A_AE_EV_COMP_n15         =  55,          // EV compensate -1.5
    LIB3A_AE_EV_COMP_n16         =  56,          // EV compensate -1.6
    LIB3A_AE_EV_COMP_n17         =  57,          // EV compensate -1.7
    LIB3A_AE_EV_COMP_n18         =  58,          // EV compensate -1.8
    LIB3A_AE_EV_COMP_n19         =  59,          // EV compensate -1.9
    LIB3A_AE_EV_COMP_n20         =  60,          // EV compensate -2.0
    LIB3A_AE_EV_COMP_n21         =  61,          // EV compensate -2.1
    LIB3A_AE_EV_COMP_n22         =  62,          // EV compensate -2.2
    LIB3A_AE_EV_COMP_n23         =  63,          // EV compensate -2.3
    LIB3A_AE_EV_COMP_n24         =  64,          // EV compensate -2.4
    LIB3A_AE_EV_COMP_n25         =  65,          // EV compensate -2.5
    LIB3A_AE_EV_COMP_n26         =  66,          // EV compensate -2.6
    LIB3A_AE_EV_COMP_n27         =  67,          // EV compensate -2.7
    LIB3A_AE_EV_COMP_n28         =  68,          // EV compensate -2.8
    LIB3A_AE_EV_COMP_n29         =  69,          // EV compensate -2.9
    LIB3A_AE_EV_COMP_n30         =  70,          // EV compensate -3.0
    LIB3A_AE_EV_COMP_n31         =  71,          // EV compensate -3.1
    LIB3A_AE_EV_COMP_n32         =  72,          // EV compensate -3.2
    LIB3A_AE_EV_COMP_n33         =  73,          // EV compensate -3.3
    LIB3A_AE_EV_COMP_n34         =  74,          // EV compensate -3.4
    LIB3A_AE_EV_COMP_n35         =  75,          // EV compensate -3.5
    LIB3A_AE_EV_COMP_n36         =  76,          // EV compensate -3.6
    LIB3A_AE_EV_COMP_n37         =  77,          // EV compensate -3.7
    LIB3A_AE_EV_COMP_n38         =  78,          // EV compensate -3.8
    LIB3A_AE_EV_COMP_n39         =  79,          // EV compensate -3.9
    LIB3A_AE_EV_COMP_n40         =  80,          // EV compensate -4.0
    LIB3A_AE_EV_COMP_n41         =	81,	         // EV compensate -4.1
    LIB3A_AE_EV_COMP_n42         =	82,          // EV compensate -4.2
    LIB3A_AE_EV_COMP_n43         =	83,          // EV compensate -4.3
    LIB3A_AE_EV_COMP_n44         =	84,          // EV compensate -4.4
    LIB3A_AE_EV_COMP_n45         =	85,          // EV compensate -4.5
    LIB3A_AE_EV_COMP_n46         =	86,          // EV compensate -4.6
    LIB3A_AE_EV_COMP_n47         =	87,          // EV compensate -4.7
    LIB3A_AE_EV_COMP_n48         =	88,          // EV compensate -4.8
    LIB3A_AE_EV_COMP_n49         =	89,          // EV compensate -4.9
    LIB3A_AE_EV_COMP_n50         =	90,          // EV compensate -5.0
    LIB3A_AE_EV_COMP_n51         =	91,          // EV compensate -5.1
    LIB3A_AE_EV_COMP_n52         =	92,          // EV compensate -5.2
    LIB3A_AE_EV_COMP_n53         =	93,          // EV compensate -5.3
    LIB3A_AE_EV_COMP_n54         =	94,          // EV compensate -5.4
    LIB3A_AE_EV_COMP_n55         =	95,          // EV compensate -5.5
    LIB3A_AE_EV_COMP_n56         =	96,          // EV compensate -5.6
    LIB3A_AE_EV_COMP_n57         =	97,          // EV compensate -5.7
    LIB3A_AE_EV_COMP_n58         =	98,          // EV compensate -5.8
    LIB3A_AE_EV_COMP_n59         =	99,          // EV compensate -5.9
    LIB3A_AE_EV_COMP_n60         = 100,          // EV compensate -6.0
    LIB3A_AE_EV_COMP_MAX
}LIB3A_AE_EVCOMP_T;

// AE metering mode
typedef enum                                  // enum for metering
{
    LIB3A_AE_METERING_MODE_UNSUPPORTED    = -1,
    LIB3A_AE_METERING_MODE_CENTER_WEIGHT,           // CENTER WEIGHTED MODE
    LIB3A_AE_METERING_MODE_SOPT,                    // SPOT MODE
    LIB3A_AE_METERING_MODE_AVERAGE,                 // AVERAGE MODE
    LIB3A_AE_METERING_MODE_MULTI,                   // MULTI MODE
    LIB3A_AE_METERING_MODE_MAX
}LIB3A_AE_METERING_MODE_T;

// AE set flicker mode
typedef enum
{
    LIB3A_AE_FLICKER_MODE_UNSUPPORTED = -1,
    LIB3A_AE_FLICKER_MODE_60HZ,
    LIB3A_AE_FLICKER_MODE_50HZ,
    LIB3A_AE_FLICKER_MODE_AUTO,    // No support in MT6516
    LIB3A_AE_FLICKER_MODE_OFF,     // No support in MT6516
    LIB3A_AE_FLICKER_MODE_MAX
}LIB3A_AE_FLICKER_MODE_T;

// AE set frame rate mode   //10base
typedef enum
{
    LIB3A_AE_FRAMERATE_MODE_UNSUPPORTED = -1,
    LIB3A_AE_FRAMERATE_MODE_DYNAMIC = 0,
    LIB3A_AE_FRAMERATE_MODE_05FPS = 50,
    LIB3A_AE_FRAMERATE_MODE_15FPS = 150,
    LIB3A_AE_FRAMERATE_MODE_30FPS = 300,
    LIB3A_AE_FRAMERATE_MODE_60FPS = 600,
    LIB3A_AE_FRAMERATE_MODE_90FPS = 900,
    LIB3A_AE_FRAMERATE_MODE_120FPS = 1200,
    LIB3A_AE_FRAMERATE_MODE_240FPS = 2400,
    LIB3A_AE_FRAMERATE_MODE_480FPS = 4800,
    LIB3A_AE_FRAMERATE_MODE_MAX = LIB3A_AE_FRAMERATE_MODE_480FPS
}LIB3A_AE_FRAMERATE_MODE_T;

// for flicker detection algorithm used only.
typedef enum
{
    LIB3A_AE_FLICKER_AUTO_MODE_UNSUPPORTED = -1,
    LIB3A_AE_FLICKER_AUTO_MODE_50HZ,
    LIB3A_AE_FLICKER_AUTO_MODE_60HZ,
    LIB3A_AE_FLICKER_AUTO_MODE_MAX
}LIB3A_AE_FLICKER_AUTO_MODE_T;

// remove later
typedef enum
{
    LIB3A_AE_STROBE_MODE_UNSUPPORTED = -1,
    LIB3A_AE_STROBE_MODE_AUTO        =  0,
    LIB3A_AE_STROBE_MODE_SLOWSYNC    =  0, //NOW DO NOT SUPPORT SLOW SYNC, TEMPERALLY THE SAME WITH AUTO
    LIB3A_AE_STROBE_MODE_FORCE_ON    =  1,
    LIB3A_AE_STROBE_MODE_FORCE_OFF   =  2,
    LIB3A_AE_STROBE_MODE_FORCE_TORCH =  3,
    LIB3A_AE_STROBE_MODE_REDEYE      =  4,
    LIB3A_AE_STROBE_MODE_TOTAL_NUM,
    LIB3A_AE_STROBE_MODE_MIN = LIB3A_AE_STROBE_MODE_AUTO,
    LIB3A_AE_STROBE_MODE_MAX = LIB3A_AE_STROBE_MODE_FORCE_OFF
}LIB3A_AE_STROBE_MODE_T;

#endif
