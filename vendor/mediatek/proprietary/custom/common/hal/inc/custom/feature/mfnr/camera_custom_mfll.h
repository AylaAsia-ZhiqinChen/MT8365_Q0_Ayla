/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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
#ifndef _CAMERA_CUSTOM_MFLL_H_
#define _CAMERA_CUSTOM_MFLL_H_

/**************************************************************************
 *                      D E F I N E S / M A C R O S                       *
 **************************************************************************/
// For MFLL Customer Parameters
//
// [SW Workaround Solution]
//     - When CUST_MFLL_ENABLE_SWWKN_FIX_MIX==0,
//          Use software Workaround for hardware MIX3.
//          Some hardware "MIXER3" may have hardware limitation, we have to perform
//          this SW Workaround solution but performance may be slow down.
//          (12M Pixel costs 70~80 ms)
//     - When CUST_MFLL_ENABLE_SWWKN_FIX_MIX==1,
//          Do not use software Workaround for hardware MIX3.
//          If the software Workaround is disabled, the image quality may have
//          a little problem. However, MIX3 problem is not in all chips.
#define CUST_MFLL_ENABLE_SWWKN_FIX_MIX  0

//
// [Content-Aware AIS]
//     CUST_MFLL_ENABLE_CONTENT_AWARE_AIS is a bitwise flag for enable/disable
//     content-aware AIS by sensor index. (still experimental, not suggest to enable)
//     - When the i-th bits of CUST_MFLL_ENABLE_CONTENT_AWARE_AIS==0,
//          Disable content-aware AIS at the i-th sensor,
//
//     - When the i-th bits of CUST_MFLL_ENABLE_CONTENT_AWARE_AIS==1,
//          Enable content-aware AIS at the i-th sensor.
#define CUST_MFLL_ENABLE_CONTENT_AWARE_AIS 0

// [Best Shot Selection (BSS)]
//     - When CUST_MFLL_ENABLE_BSS_FOR_MFLL==0,
//          Use the first image as base image for blending.
//          The advantage is shutter lag has been minimized.
//          The drawback is final image has higher motion blur (cause by handshack)possibility .
//     - When CUST_MFLL_ENABLE_BSS_FOR_MFLL==1 (recommended),
//          Use the image with the highest sharpness as base image.
//          The advantage is the motion blur (cause by handshack) has been minimized.
//          The drawback is a longer shutter lag (average ~+100ms)
#define CUST_MFLL_ENABLE_BSS_FOR_MFLL   1

//     - When CUST_MFLL_ENABLE_BSS_FOR_AIS==0,
//          Use the first image as base image for blending.
//          The advantage is shutter lag has been minimized.
//          The drawback is final image has higher motion blur (cause by handshack)possibility .
//     - When CUST_MFLL_ENABLE_BSS_FOR_AIS==1 (recommended),
//          Use the image with the highest sharpness as base image.
//          The advantage is the motion blur (cause by handshack) has been minimized.
//          The drawback is a longer shutter lag (average ~+100ms)
#define CUST_MFLL_ENABLE_BSS_FOR_AIS    1

//     - how many rows are skipped during processing
//          recommand range: >=8
//          recommand value: 8
//          larger scale factor cause less accurate but faster execution time.
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_SCALE_FACTOR      8
#define CUST_MFLL_BSS_SCALE_FACTOR_00   8
#define CUST_MFLL_BSS_SCALE_FACTOR_01   8
#define CUST_MFLL_BSS_SCALE_FACTOR_02   8
#define CUST_MFLL_BSS_SCALE_FACTOR_03   8

//     - minimum edge response
//          recommand range: 10~40
//          recommand value: 20
//          larger th0 cause better noise resistence but may miss real edges.
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_CLIP_TH0          10
#define CUST_MFLL_BSS_CLIP_TH0_00       10
#define CUST_MFLL_BSS_CLIP_TH0_01       10
#define CUST_MFLL_BSS_CLIP_TH0_02       10
#define CUST_MFLL_BSS_CLIP_TH0_03       10

//     - maximum edge response
//          recommand range: 50~120
//          recommand value: 100
//          larger th1 will suppress less high contrast edges
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_CLIP_TH1          20
#define CUST_MFLL_BSS_CLIP_TH1_00       20
#define CUST_MFLL_BSS_CLIP_TH1_01       20
#define CUST_MFLL_BSS_CLIP_TH1_02       20
#define CUST_MFLL_BSS_CLIP_TH1_03       20

