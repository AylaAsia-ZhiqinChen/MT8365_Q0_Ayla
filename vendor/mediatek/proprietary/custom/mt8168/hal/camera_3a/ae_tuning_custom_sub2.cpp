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

#include "camera_custom_types.h"
#include "camera_custom_nvram.h"
#include "ae_feature.h"
#include <aaa_types.h>
#include "camera_custom_AEPlinetable.h"
#include "awb_param.h"
#include "ae_param.h"
#include "ae_tuning_custom.h"

using namespace NSIspTuning;
/*******************************************************************************
*
********************************************************************************/
template <>
AE_PARAM_T const&
getAEParam<ESensorDev_SubSecond>()
{
    static strAEParamCFG g_AEParasetting =
    {
        FALSE,               // bEnableSaturationCheck
        TRUE,                // bEnablePreIndex
        TRUE,                // bEnableRotateWeighting;
        TRUE,               // bEV0TriggerStrobe
        FALSE,               // bLockCamPreMeteringWin;
        FALSE,               // bLockVideoPreMeteringWin;
        TRUE,                // bLockVideoRecMeteringWin;
        TRUE,                // bSkipAEinBirghtRange;
        TRUE,                // bPreAFLockAE
        TRUE,                // bStrobeFlarebyCapture
        TRUE,                // bEnableFaceAE
        TRUE,                // bEnableMeterAE
        TRUE,                // b4FlarMaxStepGapLimitEnable
        TRUE,                // bEnableAESmoothGain
        TRUE,                // bEnableLongCaptureCtrl
        FALSE,                //bEnableTouchSmoothRatio : enable Touch mode smooth converge
        TRUE,                //bEnableTouchSmooth : enable Touch mode smooth with peframe
        
        TRUE,                // bEnablePixelBaseHist
        TRUE,                // bEnableHDRSTATconfig   : enable HDR Y LSB config
        TRUE,                // bEnableAEOVERCNTconfig  : enable AE over-exposure count config
        TRUE,                // bEnableTSFSTATconfig    : enable TSF RGB STAT config
        1,                   // bEnableHDRLSB
        256,                  // u4BackLightStrength : strength of backlight condtion
        256,                  // u4OverExpStrength : strength of anti over exposure
        256,                  // u4HistStretchStrength : strength of  histogram stretch
        0,                     // u4SmoothLevel : time LPF smooth level , internal use
        4,                     // u4TimeLPFLevel : time LOW pass filter level
        120,                  // u4AEWinodwNumX;                   // AE statistic winodw number X
        90,                   // u4AEWinodwNumY;                   // AE statistic winodw number Y
        AE_BLOCK_NO,  // uBockXNum : AE X block number;
        AE_BLOCK_NO,  // uBockYNum : AE Yblock number;

        20,                    // uSatBlockCheckLow : saturation block check , low thres
        50,                     // uSatBlockCheckHigh : sturation  block check , hight thres
        50,                     // uSatBlockAdjustFactor : adjust factore , to adjust central weighting target value

        80,                     // uMeteringYLowSkipRatio : metering area min Y value to skip AE
       120,                     // uMeteringYHighSkipRatio : metering area max Y value to skip AE
        79,                     // u4MinYLowBound;        // metering and face boundary min Y value
       256,                     // u4MaxYHighBound;      // metering and face boundary max Y value
      1024,                     // u4MeterWeight 1024 base.100%=>1024
      1024,                     // u4VideoMeterWeight 1024 base.100%=>1024        
        1,                      // u4MinCWRecommend;    // mini target value
      250,                      // u4MaxCWRecommend;    // max target value
      -50,                      // iMiniBVValue;               // mini BV value.
        2,                      // uAEShutterDelayCycle;         // for AE smooth used.
        2,                      // uAESensorGainDelayCycleWShutter;
        1,                      // uAESensorGainDelayCycleWOShutter;
        0,                      // uAEIspGainDelayCycle;

        200000,             // u4LongCaptureThres 100ms
        1,                  //u4CenterFaceExtraWeighting;
        7,                  //u2AEStatThrs;
        1,                  //uCycleNumAESkipAfterExit;
        5,                  //uSkyDetInStableThd is B_Sum 5%
        2,                  //uSkyDetOutStableThd is B_Sum 2%
        254,                //overexposure cnt threshold
        11,                 //u4HSSmoothTHD
    };

    static strWeightTable  g_Weight_Matrix =
    {
        AE_WEIGHTING_CENTRALWEIGHT,
        {
            {6 ,19 ,28 ,19 ,6 },
            {19 ,60 ,88 ,60 ,19 },
            {28 ,88 ,128+32 ,88 ,28 },
            {19 ,60 ,88 ,60 ,19 },
            {6 ,19 ,28 ,19 ,6 },
        }
    };

    static strWeightTable  g_Weight_Spot =
    {
        AE_WEIGHTING_SPOT,
        {
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
        }
    };

    static strWeightTable  g_Weight_Average =
    {
        AE_WEIGHTING_AVERAGE,
        {
            {1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1},
        }
    };

    static strAFPlineInfo g_strAFPLineTable =     // from high lv to low lv
    {
         0,                  // 1 enable the AF Pline, 0 disable the AF Pline (bypass)
         {
             {73, 30},       // {LV value, AF frame rate}
             {65, 25},
             {55, 19},
             {45, 17},
             {0, 17},
         }
    };

    static strAFPlineInfo g_strAFZSDPLineTable =     // from high lv to low lv
    {
        0,                  // 1 enable the AF ZSD Pline, 0 disable the AF ZSD Pline (bypass)
        {
            {73, 15},       // {LV value, AF frame rate}
            {65, 15},
            {55, 15},
            {45, 15},
            {0, 10},
        }
    };

    static strAFPlineInfo g_strStrobePLineTable =     // from high lv to low lv
    {
         1,                  // 1 enable the Strobe Pline, 0 disable the Strobe Pline (bypass)
         {
             {73, 30},       // {LV value, Strobe frame rate}
             {65, 25},
             {55, 19},
             {45, 17},
             {0, 17},
         }
    };

    static strAFPlineInfo g_strStrobeZSDPLineTable =     // from high lv to low lv
    {
        1,                  // 1 enable the Strobe ZSD Pline, 0 disable the Strobe ZSD Pline (bypass)
        {
            {73, 15},       // {LV value, Strobe frame rate}
            {65, 15},
            {55, 15},
            {45, 15},
            {0, 10},
        }
    };

    static MUINT32 g_strEVValueArray[LIB3A_AE_EV_COMP_MAX] =
    {
        1024,  // EV   0
       1097,   //EV    0.1
       1176,   //EV    0.2
       1261,   //EV    0.3
       1351,   //EV    0.4
       1448,   //EV    0.5
       1552,   //EV    0.6
       1663,   //EV    0.7
       1783,   //EV    0.8
       1911,   //EV    0.9
       2048,   //EV    1
       2195,   //EV    1.1
       2353,   //EV    1.2
       2521,   //EV    1.3
       2702,   //EV    1.4
       2896,   //EV    1.5
       3104,   //EV    1.6
       3327,   //EV    1.7
       3566,   //EV    1.8
       3822,   //EV    1.9
       4096,   //EV    2
       4390,   //EV    2.1
       4705,   //EV    2.2
       5043,   //EV    2.3
       5405,   //EV    2.4
       5793,   //EV    2.5
       6208,   //EV    2.6
       6654,   //EV    2.7
       7132,   //EV    2.8
       7643,   //EV    2.9
       8192,   //EV    3
       8780,   //EV    3.1
       9410,   //EV    3.2
       10086,  //EV    3.3
       10809,  //EV    3.4
       11585,  //EV    3.5
       12417,  //EV    3.6
       13308,  //EV    3.7
       14263,  //EV    3.8
       15287,  //EV    3.9
       16384,  //EV    4
       955,    //EV    -0.1
       891,    //EV    -0.2
       832,    //EV    -0.3
       776,    //EV    -0.4
       724,    //EV    -0.5
       676,    //EV    -0.6
       630,    //EV    -0.7
       588,    //EV    -0.8
       549,    //EV    -0.9
       512,    //EV    -1
       478,    //EV    -1.1
       446,    //EV    -1.2
       416,    //EV    -1.3
       388,    //EV    -1.4
       362,    //EV    -1.5
       338,    //EV    -1.6
       315,    //EV    -1.7
       294,    //EV    -1.8
       274,    //EV    -1.9
       256,    //EV    -2
       239,    //EV    -2.1
       223,    //EV    -2.2
       208,    //EV    -2.3
       194,    //EV    -2.4
       181,    //EV    -2.5
       169,    //EV    -2.6
       158,    //EV    -2.7
       147,    //EV    -2.8
       137,    //EV    -2.9
       128,    //EV    -3
       119,    //EV    -3.1
       111,    //EV    -3.2
       104,    //EV    -3.3
       97, //EV    -3.4
       91, //EV    -3.5
       84, //EV    -3.6
       79, //EV    -3.7
       74, //EV    -3.8
       69, //EV    -3.9
       64, //EV    -4      
    };

    // total 24 sets
    static strAEMOVE  g_AEMoveTable[] =
    {
        {-20,   17,    24}, //   mean below -2.5  move increase 25 index
        {-20,   25,    19}, //   -2.5~-2  move increase 20 index
        {-15,   33,    15}, //   -2~-1.6
        {-15,   40,    12}, //   -1.6~-1.3
        {-10,   50,    9}, //   -1.3~-1
        { -8,   57,     7}, //   -1~-0.8
        { -5,   71,     4}, //   -0.8~-0.5
        { -4,   75,     3}, //   -0.5~-0.4
        { -3,   81,     2}, //   -0.4~-0.3
        { -1,   90,     1}, //   -0.3~-0.1
        {   0,  100,     0}, //   -0.1~0
        {   1,  110,     0}, //     0~0.1
        {   2,  114,    -1}, //    0.1~0.2       move decrease 1 index
        {   3,  123,    -1}, //    0.2~0.3
        {   4,  131,    -2}, //    0.3~0.4
        {   5,  141,    -3}, //    0.4~0.5
        {   7,  162,    -4}, //    0.5~0.7
        {   9,  186,    -6}, //    0.7~0.9
        { 10,  200,   -8}, //    0.9~1.0
        { 13,  246,   -9}, //    1.0~1.3
        { 16,  303,   -12}, //    1.3~1.6
        { 20,  400,   -15}, //    1.6~2       move decrease 10  index
        { 25,  566,   -19}, //    2~2.5       move decrease 20  index
        { 30,  800,   -22}, //    2.5~3      move decrease 30  index
    };

    // total 24 sets
    static strAEMOVE  g_AEVideoMoveTable[] =
    {
        {-20,   17,    20}, //   mean below -2.5  move increase 25 index
        {-20,   25,    15}, //   -2.5~-2  move increase 20 index
        {-15,   33,    10}, //   -2~-1.6
        {-15,   40,    6}, //   -1.6~-1.3
        {-10,   50,    4}, //   -1.3~-1
        { -8,   57,     3}, //   -1~-0.8
        { -5,   71,     2}, //   -0.8~-0.5
        { -4,   75,     1}, //   -0.5~-0.4
        { -3,   81,     1}, //   -0.4~-0.3
        { -1,   90,     1}, //   -0.3~-0.1
        {   0,  100,     0}, //   -0.1~0
        {   1,  110,     0}, //     0~0.1
        {   2,  114,    -1}, //    0.1~0.2       move decrease 1 index
        {   3,  123,    -1}, //    0.2~0.3
        {   4,  131,    -1}, //    0.3~0.4
        {   5,  141,    -2}, //    0.4~0.5
        {   7,  162,    -2}, //    0.5~0.7
        {   9,  186,    -3}, //    0.7~0.9
        { 10,  200,   -4}, //    0.9~1.0
        { 13,  246,   -4}, //    1.0~1.3
        { 16,  303,   -6}, //    1.3~1.6
        { 20,  400,   -7}, //    1.6~2       move decrease 10  index
        { 25,  566,   -9}, //    2~2.5       move decrease 20  index
        { 30,  800,   -11}, //    2.5~3      move decrease 30  index
    };

    // total 24 sets
    static strAEMOVE  g_AEFaceMoveTable[] =
    {
        {-20,   17,    20}, //   mean below -2.5  move increase 25 index
        {-20,   25,    15}, //   -2.5~-2  move increase 20 index
        {-15,   33,    10}, //   -2~-1.6
        {-15,   40,    6}, //   -1.6~-1.3
        {-10,   50,    4}, //   -1.3~-1
        { -8,   57,     3}, //   -1~-0.8
        { -5,   71,     2}, //   -0.8~-0.5
        { -4,   75,     1}, //   -0.5~-0.4
        { -3,   81,     1}, //   -0.4~-0.3
        { -1,   90,     1}, //   -0.3~-0.1
        {   0,  100,     0}, //   -0.1~0
        {   1,  110,     0}, //     0~0.1
        {   2,  114,    -1}, //    0.1~0.2       move decrease 1 index
        {   3,  123,    -1}, //    0.2~0.3
        {   4,  131,    -1}, //    0.3~0.4
        {   5,  141,    -2}, //    0.4~0.5
        {   7,  162,    -2}, //    0.5~0.7
        {   9,  186,    -3}, //    0.7~0.9
        { 10,  200,   -4}, //    0.9~1.0
        { 13,  246,   -4}, //    1.0~1.3
        { 16,  303,   -6}, //    1.3~1.6
        { 20,  400,   -7}, //    1.6~2       move decrease 10  index
        { 25,  566,   -9}, //    2~2.5       move decrease 20  index
        { 30,  800,   -11}, //    2.5~3      move decrease 30  index
    };

    // total 24 sets
    static strAEMOVE  g_AETrackingMoveTable[] =
    {
        {-20,   17,    20}, //   mean below -2.5  move increase 25 index
        {-20,   25,    15}, //   -2.5~-2  move increase 20 index
        {-15,   33,    10}, //   -2~-1.6
        {-15,   40,    6}, //   -1.6~-1.3
        {-10,   50,    4}, //   -1.3~-1
        { -8,   57,     3}, //   -1~-0.8
        { -5,   71,     2}, //   -0.8~-0.5
        { -4,   75,     1}, //   -0.5~-0.4
        { -3,   81,     1}, //   -0.4~-0.3
        { -1,   90,     1}, //   -0.3~-0.1
        {   0,  100,     0}, //   -0.1~0
        {   1,  110,     0}, //     0~0.1
        {   2,  114,    -1}, //    0.1~0.2       move decrease 1 index
        {   3,  123,    -1}, //    0.2~0.3
        {   4,  131,    -1}, //    0.3~0.4
        {   5,  141,    -2}, //    0.4~0.5
        {   7,  162,    -2}, //    0.5~0.7
        {   9,  186,    -3}, //    0.7~0.9
        { 10,  200,   -4}, //    0.9~1.0
        { 13,  246,   -4}, //    1.0~1.3
        { 16,  303,   -6}, //    1.3~1.6
        { 20,  400,   -7}, //    1.6~2       move decrease 10  index
        { 25,  566,   -9}, //    2~2.5       move decrease 20  index
        { 30,  800,   -11}, //    2.5~3      move decrease 30  index
    };


    // for AE limiter
    static strAELimiterTable g_AELimiterDataTable =
    {
        2,    //  iLEVEL1_GAIN
        10,  //  iLEVEL2_GAIN
        18,  //  iLEVEL3_GAIN
        28,  //  iLEVEL4_GAIN
        40,  //  iLEVEL5_GAIN
        50,  //  iLEVEL6_GAIN
        0,    //  iLEVEL1_TARGET_DIFFERENCE
        4,    //  iLEVEL2_TARGET_DIFFERENCE
        7,    //  iLEVEL3_TARGET_DIFFERENCE
        7,    //  iLEVEL4_TARGET_DIFFERENCE
        7,    //  iLEVEL5_TARGET_DIFFERENCE
        7,    //  iLEVEL6_TARGET_DIFFERENCE
        2,    //  iLEVEL1_GAINH
        -2,  //  iLEVEL1_GAINL
        6,    //  iLEVEL2_GAINH
        -7,  //  iLEVEL2_GAINL
        8,    // iLEVEL3_GAINH
        -9,  //  iLEVEL3_GAINL
        10,  //  iLEVEL4_GAINH
        -12, // iLEVEL4_GAINL
        12,   // iLEVEL5_GAINH
      -16, // iLEVEL5_GAINL
       15,   // iLEVEL6_GAINH
      -20, // iLEVEL6_GAINL
          1,     // iGAIN_DIFFERENCE_LIMITER
    };

    // for video dynamic frame rate
    static VdoDynamicFrameRate_T g_VdoDynamicFpsTable =
    {
       TRUE, // isEnableDFps
       50,  // EVThresNormal
       50,  // EVThresNight
    };

    static AE_HIST_WIN_CFG_T g_AEBlockHistWINCFG =
    {
        0,                      // uHist0StartBlockXRatio : Histogram 0 window config start block X ratio (0~100)
        100,                   // uHist0EndBlockXRatio : Histogram 0 window config end block X ratio (0~100)
        0,                      // uHist0StartBlockYRatio : Histogram 0 window config start block Y ratio (0~100)
        100,                   // uHist0EndBlockYRatio : Histogram 0 window config end block Y ratio (0~100)
        3,                      // uHist0OutputMode : Histogram 0 output source mode
        0,                      // uHist0BinMode : Histogram 0 bin mode range
        0,                      // uHist1StartBlockXRatio : Histogram 1 window config start block X ratio (0~100)
        100,                   // uHist1EndBlockXRatio : Histogram 1 window config end block X ratio (0~100)
        0,                      // uHist1StartBlockYRatio : Histogram 1 window config start block Y ratio (0~100)
        100,                   // uHist1EndBlockYRatio : Histogram 1 window config end block Y ratio (0~100)
        3,                      // uHist1OutputMode : Histogram 1 output source mode
        0,                      // uHist1BinMode : Histogram 1 bin mode range
        0,                      // uHist2StartBlockXRatio : Histogram 2 window config start block X ratio (0~100)
        100,                   // uHist2EndBlockXRatio : Histogram 2 window config end block X ratio (0~100)
        0,                      // uHist2StartBlockYRatio : Histogram 2 window config start block Y ratio (0~100)
        100,                   // uHist2EndBlockYRatio : Histogram 2 window config end block Y ratio (0~100)
        4,                      // uHist2OutputMode : Histogram 2 output source mode
        0,                      // uHist2BinMode : Histogram 2 bin mode range
        25,                     // uHist3StartBlockXRatio : Histogram 3 window config start block X ratio (0~100)
        75,                    // uHist3EndBlockXRatio : Histogram 3 window config end block X ratio (0~100)
        25,                     // uHist3StartBlockYRatio : Histogram 3 window config start block Y ratio (0~100)
        75,                    // uHist3EndBlockYRatio : Histogram 3 window config end block Y ratio (0~100)
        4,                      // uHist3OutputMode : Histogram 3 output source mode
        0,                      // uHist3BinMode : Histogram 3 bin mode range
    };

    static AE_HIST_WIN_CFG_T g_AEPixelHistWINCFG =
    {
        0,                      // uHist0StartBlockXRatio : Histogram 0 window config start block X ratio (0~100)
        100,                   // uHist0EndBlockXRatio : Histogram 0 window config end block X ratio (0~100)
        0,                      // uHist0StartBlockYRatio : Histogram 0 window config start block Y ratio (0~100)
        100,                   // uHist0EndBlockYRatio : Histogram 0 window config end block Y ratio (0~100)
        3,                      // uHist0OutputMode : Histogram 0 output source mode
        0,                      // uHist0BinMode : Histogram 0 bin mode range
        0,                      // uHist1StartBlockXRatio : Histogram 1 window config start block X ratio (0~100)
        100,                   // uHist1EndBlockXRatio : Histogram 1 window config end block X ratio (0~100)
        0,                      // uHist1StartBlockYRatio : Histogram 1 window config start block Y ratio (0~100)
        100,                   // uHist1EndBlockYRatio : Histogram 1 window config end block Y ratio (0~100)
        3,                      // uHist1OutputMode : Histogram 1 output source mode
        0,                      // uHist1BinMode : Histogram 1 bin mode range
        0,                      // uHist2StartBlockXRatio : Histogram 2 window config start block X ratio (0~100)
        100,                   // uHist2EndBlockXRatio : Histogram 2 window config end block X ratio (0~100)
        0,                      // uHist2StartBlockYRatio : Histogram 2 window config start block Y ratio (0~100)
        100,                   // uHist2EndBlockYRatio : Histogram 2 window config end block Y ratio (0~100)
        4,                      // uHist2OutputMode : Histogram 2 output source mode
        0,                      // uHist2BinMode : Histogram 2 bin mode range
        25,                     // uHist3StartBlockXRatio : Histogram 3 window config start block X ratio (0~100)
        75,                    // uHist3EndBlockXRatio : Histogram 3 window config end block X ratio (0~100)
        25,                     // uHist3StartBlockYRatio : Histogram 3 window config start block Y ratio (0~100)
        75,                    // uHist3EndBlockYRatio : Histogram 3 window config end block Y ratio (0~100)
        4,                      // uHist3OutputMode : Histogram 3 output source mode
        0,                      // uHist3BinMode : Histogram 3 bin mode range
    };

    static strGammaCFG g_GammaCFG =
    {
        FALSE,   //bEnable;
        4,        //u4IdxNum;
        {2000, 3000, 7000, 9000},        //u4X_BV[16];
        {0,     2,    4,    6},        //u4Y_GmaIdx[16];
        {10, 10, 10, 10}        //u4MidPointThd[16];
    };

    static strFlatScene g_FlatSceneCheck =
    {
        FALSE,       //bFlatSceneCheck;
        100,        //u4B1Pcent;
        600,        //u4B2Pcent;
        {300, 500}, //u4BP_X[2];
        {1024, 0}   //u4BP_Y[2];
    };
    static strFaceLocSize g_FaceLocSizeCheck =
    {
        FALSE,       //bFaceLocSizeCheck;
        {35,    10},//u4Size_X[2];
        {1024, 256},//u4Size_Y[2];
        {200,  600},//u4Loc_X[2];
        {1024, 256} //u4Loc_Y[2];
    };
    static strPerframeCFG g_PerframeCFG =
    {
        50,
        95,
        600,
        1080,
        920,
        200,
        80,
        80,
        240,
        4,
        254,
        510,
        1,
        TRUE,        
    };
    static strBVAccRatio  g_AEBVAccRatio = {
        TRUE,
        90,
        220,
        {
        100, //u4SpeedUpRatio
        100, //u4GlobalRatio
        255, //u4Bright2TargetEnd
         20, //u4Dark2TargetStart
        200, //u4B2TEnd
        100, //u4B2TStart
         40, //u4D2TEnd
         90, //u4D2TStart
         },
    };
    static strNonCWRAcc g_AENonCWRAcc = 
    {
        FALSE,
        255,
        160,
        130,
        47,
    };        
    static strFaceSmooth g_AEFaceSmooth = {
         10,                  // u4MinYLowBound;        // metering and face boundary min Y value
        256,                  // u4MaxYHighBound;      // metering and face boundary max Y value
          0,                  // i4FaceBVLowBound;     // face boundary min BV value
       4000,                  // i4FaceBVHighBound;    // face boundary max BV value
        235,                  // tempFDY HighBound
          8,                  // tempFDY LowBound
          5,                  // face lost max count
         90,                  // face window scale ratio range 1~400% default 90%         
    };
    static strAECWRTempSmooth g_AECWRTS =
    {
        TRUE,                                // enable AE HS Target temp smooth
        TRUE,                                // enable AE ABL Target temp smooth
        TRUE,                                // enable AE AOE Target temp smooth
        TRUE,                                // enable AE NS Target temp smooth
        TRUE,                                // enable AE Face Target temp smooth
        TRUE,                                // enable AE Video Target temp smooth
    };
    static strAEStableThd g_AEStableThd =
    {
        0,                      // u4InStableThd;  // 0.08EV
        1,                      // u4OutStableThd

        TRUE,             // enable ae different mode stable threshold setting
        1,                      // u4VideoInStableThd;  // 0.08EV
        1,                      // u4VideoOutStableThd
        1,                      // u4FaceInStableThd;  // 0.08EV
        2,                      // u4FaceOutStableThd
        0,                      // u4TouchInStableThd;  // 0.08EV
        1                      // u4TouchOutStableThd

    };

    static strAEMovingRatio g_AETouchMovingRatio =
    {
        100, //u4SpeedUpRatio
        100, //u4GlobalRatio
        190, //u4Bright2TargetEnd
        20,   //u4Dark2TargetStart
        80, //u4B2TEnd
        60,  //u4B2TStart
        40,  //u4D2TEnd
        80,  //u4D2TStart
    };

    static strHSrealBVRef g_HSbyRealBV =
     {
         TRUE,
         160,
         1500,
         {5500,  8000},
         {0, 1024}
     };

    static strHSMultiStep g_HSMultiStep =
    {
       TRUE,                                            // bEnableMultiStepHS
       4,                                                   // u4TblLength
       {1100, 2600, 4500,6000},      // u4MS_EVDiff
       {75, 160, 160, 195},                 // u4MS_OutdoorTHD
       {75, 144, 160,160},                  // u4MS_IndoorTHD
       {5500,  8000},                           // i4MS_BVRatio
    };

    static strAOEBVRef g_strAOEByBV =
    {
        TRUE,                                       // bEnable
        4,                                               // u4TblLength
        {-2000, 0, 3000,5500},         // i4BV
        {150, 170, 180, 180}            // u4THD
    };

    static strFaceBgYWeight g_FaceBgYWeightingTbl =
    {
       FALSE,                                      // bEnable
       5,                                               // u4TblLength
       {0, 500, 1000, 3000, 5000},  // u4RatioTbl_X
       {0, 128, 196,  240,  256}  // u4WghtTbl_Y
    };

    static strABL_absDiff g_ABLabsDiff =
    {
        TRUE,                           //enable enhance ABL(absDiff)
        {2500,  5000},            //u4EVDiffRatio_X
        {0, 1024},                    //u4EVDiffRatio_Y
        {9000,  11000},         //u4BVRatio_X
        {0, 1024}                     //u4BVRatio_Y
    };

    static strNS_CDF g_strNSCDF =
    {
       TRUE,                                       // bEnable
       600,
       {-1000, -300},
       {1024,   0},
    };
    static strHistStableCFG g_HistStableCFG =
    {
        TRUE,                                            // bEnableStablebyHist
        250,                                                //u4HistEVDiff : 0.25EV
        242,                                                //u4OverexpoTHD  : 95%
        13,                                                 //u4UnderexpoTHD : 5%
        140,                                                //u4HistStableTHD : 15%(1024base)
    };
    static strOverExpoAOECFG g_strOverExpoAOECFG =
    {
        FALSE,                                            // bEnableOverExpoAOE
        30,                                                //u4MaxDeltaTHD : AOE THD
        {                                                   //u4OEWeight[5][5] : Gaussian
            {55, 83, 95, 83, 55},
            {83, 125, 143, 125, 83},
            {95, 143, 164, 143, 95},
            {83, 125, 143, 125, 83},
            {55, 83, 95, 83, 55},
        }
    };
    static strHdrNightCFG g_strHdrNightCFG =
    {
       700,     // i4HistgramRatio_L
       200,     // i4HistgramRatio_H
       160,     // i4NightTarget
         3,     // i4HistSeg
        {       // i4HistgramRatio
        //  0    1    2    3    4    5    6    7    8    9   10
           100, 100, 100, 100, 100, 100, 66,  33,   0,   0,   0
        },
        {   // i4LVWtTbl
        //LV0   1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18   19
            0,  0,  0,  0,  0,  0,  0,  0,  0, 33,  66, 100, 100, 100, 100, 100, 100, 100, 100, 100
        }
    };
    static strAESceneMapping g_strStereoPlineMapping = 
    {
    {
    {LIB3A_AE_SCENE_AUTO, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_NIGHT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_ACTION, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_BEACH, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_CANDLELIGHT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_FIREWORKS, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_LANDSCAPE, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_PORTRAIT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_NIGHT_PORTRAIT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_PARTY, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_SNOW, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_SPORTS, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_STEADYPHOTO, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_SUNSET, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_THEATRE, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_ISO_ANTI_SHAKE, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_ISO100, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX15, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX15}},
    {LIB3A_AE_SCENE_ISO200, {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO200, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO200}},
    {LIB3A_AE_SCENE_ISO400, {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO400, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO400}},
    {LIB3A_AE_SCENE_ISO800, {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO800, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO800}},
    {LIB3A_AE_SCENE_ISO1600 , {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO1600, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO1600}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    },
    };

    static strAESceneMapping g_strHDRPlineMapping = 
    {
    {
    {LIB3A_AE_SCENE_AUTO, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_NIGHT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_ACTION, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_BEACH, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_CANDLELIGHT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_FIREWORKS, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_LANDSCAPE, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_PORTRAIT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_NIGHT_PORTRAIT, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_PARTY, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_SNOW, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_SPORTS, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_STEADYPHOTO, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_SUNSET, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_THEATRE, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_ISO_ANTI_SHAKE, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX12}},
    {LIB3A_AE_SCENE_ISO100, {AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX15, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX15}},
    {LIB3A_AE_SCENE_ISO200, {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO200, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO200}},
    {LIB3A_AE_SCENE_ISO400, {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO400, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO400}},
    {LIB3A_AE_SCENE_ISO800, {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO800, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO800}},
    {LIB3A_AE_SCENE_ISO1600 , {AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO1600, AETABLE_SCENE_INDEX13, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_SCENE_INDEX11, AETABLE_CAPTURE_ISO1600}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    },
    };

    static MINT32 g_strDeltaEVtoRatioArray[MAX_MAPPING_DELTABV_ISPRATIO] =
    { // ratio*1000
       1000,  // EV   0
       1072,   //EV    0.1
       1149,   //EV    0.2
       1231,   //EV    0.3
       1320,   //EV    0.4
       1414,   //EV    0.5
       1516,   //EV    0.6
       1625,   //EV    0.7
       1625,   //EV    0.8
       1625,   //EV    0.9
       1625,   //EV    1
       1625,   //EV    1.1
       1625,   //EV    1.2
       1625,   //EV    1.3
       1625,   //EV    1.4
       1625,   //EV    1.5
       1625,   //EV    1.6
       1625,   //EV    1.7
       1625,   //EV    1.8
       1625,   //EV    1.9
       1625,   //EV    2
       1625,   //EV    2.1
       1625,   //EV    2.2
       1625,   //EV    2.3
       1625,   //EV    2.4
       1625,   //EV    2.5
       1625,   //EV    2.6
       1625,   //EV    2.7
       1625,   //EV    2.8
       1625,   //EV    2.9   
    };
    static AE_PARAM_T strAEInitPara =
    {
        g_AEParasetting,
        {
            &g_Weight_Matrix,
            &g_Weight_Spot,
            &g_Weight_Average,
            NULL
        },
        g_strAFPLineTable,
        g_strAFZSDPLineTable,
        g_strStrobePLineTable,
        g_strStrobeZSDPLineTable,
        &g_strEVValueArray[0],
        g_AEMoveTable,
        g_AEVideoMoveTable,
        g_AEFaceMoveTable,
        g_AETrackingMoveTable,
        g_AELimiterDataTable,
        g_VdoDynamicFpsTable,
        &g_AEBlockHistWINCFG,
        &g_AEPixelHistWINCFG,

        // v1.2
        FALSE,              // bOldAESmooth
        TRUE,               // bEnableSubPreIndex
        0,                      // u4VideoLPFWeight; // 0~23

        &g_GammaCFG,
        &g_FlatSceneCheck,
        &g_FaceLocSizeCheck,
        &g_PerframeCFG,
        FALSE,               // Perframe AE smooth option
        FALSE,              // Perframe HDR AE smooth option

        //Open AE
        &g_AEStableThd,
        &g_AEBVAccRatio,
        &g_AENonCWRAcc,
        &g_AEFaceSmooth,
        &g_AECWRTS,
        &g_strAOEByBV,
        &g_FaceBgYWeightingTbl,
        &g_strNSCDF,

        &g_AETouchMovingRatio,
        &g_HSbyRealBV,
        &g_ABLabsDiff,
        &g_HSMultiStep,
        &g_HistStableCFG,
        &g_strOverExpoAOECFG,
        &g_strHdrNightCFG,
        g_strStereoPlineMapping,
        g_strHDRPlineMapping,
        &g_strDeltaEVtoRatioArray[0]
    };

    return strAEInitPara;
}


