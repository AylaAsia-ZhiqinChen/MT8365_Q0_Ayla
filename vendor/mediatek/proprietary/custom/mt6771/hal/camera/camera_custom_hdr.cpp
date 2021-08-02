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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#define LOG_TAG "HDR_CUST"

#include "camera_custom_hdr.h"
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <cutils/properties.h>
#include <utils/Errors.h>
#include <string.h>

#ifdef MTK_LOG_ENABLE
#undef MTK_LOG_ENABLE
#endif
#define MTK_LOG_ENABLE 1

#include <log/log.h>

/**************************************************************************
 *                      D E F I N E S / M A C R O S                       *
 **************************************************************************/
#define MAX_HDR_GAIN_ARRAY_ELEM     16  // Maximun HDR GainArray element number.

#define CLIP(a,min,max) ( (a)>(max) ? (max): (a)<(min)? (min) : (a) )
/**************************************************************************
 *     E N U M / S T R U C T / T Y P E D E F    D E C L A R A T I O N     *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static MUINT32 au4HdrGainArray[MAX_HDR_GAIN_ARRAY_ELEM] =
{
    CUST_HDR_GAIN_00,
    CUST_HDR_GAIN_01,
    CUST_HDR_GAIN_02,
    CUST_HDR_GAIN_03,
    CUST_HDR_GAIN_04,
    CUST_HDR_GAIN_05,
    CUST_HDR_GAIN_06,
    CUST_HDR_GAIN_07,
    CUST_HDR_GAIN_08,
    CUST_HDR_GAIN_09,
    CUST_HDR_GAIN_10,
    CUST_HDR_GAIN_11,
    CUST_HDR_GAIN_12,
    CUST_HDR_GAIN_13,
    CUST_HDR_GAIN_14,
    CUST_HDR_GAIN_15,
};

/**************************************************************************
 *       P R I V A T E    F U N C T I O N    D E C L A R A T I O N        *
 **************************************************************************/


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for Core Number.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrCoreNumberGet(void)
{
    return CUST_HDR_CORE_NUMBER;
}

///////////////////////////////////////////////////////////////////////////
/// @brief Get prolonged VD number.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrProlongedVdGet(void)
{
    return CUST_HDR_PROLONGED_VD;
}