//     - minimum edge response
//          recommand range: 10~40
//          recommand value: 20
//          larger th0 cause better noise resistence but may miss real edges.
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_CLIP_TH2          10
#define CUST_MFLL_BSS_CLIP_TH2_00       10
#define CUST_MFLL_BSS_CLIP_TH2_01       10
#define CUST_MFLL_BSS_CLIP_TH2_02       10
#define CUST_MFLL_BSS_CLIP_TH2_03       10

//     - maximum edge response
//          recommand range: 50~120
//          recommand value: 100
//          larger th1 will suppress less high contrast edges
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_CLIP_TH3          20
#define CUST_MFLL_BSS_CLIP_TH3_00       20
#define CUST_MFLL_BSS_CLIP_TH3_01       20
#define CUST_MFLL_BSS_CLIP_TH3_02       20
#define CUST_MFLL_BSS_CLIP_TH3_03       20

//     - tri-pod/static scene detection
//          recommand range: 0~10
//          recommand value: 10
//          larger zero cause more scene will be considered as static
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_ZERO              12
#define CUST_MFLL_BSS_ZERO_00           12
#define CUST_MFLL_BSS_ZERO_01           12
#define CUST_MFLL_BSS_ZERO_02           12
#define CUST_MFLL_BSS_ZERO_03           12

//     - brightness difference threshold
//          recommand range: 0~31
//          recommand value: 12
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_ADF_TH            18
#define CUST_MFLL_BSS_ADF_TH_00         18
#define CUST_MFLL_BSS_ADF_TH_01         18
#define CUST_MFLL_BSS_ADF_TH_02         18
#define CUST_MFLL_BSS_ADF_TH_03         18

//     - sharpness difference threshold
//          recommand range: 0~90
//          recommand value: 70
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_SDF_TH            80
#define CUST_MFLL_BSS_SDF_TH_00         80
#define CUST_MFLL_BSS_SDF_TH_01         80
#define CUST_MFLL_BSS_SDF_TH_02         80
#define CUST_MFLL_BSS_SDF_TH_03         80

//     - minimum gain difference threshold
//          recommand range:
//          recommand value: 853
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_GAIN_TH0          853
#define CUST_MFLL_BSS_GAIN_TH0_00       853
#define CUST_MFLL_BSS_GAIN_TH0_01       853
#define CUST_MFLL_BSS_GAIN_TH0_02       853
#define CUST_MFLL_BSS_GAIN_TH0_03       853

//     - maximum gain difference threshold
//          recommand range:
//          recommand value: 1229
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_GAIN_TH1          1229
#define CUST_MFLL_BSS_GAIN_TH1_00       1229
#define CUST_MFLL_BSS_GAIN_TH1_01       1229
#define CUST_MFLL_BSS_GAIN_TH1_02       1229
#define CUST_MFLL_BSS_GAIN_TH1_03       1229

//     - minimum isp gain
//          recommand range:
//          recommand value: 1024
//     - CUST_MFLL_BSS_xxx + (_00, _01, _02, _03) for each sensor id (0, 1, 2, 3).
#define CUST_MFLL_BSS_MIN_ISP_GAIN_     546
#define CUST_MFLL_BSS_MIN_ISP_GAIN_00   546
#define CUST_MFLL_BSS_MIN_ISP_GAIN_01   546
#define CUST_MFLL_BSS_MIN_ISP_GAIN_02   546
#define CUST_MFLL_BSS_MIN_ISP_GAIN_03   546

#define CUST_MFLL_BSS_YPF_EN            1
#define CUST_MFLL_BSS_YPF_EN_00         1
#define CUST_MFLL_BSS_YPF_EN_01         1
#define CUST_MFLL_BSS_YPF_EN_02         1
#define CUST_MFLL_BSS_YPF_EN_03         1

#define CUST_MFLL_BSS_YPF_FAC           50
#define CUST_MFLL_BSS_YPF_FAC_00        50
#define CUST_MFLL_BSS_YPF_FAC_01        50
#define CUST_MFLL_BSS_YPF_FAC_02        50
#define CUST_MFLL_BSS_YPF_FAC_03        50

