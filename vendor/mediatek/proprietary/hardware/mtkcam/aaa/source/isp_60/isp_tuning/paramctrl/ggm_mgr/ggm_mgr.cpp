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
#define LOG_TAG "ggm_mgr"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif


#include "ggm_mgr.h"
#include <private/aaa_hal_private.h>
#include <private/aaa_utils.h>
#include <array>

using namespace NSIspTuning;

//static int maxSlope[4] = {32,16,8,2};
#define MY_INST NS3Av3::INST_T<GgmMgr>
static std::array<MY_INST, SENSOR_IDX_MAX> gMultiton;


namespace NSIspTuning
{
GgmMgr*
GgmMgr::
createInstance(MUINT32 const eSensorDev)
{
    auto i4SensorIdx = NS3Av3::mapSensorDevToIdx(static_cast<MINT32>(eSensorDev));

    if(i4SensorIdx >= SENSOR_IDX_MAX || i4SensorIdx < 0) {
        CAM_LOGE("[%s] Unsupport sensor Index: %d\n", __FUNCTION__, i4SensorIdx);
        return NULL;
    }

    MY_INST& rSingleton = gMultiton[i4SensorIdx];
    std::call_once(rSingleton.onceFlag, [&] {
        rSingleton.instance = std::make_unique<GgmMgr>(eSensorDev);
    } );

    return rSingleton.instance.get();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
getTonemapCurve_Red(MFLOAT*& p_in_red, MFLOAT*& p_out_red, MINT32 *pCurvePointNum)
{
    for (MINT32 i = 0; i < ENC_BUF_SIZE; i++) {
        m_encBuf[i] = m_ggm.lut[i].bits.GGM_R;
    }

    p_in_red = m_pin;
    p_out_red = m_pout;
    *pCurvePointNum = MAX_CURVE_POINT_NUM;

    GMA_to_array(m_encBuf, p_in_red, MAX_CURVE_POINT_NUM, p_out_red);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
getTonemapCurve_Green(MFLOAT*& p_in_green, MFLOAT*& p_out_green, MINT32 *pCurvePointNum)
{
    for (MINT32 i = 0; i < ENC_BUF_SIZE; i++) {
        m_encBuf[i] = m_ggm.lut[i].bits.GGM_G;
    }

    p_in_green = m_pin;
    p_out_green = m_pout;
    *pCurvePointNum = MAX_CURVE_POINT_NUM;

    GMA_to_array(m_encBuf, p_in_green, MAX_CURVE_POINT_NUM, p_out_green);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
getTonemapCurve_Blue(MFLOAT*& p_in_blue, MFLOAT*& p_out_blue, MINT32 *pCurvePointNum)
{
    for (MINT32 i = 0; i < ENC_BUF_SIZE; i++) {
        m_encBuf[i] = m_ggm.lut[i].bits.GGM_B;
    }

    p_in_blue = m_pin;
    p_out_blue = m_pout;
    *pCurvePointNum = MAX_CURVE_POINT_NUM;

    GMA_to_array(m_encBuf, p_in_blue, MAX_CURVE_POINT_NUM, p_out_blue);

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
setTonemapCurve_Red(MFLOAT *p_in_red, MFLOAT *p_out_red, MINT32 *pCurvePointNum)
{
    array_to_GMA(p_in_red, p_out_red, *pCurvePointNum, m_encBuf);

    for (MINT32 i = 0; i < ENC_BUF_SIZE; i++) {
        m_ggm.lut[i].bits.GGM_R = m_encBuf[i];
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
setTonemapCurve_Green(MFLOAT *p_in_green, MFLOAT *p_out_green, MINT32 *pCurvePointNum)
{
    array_to_GMA(p_in_green, p_out_green, *pCurvePointNum, m_encBuf);

    for (MINT32 i = 0; i < ENC_BUF_SIZE; i++) {
        m_ggm.lut[i].bits.GGM_G = m_encBuf[i];
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
setTonemapCurve_Blue(MFLOAT *p_in_blue, MFLOAT *p_out_blue, MINT32 *pCurvePointNum)
{
    array_to_GMA(p_in_blue, p_out_blue, *pCurvePointNum, m_encBuf);

    for (MINT32 i = 0; i < ENC_BUF_SIZE; i++) {
        m_ggm.lut[i].bits.GGM_B = m_encBuf[i];
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
GMA_to_array( MINT32* enc_buff, MFLOAT* request_X, MINT32 numRequest, MFLOAT* mapped_Y)
{
//    #warning "Not a reentrant function, this should be a constant function!"
    //int gamma[ 4096 ];// enc_buff[144];
    MINT32 wk1, wk2, wk3;
    MINT32 p = 0;
    MINT32 xvalue[MAX_CURVE_POINT_NUM];

    for (MINT32 i = 0 ; i < numRequest ; i ++)
        xvalue[i] = (MINT32) (request_X[i]*4095 + 0.5);

    for ( MINT32 j = 0 ; j < 4096 ; j ++ ) {    // Gamma decode

        if ( j <  512 ) {
						wk1 = enc_buff[j / 8] & 0x03ff;
						wk3 = enc_buff[j / 8 + 1] & 0x03ff;
						wk2 = wk3 - wk1;
            m_gamma [ p ++ ] =   wk1 + ( ( wk2 * ( j % 8 ) + 4 )  >> 3 ) ;
        }
        else if ( j < 1024 ) {
						wk1 = enc_buff[64 + (j - 512) / 16] & 0x03ff;
						wk3 = enc_buff[64 + (j - 512) / 16 + 1] & 0x03ff;
						wk2 = wk3 - wk1;
            m_gamma [ p ++ ] =   wk1 + ( ( wk2 * ( (j-512) % 16 ) + 8 )  >> 4 )  ;
        }
        else if ( j < 2048 ) {
						wk1 = enc_buff[96 + (j - 1024) / 32] & 0x03ff;
						wk3 = enc_buff[96 + (j - 1024) / 32 + 1] & 0x03ff;
						wk2 = wk3 - wk1;
            m_gamma [ p ++ ] =   wk1 + ( ( wk2 * ( (j-1024) % 32 ) + 16 )  >> 5 ) ;
        }
        else if ( j < 4064 ) {
						wk1 = enc_buff[128 + (j - 2048) / 32] & 0x03ff;
						wk3 = enc_buff[128 + (j - 2048) / 32 + 1] & 0x03ff;
						wk2 = wk3 - wk1;
            m_gamma [ p ++ ] =   wk1 + ( ( wk2 * ( (j-2048) % 32 ) + 16 )  >> 5 ) ;
        }
        else if ( j < 4096 ) {
						wk1 = enc_buff[128 + (j - 2048) / 32] & 0x03ff;
						wk3 = 1023;
						wk2 = wk3 - wk1;
						m_gamma [ p ++ ] =   wk1 + ( ( wk2 * ( (j-2048) % 32 ) + 16 )  >> 5 ) ;
        }
    }

    for ( int j = 0 ; j < numRequest ; j ++ )
        mapped_Y[j] = (MFLOAT)m_gamma[xvalue[j]]/1023;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
GgmMgr::
array_to_GMA(MFLOAT* in_GMA_X, MFLOAT* in_GMA_Y, MINT32 numPair, MINT32* enc_buff)
{
//    #warning "Not a reentrant function, this should be a constant function!"
    int wk, wk1, wk2;
    //int gamma[ 4096 ], enc_buff[ ENC_BUF_SIZE ];
    double in_GMA_S[maxCurvePoints] = {0};
    int checkZone[maxCurvePoints] = {0};
    bool nonSetEnd = 0;
    bool nonSetStart = 0;

    // check for valid input
    for (int i = 0 ; i < numPair-1 ; i++) {
        if ((in_GMA_X[i+1] <= in_GMA_X[i]) || (in_GMA_Y[i+1] < in_GMA_Y[i])) {
            CAM_LOGE("ERROR: Non-monotonic inputs\n");
            return MFALSE;
        }
    }
    if (in_GMA_X[0]<0 || in_GMA_Y[0]<0 || in_GMA_X[numPair-1]>1 || in_GMA_Y[numPair-1]>1) {
        CAM_LOGE("ERROR: Non-valid range for inputs\n");
        return MFALSE;
    }

    for (int i = 0 ; i < numPair ; i++) {
        in_GMA_X[i] = in_GMA_X[i] * 4095;
        in_GMA_Y[i] = in_GMA_Y[i] * 4095;
        if (i==0) {
            if (in_GMA_X[0]!=0)
                in_GMA_S[0] = in_GMA_Y[0]/in_GMA_X[0];
            else
                in_GMA_S[0] = 0;
        }
        else
            in_GMA_S[i] = (in_GMA_Y[i] - in_GMA_Y[i-1]) / (in_GMA_X[i] - in_GMA_X[i-1]);

        if (in_GMA_X[i]<512)
            checkZone[i] = 0;
        else if (in_GMA_X[i]<1024)
            checkZone[i] = 1;
        else if (in_GMA_X[i]<2048)
            checkZone[i] = 2;
        else
            checkZone[i] = 3;
    }
    if (in_GMA_X[numPair-1]!=4095) {    // not set for end point: assume the end point to 1
        in_GMA_S[numPair] = (4095 - in_GMA_Y[numPair-1]) / (4095 - in_GMA_X[numPair-1]);
        nonSetEnd = 1;
    }

    checkZone[numPair] = 3;

    if (in_GMA_X[0]!=0)
        nonSetStart = 1;

    // Check valid slope for gamma
    for (int i = 0 ; i < numPair ; i++) {
        if (in_GMA_S[i] >= maxSlope[checkZone[i]]) {
            CAM_LOGE("ERROR: the slope is too large for gamma can encoded\n");
            return MFALSE;
        }
    }
    if (nonSetEnd && (in_GMA_S[numPair] >= maxSlope[checkZone[numPair]])) {
        CAM_LOGE("ERROR: the slope is too large for gamma can encoded\n");
        return MFALSE;
    }

#ifndef SPLINE_MODE     // Linear mode

    for (int i = 0 ; i < 4096 ; i++ ) {

        if (i<=in_GMA_X[0]) {
            if (in_GMA_X[0]!=0)
                m_gamma[i] = 0 + (i-0)*in_GMA_S[0];
            else
                m_gamma[i] = in_GMA_Y[0];
        }
        else {
            for (int j = 1 ; j < numPair ; j++) {
                if (i<in_GMA_X[j]) {
                    m_gamma[i] = in_GMA_Y[j-1] + (i-in_GMA_X[j-1])*in_GMA_S[j] + 0.5;
                    break;
                }
                if (nonSetEnd)
                    m_gamma[i] = in_GMA_Y[j] + (i-in_GMA_X[j])*in_GMA_S[j+1] + 0.5;
                else
                    m_gamma[i] = in_GMA_Y[numPair-1];
            }
        }
        m_gamma[i] = (m_gamma[i] / 4.0029) + 0.5;
        // cout<<i<<" : "<<m_gamma[i]<<endl;
    }
#else   // Spline mode

    // add start point & end point in data point array
    if (nonSetStart) {
        inData_X[0] = 0;
        inData_Y[0] = 0;
        for (int i = 1 ; i < numPair+1 ; i++) {
            inData_X[i] = in_GMA_X[i-1];
            inData_Y[i] = in_GMA_Y[i-1];
        }
        numPair = numPair + 1;
    }
    else {
        for (int i = 0 ; i < numPair ; i++) {
            inData_X[i] = in_GMA_X[i];
            inData_Y[i] = in_GMA_Y[i];
        }
    }
    if (nonSetEnd) {
        inData_X[numPair+1] = 4095;
        inData_Y[numPair+1] = 4095;
        numPair = numPair + 1;
    }

    int spt_Length;

    int check = getSpinePoint(inData_X, inData_Y, numPair, spt_X, spt_Y, spt_Length);

    // Fore monotonic interpolation check.
    // If yes, continue cubic spline interpolation.
    // If no, linear interpolation
    if (check!=1) {

        // Due to sp.SplinePoint may contain duplicated X points or missed X points,
        // need to remove/add those points back to make sure spt has every X point once.

        if (inData_X[0]==0) {
            sequential_spt_X[0] = inData_X[0];
            sequential_spt_Y[0] = inData_Y[0];
        }
        else {
            sequential_spt_X[0] = 0;
            sequential_spt_Y[0] = 0;
        }
        m_gamma[0] = (sequential_spt_Y[0] / 4.0029) + 0.5;

        int k = 0;  //  index of spt
        for(int i = 1; i < 4096; i++){    //    i: index of sequential_spt
            //sequential_spt[i]
            for(int j = k; j < spt_Length; j++){
                if(spt_X[k] < i){                        // duplicated points with the same X
                    k++;
                }
                else if((int)(spt_X[k] + 0.5) == i){     // exactly the X
                    sequential_spt_X[i] = (int) spt_X[k] + 0.5;
                    sequential_spt_Y[i] = spt_Y[k];
                    k++;
                    break;
                }
                else{                                    // the X is missing, need to be interpolated
                    sequential_spt_X[i] = i;
                    if(k != 0){
                        sequential_spt_Y[i] = (int)(((spt_Y[k - 1] * (spt_X[k] - i) + spt_Y[k] * (i - spt_X[k - 1])) / (spt_X[k] - spt_X[k - 1])) + 0.5);
                    }
                    else{
                        sequential_spt_Y[i] = (int)(((spt_Y[k] * i) / spt_X[k]) + 0.5);
                    }
                    break;
                }
            }
            if(k == spt_Length){
                sequential_spt_X[i] = i;
                sequential_spt_Y[i] = (int)(((spt_Y[k - 1] * (4095 - i) + 4095 * (i - spt_X[k - 1])) / (4095 - spt_X[k - 1])) + 0.5);
            }

            m_gamma[i] = (sequential_spt_Y[i] / 4.0029) + 0.5;
        }

    }
    else {
        CAM_LOGD("Not pass monotonic test : Go to linear mode\n");
        // Linear mode
        for (int i = 0 ; i < 4096 ; i++ ) {

            if (i<=in_GMA_X[0]) {
                if (in_GMA_X[0]!=0)
                    m_gamma[i] = 0 + (i-0)*in_GMA_S[0];
                else
                    m_gamma[i] = in_GMA_Y[0];
            }
            else {
                for (int j = 1 ; j < numPair ; j++) {
                    if (i<in_GMA_X[j]) {
                        m_gamma[i] = in_GMA_Y[j-1] + (i-in_GMA_X[j-1])*in_GMA_S[j] + 0.5;
                        break;
                    }
                    if (nonSetEnd)
                        m_gamma[i] = in_GMA_Y[j] + (i-in_GMA_X[j])*in_GMA_S[j+1] + 0.5;
                    else
                        m_gamma[i] = in_GMA_Y[numPair-1];
                }
            }
            m_gamma[i] = (m_gamma[i] / 4.0029) + 0.5;
            // cout<<i<<" : "<<m_gamma[i]<<endl;
        }
    }

#endif

    // Gamma encode
    for ( int j = 0 ; j < 4096 ; j++ ) {
        if ( j <  512 ) {
            if (j % 8 == 0) {
                wk1  =  m_gamma[j];
                enc_buff[ j / 8 ] = wk1;                // 10-bit
            }
        }
        else if ( j <  1024 ) {
            if (j % 16 == 0) {
                wk1  =  m_gamma[j];
                enc_buff[ j / 16 + 32] = wk1;           // 10-bit
            }
        }
        else if ( j <  4096 ) {
            if (j % 32 == 0) {
                wk1  =  m_gamma[j];
                enc_buff[ j / 32 + 64] = wk1;           // 10-bit
            }
        }
#if 0
        else if ( j <  4096 ) {
            if (j % 32 == 0) {
                wk1  =  m_gamma[j];
                enc_buff[ j / 128 + 112] = wk1;         // 10-bit
            }

        }
#endif
        //cout<<j<<" "<<m_gamma[j]<<endl;
    }

    // Save gamma table (89 format) in enc_buff

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper functions
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL GgmMgr::getControlPoints(double* dataPoint_X, double* dataPoint_Y, int dataPoint_Length, double* controlPoint_X, double* controlPoint_Y, int &controlPoint_Length)
{
    if(dataPoint_Length == 3) {
        controlPoint_X[0] = dataPoint_X[0];
        controlPoint_X[1] = (6 * dataPoint_X[1] - dataPoint_X[0] - dataPoint_X[2]) / 4;
        controlPoint_X[2] = dataPoint_X[2];
        controlPoint_Y[0] = dataPoint_Y[0];
        controlPoint_Y[1] = (6 * dataPoint_Y[1] - dataPoint_Y[0] - dataPoint_Y[2]) / 4;
        controlPoint_Y[2] = dataPoint_Y[2];
        controlPoint_Length = 3;
    }

    if(dataPoint_Length > 3) {
        int n = dataPoint_Length;
        double diag[maxCurvePoints];
        double sub[maxCurvePoints];
        double sup[maxCurvePoints];

        for(int i = 0; i < n; i++) {
            controlPoint_X[i] = dataPoint_X[i];
            controlPoint_Y[i] = dataPoint_Y[i];
            diag[i]         = 4;
            sub[i]          = 1;
            sup[i]          = 1;
        }

        controlPoint_X[1]   = 6 * controlPoint_X[1] - controlPoint_X[0];
        controlPoint_X[n - 2] = 6 * controlPoint_X[n - 2] - controlPoint_X[n - 1];
        controlPoint_Y[1]   = 6 * controlPoint_Y[1] - controlPoint_Y[0];
        controlPoint_Y[n - 2] = 6 * controlPoint_Y[n - 2] - controlPoint_Y[n - 1];

        for(int i = 2; i < n - 2; i++){
            controlPoint_X[i] = 6 * controlPoint_X[i];
            controlPoint_Y[i] = 6 * controlPoint_Y[i];
        }

        // Gaussian elimination front row 1 to n-2
        for(int i = 2; i < n - 1; i++){
            sub[i]          = sub[i] / diag[i - 1];
            diag[i]         = diag[i] - sub[i] * sup[i - 1];
            controlPoint_X[i] = controlPoint_X[i] - sub[i] * controlPoint_X[i - 1];
            controlPoint_Y[i] = controlPoint_Y[i] - sub[i] * controlPoint_Y[i - 1];
        }

        controlPoint_X[n - 2] = controlPoint_X[n - 2] / diag[n - 2];
        controlPoint_Y[n - 2] = controlPoint_Y[n - 2] / diag[n - 2];

        for(int i = n - 3; i > 0; i--){
            controlPoint_X[i] = (controlPoint_X[i] - sup[i] * controlPoint_X[i + 1]) / diag[i];
            controlPoint_Y[i] = (controlPoint_Y[i] - sup[i] * controlPoint_Y[i + 1]) / diag[i];
        }
        controlPoint_Length = n;
    }

    return MTRUE;
}

MBOOL GgmMgr::getSpinePoint(double* dataPoint_X, double* dataPoint_Y, int dataPoint_Length, double* splinePoint_X, double* splinePoint_Y, int& spt_Length)
{
    double controlPoint_X[maxCurvePoints] = {0.0}, controlPoint_Y[maxCurvePoints] = {0.0};
    int controlPoint_Length = 0;

    if(dataPoint_Length == 1) {
        for(int j = 0; j < maxCurvePoints; j++) {
            splinePoint_X[j] = dataPoint_X[0];
            splinePoint_Y[j] = dataPoint_Y[0];
        }
    }

    if(dataPoint_Length == 2) {
        int n = 1;
        if(isXcalibrated)
            n = (int)((dataPoint_X[1] - dataPoint_X[0]) / precision);
        else n = (int)((dataPoint_Y[1] - dataPoint_Y[0]) / precision);
        if(n == 0) n = 1;
        if(n < 0) n = -n;
        for(int j = 0; j < n; j++) {
            double t = (double)j / (double)n;

            splinePoint_X[j] = ((1 - t) * dataPoint_X[0] + t * dataPoint_X[1]);
            splinePoint_Y[j] = ((1 - t) * dataPoint_Y[0] + t * dataPoint_Y[1]);
        }
    }

    if(dataPoint_Length > 2) {
        getControlPoints(dataPoint_X, dataPoint_Y, dataPoint_Length, controlPoint_X, controlPoint_Y, controlPoint_Length);

/*      for (int i = 0 ; i < controlPoint_Length ; i++)                 // tmp
            cout << controlPoint_X[i] << endl;                          // tmp
        for (int i = 0 ; i < controlPoint_Length ; i++)                 // tmp
            cout << controlPoint_Y[i]/4.0029 <<endl;                    // tmp
*/
        // Monotonic test
        for(int i = 0 ; i < controlPoint_Length-1 ; i++){
            if ((controlPoint_Y[i+1]-controlPoint_Y[i]) <= -200) {
                CAM_LOGE("Not pass monotonic test!\n");
                return MFALSE;
            }
        }
        // First spline test (To avoid large gap with first point)
        if (controlPoint_X[1] < (controlPoint_Y[1]/24))
            controlPoint_X[1] = controlPoint_Y[1]/24;

        int count = 0;
        //draw bezier curves using Bernstein Polynomials
        for(int i = 0; i < controlPoint_Length - 1; i++){
            double b1_X = controlPoint_X[i] * 2.0 / 3.0 + controlPoint_X[i + 1] / 3.0;
            double b1_Y = controlPoint_Y[i] * 2.0 / 3.0 + controlPoint_Y[i + 1] / 3.0;
            double b2_X = controlPoint_X[i] / 3.0 + controlPoint_X[i + 1] * 2.0 / 3.0;
            double b2_Y = controlPoint_Y[i] / 3.0 + controlPoint_Y[i + 1] * 2.0 / 3.0;

            int n = 1;
            if(isXcalibrated)
                n = (int)((dataPoint_X[i + 1] - dataPoint_X[i]) / precision);
            else n = (int)((dataPoint_Y[i + 1] - dataPoint_Y[i]) / precision);
            if(n == 0) n = 1;
            if(n < 0) n = -n;
            for(int j = 0; j < n; j++){
                double t   = (double)j / (double)n;
                double v_X = (1 - t) * (1 - t) * (1 - t) * dataPoint_X[i] + 3 * (1 - t) * (1 - t) * t * b1_X + 3 * (1 - t) * t * t * b2_X + t * t * t * dataPoint_X[i + 1];
                double v_Y = (1 - t) * (1 - t) * (1 - t) * dataPoint_Y[i] + 3 * (1 - t) * (1 - t) * t * b1_Y + 3 * (1 - t) * t * t * b2_Y + t * t * t * dataPoint_Y[i + 1];
                splinePoint_X[count] = v_X;
                splinePoint_Y[count] = v_Y;
                count++;
            }
        }
        spt_Length = count;
    }

    return MTRUE;
}

MBOOL GgmMgr::convertPtPairsToGMA(const MFLOAT* inPtPairs, MUINT32 u4NumOfPts, MINT32* outGMA)
{
    MBOOL ret = MFALSE;

    if (u4NumOfPts > MAX_CURVE_POINT_NUM || outGMA == NULL || inPtPairs == NULL)
    {
        CAM_LOGE("#Pts(%d), inPtPairs(%p), outGMA(%p)", u4NumOfPts, inPtPairs, outGMA);
        return MFALSE;
    }

    MFLOAT* pArrayIn  = new MFLOAT[u4NumOfPts]{0};
    MFLOAT* pArrayOut = new MFLOAT[u4NumOfPts]{0};
    const MFLOAT* pCurve = inPtPairs;

    for (MUINT32 i = 0; i < u4NumOfPts; i ++)
    {
        MFLOAT x, y;

        x = *pCurve++;
        y = *pCurve++;

        pArrayIn[i] = x;
        pArrayOut[i] = y;
    }

    if(u4NumOfPts < maxCurvePoints){
        ret = array_to_GMA(pArrayIn, pArrayOut, u4NumOfPts, outGMA);
    }

    delete [] pArrayIn;
    delete [] pArrayOut;

    return ret;
}
};