///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for BRatio.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrBRatioGet(void)
{
    return CUST_HDR_BRATIO;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for Gain.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrGainArrayGet(MUINT32 u4ArrayIndex)
{
    if (u4ArrayIndex > MAX_HDR_GAIN_ARRAY_ELEM-1)
        u4ArrayIndex = MAX_HDR_GAIN_ARRAY_ELEM-1;

    return au4HdrGainArray[u4ArrayIndex];
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for BottomFRatio.
///////////////////////////////////////////////////////////////////////////
double CustomHdrBottomFRatioGet(void)
{
    return CUST_HDR_BOTTOM_FRATIO;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for TopFRatio.
///////////////////////////////////////////////////////////////////////////
double CustomHdrTopFRatioGet(void)
{
    return CUST_HDR_TOP_FRATIO;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for BottomFBound.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrBottomFBoundGet(void)
{
    return CUST_HDR_BOTTOM_FBOUND;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for TopFBound.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrTopFBoundGet(void)
{
    return CUST_HDR_TOP_FBOUND;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for ThHigh.
///////////////////////////////////////////////////////////////////////////
MINT32 CustomHdrThHighGet(void)
{
    return CUST_HDR_TH_HIGH;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for ThLow.
///////////////////////////////////////////////////////////////////////////
MINT32 CustomHdrThLowGet(void)
{
    return CUST_HDR_TH_LOW;
}


///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for TargetLevelSub.
///////////////////////////////////////////////////////////////////////////
MUINT32 CustomHdrTargetLevelSubGet(void)
{
    return CUST_HDR_TARGET_LEVEL_SUB;
}

///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for UseIspGamma.
///////////////////////////////////////////////////////////////////////////
MBOOL CustomHdrUseIspGamma(void)
{
    return CUST_HDR_USE_ISP_GAMMA;
}

///////////////////////////////////////////////////////////////////////////
/// @brief Get the customer-set value for Match Point Threshold.
///////////////////////////////////////////////////////////////////////////
MBOOL CustomHdrMatchPointThreshold(void)
{
    return CUST_HDR_MATCH_POINT_THRESHOLD;
}

/*******************************************************************************
* HDR exposure setting
*******************************************************************************/
#define MAX_LOG_BUF_SIZE    5000
static unsigned char GS_ucLogBuf[MAX_LOG_BUF_SIZE]; // Buffer to put log message. Will be outputed to file.
static char* pucLogBufPosition = NULL;  // A pointer pointing to some position in the GS_ucLogBuf[].
static MINT32 S_i4RunningNumber = 0;  // Record execution counts.

static unsigned int DumpToFile(
    char *fname,
    unsigned char *pbuf,
    unsigned int size
)
{
    int nw, cnt = 0;
    unsigned int written = 0;


    ALOGD("[DumpToFile] S_i4RunningNumber: %d", S_i4RunningNumber);
    ALOGD("[DumpToFile] opening file [%s]", fname);
    FILE* pFp = fopen(fname, "w+");
    if (NULL == pFp) {
        ALOGE("[DumpToFile] failed to create file [%s]: %s", fname, strerror(errno));
        return 0x80000000;
    }

    ALOGD("[DumpToFile] writing %d bytes to file [%s]", size, fname);
    while (written < size) {
        nw = fwrite(pbuf + written, sizeof( char ), size - written, pFp);
        if (nw < 0) {
            ALOGE("[DumpToFile] failed to write to file [%s]: %s", fname, strerror(errno));
            break;
        }
        written += nw;
        cnt++;
    }
    ALOGD("[DumpToFile] done writing %d bytes to file [%s] in %d passes", size, fname, cnt);
    fclose(pFp);

    return 0;

}

/*
HDRFlag = 0;  // original version, always capture 3 frames
HDRFlag = 1;  // adaptive version, if original version use -2EV less, we only capture 2 frames (0EV and +2EV). If original version use -2EV a lot, we still capture 3 frames.
HDRFlag = 2;  // performance priority version, always capture 2 frames. The EV settings are decided adapively.
HDR_NEOverExp_Percent = 15; // this is a customer tuning parameter. When HDRFlag==1, it means if there is less than HDR_NEOverExp_Percent/1000 pixels over saturation in 0EV, we capture 2 frames instead.
*/
MVOID getManualHDRExpSetting(const HDRExpSettingInputParam_T& rInput, HDRExpSettingOutputParam_T& rOutput , MINT32 evMode , MINT32 BYPASS_LTM , MINT32 BYPASS_LTM_SE , MINT32 BYPASS_LTM_NE , MINT32 BYPASS_LTM_LE)
{
    MUINT32 u4MaxHDRExpTimeInUS     = 200000; // Manually set, no longer than 0.5s (unit: us)
    MUINT32 u4MaxHDRSensorGain      = 4848; //Manually set, no larger than max gain in normal capture

    strAETable PLineAETable = rInput.PLineAETable;
    MINT32 i4aeTableCurrentIndex = rInput.i4aeTableCurrentIndex;
    MUINT32 u4MaxIndex = rInput.PLineAETable.u4TotalIndex-1;

    MUINT32 u4TargetIdx[3];
    MINT32 dfTargetEV[3];

    // 10x EX: -10 = -1 EV , 20 = +2 EV
    dfTargetEV[0] = property_get_int32("vendor.mediatek.hdr.evmode.frm1" , 0);
    dfTargetEV[1] = property_get_int32("vendor.mediatek.hdr.evmode.frm2" , 0);
    dfTargetEV[2] = property_get_int32("vendor.mediatek.hdr.evmode.frm3" , 0);

    if(evMode == 1) // Frame Num = 2 , duplicate arg[1] to arg[2]
    {
        rOutput.u4OutputFrameNum = 2;
        u4TargetIdx[0] = CLIP(i4aeTableCurrentIndex + dfTargetEV[0], 0, u4MaxIndex);
        u4TargetIdx[1] = CLIP(i4aeTableCurrentIndex + dfTargetEV[1], 0, u4MaxIndex);

        // get shutter and gain from pline table
        rOutput.u4SensorGain[0] = PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[0]].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[0]].u4IspGain >> 10;
        rOutput.u4SensorGain[1] = PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[1]].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[1]].u4IspGain >> 10;
        rOutput.u4SensorGain[2] = rOutput.u4SensorGain[1];

        rOutput.u4ExpTimeInUS[0] =  (PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[0]].u4Eposuretime)*1;
        rOutput.u4ExpTimeInUS[1] =  (PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[1]].u4Eposuretime)*1;
        rOutput.u4ExpTimeInUS[2] =  rOutput.u4ExpTimeInUS[1];

        rOutput.u4FinalGainDiff[0] = static_cast<int>( 1024 / (static_cast<double>((rOutput.u4SensorGain[0] * rOutput.u4ExpTimeInUS[0]) / (rOutput.u4SensorGain[1] * rOutput.u4ExpTimeInUS[1])) + 0.5 ));
        rOutput.u4FinalGainDiff[1] = rOutput.u4FinalGainDiff[0];
    }
    else if( evMode == 2) // Frame Num = 3
    {
        rOutput.u4OutputFrameNum = 3;
        u4TargetIdx[0] = CLIP(i4aeTableCurrentIndex + dfTargetEV[0]*10, 0, u4MaxIndex);
        u4TargetIdx[1] = CLIP(i4aeTableCurrentIndex + dfTargetEV[1]*10, 0, u4MaxIndex);
        u4TargetIdx[2] = CLIP(i4aeTableCurrentIndex + dfTargetEV[2]*10, 0, u4MaxIndex);

        // get shutter and gain from pline table
        rOutput.u4SensorGain[0] = PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[0]].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[0]].u4IspGain >> 10;
        rOutput.u4SensorGain[1] = PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[1]].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[1]].u4IspGain >> 10;
        rOutput.u4SensorGain[2] = PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[2]].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[2]].u4IspGain >> 10;

        rOutput.u4ExpTimeInUS[0] =  (PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[0]].u4Eposuretime)*1;
        rOutput.u4ExpTimeInUS[1] =  (PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[1]].u4Eposuretime)*1;
        rOutput.u4ExpTimeInUS[2] =  (PLineAETable.pCurrentTable->sPlineTable[u4TargetIdx[2]].u4Eposuretime)*1;

        rOutput.u4FinalGainDiff[0] = static_cast<int>( 1024 / (static_cast<double>((rOutput.u4SensorGain[0] * rOutput.u4ExpTimeInUS[0]) / (rOutput.u4SensorGain[1] * rOutput.u4ExpTimeInUS[1])) + 0.5 ));
        rOutput.u4FinalGainDiff[1] = static_cast<int>( 1024 / (static_cast<double>((rOutput.u4SensorGain[2] * rOutput.u4ExpTimeInUS[2]) / (rOutput.u4SensorGain[1] * rOutput.u4ExpTimeInUS[1])) + 0.5 ));
    }
    else if( evMode == 3) //Reserve for special purpose
    {
    }

    rOutput.u4BypassLTM[0] = (BYPASS_LTM || BYPASS_LTM_SE);
    rOutput.u4BypassLTM[1] = (BYPASS_LTM || BYPASS_LTM_NE);
    rOutput.u4BypassLTM[2] = (BYPASS_LTM || BYPASS_LTM_LE);


    // set flare offset as 0, and reduce in HDR software
    rOutput.u1FlareOffset[0] = 0;
    rOutput.u1FlareOffset[1] = 0;
    rOutput.u1FlareOffset[2] = 0;
    rOutput.u4TargetTone = 150;   //for HDR1.0
    ALOGD("SWHDR FrmNum = %d", rOutput.u4OutputFrameNum);
    ALOGD("SWHDR Output 0 Time = %d, Gain = %d", rOutput.u4ExpTimeInUS[0], rOutput.u4SensorGain[0]);
    ALOGD("SWHDR Output 1 Time = %d, Gain = %d", rOutput.u4ExpTimeInUS[1], rOutput.u4SensorGain[1]);
    ALOGD("SWHDR Output 2 Time = %d, Gain = %d", rOutput.u4ExpTimeInUS[2], rOutput.u4SensorGain[2]);
    ALOGD("SWHDR Flare Ofset = %d %d %d", rOutput.u1FlareOffset[0], rOutput.u1FlareOffset[1], rOutput.u1FlareOffset[2]);
    ALOGD("SWHDR BYPASS_LTM = %d %d %d", rOutput.u4BypassLTM[0], rOutput.u4BypassLTM[1], rOutput.u4BypassLTM[2]);
    ALOGD("SWHDR Gain Diff = %d %d", rOutput.u4FinalGainDiff[0], rOutput.u4FinalGainDiff[1]);
}
MVOID getHDRExpSetting(const HDRExpSettingInputParam_T& rInput, HDRExpSettingOutputParam_T& rOutput)
{
    MINT32 evMode = property_get_int32("vendor.mediatek.hdr.evmode" , 0);
    MINT32 BYPASS_LTM    = property_get_int32("vendor.mediatek.hdr.ltm" , 1);    // 0 : w/ LTM , 1 : w/o LTM
    MINT32 BYPASS_LTM_SE = property_get_int32("vendor.mediatek.hdr.ltm.se" , 0);
    MINT32 BYPASS_LTM_NE = property_get_int32("vendor.mediatek.hdr.ltm.ne" , 0);
    MINT32 BYPASS_LTM_LE = property_get_int32("vendor.mediatek.hdr.ltm.le" , 0);
    if( (evMode == 1) || (evMode == 2) || (evMode == 3)) //prevent to unexpected evmode value
    {
        return getManualHDRExpSetting(rInput , rOutput , evMode , BYPASS_LTM , BYPASS_LTM_SE , BYPASS_LTM_NE , BYPASS_LTM_LE);
    }

    static MUINT32 HDRFlag = CUST_HDR_CAPTURE_ALGORITHM;
    MUINT32 HDR_NEOverExp_Percent   = CUST_HDR_NEOverExp_Percent;
    MUINT32 u4MaxHDRExpTimeInUS     = 200000; // Manually set, no longer than 0.5s (unit: us)
    MUINT32 u4MaxSafeHDRExpTimeInUS = 31250; // Manually set, no longer than 0.5s (unit: us)
    MUINT32 u4MaxHDRSensorGain      = 4848; //Manually set, no larger than max gain in normal capture
    MUINT32 u4MaxExpTimeInUS;
    MUINT32 u4MaxSensorGain;
    MUINT32 u4TimeMode  = 1;                // 0:Depend on default AE parameters; 1: Manually set
    MUINT32 u4GainMode  = 1;                // 0:Depend on default AE parameters; 1: Manually set
    MBOOL bGain0EVLimit = MFALSE;           // True: Limit the gain of 0EV and short exposure image; False: Keep it

    MUINT32 Hightlightsum = 0;
    MUINT32 HISsum = 0;
    MUINT32 minV;
    double DarkAvg = 0;
    MUINT32 u4TargetTopIdx;
    MUINT32 u4TargetNormalIdx;
    MUINT32 u4TargetBottomIdx;
    MUINT32 u4MaxIndex = rInput.PLineAETable.u4TotalIndex-1;
    MINT32 i4aeTableCurrentIndex = rInput.i4aeTableCurrentIndex;
    strAETable PLineAETable = rInput.PLineAETable;
    MINT32 u4AOEMode = rInput.u4AOEMode;
    double dfEVDiffTh   = EVDIFFTH;// 3EV
    double dfGainDiffTh = pow(2, dfEVDiffTh);

    double LOG10mean[20]={0, 0.3979, 0.6532, 0.8129, 0.9294, 1.0212, 1.0969, 1.1614, 1.2175, 1.2672, 1.3118, 1.3522, 1.3892, 1.4232, 1.4548, 1.4843, 1.5119, 1.5378, 1.5623, 1.5855};
    double LOG10meanF[40]={0.0000,   0.0000,   0.3010,   0.4771,   0.6021,   0.6990,   0.7782,   0.8451,   0.9031,   0.9542,   1.0000,   1.0414,   1.0792,   1.1139,   1.1461,   1.1761,   1.2041,   1.2304,   1.2553,   1.2788,   1.3010,   1.3222,   1.3424,   1.3617,   1.3802,   1.3979,   1.4150,   1.4314,   1.4472,   1.4624,   1.4771,   1.4914,   1.5051,   1.5185,   1.5315,   1.5441,   1.5563,   1.5682,   1.5798,   1.5911};
    double dfTargetTopEV    =  2; // Target EV of long exposure image
    double dfTargetNormalEV =  0; // Target EV of normal exposure image
    double dfTargetBottomEV = -2; // Target EV of short exposure image
    double dfSafeTargetTopEV = 0.5; // Target EV of long exposure image

    // Temporary parameters
    MUINT32 i;
    double dfGainDiff[2] = {0};

    double dfTopGain     = pow(2, dfTargetTopEV);
    double dfSafeTopGain = pow(2, dfSafeTargetTopEV);
    double dfBottomGain  = pow(2, dfTargetBottomEV);

    ALOGD("u4AOEMode(%d), u4MaxSensorAnalogGain(%d), u4MaxAEExpTimeInUS(%d), u4MinAEExpTimeInUS(%d), u4ShutterLineTime(%d)"
            , u4AOEMode
            , rInput.u4MaxSensorAnalogGain
            , rInput.u4MaxAEExpTimeInUS
            , rInput.u4MinAEExpTimeInUS
            , rInput.u4ShutterLineTime
            );
    ALOGD("u4MaxAESensorGain(%d), u4MinAESensorGain(%d), u4ExpTimeInUS0EV(%d), u4SensorGain0EV(%d), u1FlareOffset0EV(%d)"
            , rInput.u4MaxAESensorGain
            , rInput.u4MinAESensorGain
            , rInput.u4ExpTimeInUS0EV
            , rInput.u4SensorGain0EV
            , rInput.u1FlareOffset0EV
            );
    ALOGD("i4GainBase0EV(%d), i4LE_LowAvg(%d), i4SEDeltaEVx100(%d)"
            , rInput.i4GainBase0EV
            , rInput.i4LE_LowAvg
            , rInput.i4SEDeltaEVx100
            );
    MINT32  i4GainBase0EV;      // AOE application for LE calculation
    MINT32  i4LE_LowAvg;        // AOE application for LE calculation, def: 0 ~ 39 avg
    MINT32  i4SEDeltaEVx100;    // AOE application for SE calculation

    if(rInput.u4SensorGain0EV == 0
        || rInput.u4ExpTimeInUS0EV == 0
        || rInput.u4MaxAEExpTimeInUS == 0
        || rInput.u4MaxAESensorGain == 0
        || rInput.u4MaxSensorAnalogGain == 0
        )
    {
        ALOGD("u4SensorGain0EV(%d), u4ExpTimeInUS0EV(%d), u4MaxAEExpTimeInUS(%d), u4MaxAESensorGain(%d), u4MaxSensorAnalogGain(%d) shouldn't be 0"
                , rInput.u4SensorGain0EV
                , rInput.u4ExpTimeInUS0EV
                , rInput.u4MaxAEExpTimeInUS
                , rInput.u4MaxAESensorGain
                , rInput.u4MaxSensorAnalogGain
                );

        ALOGD("Invalid AE Setting for HDR Capture !");
        exit(-1);
    }

    if(u4AOEMode==1) //Dynamic EV Bracket
    {
        //Initial AE Setting of LE Calculation without any limitation
        HISsum = 0;
        DarkAvg  = 0;
        for(i=0 ; i<SE_HIST_BIN_NUM ; i++)
        {
            HISsum += rInput.u4Histogram[i];
            DarkAvg  += LOG10mean[i]*rInput.u4Histogram[i];
        }
        if(HISsum!=0)
        {
            DarkAvg /= HISsum; //Average of Bin0~Bin39, should same as i4LEAvg0_40
        }
        else
        {
            ALOGD("Invalid AE histogram for HDR Capture !");
        }

        if(INTENSITY_TARGET>DarkAvg)
        {
            dfTargetTopEV = (INTENSITY_TARGET - DarkAvg)/0.3; //0.3 diff in log10 base = 1EV
            dfTargetTopEV = (dfTargetTopEV>MAX_LE_EV) ? MAX_LE_EV : dfTargetTopEV;
        }
        else
        {
            ALOGD("AE Setting Needs Check for HDR Capture !");
        }

        //SE Calculation
        dfTargetBottomEV = (double)(rInput.i4SEDeltaEVx100 / 100.0) - SE_OFFSET; // i4SEDeltaEVx100 = diff to no over exposure SE

        // decide capture 2 inputs or 3 inputs
        if( HDRFlag == 0 )          rOutput.u4OutputFrameNum   = 3;
        else if( HDRFlag == 2 )     rOutput.u4OutputFrameNum   = 2;
        else   //HDRFlag == 1
        {
            if(dfTargetTopEV-dfTargetBottomEV > EVDIFFTH)
                rOutput.u4OutputFrameNum   = 3;
            else
                rOutput.u4OutputFrameNum   = 2;
        }

        if(rOutput.u4OutputFrameNum == 3)
            dfTargetNormalEV = (dfTargetTopEV + dfTargetBottomEV) / 2;

    }
    else //if(rInput.u4AOEMode!=1) //for normal mode (non-VHDR mode) //rInput.u4AOEMode==0 : standard bracketing, rInput.u4AOEMode==2 : adaptive bracketing
    {

        char adaptmp[256] = {'\0'};
        property_get("vendor.mediatek.hdr.adapbracket", adaptmp, "0");
        int adapBR = atoi(adaptmp);
        if(adapBR)
        {
            u4AOEMode = 2;
        }
        else
        {
            u4AOEMode = BRACKETING_TYPE;
        }

        /* For compare with static bracketing
        static int switch_type= 0;
        if(switch_type==0||switch_type==1)
        {
            u4AOEMode = 2;
        }
        else//(switch_type==2||switch_type==3)
        {
            u4AOEMode = 0;
        }
        switch_type++;
        switch_type=(switch_type==4)?0:switch_type;
        */
        //if face is detected, use only static bracketing
        if(rInput.bDetectFace)
        {
            u4AOEMode = 0;
        }

        //use only 3 frame mode for adaptive bracketing
        if(u4AOEMode==2)
        {
            HDRFlag = 0;
        }
        else
        {
            HDRFlag = CUST_HDR_CAPTURE_ALGORITHM;
        }

        // decide capture 2 inputs or 3 inputs
        if( HDRFlag == 0 )          rOutput.u4OutputFrameNum   = 3;
        else if( HDRFlag == 2 )     rOutput.u4OutputFrameNum   = 2;
        else   //HDRFlag == 1
        {
            HISsum = 0;
            for(i=0 ; i<128 ; i++)
                HISsum = HISsum + rInput.u4Histogram[i];

            // calculate the percentage of bin[126] + bin[127]
            if(static_cast<int>(static_cast<double>((rInput.u4Histogram[126] + rInput.u4Histogram[127]) * 1000) / HISsum + 0.5) < HDR_NEOverExp_Percent)
                rOutput.u4OutputFrameNum   = 2;
            else
                rOutput.u4OutputFrameNum   = 3;
        }

        if(u4AOEMode==2)
        {
            // Histogram Calculation :DarkAvg
            HISsum = 0;
            DarkAvg  = 0;
            for(i=0 ; i<40 ; i++)
            {
                minV = (rInput.u4FlareHistogram[i] < rInput.u4FlareHistogram[i+40]) ? rInput.u4FlareHistogram[i] : rInput.u4FlareHistogram[i+40];
                minV = (minV                       < rInput.u4FlareHistogram[i+80]) ? minV                       : rInput.u4FlareHistogram[i+80];
                HISsum += minV;
                DarkAvg  += LOG10meanF[i]*minV;
            }
            if(HISsum!=0)
            {
                DarkAvg /= HISsum; //Average of Bin0~Bin39, should same as i4LEAvg0_40
            }
            else
            {
                ALOGD("Invalid AE histogram for HDR Capture ! \n");
            }

            // Histogram Calculation :Hightlightsum
            Hightlightsum = 0;
            for(i=120 ; i<128 ; i++)
            {
                Hightlightsum += rInput.u4Histogram[i];
            }

            // Adaptive bracketing algorithm
            if(INTENSITY_TARGET>DarkAvg)
            {
                dfTargetTopEV = (INTENSITY_TARGET - DarkAvg)/0.3; //0.3 diff in log10 base = 1EV
            }
            else
            {
                ALOGD("AE Setting Needs Check for HDR Capture ! \n");
            }

            dfTargetTopEV = (dfTargetTopEV>MAX_LE_EV) ? MAX_LE_EV : dfTargetTopEV;   //MAX_LE_EV = 3.33  //this value should be change if iVHDR/mVHDR AE
            dfTargetTopEV = (dfTargetTopEV<MIN_LE_EV) ? MIN_LE_EV : dfTargetTopEV;   //MIN_LE_EV = 0.34
        }


        if(u4AOEMode==0 && rOutput.u4OutputFrameNum == 3) // { -2, 0, +2 } for standard bracketing
        {
            // decide ExpTime and Sensor Gain for all images
            dfTargetBottomEV  = -2; // Target EV of short exposure image
            dfTargetNormalEV  = 0;
            dfTargetTopEV     = 2; // Target EV of long exposure image

        }
        else if(u4AOEMode==0 && rOutput.u4OutputFrameNum == 2) // { -1, +2 } for standard bracketing
        {
            // decide ExpTime and Sensor Gain for all images
            dfTargetBottomEV  = -1; // Target EV of short exposure image
            dfTargetTopEV     = 2; // Target EV of long exposure image

        }
        else if(u4AOEMode==2 && rOutput.u4OutputFrameNum == 3) // { SE, NE, LE } for adaptive bracketing
        {
            if(dfTargetTopEV<=0.667)
            {
                dfTargetBottomEV = dfTargetTopEV - 3.333;
                dfTargetNormalEV = dfTargetTopEV - 1.667;
            }
            else
            {
                dfTargetBottomEV = dfTargetTopEV - 4;
                // dfTargetNormalEV = dfTargetTopEV - 2;
                dfTargetNormalEV = 0;
            }

            if(Hightlightsum <  HIGHLIGHT_TH && dfTargetTopEV < 2)
            {
                dfTargetBottomEV = -2;
                dfTargetNormalEV = 0;
            }

        }
        else if(u4AOEMode==2 && rOutput.u4OutputFrameNum == 2) // { LE, SE } for adaptive bracketing
        {
            if(dfTargetTopEV<=0.667)
            {
                dfTargetBottomEV = dfTargetTopEV - 2.333;
            }
            else
            {
                dfTargetBottomEV = dfTargetTopEV - 3;
            }

            if(Hightlightsum <  HIGHLIGHT_TH && dfTargetTopEV < 2)
            {
                dfTargetBottomEV = -1;
            }
        }
    }

        //For very dark
        if( (i4aeTableCurrentIndex + dfTargetTopEV*10) > u4MaxIndex ){
            rOutput.u4OutputFrameNum = 1;
        }
        // get p-line index
        u4TargetTopIdx    = CLIP(i4aeTableCurrentIndex + dfTargetTopEV*10, 0, u4MaxIndex);
        u4TargetNormalIdx = CLIP(i4aeTableCurrentIndex + dfTargetNormalEV*10, 0, u4MaxIndex);
        u4TargetBottomIdx = CLIP(i4aeTableCurrentIndex + dfTargetBottomEV*10, 0, u4MaxIndex);

        // get shutter and gain from pline table
        rOutput.u4SensorGain[0] = PLineAETable.pCurrentTable->sPlineTable[u4TargetBottomIdx].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetBottomIdx].u4IspGain >> 10;
        rOutput.u4SensorGain[1] = PLineAETable.pCurrentTable->sPlineTable[u4TargetNormalIdx].u4AfeGain  *  PLineAETable.pCurrentTable->sPlineTable[u4TargetNormalIdx].u4IspGain >> 10;
        rOutput.u4SensorGain[2] = PLineAETable.pCurrentTable->sPlineTable[u4TargetTopIdx].u4AfeGain     *  PLineAETable.pCurrentTable->sPlineTable[u4TargetTopIdx].u4IspGain >> 10;
        rOutput.u4ExpTimeInUS[0]=  (PLineAETable.pCurrentTable->sPlineTable[u4TargetBottomIdx].u4Eposuretime)*1;
        rOutput.u4ExpTimeInUS[1]=  (PLineAETable.pCurrentTable->sPlineTable[u4TargetNormalIdx].u4Eposuretime)*1;
        rOutput.u4ExpTimeInUS[2]=  (PLineAETable.pCurrentTable->sPlineTable[u4TargetTopIdx].u4Eposuretime)*1;
        rOutput.u4BypassLTM[0] = (BYPASS_LTM || BYPASS_LTM_SE);
        rOutput.u4BypassLTM[1] = (BYPASS_LTM || BYPASS_LTM_NE);
        rOutput.u4BypassLTM[2] = (BYPASS_LTM || BYPASS_LTM_LE);
        ALOGD("SWHDR (PLineAETable.pCurrentTable->sPlineTable[u4TargetTopIdx].u4Eposuretime) = %d\n",(PLineAETable.pCurrentTable->sPlineTable[u4TargetTopIdx].u4Eposuretime));
        ALOGD("SWHDR (PLineAETable.pCurrentTable->sPlineTable[u4TargetNormalIdx].u4Eposuretime) = %d\n",(PLineAETable.pCurrentTable->sPlineTable[u4TargetNormalIdx].u4Eposuretime));
        ALOGD("SWHDR (PLineAETable.pCurrentTable->sPlineTable[u4TargetBottomIdx].u4Eposuretime) = %d\n",(PLineAETable.pCurrentTable->sPlineTable[u4TargetBottomIdx].u4Eposuretime));
        // set flare offset as 0, and reduce in HDR software
        rOutput.u1FlareOffset[0] = 0;
        rOutput.u1FlareOffset[1] = 0;
        rOutput.u1FlareOffset[2] = 0;
        rOutput.u4TargetTone = 150;   //for HDR1.0

        if(rOutput.u4OutputFrameNum == 3)
        {
            // calculate GainDiff
            if(rOutput.u4ExpTimeInUS[1]*rOutput.u4SensorGain[1]!=0)
            {
                dfGainDiff[0] = static_cast<double>(rOutput.u4SensorGain[0]*rOutput.u4ExpTimeInUS[0]) / (rOutput.u4ExpTimeInUS[1]*rOutput.u4SensorGain[1]);
                dfGainDiff[1] = static_cast<double>(rOutput.u4SensorGain[2]*rOutput.u4ExpTimeInUS[2]) / (rOutput.u4ExpTimeInUS[1]*rOutput.u4SensorGain[1]);
            }
            else
            {
                ALOGD("SWHDR Error calculation of GainDiff = %f %f\n",dfGainDiff[0], dfGainDiff[1]);
            }


            //Target tone and Gain for Img Normalization
            if(dfGainDiff[0]!=0 && dfGainDiff[1]!=0)
            {
                rOutput.u4FinalGainDiff[0] = static_cast<MUINT32>(1024 / dfGainDiff[0] + 0.5);
                rOutput.u4FinalGainDiff[1] = static_cast<MUINT32>(1024 / dfGainDiff[1] + 0.5);
            }
            else
            {
                ALOGD("getHDRExpSetting dfGainDiff Error ! %d %d\n", dfGainDiff[0], dfGainDiff[1]);
            }
        }
        else if(rOutput.u4OutputFrameNum == 2)
        {
            rOutput.u4SensorGain[1] = rOutput.u4SensorGain[2];
            rOutput.u4ExpTimeInUS[1]=  rOutput.u4ExpTimeInUS[2];

            // calculate GainDiff
            if(rOutput.u4ExpTimeInUS[1]*rOutput.u4SensorGain[1]!=0)
            {
                dfGainDiff[0] = static_cast<double>(rOutput.u4SensorGain[0]*rOutput.u4ExpTimeInUS[0]) / (rOutput.u4ExpTimeInUS[1]*rOutput.u4SensorGain[1]);
            }
            else
            {
                ALOGD("SWHDR Error calculation of GainDiff = %f\n",dfGainDiff[0]);
            }

            //Target tone and Gain for Img Normalization
            if(dfGainDiff[0]!=0)
            {
                rOutput.u4FinalGainDiff[0] = static_cast<MUINT32>(1024 / dfGainDiff[0] + 0.5);
                rOutput.u4FinalGainDiff[1] = 1024;//default value, no use
            }
            else
            {
                ALOGD("getHDRExpSetting dfGainDiff Error ! %d\n", dfGainDiff[0]);
            }
        }
        else if(rOutput.u4OutputFrameNum == 1){ //For very dark
            rOutput.u4SensorGain[0] = rOutput.u4SensorGain[1] = rOutput.u4SensorGain[2] = rInput.u4SensorGain0EV;
            rOutput.u4ExpTimeInUS[0] = rOutput.u4ExpTimeInUS[1] = rOutput.u4ExpTimeInUS[2] = rInput.u4ExpTimeInUS0EV;
            rOutput.u4FinalGainDiff[0] = rOutput.u4FinalGainDiff[1] = 0;
        }