#define CUST_MFLL_BSS_YPF_ADJTH         12
#define CUST_MFLL_BSS_YPF_ADJTH_00      12
#define CUST_MFLL_BSS_YPF_ADJTH_01      12
#define CUST_MFLL_BSS_YPF_ADJTH_02      12
#define CUST_MFLL_BSS_YPF_ADJTH_03      12

#define CUST_MFLL_BSS_YPF_DFMED0        20
#define CUST_MFLL_BSS_YPF_DFMED0_00     20
#define CUST_MFLL_BSS_YPF_DFMED0_01     20
#define CUST_MFLL_BSS_YPF_DFMED0_02     20
#define CUST_MFLL_BSS_YPF_DFMED0_03     20

#define CUST_MFLL_BSS_YPF_DFMED1        32
#define CUST_MFLL_BSS_YPF_DFMED1_00     32
#define CUST_MFLL_BSS_YPF_DFMED1_01     32
#define CUST_MFLL_BSS_YPF_DFMED1_02     32
#define CUST_MFLL_BSS_YPF_DFMED1_03     32

#define CUST_MFLL_BSS_YPF_TH0           102
#define CUST_MFLL_BSS_YPF_TH0_00        102
#define CUST_MFLL_BSS_YPF_TH0_01        102
#define CUST_MFLL_BSS_YPF_TH0_02        102
#define CUST_MFLL_BSS_YPF_TH0_03        102

#define CUST_MFLL_BSS_YPF_TH1           104
#define CUST_MFLL_BSS_YPF_TH1_00        104
#define CUST_MFLL_BSS_YPF_TH1_01        104
#define CUST_MFLL_BSS_YPF_TH1_02        104
#define CUST_MFLL_BSS_YPF_TH1_03        104

#define CUST_MFLL_BSS_YPF_TH2           98
#define CUST_MFLL_BSS_YPF_TH2_00        98
#define CUST_MFLL_BSS_YPF_TH2_01        98
#define CUST_MFLL_BSS_YPF_TH2_02        98
#define CUST_MFLL_BSS_YPF_TH2_03        98

#define CUST_MFLL_BSS_YPF_TH3           96
#define CUST_MFLL_BSS_YPF_TH3_00        96
#define CUST_MFLL_BSS_YPF_TH3_01        96
#define CUST_MFLL_BSS_YPF_TH3_02        96
#define CUST_MFLL_BSS_YPF_TH3_03        96

#define CUST_MFLL_BSS_YPF_TH4           96
#define CUST_MFLL_BSS_YPF_TH4_00        96
#define CUST_MFLL_BSS_YPF_TH4_01        96
#define CUST_MFLL_BSS_YPF_TH4_02        96
#define CUST_MFLL_BSS_YPF_TH4_03        96

#define CUST_MFLL_BSS_YPF_TH5           96
#define CUST_MFLL_BSS_YPF_TH5_00        96
#define CUST_MFLL_BSS_YPF_TH5_01        96
#define CUST_MFLL_BSS_YPF_TH5_02        96
#define CUST_MFLL_BSS_YPF_TH5_03        96

#define CUST_MFLL_BSS_YPF_TH6           96
#define CUST_MFLL_BSS_YPF_TH6_00        96
#define CUST_MFLL_BSS_YPF_TH6_01        96
#define CUST_MFLL_BSS_YPF_TH6_02        96
#define CUST_MFLL_BSS_YPF_TH6_03        96

#define CUST_MFLL_BSS_YPF_TH7           96
#define CUST_MFLL_BSS_YPF_TH7_00        96
#define CUST_MFLL_BSS_YPF_TH7_01        96
#define CUST_MFLL_BSS_YPF_TH7_02        96
#define CUST_MFLL_BSS_YPF_TH7_03        96

#define CUST_MFLL_BSS_FD_EN             1
#define CUST_MFLL_BSS_FD_EN_00          1
#define CUST_MFLL_BSS_FD_EN_01          1
#define CUST_MFLL_BSS_FD_EN_02          1
#define CUST_MFLL_BSS_FD_EN_03          1

#define CUST_MFLL_BSS_FD_FAC            2
#define CUST_MFLL_BSS_FD_FAC_00         2
#define CUST_MFLL_BSS_FD_FAC_01         2
#define CUST_MFLL_BSS_FD_FAC_02         2
#define CUST_MFLL_BSS_FD_FAC_03         2