/*******************************************************************************
*
********************************************************************************/
template <>
MBOOL
isAEEnabled<ESensorDev_SubSecond>()
{
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define AE_CYCLE_NUM (3)

template <>
const MINT32*
getAEActiveCycle<ESensorDev_SubSecond>()
{
    // Default AE cycle
    static MINT32 i4AEActiveCycle[AE_CYCLE_NUM] =
    {
        MTRUE,
        MFALSE,
        MFALSE,
    };

    return (&i4AEActiveCycle[0]);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template <>
MINT32
getAECycleNum<ESensorDev_SubSecond>()
{
    return AE_CYCLE_NUM;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// AE Param Mapping Table by Scenario
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_PREVIEW>() {
    return getAEParam<ESensorDev_SubSecond>();
}
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_VIDEO>() {
    return getAEParam<ESensorDev_SubSecond>();
}
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_CAPTURE>() {
    return getAEParam<ESensorDev_SubSecond>();
}
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_CUSTOM1>() {
    return getAEParam<ESensorDev_SubSecond>();
}
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_CUSTOM2>() {
    return getAEParam<ESensorDev_SubSecond>();
}
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_CUSTOM3>() {
    return getAEParam<ESensorDev_SubSecond>();
}
template <> AE_PARAM_T const& getAEParamData<ESensorDev_SubSecond, CAM_SCENARIO_CUSTOM4>() {
    return getAEParam<ESensorDev_SubSecond>();
}