#if 1
    char value[256] = {'\0'};
    property_get("vendor.mediatek.hdr.debug", value, "0");
    int hdr_debug_mode = atoi(value) || CUST_HDR_DEBUG;
    if(hdr_debug_mode)
    {
        // Increase 4-digit running number (range: 1 ~ 9999).
        if (S_i4RunningNumber >= 9999)
            S_i4RunningNumber = 1;
        else
            S_i4RunningNumber++;

        rOutput.i4RunningNumber = S_i4RunningNumber;

        pucLogBufPosition = (char*)GS_ucLogBuf;
        ::sprintf(pucLogBufPosition, "< No.%04d > ----------------------------------------------------------------------\n", S_i4RunningNumber);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "[System Paramters]\n");
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "0EV Exposure Time = %d 0EV Sensor Gain = %d 0EV Flare Offset %d\n", rInput.u4ExpTimeInUS0EV, rInput.u4SensorGain0EV, rInput.u1FlareOffset0EV);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "Max Exposure Time Sensor= %d\nMaxSensor Gain Sensor= %d\n", rInput.u4MaxAEExpTimeInUS, rInput.u4MaxAESensorGain);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "Max Exposure Time Manual= %d\nMaxSensor Gain Manual= %d\n", u4MaxHDRExpTimeInUS, u4MaxHDRSensorGain);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "Max Exposure Time = %d\nMaxSensor Gain = %d\n", u4MaxExpTimeInUS, u4MaxSensorGain);
        pucLogBufPosition += strlen(pucLogBufPosition);

        ::sprintf(pucLogBufPosition, "\n[Tuning Paramters]\n");
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "SWHDR AE Mode = %d\n",u4AOEMode);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "SWHDR INTENSITY_TARGET = %f, NE_AOE_GAIN = %f, SE_OFFSET = %f, MAX_LE_EV = %f\n", INTENSITY_TARGET, NE_AOE_GAIN, SE_OFFSET, MAX_LE_EV);
        pucLogBufPosition += strlen(pucLogBufPosition);

        ::sprintf(pucLogBufPosition, "SWHDR Input delta EV = %f, Avg0_40 = %d Avg0_40_2 = %f\n", dfTargetBottomEV, rInput.i4LE_LowAvg, DarkAvg);
        pucLogBufPosition += strlen(pucLogBufPosition);

        ::sprintf(pucLogBufPosition, "SWHDR AE HDRFlag Mode = %d\n", HDRFlag);
        pucLogBufPosition += strlen(pucLogBufPosition);

        ::sprintf(pucLogBufPosition, "Target Top EV = %f\n Target Normal EV = %f\n Target Bottom EV = %f\n", dfTargetTopEV, dfTargetNormalEV, dfTargetBottomEV);
        pucLogBufPosition += strlen(pucLogBufPosition);

        ::sprintf(pucLogBufPosition, "bGain0EVLimit = %s\n", (bGain0EVLimit ? "true" : "false"));
        pucLogBufPosition += strlen(pucLogBufPosition);

        ::sprintf(pucLogBufPosition, "\n[Output Paramters]\n");
        pucLogBufPosition += strlen(pucLogBufPosition);
        for (i = 0; i < 3; i++) {
                ::sprintf(pucLogBufPosition, "Final Frame %d ExposureTime = %d SensorGain = %d Flare Offset = %d BYPASS_LTM = %d\n", i, rOutput.u4ExpTimeInUS[i], rOutput.u4SensorGain[i], rOutput.u1FlareOffset[i] , rOutput.u4BypassLTM[i]);
            pucLogBufPosition += strlen(pucLogBufPosition);
        }
        ::sprintf(pucLogBufPosition, "Final EVdiff[0] = %d\nFinal EVdiff[1] = %d\n", rOutput.u4FinalGainDiff[0], rOutput.u4FinalGainDiff[1]);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "OutputFrameNum = %d\n", rOutput.u4OutputFrameNum);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "Final FlareOffsetOut[0]= %d\nFinal FlareOffsetOut[1]= %d\nFinal FlareOffsetOut[2]= %d\n", rOutput.u1FlareOffset[0], rOutput.u1FlareOffset[1], rOutput.u1FlareOffset[2]);
        pucLogBufPosition += strlen(pucLogBufPosition);
        ::sprintf(pucLogBufPosition, "Final TargetTone= %d\n", rOutput.u4TargetTone);
        pucLogBufPosition += strlen(pucLogBufPosition);

        char szFileName[100];
        ::sprintf(szFileName, HDR_DEBUG_OUTPUT_FOLDER"%04d-HDR_ExposureSetting.txt", S_i4RunningNumber);
        DumpToFile(szFileName, (unsigned char *)GS_ucLogBuf, MAX_LOG_BUF_SIZE);
    }
        ALOGD("SWHDR AE Mode = %d", u4AOEMode);
        ALOGD("SWHDR FrmNum = %d", rOutput.u4OutputFrameNum);
        ALOGD("SWHDR Input delta EV(Bottom) = %f, Normal EV = %f, TopEV = %f, Avg0_40 = %d Avg0_40_2 = %f", dfTargetBottomEV, dfTargetNormalEV, dfTargetTopEV, rInput.i4LE_LowAvg, DarkAvg);

        ALOGD("SWHDR Face detected= %d", rInput.bDetectFace);

        ALOGD("SWHDR AE HDRFlag Mode = %d", HDRFlag);
        ALOGD("SWHDR Input i4aeTableCurrentIndex = %d, 0EV Time = %d, Gain = %d Flare = %d", rInput.i4aeTableCurrentIndex, rInput.u4ExpTimeInUS0EV, rInput.u4SensorGain0EV,rInput.u1FlareOffset0EV);
        ALOGD("SWHDR Output u4TargetTopIdx %d, u4TargetNormalIdx %d, u4TargetBottomIdx %d", u4TargetTopIdx, u4TargetNormalIdx, u4TargetBottomIdx);

        ALOGD("SWHDR Output 0 Time = %d, Gain = %d", rOutput.u4ExpTimeInUS[0], rOutput.u4SensorGain[0]);
        ALOGD("SWHDR Output 1 Time = %d, Gain = %d", rOutput.u4ExpTimeInUS[1], rOutput.u4SensorGain[1]);
        ALOGD("SWHDR Output 2 Time = %d, Gain = %d", rOutput.u4ExpTimeInUS[2], rOutput.u4SensorGain[2]);
        ALOGD("SWHDR Flare Ofset = %d %d %d", rOutput.u1FlareOffset[0], rOutput.u1FlareOffset[1], rOutput.u1FlareOffset[2]);
        ALOGD("SWHDR BYPASS_LTM = %d %d %d", rOutput.u4BypassLTM[0], rOutput.u4BypassLTM[1], rOutput.u4BypassLTM[2]);
        ALOGD("SWHDR Gain Diff = %d %d", rOutput.u4FinalGainDiff[0], rOutput.u4FinalGainDiff[1]);
#endif
}

/*******************************************************************************
*
*******************************************************************************/