#define CUST_MFLL_BSS_FD_FNUM           1
#define CUST_MFLL_BSS_FD_FNUM_00        1
#define CUST_MFLL_BSS_FD_FNUM_01        1
#define CUST_MFLL_BSS_FD_FNUM_02        1
#define CUST_MFLL_BSS_FD_FNUM_03        1

#define CUST_MFLL_BSS_EYE_EN            1
#define CUST_MFLL_BSS_EYE_EN_00         1
#define CUST_MFLL_BSS_EYE_EN_01         1
#define CUST_MFLL_BSS_EYE_EN_02         1
#define CUST_MFLL_BSS_EYE_EN_03         1

#define CUST_MFLL_BSS_EYE_CFTH          60
#define CUST_MFLL_BSS_EYE_CFTH_00       60
#define CUST_MFLL_BSS_EYE_CFTH_01       60
#define CUST_MFLL_BSS_EYE_CFTH_02       60
#define CUST_MFLL_BSS_EYE_CFTH_03       60

#define CUST_MFLL_BSS_EYE_RATIO0        60
#define CUST_MFLL_BSS_EYE_RATIO0_00     60
#define CUST_MFLL_BSS_EYE_RATIO0_01     60
#define CUST_MFLL_BSS_EYE_RATIO0_02     60
#define CUST_MFLL_BSS_EYE_RATIO0_03     60

#define CUST_MFLL_BSS_EYE_RATIO1        40
#define CUST_MFLL_BSS_EYE_RATIO1_00     40
#define CUST_MFLL_BSS_EYE_RATIO1_01     40
#define CUST_MFLL_BSS_EYE_RATIO1_02     40
#define CUST_MFLL_BSS_EYE_RATIO1_03     40

#define CUST_MFLL_BSS_EYE_FAC           50
#define CUST_MFLL_BSS_EYE_FAC_00        50
#define CUST_MFLL_BSS_EYE_FAC_01        50
#define CUST_MFLL_BSS_EYE_FAC_02        50
#define CUST_MFLL_BSS_EYE_FAC_03        50

#define CUST_MFLL_BSS_AEVC_EN           0
#define CUST_MFLL_BSS_AEVC_EN_00        0
#define CUST_MFLL_BSS_AEVC_EN_01        0
#define CUST_MFLL_BSS_AEVC_EN_02        0
#define CUST_MFLL_BSS_AEVC_EN_03        0

#define CUST_MFLL_BSS_AEVC_DCNT         512
#define CUST_MFLL_BSS_AEVC_DCNT_00      512
#define CUST_MFLL_BSS_AEVC_DCNT_01      512
#define CUST_MFLL_BSS_AEVC_DCNT_02      512
#define CUST_MFLL_BSS_AEVC_DCNT_03      512

 // BSS 3.0
#define CUST_MFLL_BSS_FaceCVTh          50
#define CUST_MFLL_BSS_FaceCVTh_00       50
#define CUST_MFLL_BSS_FaceCVTh_01       50
#define CUST_MFLL_BSS_FaceCVTh_02       50
#define CUST_MFLL_BSS_FaceCVTh_03       50

#define CUST_MFLL_BSS_GradThL           4
#define CUST_MFLL_BSS_GradThL_00        4
#define CUST_MFLL_BSS_GradThL_01        4
#define CUST_MFLL_BSS_GradThL_02        4
#define CUST_MFLL_BSS_GradThL_03        4

#define CUST_MFLL_BSS_GradThH           58
#define CUST_MFLL_BSS_GradThH_00        58
#define CUST_MFLL_BSS_GradThH_01        58
#define CUST_MFLL_BSS_GradThH_02        58
#define CUST_MFLL_BSS_GradThH_03        58

#define CUST_MFLL_BSS_FaceAreaThL00     256
#define CUST_MFLL_BSS_FaceAreaThL00_00  256
#define CUST_MFLL_BSS_FaceAreaThL00_01  256
#define CUST_MFLL_BSS_FaceAreaThL00_02  256
#define CUST_MFLL_BSS_FaceAreaThL00_03  256

#define CUST_MFLL_BSS_FaceAreaThL01     256
#define CUST_MFLL_BSS_FaceAreaThL01_00  256
#define CUST_MFLL_BSS_FaceAreaThL01_01  256
#define CUST_MFLL_BSS_FaceAreaThL01_02  256
#define CUST_MFLL_BSS_FaceAreaThL01_03  256

#define CUST_MFLL_BSS_FaceAreaThH00     16384
#define CUST_MFLL_BSS_FaceAreaThH00_00  16384
#define CUST_MFLL_BSS_FaceAreaThH00_01  16384
#define CUST_MFLL_BSS_FaceAreaThH00_02  16384
#define CUST_MFLL_BSS_FaceAreaThH00_03  16384

#define CUST_MFLL_BSS_FaceAreaThH01     32768
#define CUST_MFLL_BSS_FaceAreaThH01_00  32768
#define CUST_MFLL_BSS_FaceAreaThH01_01  32768
#define CUST_MFLL_BSS_FaceAreaThH01_02  32768
#define CUST_MFLL_BSS_FaceAreaThH01_03  32768

#define CUST_MFLL_BSS_APLDeltaTh00      70
#define CUST_MFLL_BSS_APLDeltaTh00_00   70
#define CUST_MFLL_BSS_APLDeltaTh00_01   70
#define CUST_MFLL_BSS_APLDeltaTh00_02   70
#define CUST_MFLL_BSS_APLDeltaTh00_03   70

#define CUST_MFLL_BSS_APLDeltaTh01      75
#define CUST_MFLL_BSS_APLDeltaTh01_00   75
#define CUST_MFLL_BSS_APLDeltaTh01_01   75
#define CUST_MFLL_BSS_APLDeltaTh01_02   75
#define CUST_MFLL_BSS_APLDeltaTh01_03   75

#define CUST_MFLL_BSS_APLDeltaTh02      80
#define CUST_MFLL_BSS_APLDeltaTh02_00   80
#define CUST_MFLL_BSS_APLDeltaTh02_01   80
#define CUST_MFLL_BSS_APLDeltaTh02_02   80
#define CUST_MFLL_BSS_APLDeltaTh02_03   80

#define CUST_MFLL_BSS_APLDeltaTh03      85
#define CUST_MFLL_BSS_APLDeltaTh03_00   85
#define CUST_MFLL_BSS_APLDeltaTh03_01   85
#define CUST_MFLL_BSS_APLDeltaTh03_02   85
#define CUST_MFLL_BSS_APLDeltaTh03_03   85

#define CUST_MFLL_BSS_APLDeltaTh04      90
#define CUST_MFLL_BSS_APLDeltaTh04_00   90
#define CUST_MFLL_BSS_APLDeltaTh04_01   90
#define CUST_MFLL_BSS_APLDeltaTh04_02   90
#define CUST_MFLL_BSS_APLDeltaTh04_03   90

#define CUST_MFLL_BSS_APLDeltaTh05      95
#define CUST_MFLL_BSS_APLDeltaTh05_00   95
#define CUST_MFLL_BSS_APLDeltaTh05_01   95
#define CUST_MFLL_BSS_APLDeltaTh05_02   95
#define CUST_MFLL_BSS_APLDeltaTh05_03   95

#define CUST_MFLL_BSS_APLDeltaTh06      100
#define CUST_MFLL_BSS_APLDeltaTh06_00   100
#define CUST_MFLL_BSS_APLDeltaTh06_01   100
#define CUST_MFLL_BSS_APLDeltaTh06_02   100
#define CUST_MFLL_BSS_APLDeltaTh06_03   100

#define CUST_MFLL_BSS_APLDeltaTh07      105
#define CUST_MFLL_BSS_APLDeltaTh07_00   105
#define CUST_MFLL_BSS_APLDeltaTh07_01   105
#define CUST_MFLL_BSS_APLDeltaTh07_02   105
#define CUST_MFLL_BSS_APLDeltaTh07_03   105

#define CUST_MFLL_BSS_APLDeltaTh08      110
#define CUST_MFLL_BSS_APLDeltaTh08_00   110
#define CUST_MFLL_BSS_APLDeltaTh08_01   110
#define CUST_MFLL_BSS_APLDeltaTh08_02   110
#define CUST_MFLL_BSS_APLDeltaTh08_03   110

#define CUST_MFLL_BSS_APLDeltaTh09      120
#define CUST_MFLL_BSS_APLDeltaTh09_00   120
#define CUST_MFLL_BSS_APLDeltaTh09_01   120
#define CUST_MFLL_BSS_APLDeltaTh09_02   120
#define CUST_MFLL_BSS_APLDeltaTh09_03   120

#define CUST_MFLL_BSS_APLDeltaTh10      130
#define CUST_MFLL_BSS_APLDeltaTh10_00   130
#define CUST_MFLL_BSS_APLDeltaTh10_01   130
#define CUST_MFLL_BSS_APLDeltaTh10_02   130
#define CUST_MFLL_BSS_APLDeltaTh10_03   130

#define CUST_MFLL_BSS_APLDeltaTh11      140
#define CUST_MFLL_BSS_APLDeltaTh11_00   140
#define CUST_MFLL_BSS_APLDeltaTh11_01   140
#define CUST_MFLL_BSS_APLDeltaTh11_02   140
#define CUST_MFLL_BSS_APLDeltaTh11_03   140

#define CUST_MFLL_BSS_APLDeltaTh12      150
#define CUST_MFLL_BSS_APLDeltaTh12_00   150
#define CUST_MFLL_BSS_APLDeltaTh12_01   150
#define CUST_MFLL_BSS_APLDeltaTh12_02   150
#define CUST_MFLL_BSS_APLDeltaTh12_03   150

#define CUST_MFLL_BSS_APLDeltaTh13      160
#define CUST_MFLL_BSS_APLDeltaTh13_00   160
#define CUST_MFLL_BSS_APLDeltaTh13_01   160
#define CUST_MFLL_BSS_APLDeltaTh13_02   160
#define CUST_MFLL_BSS_APLDeltaTh13_03   160

#define CUST_MFLL_BSS_APLDeltaTh14      170
#define CUST_MFLL_BSS_APLDeltaTh14_00   170
#define CUST_MFLL_BSS_APLDeltaTh14_01   170
#define CUST_MFLL_BSS_APLDeltaTh14_02   170
#define CUST_MFLL_BSS_APLDeltaTh14_03   170

#define CUST_MFLL_BSS_APLDeltaTh15      180
#define CUST_MFLL_BSS_APLDeltaTh15_00   180
#define CUST_MFLL_BSS_APLDeltaTh15_01   180
#define CUST_MFLL_BSS_APLDeltaTh15_02   180
#define CUST_MFLL_BSS_APLDeltaTh15_03   180

#define CUST_MFLL_BSS_APLDeltaTh16      190
#define CUST_MFLL_BSS_APLDeltaTh16_00   190
#define CUST_MFLL_BSS_APLDeltaTh16_01   190
#define CUST_MFLL_BSS_APLDeltaTh16_02   190
#define CUST_MFLL_BSS_APLDeltaTh16_03   190

#define CUST_MFLL_BSS_APLDeltaTh17      200
#define CUST_MFLL_BSS_APLDeltaTh17_00   200
#define CUST_MFLL_BSS_APLDeltaTh17_01   200
#define CUST_MFLL_BSS_APLDeltaTh17_02   200
#define CUST_MFLL_BSS_APLDeltaTh17_03   200

#define CUST_MFLL_BSS_APLDeltaTh18      200
#define CUST_MFLL_BSS_APLDeltaTh18_00   200
#define CUST_MFLL_BSS_APLDeltaTh18_01   200
#define CUST_MFLL_BSS_APLDeltaTh18_02   200
#define CUST_MFLL_BSS_APLDeltaTh18_03   200

#define CUST_MFLL_BSS_APLDeltaTh19      200
#define CUST_MFLL_BSS_APLDeltaTh19_00   200
#define CUST_MFLL_BSS_APLDeltaTh19_01   200
#define CUST_MFLL_BSS_APLDeltaTh19_02   200
#define CUST_MFLL_BSS_APLDeltaTh19_03   200

#define CUST_MFLL_BSS_APLDeltaTh20      200
#define CUST_MFLL_BSS_APLDeltaTh20_00   200
#define CUST_MFLL_BSS_APLDeltaTh20_01   200
#define CUST_MFLL_BSS_APLDeltaTh20_02   200
#define CUST_MFLL_BSS_APLDeltaTh20_03   200

#define CUST_MFLL_BSS_APLDeltaTh21      200
#define CUST_MFLL_BSS_APLDeltaTh21_00   200
#define CUST_MFLL_BSS_APLDeltaTh21_01   200
#define CUST_MFLL_BSS_APLDeltaTh21_02   200
#define CUST_MFLL_BSS_APLDeltaTh21_03   200

#define CUST_MFLL_BSS_APLDeltaTh22      200
#define CUST_MFLL_BSS_APLDeltaTh22_00   200
#define CUST_MFLL_BSS_APLDeltaTh22_01   200
#define CUST_MFLL_BSS_APLDeltaTh22_02   200
#define CUST_MFLL_BSS_APLDeltaTh22_03   200

#define CUST_MFLL_BSS_APLDeltaTh23      200
#define CUST_MFLL_BSS_APLDeltaTh23_00   200
#define CUST_MFLL_BSS_APLDeltaTh23_01   200
#define CUST_MFLL_BSS_APLDeltaTh23_02   200
#define CUST_MFLL_BSS_APLDeltaTh23_03   200

#define CUST_MFLL_BSS_APLDeltaTh24      200
#define CUST_MFLL_BSS_APLDeltaTh24_00   200
#define CUST_MFLL_BSS_APLDeltaTh24_01   200
#define CUST_MFLL_BSS_APLDeltaTh24_02   200
#define CUST_MFLL_BSS_APLDeltaTh24_03   200

#define CUST_MFLL_BSS_APLDeltaTh25      200
#define CUST_MFLL_BSS_APLDeltaTh25_00   200
#define CUST_MFLL_BSS_APLDeltaTh25_01   200
#define CUST_MFLL_BSS_APLDeltaTh25_02   200
#define CUST_MFLL_BSS_APLDeltaTh25_03   200

#define CUST_MFLL_BSS_APLDeltaTh26      200
#define CUST_MFLL_BSS_APLDeltaTh26_00   200
#define CUST_MFLL_BSS_APLDeltaTh26_01   200
#define CUST_MFLL_BSS_APLDeltaTh26_02   200
#define CUST_MFLL_BSS_APLDeltaTh26_03   200

#define CUST_MFLL_BSS_APLDeltaTh27      200
#define CUST_MFLL_BSS_APLDeltaTh27_00   200
#define CUST_MFLL_BSS_APLDeltaTh27_01   200
#define CUST_MFLL_BSS_APLDeltaTh27_02   200
#define CUST_MFLL_BSS_APLDeltaTh27_03   200

#define CUST_MFLL_BSS_APLDeltaTh28      200
#define CUST_MFLL_BSS_APLDeltaTh28_00   200
#define CUST_MFLL_BSS_APLDeltaTh28_01   200
#define CUST_MFLL_BSS_APLDeltaTh28_02   200
#define CUST_MFLL_BSS_APLDeltaTh28_03   200

#define CUST_MFLL_BSS_APLDeltaTh29      200
#define CUST_MFLL_BSS_APLDeltaTh29_00   200
#define CUST_MFLL_BSS_APLDeltaTh29_01   200
#define CUST_MFLL_BSS_APLDeltaTh29_02   200
#define CUST_MFLL_BSS_APLDeltaTh29_03   200

#define CUST_MFLL_BSS_APLDeltaTh30      200
#define CUST_MFLL_BSS_APLDeltaTh30_00   200
#define CUST_MFLL_BSS_APLDeltaTh30_01   200
#define CUST_MFLL_BSS_APLDeltaTh30_02   200
#define CUST_MFLL_BSS_APLDeltaTh30_03   200

#define CUST_MFLL_BSS_APLDeltaTh31      200
#define CUST_MFLL_BSS_APLDeltaTh31_00   200
#define CUST_MFLL_BSS_APLDeltaTh31_01   200
#define CUST_MFLL_BSS_APLDeltaTh31_02   200
#define CUST_MFLL_BSS_APLDeltaTh31_03   200

#define CUST_MFLL_BSS_APLDeltaTh32      200
#define CUST_MFLL_BSS_APLDeltaTh32_00   200
#define CUST_MFLL_BSS_APLDeltaTh32_01   200
#define CUST_MFLL_BSS_APLDeltaTh32_02   200
#define CUST_MFLL_BSS_APLDeltaTh32_03   200

#define CUST_MFLL_BSS_GradRatioTh00     1500
#define CUST_MFLL_BSS_GradRatioTh00_00  1500
#define CUST_MFLL_BSS_GradRatioTh00_01  1500
#define CUST_MFLL_BSS_GradRatioTh00_02  1500
#define CUST_MFLL_BSS_GradRatioTh00_03  1500

#define CUST_MFLL_BSS_GradRatioTh01     2000
#define CUST_MFLL_BSS_GradRatioTh01_00  2000
#define CUST_MFLL_BSS_GradRatioTh01_01  2000
#define CUST_MFLL_BSS_GradRatioTh01_02  2000
#define CUST_MFLL_BSS_GradRatioTh01_03  2000

#define CUST_MFLL_BSS_GradRatioTh02     2500
#define CUST_MFLL_BSS_GradRatioTh02_00  2500
#define CUST_MFLL_BSS_GradRatioTh02_01  2500
#define CUST_MFLL_BSS_GradRatioTh02_02  2500
#define CUST_MFLL_BSS_GradRatioTh02_03  2500

#define CUST_MFLL_BSS_GradRatioTh03     3000
#define CUST_MFLL_BSS_GradRatioTh03_00  3000
#define CUST_MFLL_BSS_GradRatioTh03_01  3000
#define CUST_MFLL_BSS_GradRatioTh03_02  3000
#define CUST_MFLL_BSS_GradRatioTh03_03  3000

#define CUST_MFLL_BSS_GradRatioTh04     4000
#define CUST_MFLL_BSS_GradRatioTh04_00  4000
#define CUST_MFLL_BSS_GradRatioTh04_01  4000
#define CUST_MFLL_BSS_GradRatioTh04_02  4000
#define CUST_MFLL_BSS_GradRatioTh04_03  4000

#define CUST_MFLL_BSS_GradRatioTh05     6000
#define CUST_MFLL_BSS_GradRatioTh05_00  6000
#define CUST_MFLL_BSS_GradRatioTh05_01  6000
#define CUST_MFLL_BSS_GradRatioTh05_02  6000
#define CUST_MFLL_BSS_GradRatioTh05_03  6000

#define CUST_MFLL_BSS_GradRatioTh06     8000
#define CUST_MFLL_BSS_GradRatioTh06_00  8000
#define CUST_MFLL_BSS_GradRatioTh06_01  8000
#define CUST_MFLL_BSS_GradRatioTh06_02  8000
#define CUST_MFLL_BSS_GradRatioTh06_03  8000

#define CUST_MFLL_BSS_GradRatioTh07     10000
#define CUST_MFLL_BSS_GradRatioTh07_00  10000
#define CUST_MFLL_BSS_GradRatioTh07_01  10000
#define CUST_MFLL_BSS_GradRatioTh07_02  10000
#define CUST_MFLL_BSS_GradRatioTh07_03  10000

#define CUST_MFLL_BSS_EyeDistThL        7
#define CUST_MFLL_BSS_EyeDistThL_00     7
#define CUST_MFLL_BSS_EyeDistThL_01     7
#define CUST_MFLL_BSS_EyeDistThL_02     7
#define CUST_MFLL_BSS_EyeDistThL_03     7

#define CUST_MFLL_BSS_EyeDistThH        11
#define CUST_MFLL_BSS_EyeDistThH_00     11
#define CUST_MFLL_BSS_EyeDistThH_01     11
#define CUST_MFLL_BSS_EyeDistThH_02     11
#define CUST_MFLL_BSS_EyeDistThH_03     11

#define CUST_MFLL_BSS_EyeMinWeight      9700
#define CUST_MFLL_BSS_EyeMinWeight_00   9700
#define CUST_MFLL_BSS_EyeMinWeight_01   9700
#define CUST_MFLL_BSS_EyeMinWeight_02   9700
#define CUST_MFLL_BSS_EyeMinWeight_03   9700

//     - config MFLL mode when App trigger auto mode
//          recommand range: 0~2 (0:disable, 1:MFNR, 2:AIS)
//          recommand value: 1 or 2
#define CUST_MFLL_AUTO_MODE             1

#endif  // _CAMERA_CUSTOM_MFLL_H_
