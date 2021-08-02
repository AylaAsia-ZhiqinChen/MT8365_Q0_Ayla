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

#ifndef _CAMERA_AE_PLINETABLE_SP2509MIPIRAW_H
#define _CAMERA_AE_PLINETABLE_SP2509MIPIRAW_H

#include "camera_custom_AEPlinetable.h"
static strEvPline sPreviewPLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sPreviewPLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_PreviewAutoTable =
{
    AETABLE_RPEVIEW_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    -10,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sPreviewPLineTable_60Hz,
    sPreviewPLineTable_50Hz,
    NULL,
};

static strEvPline sCapturePLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCapturePLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureAutoTable =
{
    AETABLE_CAPTURE_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCapturePLineTable_60Hz,
    sCapturePLineTable_50Hz,
    NULL,
};

static strEvPline sVideoPLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33185,1140,1048, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.48  BV=3.41
    {33185,1216,1056, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.58  BV=3.30
    {33185,1328,1032, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.67  BV=3.21
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66527,2144,1032, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.36  BV=1.52
    {66527,2240,1064, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.47  BV=1.41
    {66527,2448,1040, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.57  BV=1.31
    {66527,2656,1032, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.67  BV=1.21
    {66527,2864,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.77  BV=1.11
    {66527,2960,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.86  BV=1.02
    {66527,3200,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.98  BV=0.91
    {66527,3456,1048, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.08  BV=0.81
    {66527,3712,1040, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.17  BV=0.71
    {66527,3968,1040, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.26  BV=0.62
    {66527,4352,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.37  BV=0.51
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideoPLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_VideoAutoTable =
{
    AETABLE_VIDEO_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideoPLineTable_60Hz,
    sVideoPLineTable_50Hz,
    NULL,
};

static strEvPline sVideo1PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1424,1104, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {8326,2240,1064, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.47  BV=4.41
    {8326,2448,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.57  BV=4.31
    {8326,2656,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.66  BV=4.22
    {8326,2752,1064, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.77  BV=4.11
    {8326,3072,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.87  BV=4.01
    {8326,3200,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.96  BV=3.91
    {8326,3456,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.06  BV=3.81
    {8326,3712,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.17  BV=3.71
    {8326,3968,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.26  BV=3.62
    {8326,4352,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.37  BV=3.50
    {8326,4608,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.47  BV=3.41
    {8326,4992,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.57  BV=3.31
    {8326,5248,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.67  BV=3.21
    {8326,5632,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.77  BV=3.11
    {8326,6016,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.86  BV=3.02
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideo1PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8325,1140,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.48  BV=5.40
    {8325,1216,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.57  BV=5.31
    {8325,1328,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.66  BV=5.22
    {8325,1424,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.76  BV=5.12
    {8325,1424,1104, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.87  BV=5.01
    {8325,1632,1032, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.97  BV=4.91
    {8325,1728,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.06  BV=4.82
    {8325,1840,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.17  BV=4.71
    {8325,1936,1072, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.27  BV=4.61
    {8325,2144,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.38  BV=4.50
    {8325,2240,1064, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.47  BV=4.41
    {8325,2448,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.57  BV=4.31
    {8325,2656,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.66  BV=4.22
    {8325,2752,1064, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.77  BV=4.11
    {8325,3072,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.87  BV=4.01
    {8325,3200,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.96  BV=3.92
    {8325,3456,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.06  BV=3.82
    {8325,3712,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.17  BV=3.71
    {8325,3968,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.26  BV=3.62
    {8325,4352,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.37  BV=3.50
    {8325,4608,1032, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.47  BV=3.41
    {8325,4992,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.57  BV=3.31
    {8325,5248,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.67  BV=3.21
    {8325,5632,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.77  BV=3.11
    {8325,6016,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.86  BV=3.02
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Video1AutoTable =
{
    AETABLE_VIDEO1_AUTO,    //eAETableID
    80,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    30,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideo1PLineTable_60Hz,
    sVideo1PLineTable_50Hz,
    NULL,
};

static strEvPline sVideo2PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1424,1104, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {8326,2240,1064, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.47  BV=4.41
    {8326,2448,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.57  BV=4.31
    {8326,2656,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.66  BV=4.22
    {8326,2752,1064, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.77  BV=4.11
    {8326,3072,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.87  BV=4.01
    {8326,3200,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.96  BV=3.91
    {8326,3456,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.06  BV=3.81
    {8326,3712,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.17  BV=3.71
    {8326,3968,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.26  BV=3.62
    {8326,4352,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.37  BV=3.50
    {8326,4608,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.47  BV=3.41
    {8326,4992,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.57  BV=3.31
    {8326,5248,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.67  BV=3.21
    {8326,5632,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.77  BV=3.11
    {8326,6016,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.86  BV=3.02
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideo2PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8325,1140,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.48  BV=5.40
    {8325,1216,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.57  BV=5.31
    {8325,1328,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.66  BV=5.22
    {8325,1424,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.76  BV=5.12
    {8325,1424,1104, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.87  BV=5.01
    {8325,1632,1032, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=4.97  BV=4.91
    {8325,1728,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.06  BV=4.82
    {8325,1840,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.17  BV=4.71
    {8325,1936,1072, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.27  BV=4.61
    {8325,2144,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.38  BV=4.50
    {8325,2240,1064, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.47  BV=4.41
    {8325,2448,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.57  BV=4.31
    {8325,2656,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.66  BV=4.22
    {8325,2752,1064, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.77  BV=4.11
    {8325,3072,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.87  BV=4.01
    {8325,3200,1048, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=5.96  BV=3.92
    {8325,3456,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.06  BV=3.82
    {8325,3712,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.17  BV=3.71
    {8325,3968,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.26  BV=3.62
    {8325,4352,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.37  BV=3.50
    {8325,4608,1032, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.47  BV=3.41
    {8325,4992,1024, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.57  BV=3.31
    {8325,5248,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.67  BV=3.21
    {8325,5632,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.77  BV=3.11
    {8325,6016,1040, 0, 0, 0},  //TV = 6.91(210 lines)  AV=2.97  SV=6.86  BV=3.02
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Video2AutoTable =
{
    AETABLE_VIDEO2_AUTO,    //eAETableID
    80,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    30,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideo2PLineTable_60Hz,
    sVideo2PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom1PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom1PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom1AutoTable =
{
    AETABLE_CUSTOM1_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom1PLineTable_60Hz,
    sCustom1PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom2PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom2PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom2AutoTable =
{
    AETABLE_CUSTOM2_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom2PLineTable_60Hz,
    sCustom2PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom3PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom3PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom3AutoTable =
{
    AETABLE_CUSTOM3_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom3PLineTable_60Hz,
    sCustom3PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom4PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom4PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom4AutoTable =
{
    AETABLE_CUSTOM4_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom4PLineTable_60Hz,
    sCustom4PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom5PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom5PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom5AutoTable =
{
    AETABLE_CUSTOM5_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom5PLineTable_60Hz,
    sCustom5PLineTable_50Hz,
    NULL,
};

static strEvPline sVideoNightPLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33185,1140,1048, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.48  BV=3.41
    {33185,1216,1056, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.58  BV=3.30
    {33185,1328,1032, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.67  BV=3.21
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66527,2144,1032, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.36  BV=1.52
    {66527,2240,1064, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.47  BV=1.41
    {66527,2448,1040, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.57  BV=1.31
    {66527,2656,1032, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.67  BV=1.21
    {66527,2864,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.77  BV=1.11
    {66527,2960,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.86  BV=1.02
    {66527,3200,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.98  BV=0.91
    {66527,3456,1048, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.08  BV=0.81
    {66527,3712,1040, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.17  BV=0.71
    {66527,3968,1040, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.26  BV=0.62
    {66527,4352,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=6.37  BV=0.51
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4352,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.37  BV=-0.40
    {141655,4096,1032, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=6.30  BV=-0.51
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {158346,4224,1024, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=6.33  BV=-0.70
    {166672,4224,1048, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.36  BV=-0.81
    {166672,4608,1024, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.46  BV=-0.90
    {166672,4864,1040, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.56  BV=-1.00
    {166672,5248,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.66  BV=-1.10
    {166672,5632,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.76  BV=-1.20
    {166672,6016,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.85  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideoNightPLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {130015,4096,1048, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=6.32  BV=-0.41
    {139997,4096,1040, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=6.31  BV=-0.50
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {160003,4096,1048, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.32  BV=-0.71
    {160003,4480,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.42  BV=-0.80
    {160003,4736,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.52  BV=-0.90
    {160003,5120,1032, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.62  BV=-1.01
    {160003,5504,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.71  BV=-1.10
    {160003,5888,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.81  BV=-1.20
    {160003,6144,1056, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.92  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_VideoNightTable =
{
    AETABLE_VIDEO_NIGHT,    //eAETableID
    123,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -13,    //i4MinBV
    90,    //i4EffectiveMaxBV
    -10,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideoNightPLineTable_60Hz,
    sVideoNightPLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO100PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {58319,1140,1040, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=4.46  BV=2.61
    {58319,1216,1040, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=4.56  BV=2.51
    {66645,1140,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=4.48  BV=2.40
    {66645,1216,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=4.57  BV=2.31
    {75011,1140,1064, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=4.50  BV=2.21
    {83336,1140,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=4.44  BV=2.11
    {83336,1216,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=4.55  BV=2.01
    {91661,1140,1072, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=4.51  BV=1.91
    {99987,1140,1056, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=4.49  BV=1.81
    {108313,1140,1048, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=4.48  BV=1.70
    {116678,1140,1040, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=4.46  BV=1.61
    {125004,1140,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=4.46  BV=1.51
    {133330,1140,1048, 0, 0, 0},  //TV = 2.91(3379 lines)  AV=2.97  SV=4.48  BV=1.40
    {141655,1140,1056, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=4.49  BV=1.30
    {149980,1140,1064, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=4.50  BV=1.21
    {166672,1140,1024, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=4.44  BV=1.11
    {174997,1140,1048, 0, 0, 0},  //TV = 2.51(4435 lines)  AV=2.97  SV=4.48  BV=1.01
    {191648,1140,1024, 0, 0, 0},  //TV = 2.38(4857 lines)  AV=2.97  SV=4.44  BV=0.91
    {199974,1140,1056, 0, 0, 0},  //TV = 2.32(5068 lines)  AV=2.97  SV=4.49  BV=0.81
    {216665,1140,1048, 0, 0, 0},  //TV = 2.21(5491 lines)  AV=2.97  SV=4.48  BV=0.70
    {233316,1140,1040, 0, 0, 0},  //TV = 2.10(5913 lines)  AV=2.97  SV=4.46  BV=0.61
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO100PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.70  BV=2.60
    {60017,1140,1080, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=4.52  BV=2.51
    {60017,1216,1088, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=4.62  BV=2.41
    {69999,1140,1064, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=4.50  BV=2.31
    {69999,1216,1064, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=4.59  BV=2.22
    {79982,1140,1064, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=4.50  BV=2.12
    {79982,1216,1072, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=4.60  BV=2.01
    {90005,1140,1088, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=4.53  BV=1.92
    {99987,1140,1056, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=4.49  BV=1.81
    {110010,1140,1024, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=4.44  BV=1.71
    {110010,1216,1032, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=4.55  BV=1.61
    {119992,1140,1080, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=4.52  BV=1.51
    {130015,1140,1064, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=4.50  BV=1.42
    {139997,1140,1064, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=4.50  BV=1.31
    {149980,1140,1064, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=4.50  BV=1.21
    {160003,1140,1064, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=4.50  BV=1.12
    {180008,1140,1024, 0, 0, 0},  //TV = 2.47(4562 lines)  AV=2.97  SV=4.44  BV=1.00
    {189991,1140,1040, 0, 0, 0},  //TV = 2.40(4815 lines)  AV=2.97  SV=4.46  BV=0.90
    {200014,1140,1056, 0, 0, 0},  //TV = 2.32(5069 lines)  AV=2.97  SV=4.49  BV=0.81
    {220019,1140,1024, 0, 0, 0},  //TV = 2.18(5576 lines)  AV=2.97  SV=4.44  BV=0.71
    {230002,1140,1056, 0, 0, 0},  //TV = 2.12(5829 lines)  AV=2.97  SV=4.49  BV=0.61
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO100Table =
{
    AETABLE_CAPTURE_ISO100,    //eAETableID
    104,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    6,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_100, //ISO SPEED
    sCaptureISO100PLineTable_60Hz,
    sCaptureISO100PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO200PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {198,2144,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=5.35  BV=9.92
    {198,2240,1056, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=5.46  BV=9.81
    {237,2048,1032, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=5.30  BV=9.72
    {237,2144,1056, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=5.40  BV=9.62
    {237,2352,1032, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=5.50  BV=9.52
    {277,2144,1040, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=5.38  BV=9.41
    {316,2048,1032, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=5.30  BV=9.30
    {316,2144,1048, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=5.39  BV=9.21
    {356,2048,1048, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=5.32  BV=9.11
    {356,2240,1024, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=5.42  BV=9.01
    {395,2144,1032, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=5.36  BV=8.91
    {435,2048,1056, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=5.33  BV=8.81
    {474,2048,1032, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=5.30  BV=8.72
    {513,2048,1024, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=5.29  BV=8.61
    {513,2144,1048, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=5.39  BV=8.51
    {592,1936,1080, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=5.28  BV=8.41
    {632,2048,1032, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=5.30  BV=8.30
    {671,2048,1032, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=5.30  BV=8.21
    {711,2048,1048, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=5.32  BV=8.11
    {750,2048,1064, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=5.34  BV=8.01
    {829,2048,1032, 0, 0, 0},  //TV = 10.24(21 lines)  AV=2.97  SV=5.30  BV=7.91
    {869,2048,1056, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=5.33  BV=7.81
    {947,2048,1040, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=5.31  BV=7.71
    {1026,2048,1024, 0, 0, 0},  //TV = 9.93(26 lines)  AV=2.97  SV=5.29  BV=7.61
    {1105,1936,1080, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=5.28  BV=7.51
    {1184,2048,1024, 0, 0, 0},  //TV = 9.72(30 lines)  AV=2.97  SV=5.29  BV=7.41
    {1263,2048,1024, 0, 0, 0},  //TV = 9.63(32 lines)  AV=2.97  SV=5.29  BV=7.31
    {1342,2048,1040, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=5.31  BV=7.20
    {1421,2048,1048, 0, 0, 0},  //TV = 9.46(36 lines)  AV=2.97  SV=5.32  BV=7.11
    {1539,2048,1040, 0, 0, 0},  //TV = 9.34(39 lines)  AV=2.97  SV=5.31  BV=7.01
    {1658,2048,1032, 0, 0, 0},  //TV = 9.24(42 lines)  AV=2.97  SV=5.30  BV=6.91
    {1776,2048,1032, 0, 0, 0},  //TV = 9.14(45 lines)  AV=2.97  SV=5.30  BV=6.81
    {1894,2048,1040, 0, 0, 0},  //TV = 9.04(48 lines)  AV=2.97  SV=5.31  BV=6.71
    {2052,2048,1032, 0, 0, 0},  //TV = 8.93(52 lines)  AV=2.97  SV=5.30  BV=6.60
    {2210,1936,1080, 0, 0, 0},  //TV = 8.82(56 lines)  AV=2.97  SV=5.28  BV=6.51
    {2329,2048,1040, 0, 0, 0},  //TV = 8.75(59 lines)  AV=2.97  SV=5.31  BV=6.41
    {2526,2048,1024, 0, 0, 0},  //TV = 8.63(64 lines)  AV=2.97  SV=5.29  BV=6.31
    {2723,1936,1080, 0, 0, 0},  //TV = 8.52(69 lines)  AV=2.97  SV=5.28  BV=6.21
    {2881,2048,1032, 0, 0, 0},  //TV = 8.44(73 lines)  AV=2.97  SV=5.30  BV=6.11
    {3118,1936,1080, 0, 0, 0},  //TV = 8.33(79 lines)  AV=2.97  SV=5.28  BV=6.01
    {3315,2048,1032, 0, 0, 0},  //TV = 8.24(84 lines)  AV=2.97  SV=5.30  BV=5.91
    {3552,2048,1032, 0, 0, 0},  //TV = 8.14(90 lines)  AV=2.97  SV=5.30  BV=5.81
    {3828,2048,1032, 0, 0, 0},  //TV = 8.03(97 lines)  AV=2.97  SV=5.30  BV=5.70
    {4104,2048,1024, 0, 0, 0},  //TV = 7.93(104 lines)  AV=2.97  SV=5.29  BV=5.61
    {4380,2048,1032, 0, 0, 0},  //TV = 7.83(111 lines)  AV=2.97  SV=5.30  BV=5.51
    {4696,2048,1032, 0, 0, 0},  //TV = 7.73(119 lines)  AV=2.97  SV=5.30  BV=5.41
    {5091,1936,1080, 0, 0, 0},  //TV = 7.62(129 lines)  AV=2.97  SV=5.28  BV=5.31
    {5406,2048,1024, 0, 0, 0},  //TV = 7.53(137 lines)  AV=2.97  SV=5.29  BV=5.22
    {5840,1936,1080, 0, 0, 0},  //TV = 7.42(148 lines)  AV=2.97  SV=5.28  BV=5.11
    {6235,2048,1024, 0, 0, 0},  //TV = 7.33(158 lines)  AV=2.97  SV=5.29  BV=5.01
    {6669,2048,1024, 0, 0, 0},  //TV = 7.23(169 lines)  AV=2.97  SV=5.29  BV=4.91
    {7142,2048,1024, 0, 0, 0},  //TV = 7.13(181 lines)  AV=2.97  SV=5.29  BV=4.81
    {7655,2048,1024, 0, 0, 0},  //TV = 7.03(194 lines)  AV=2.97  SV=5.29  BV=4.71
    {8208,2048,1024, 0, 0, 0},  //TV = 6.93(208 lines)  AV=2.97  SV=5.29  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {8326,2240,1064, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.47  BV=4.41
    {8326,2448,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.57  BV=4.31
    {8326,2656,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.67  BV=4.21
    {8326,2864,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.77  BV=4.11
    {8326,3072,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.87  BV=4.01
    {8326,3200,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.96  BV=3.91
    {8326,3456,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.06  BV=3.81
    {8326,3712,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.17  BV=3.71
    {8326,3968,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.26  BV=3.62
    {16652,2144,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=5.36  BV=3.51
    {16652,2240,1064, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=5.47  BV=3.41
    {16652,2448,1040, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=5.57  BV=3.31
    {16652,2656,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=5.66  BV=3.22
    {16652,2752,1064, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=5.77  BV=3.11
    {16652,2960,1056, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=5.86  BV=3.02
    {25017,2144,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=5.38  BV=2.92
    {25017,2352,1024, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=5.49  BV=2.81
    {25017,2448,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=5.59  BV=2.70
    {25017,2656,1032, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=5.67  BV=2.62
    {33343,2144,1032, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=5.36  BV=2.51
    {33343,2240,1064, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=5.47  BV=2.41
    {33343,2448,1040, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=5.57  BV=2.31
    {41669,2048,1064, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=5.34  BV=2.21
    {41669,2240,1040, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=5.44  BV=2.12
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2352,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.49  BV=1.81
    {58319,2144,1032, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.36  BV=1.71
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {75011,2048,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.30  BV=1.41
    {75011,2144,1056, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.40  BV=1.31
    {83336,2048,1064, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.34  BV=1.21
    {91661,2048,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=5.31  BV=1.11
    {99987,2048,1024, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=5.29  BV=1.01
    {99987,2144,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=5.38  BV=0.92
    {108313,2144,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=5.36  BV=0.81
    {116678,2144,1032, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=5.36  BV=0.71
    {125004,2144,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=5.36  BV=0.61
    {133330,2144,1032, 0, 0, 0},  //TV = 2.91(3379 lines)  AV=2.97  SV=5.36  BV=0.51
    {149980,2048,1032, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=5.30  BV=0.41
    {158346,2048,1048, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=5.32  BV=0.31
    {166672,2048,1072, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=5.35  BV=0.20
    {183322,2048,1040, 0, 0, 0},  //TV = 2.45(4646 lines)  AV=2.97  SV=5.31  BV=0.11
    {199974,2048,1032, 0, 0, 0},  //TV = 2.32(5068 lines)  AV=2.97  SV=5.30  BV=-0.01
    {208339,2048,1064, 0, 0, 0},  //TV = 2.26(5280 lines)  AV=2.97  SV=5.34  BV=-0.11
    {224990,2048,1056, 0, 0, 0},  //TV = 2.15(5702 lines)  AV=2.97  SV=5.33  BV=-0.21
    {241641,2048,1048, 0, 0, 0},  //TV = 2.05(6124 lines)  AV=2.97  SV=5.32  BV=-0.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO200PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {198,2144,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=5.35  BV=9.92
    {198,2240,1056, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=5.46  BV=9.81
    {237,2048,1032, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=5.30  BV=9.72
    {237,2144,1056, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=5.40  BV=9.62
    {237,2352,1032, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=5.50  BV=9.52
    {277,2144,1040, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=5.38  BV=9.41
    {316,2048,1032, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=5.30  BV=9.30
    {316,2144,1048, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=5.39  BV=9.21
    {356,2048,1048, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=5.32  BV=9.11
    {356,2240,1024, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=5.42  BV=9.01
    {395,2144,1032, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=5.36  BV=8.91
    {435,2048,1056, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=5.33  BV=8.81
    {474,2048,1032, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=5.30  BV=8.72
    {513,2048,1024, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=5.29  BV=8.61
    {513,2144,1048, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=5.39  BV=8.51
    {592,1936,1080, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=5.28  BV=8.41
    {632,2048,1032, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=5.30  BV=8.30
    {671,2048,1032, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=5.30  BV=8.21
    {711,2048,1048, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=5.32  BV=8.11
    {750,2048,1064, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=5.34  BV=8.01
    {829,2048,1032, 0, 0, 0},  //TV = 10.24(21 lines)  AV=2.97  SV=5.30  BV=7.91
    {869,2048,1056, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=5.33  BV=7.81
    {947,2048,1040, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=5.31  BV=7.71
    {1026,2048,1024, 0, 0, 0},  //TV = 9.93(26 lines)  AV=2.97  SV=5.29  BV=7.61
    {1105,1936,1080, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=5.28  BV=7.51
    {1184,2048,1024, 0, 0, 0},  //TV = 9.72(30 lines)  AV=2.97  SV=5.29  BV=7.41
    {1263,2048,1024, 0, 0, 0},  //TV = 9.63(32 lines)  AV=2.97  SV=5.29  BV=7.31
    {1342,2048,1040, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=5.31  BV=7.20
    {1421,2048,1048, 0, 0, 0},  //TV = 9.46(36 lines)  AV=2.97  SV=5.32  BV=7.11
    {1539,2048,1040, 0, 0, 0},  //TV = 9.34(39 lines)  AV=2.97  SV=5.31  BV=7.01
    {1658,2048,1032, 0, 0, 0},  //TV = 9.24(42 lines)  AV=2.97  SV=5.30  BV=6.91
    {1776,2048,1032, 0, 0, 0},  //TV = 9.14(45 lines)  AV=2.97  SV=5.30  BV=6.81
    {1894,2048,1040, 0, 0, 0},  //TV = 9.04(48 lines)  AV=2.97  SV=5.31  BV=6.71
    {2052,2048,1032, 0, 0, 0},  //TV = 8.93(52 lines)  AV=2.97  SV=5.30  BV=6.60
    {2210,1936,1080, 0, 0, 0},  //TV = 8.82(56 lines)  AV=2.97  SV=5.28  BV=6.51
    {2329,2048,1040, 0, 0, 0},  //TV = 8.75(59 lines)  AV=2.97  SV=5.31  BV=6.41
    {2526,2048,1024, 0, 0, 0},  //TV = 8.63(64 lines)  AV=2.97  SV=5.29  BV=6.31
    {2723,1936,1080, 0, 0, 0},  //TV = 8.52(69 lines)  AV=2.97  SV=5.28  BV=6.21
    {2881,2048,1032, 0, 0, 0},  //TV = 8.44(73 lines)  AV=2.97  SV=5.30  BV=6.11
    {3118,1936,1080, 0, 0, 0},  //TV = 8.33(79 lines)  AV=2.97  SV=5.28  BV=6.01
    {3315,2048,1032, 0, 0, 0},  //TV = 8.24(84 lines)  AV=2.97  SV=5.30  BV=5.91
    {3552,2048,1032, 0, 0, 0},  //TV = 8.14(90 lines)  AV=2.97  SV=5.30  BV=5.81
    {3828,2048,1032, 0, 0, 0},  //TV = 8.03(97 lines)  AV=2.97  SV=5.30  BV=5.70
    {4104,2048,1024, 0, 0, 0},  //TV = 7.93(104 lines)  AV=2.97  SV=5.29  BV=5.61
    {4380,2048,1032, 0, 0, 0},  //TV = 7.83(111 lines)  AV=2.97  SV=5.30  BV=5.51
    {4696,2048,1032, 0, 0, 0},  //TV = 7.73(119 lines)  AV=2.97  SV=5.30  BV=5.41
    {5091,1936,1080, 0, 0, 0},  //TV = 7.62(129 lines)  AV=2.97  SV=5.28  BV=5.31
    {5406,2048,1024, 0, 0, 0},  //TV = 7.53(137 lines)  AV=2.97  SV=5.29  BV=5.22
    {5840,1936,1080, 0, 0, 0},  //TV = 7.42(148 lines)  AV=2.97  SV=5.28  BV=5.11
    {6235,2048,1024, 0, 0, 0},  //TV = 7.33(158 lines)  AV=2.97  SV=5.29  BV=5.01
    {6669,2048,1024, 0, 0, 0},  //TV = 7.23(169 lines)  AV=2.97  SV=5.29  BV=4.91
    {7142,2048,1024, 0, 0, 0},  //TV = 7.13(181 lines)  AV=2.97  SV=5.29  BV=4.81
    {7655,2048,1024, 0, 0, 0},  //TV = 7.03(194 lines)  AV=2.97  SV=5.29  BV=4.71
    {8208,2048,1024, 0, 0, 0},  //TV = 6.93(208 lines)  AV=2.97  SV=5.29  BV=4.61
    {8839,2048,1024, 0, 0, 0},  //TV = 6.82(224 lines)  AV=2.97  SV=5.29  BV=4.51
    {9470,2048,1024, 0, 0, 0},  //TV = 6.72(240 lines)  AV=2.97  SV=5.29  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {9983,2352,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.51  BV=4.11
    {9983,2560,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.61  BV=4.01
    {9983,2656,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.71  BV=3.91
    {9983,2864,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.80  BV=3.81
    {9983,3072,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.91  BV=3.71
    {9983,3328,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.01  BV=3.61
    {9983,3584,1032, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.11  BV=3.51
    {9983,3840,1032, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.20  BV=3.41
    {20006,2048,1040, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.31  BV=3.31
    {20006,2144,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.40  BV=3.22
    {20006,2352,1032, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.50  BV=3.12
    {20006,2448,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.60  BV=3.01
    {20006,2656,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.70  BV=2.92
    {20006,2864,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.80  BV=2.81
    {29989,2048,1048, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=5.32  BV=2.71
    {29989,2240,1024, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=5.42  BV=2.61
    {29989,2352,1048, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=5.52  BV=2.51
    {29989,2560,1032, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=5.62  BV=2.41
    {40011,2048,1040, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=5.31  BV=2.31
    {40011,2144,1056, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=5.40  BV=2.22
    {40011,2352,1032, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=5.50  BV=2.12
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2352,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.49  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {69999,2048,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.30  BV=1.51
    {69999,2144,1056, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.40  BV=1.41
    {79982,2048,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.31  BV=1.31
    {79982,2144,1056, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.40  BV=1.22
    {90005,2048,1056, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=5.33  BV=1.11
    {90005,2240,1032, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=5.43  BV=1.02
    {99987,2144,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=5.38  BV=0.92
    {110010,2048,1072, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=5.35  BV=0.80
    {119992,2048,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=5.32  BV=0.71
    {130015,2048,1040, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=5.31  BV=0.60
    {139997,2048,1032, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=5.30  BV=0.51
    {149980,2048,1032, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=5.30  BV=0.41
    {160003,2048,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=5.31  BV=0.31
    {169986,2048,1048, 0, 0, 0},  //TV = 2.56(4308 lines)  AV=2.97  SV=5.32  BV=0.21
    {180008,2048,1064, 0, 0, 0},  //TV = 2.47(4562 lines)  AV=2.97  SV=5.34  BV=0.10
    {200014,2048,1032, 0, 0, 0},  //TV = 2.32(5069 lines)  AV=2.97  SV=5.30  BV=-0.01
    {209996,2048,1056, 0, 0, 0},  //TV = 2.25(5322 lines)  AV=2.97  SV=5.33  BV=-0.11
    {230002,2048,1024, 0, 0, 0},  //TV = 2.12(5829 lines)  AV=2.97  SV=5.29  BV=-0.20
    {239984,2048,1056, 0, 0, 0},  //TV = 2.06(6082 lines)  AV=2.97  SV=5.33  BV=-0.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO200Table =
{
    AETABLE_CAPTURE_ISO200,    //eAETableID
    113,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -3,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_200, //ISO SPEED
    sCaptureISO200PLineTable_60Hz,
    sCaptureISO200PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO400PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {197,2144,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.36  BV=9.92
    {197,2240,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.47  BV=9.81
    {197,2448,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.57  BV=9.71
    {197,2656,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.67  BV=9.61
    {197,2752,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.77  BV=9.51
    {197,3072,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.87  BV=9.41
    {197,3200,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.98  BV=9.31
    {197,3456,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.06  BV=9.22
    {197,3712,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.17  BV=9.11
    {197,3968,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.27  BV=9.01
    {198,4224,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=6.35  BV=8.92
    {198,4608,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=6.46  BV=8.82
    {237,4096,1032, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.30  BV=8.72
    {237,4352,1048, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.41  BV=8.61
    {237,4736,1024, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.50  BV=8.52
    {277,4352,1024, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=6.37  BV=8.41
    {316,4096,1024, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.29  BV=8.31
    {316,4352,1032, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.39  BV=8.21
    {356,4096,1048, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=6.32  BV=8.11
    {356,4480,1024, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=6.42  BV=8.01
    {395,4224,1048, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=6.36  BV=7.91
    {435,4224,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=6.33  BV=7.81
    {474,4096,1032, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=6.30  BV=7.72
    {513,4096,1024, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=6.29  BV=7.61
    {513,4352,1032, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=6.39  BV=7.51
    {592,3968,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=6.29  BV=7.41
    {632,4096,1024, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=6.29  BV=7.31
    {671,4096,1032, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=6.30  BV=7.21
    {711,4096,1048, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=6.32  BV=7.11
    {750,4224,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=6.33  BV=7.02
    {829,4096,1032, 0, 0, 0},  //TV = 10.24(21 lines)  AV=2.97  SV=6.30  BV=6.91
    {869,4224,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=6.33  BV=6.81
    {947,4096,1040, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=6.31  BV=6.71
    {1026,4096,1024, 0, 0, 0},  //TV = 9.93(26 lines)  AV=2.97  SV=6.29  BV=6.61
    {1105,4096,1024, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=6.29  BV=6.51
    {1184,4096,1024, 0, 0, 0},  //TV = 9.72(30 lines)  AV=2.97  SV=6.29  BV=6.41
    {1263,4096,1024, 0, 0, 0},  //TV = 9.63(32 lines)  AV=2.97  SV=6.29  BV=6.31
    {1342,4096,1040, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=6.31  BV=6.20
    {1421,4096,1048, 0, 0, 0},  //TV = 9.46(36 lines)  AV=2.97  SV=6.32  BV=6.11
    {1539,4096,1040, 0, 0, 0},  //TV = 9.34(39 lines)  AV=2.97  SV=6.31  BV=6.01
    {1658,4096,1032, 0, 0, 0},  //TV = 9.24(42 lines)  AV=2.97  SV=6.30  BV=5.91
    {1776,4096,1032, 0, 0, 0},  //TV = 9.14(45 lines)  AV=2.97  SV=6.30  BV=5.81
    {1894,4096,1040, 0, 0, 0},  //TV = 9.04(48 lines)  AV=2.97  SV=6.31  BV=5.71
    {2052,4096,1024, 0, 0, 0},  //TV = 8.93(52 lines)  AV=2.97  SV=6.29  BV=5.61
    {2210,4096,1024, 0, 0, 0},  //TV = 8.82(56 lines)  AV=2.97  SV=6.29  BV=5.51
    {2329,4096,1040, 0, 0, 0},  //TV = 8.75(59 lines)  AV=2.97  SV=6.31  BV=5.41
    {2526,4096,1032, 0, 0, 0},  //TV = 8.63(64 lines)  AV=2.97  SV=6.30  BV=5.30
    {2723,4096,1024, 0, 0, 0},  //TV = 8.52(69 lines)  AV=2.97  SV=6.29  BV=5.20
    {2881,4096,1032, 0, 0, 0},  //TV = 8.44(73 lines)  AV=2.97  SV=6.30  BV=5.11
    {3118,3968,1056, 0, 0, 0},  //TV = 8.33(79 lines)  AV=2.97  SV=6.29  BV=5.01
    {3315,4096,1032, 0, 0, 0},  //TV = 8.24(84 lines)  AV=2.97  SV=6.30  BV=4.91
    {3591,4096,1024, 0, 0, 0},  //TV = 8.12(91 lines)  AV=2.97  SV=6.29  BV=4.81
    {3828,4096,1024, 0, 0, 0},  //TV = 8.03(97 lines)  AV=2.97  SV=6.29  BV=4.71
    {4104,4096,1024, 0, 0, 0},  //TV = 7.93(104 lines)  AV=2.97  SV=6.29  BV=4.61
    {4380,4096,1032, 0, 0, 0},  //TV = 7.83(111 lines)  AV=2.97  SV=6.30  BV=4.51
    {4696,4096,1024, 0, 0, 0},  //TV = 7.73(119 lines)  AV=2.97  SV=6.29  BV=4.42
    {5091,3968,1056, 0, 0, 0},  //TV = 7.62(129 lines)  AV=2.97  SV=6.29  BV=4.30
    {5446,4096,1024, 0, 0, 0},  //TV = 7.52(138 lines)  AV=2.97  SV=6.29  BV=4.20
    {5840,3968,1056, 0, 0, 0},  //TV = 7.42(148 lines)  AV=2.97  SV=6.29  BV=4.11
    {6235,4096,1024, 0, 0, 0},  //TV = 7.33(158 lines)  AV=2.97  SV=6.29  BV=4.01
    {6669,4096,1024, 0, 0, 0},  //TV = 7.23(169 lines)  AV=2.97  SV=6.29  BV=3.91
    {7142,4096,1024, 0, 0, 0},  //TV = 7.13(181 lines)  AV=2.97  SV=6.29  BV=3.81
    {7655,4096,1024, 0, 0, 0},  //TV = 7.03(194 lines)  AV=2.97  SV=6.29  BV=3.71
    {8208,4096,1024, 0, 0, 0},  //TV = 6.93(208 lines)  AV=2.97  SV=6.29  BV=3.61
    {8326,4224,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.36  BV=3.51
    {8326,4608,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.47  BV=3.41
    {8326,4992,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.57  BV=3.31
    {8326,5248,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.67  BV=3.21
    {8326,5632,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.77  BV=3.11
    {8326,6144,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.87  BV=3.01
    {8326,6144,1096, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.97  BV=2.91
    {8326,6144,1176, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.07  BV=2.81
    {8326,6144,1256, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.17  BV=2.71
    {8326,6144,1352, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.27  BV=2.61
    {16652,4224,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.36  BV=2.51
    {16652,4608,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.47  BV=2.41
    {16652,4864,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.57  BV=2.31
    {16652,5248,1040, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.67  BV=2.21
    {16652,5632,1040, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.77  BV=2.11
    {16652,6016,1040, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.86  BV=2.02
    {25017,4352,1024, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=6.37  BV=1.92
    {25017,4608,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=6.48  BV=1.81
    {25017,4992,1032, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=6.58  BV=1.71
    {25017,5376,1024, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=6.68  BV=1.61
    {33343,4224,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=6.36  BV=1.51
    {33343,4608,1032, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=6.47  BV=1.41
    {33343,4864,1040, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=6.56  BV=1.32
    {41669,4224,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=6.34  BV=1.21
    {41669,4480,1040, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=6.44  BV=1.12
    {41669,4864,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=6.55  BV=1.01
    {49994,4352,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.39  BV=0.91
    {49994,4608,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.48  BV=0.81
    {58319,4224,1040, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=6.35  BV=0.72
    {58319,4608,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=6.46  BV=0.61
    {66645,4224,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.36  BV=0.51
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {133330,3968,1056, 0, 0, 0},  //TV = 2.91(3379 lines)  AV=2.97  SV=6.29  BV=-0.41
    {141655,4096,1032, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=6.30  BV=-0.51
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {158346,4224,1024, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=6.33  BV=-0.70
    {174997,4096,1024, 0, 0, 0},  //TV = 2.51(4435 lines)  AV=2.97  SV=6.29  BV=-0.80
    {183322,4096,1048, 0, 0, 0},  //TV = 2.45(4646 lines)  AV=2.97  SV=6.32  BV=-0.90
    {199974,4096,1032, 0, 0, 0},  //TV = 2.32(5068 lines)  AV=2.97  SV=6.30  BV=-1.01
    {208339,4224,1024, 0, 0, 0},  //TV = 2.26(5280 lines)  AV=2.97  SV=6.33  BV=-1.10
    {224990,4096,1048, 0, 0, 0},  //TV = 2.15(5702 lines)  AV=2.97  SV=6.32  BV=-1.20
    {241641,4096,1048, 0, 0, 0},  //TV = 2.05(6124 lines)  AV=2.97  SV=6.32  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO400PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {197,2144,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.36  BV=9.92
    {197,2240,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.47  BV=9.81
    {197,2448,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.57  BV=9.71
    {197,2656,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.67  BV=9.61
    {197,2752,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.77  BV=9.51
    {197,3072,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.87  BV=9.41
    {197,3200,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.98  BV=9.31
    {197,3456,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.06  BV=9.22
    {197,3712,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.17  BV=9.11
    {197,3968,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.27  BV=9.01
    {198,4224,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=6.35  BV=8.92
    {198,4608,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=6.46  BV=8.82
    {237,4096,1032, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.30  BV=8.72
    {237,4352,1048, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.41  BV=8.61
    {237,4736,1024, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.50  BV=8.52
    {277,4352,1024, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=6.37  BV=8.41
    {316,4096,1024, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.29  BV=8.31
    {316,4352,1032, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.39  BV=8.21
    {356,4096,1048, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=6.32  BV=8.11
    {356,4480,1024, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=6.42  BV=8.01
    {395,4224,1048, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=6.36  BV=7.91
    {435,4224,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=6.33  BV=7.81
    {474,4096,1032, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=6.30  BV=7.72
    {513,4096,1024, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=6.29  BV=7.61
    {513,4352,1032, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=6.39  BV=7.51
    {592,3968,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=6.29  BV=7.41
    {632,4096,1024, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=6.29  BV=7.31
    {671,4096,1032, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=6.30  BV=7.21
    {711,4096,1048, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=6.32  BV=7.11
    {750,4224,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=6.33  BV=7.02
    {829,4096,1032, 0, 0, 0},  //TV = 10.24(21 lines)  AV=2.97  SV=6.30  BV=6.91
    {869,4224,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=6.33  BV=6.81
    {947,4096,1040, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=6.31  BV=6.71
    {1026,4096,1024, 0, 0, 0},  //TV = 9.93(26 lines)  AV=2.97  SV=6.29  BV=6.61
    {1105,4096,1024, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=6.29  BV=6.51
    {1184,4096,1024, 0, 0, 0},  //TV = 9.72(30 lines)  AV=2.97  SV=6.29  BV=6.41
    {1263,4096,1024, 0, 0, 0},  //TV = 9.63(32 lines)  AV=2.97  SV=6.29  BV=6.31
    {1342,4096,1040, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=6.31  BV=6.20
    {1421,4096,1048, 0, 0, 0},  //TV = 9.46(36 lines)  AV=2.97  SV=6.32  BV=6.11
    {1539,4096,1040, 0, 0, 0},  //TV = 9.34(39 lines)  AV=2.97  SV=6.31  BV=6.01
    {1658,4096,1032, 0, 0, 0},  //TV = 9.24(42 lines)  AV=2.97  SV=6.30  BV=5.91
    {1776,4096,1032, 0, 0, 0},  //TV = 9.14(45 lines)  AV=2.97  SV=6.30  BV=5.81
    {1894,4096,1040, 0, 0, 0},  //TV = 9.04(48 lines)  AV=2.97  SV=6.31  BV=5.71
    {2052,4096,1024, 0, 0, 0},  //TV = 8.93(52 lines)  AV=2.97  SV=6.29  BV=5.61
    {2210,4096,1024, 0, 0, 0},  //TV = 8.82(56 lines)  AV=2.97  SV=6.29  BV=5.51
    {2329,4096,1040, 0, 0, 0},  //TV = 8.75(59 lines)  AV=2.97  SV=6.31  BV=5.41
    {2526,4096,1032, 0, 0, 0},  //TV = 8.63(64 lines)  AV=2.97  SV=6.30  BV=5.30
    {2723,4096,1024, 0, 0, 0},  //TV = 8.52(69 lines)  AV=2.97  SV=6.29  BV=5.20
    {2881,4096,1032, 0, 0, 0},  //TV = 8.44(73 lines)  AV=2.97  SV=6.30  BV=5.11
    {3118,3968,1056, 0, 0, 0},  //TV = 8.33(79 lines)  AV=2.97  SV=6.29  BV=5.01
    {3315,4096,1032, 0, 0, 0},  //TV = 8.24(84 lines)  AV=2.97  SV=6.30  BV=4.91
    {3591,4096,1024, 0, 0, 0},  //TV = 8.12(91 lines)  AV=2.97  SV=6.29  BV=4.81
    {3828,4096,1024, 0, 0, 0},  //TV = 8.03(97 lines)  AV=2.97  SV=6.29  BV=4.71
    {4104,4096,1024, 0, 0, 0},  //TV = 7.93(104 lines)  AV=2.97  SV=6.29  BV=4.61
    {4380,4096,1032, 0, 0, 0},  //TV = 7.83(111 lines)  AV=2.97  SV=6.30  BV=4.51
    {4696,4096,1024, 0, 0, 0},  //TV = 7.73(119 lines)  AV=2.97  SV=6.29  BV=4.42
    {5091,3968,1056, 0, 0, 0},  //TV = 7.62(129 lines)  AV=2.97  SV=6.29  BV=4.30
    {5446,4096,1024, 0, 0, 0},  //TV = 7.52(138 lines)  AV=2.97  SV=6.29  BV=4.20
    {5840,3968,1056, 0, 0, 0},  //TV = 7.42(148 lines)  AV=2.97  SV=6.29  BV=4.11
    {6235,4096,1024, 0, 0, 0},  //TV = 7.33(158 lines)  AV=2.97  SV=6.29  BV=4.01
    {6669,4096,1024, 0, 0, 0},  //TV = 7.23(169 lines)  AV=2.97  SV=6.29  BV=3.91
    {7142,4096,1024, 0, 0, 0},  //TV = 7.13(181 lines)  AV=2.97  SV=6.29  BV=3.81
    {7655,4096,1024, 0, 0, 0},  //TV = 7.03(194 lines)  AV=2.97  SV=6.29  BV=3.71
    {8208,4096,1024, 0, 0, 0},  //TV = 6.93(208 lines)  AV=2.97  SV=6.29  BV=3.61
    {8800,4096,1024, 0, 0, 0},  //TV = 6.83(223 lines)  AV=2.97  SV=6.29  BV=3.51
    {9431,4096,1024, 0, 0, 0},  //TV = 6.73(239 lines)  AV=2.97  SV=6.29  BV=3.41
    {9983,4096,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.31  BV=3.31
    {9983,4352,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.41  BV=3.21
    {9983,4736,1032, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.51  BV=3.11
    {9983,5120,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.61  BV=3.01
    {9983,5376,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.71  BV=2.90
    {9983,5888,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.81  BV=2.81
    {9983,6144,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.91  BV=2.71
    {9983,6144,1128, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.01  BV=2.61
    {9983,6144,1208, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.11  BV=2.51
    {9983,6144,1288, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.20  BV=2.41
    {20006,4096,1032, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.30  BV=2.32
    {20006,4352,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.41  BV=2.21
    {20006,4736,1024, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.50  BV=2.12
    {20006,4992,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.61  BV=2.01
    {20006,5376,1040, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.70  BV=1.91
    {20006,5760,1040, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.80  BV=1.81
    {29989,4096,1048, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=6.32  BV=1.71
    {29989,4480,1024, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=6.42  BV=1.61
    {29989,4736,1040, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=6.52  BV=1.51
    {29989,5120,1032, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=6.62  BV=1.41
    {40011,4096,1032, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=6.30  BV=1.32
    {40011,4352,1040, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=6.40  BV=1.22
    {40011,4736,1024, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=6.50  BV=1.12
    {49994,4096,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.29  BV=1.01
    {49994,4352,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.39  BV=0.91
    {49994,4608,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.48  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.43  BV=0.60
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1032, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.34  BV=0.10
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {130015,4096,1048, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=6.32  BV=-0.41
    {139997,4096,1040, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=6.31  BV=-0.50
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {160003,4096,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.31  BV=-0.69
    {169986,4224,1024, 0, 0, 0},  //TV = 2.56(4308 lines)  AV=2.97  SV=6.33  BV=-0.80
    {180008,4224,1032, 0, 0, 0},  //TV = 2.47(4562 lines)  AV=2.97  SV=6.34  BV=-0.90
    {200014,4096,1032, 0, 0, 0},  //TV = 2.32(5069 lines)  AV=2.97  SV=6.30  BV=-1.01
    {209996,4096,1056, 0, 0, 0},  //TV = 2.25(5322 lines)  AV=2.97  SV=6.33  BV=-1.11
    {230002,4096,1032, 0, 0, 0},  //TV = 2.12(5829 lines)  AV=2.97  SV=6.30  BV=-1.21
    {239984,4224,1024, 0, 0, 0},  //TV = 2.06(6082 lines)  AV=2.97  SV=6.33  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO400Table =
{
    AETABLE_CAPTURE_ISO400,    //eAETableID
    123,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -13,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_400, //ISO SPEED
    sCaptureISO400PLineTable_60Hz,
    sCaptureISO400PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO800PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {197,2144,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.36  BV=9.92
    {197,2240,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.47  BV=9.81
    {197,2448,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.57  BV=9.71
    {197,2656,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.67  BV=9.61
    {197,2752,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.77  BV=9.51
    {197,3072,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.87  BV=9.41
    {197,3200,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.98  BV=9.31
    {197,3456,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.06  BV=9.22
    {197,3712,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.17  BV=9.11
    {197,3968,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.27  BV=9.01
    {197,4352,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.37  BV=8.91
    {197,4608,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.47  BV=8.81
    {197,4992,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.57  BV=8.71
    {197,5248,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.67  BV=8.61
    {197,5632,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.77  BV=8.51
    {197,6144,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.87  BV=8.41
    {198,6144,1096, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=6.97  BV=8.30
    {198,6144,1168, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=7.06  BV=8.21
    {237,6144,1048, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.91  BV=8.11
    {237,6144,1120, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.00  BV=8.01
    {277,6144,1032, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=6.88  BV=7.91
    {277,6144,1104, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=6.98  BV=7.81
    {316,6144,1032, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.88  BV=7.72
    {316,6144,1112, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.99  BV=7.61
    {356,6144,1056, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=6.92  BV=7.51
    {356,6144,1128, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=7.01  BV=7.42
    {395,6144,1096, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=6.97  BV=7.31
    {435,6144,1064, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=6.93  BV=7.21
    {474,6144,1048, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=6.91  BV=7.11
    {513,6144,1040, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=6.89  BV=7.01
    {553,6144,1032, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=6.88  BV=6.91
    {592,6144,1032, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=6.88  BV=6.81
    {632,6144,1032, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=6.88  BV=6.72
    {671,6144,1048, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=6.91  BV=6.61
    {711,6144,1056, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=6.92  BV=6.51
    {790,6016,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=6.86  BV=6.41
    {829,6144,1040, 0, 0, 0},  //TV = 10.24(21 lines)  AV=2.97  SV=6.89  BV=6.31
    {869,6144,1064, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=6.93  BV=6.21
    {947,6144,1048, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=6.91  BV=6.11
    {1026,6144,1040, 0, 0, 0},  //TV = 9.93(26 lines)  AV=2.97  SV=6.89  BV=6.01
    {1105,6144,1032, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=6.88  BV=5.91
    {1184,6144,1032, 0, 0, 0},  //TV = 9.72(30 lines)  AV=2.97  SV=6.88  BV=5.81
    {1263,6144,1040, 0, 0, 0},  //TV = 9.63(32 lines)  AV=2.97  SV=6.89  BV=5.71
    {1342,6144,1040, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=6.89  BV=5.62
    {1460,6144,1032, 0, 0, 0},  //TV = 9.42(37 lines)  AV=2.97  SV=6.88  BV=5.51
    {1579,6144,1024, 0, 0, 0},  //TV = 9.31(40 lines)  AV=2.97  SV=6.87  BV=5.41
    {1658,6144,1040, 0, 0, 0},  //TV = 9.24(42 lines)  AV=2.97  SV=6.89  BV=5.31
    {1776,6144,1040, 0, 0, 0},  //TV = 9.14(45 lines)  AV=2.97  SV=6.89  BV=5.21
    {1934,6144,1032, 0, 0, 0},  //TV = 9.01(49 lines)  AV=2.97  SV=6.88  BV=5.10
    {2052,6144,1040, 0, 0, 0},  //TV = 8.93(52 lines)  AV=2.97  SV=6.89  BV=5.01
    {2210,6144,1032, 0, 0, 0},  //TV = 8.82(56 lines)  AV=2.97  SV=6.88  BV=4.91
    {2368,6144,1032, 0, 0, 0},  //TV = 8.72(60 lines)  AV=2.97  SV=6.88  BV=4.81
    {2565,6016,1040, 0, 0, 0},  //TV = 8.61(65 lines)  AV=2.97  SV=6.86  BV=4.71
    {2723,6144,1032, 0, 0, 0},  //TV = 8.52(69 lines)  AV=2.97  SV=6.88  BV=4.61
    {2920,6144,1032, 0, 0, 0},  //TV = 8.42(74 lines)  AV=2.97  SV=6.88  BV=4.51
    {3157,6144,1024, 0, 0, 0},  //TV = 8.31(80 lines)  AV=2.97  SV=6.87  BV=4.41
    {3354,6144,1032, 0, 0, 0},  //TV = 8.22(85 lines)  AV=2.97  SV=6.88  BV=4.31
    {3591,6144,1032, 0, 0, 0},  //TV = 8.12(91 lines)  AV=2.97  SV=6.88  BV=4.21
    {3867,6144,1024, 0, 0, 0},  //TV = 8.01(98 lines)  AV=2.97  SV=6.87  BV=4.11
    {4144,6144,1032, 0, 0, 0},  //TV = 7.91(105 lines)  AV=2.97  SV=6.88  BV=4.00
    {4459,6144,1024, 0, 0, 0},  //TV = 7.81(113 lines)  AV=2.97  SV=6.87  BV=3.91
    {4775,6144,1024, 0, 0, 0},  //TV = 7.71(121 lines)  AV=2.97  SV=6.87  BV=3.81
    {5130,6016,1040, 0, 0, 0},  //TV = 7.61(130 lines)  AV=2.97  SV=6.86  BV=3.71
    {5485,6144,1024, 0, 0, 0},  //TV = 7.51(139 lines)  AV=2.97  SV=6.87  BV=3.61
    {5880,6016,1040, 0, 0, 0},  //TV = 7.41(149 lines)  AV=2.97  SV=6.86  BV=3.52
    {6274,6144,1024, 0, 0, 0},  //TV = 7.32(159 lines)  AV=2.97  SV=6.87  BV=3.42
    {6787,6016,1040, 0, 0, 0},  //TV = 7.20(172 lines)  AV=2.97  SV=6.86  BV=3.31
    {7261,6144,1024, 0, 0, 0},  //TV = 7.11(184 lines)  AV=2.97  SV=6.87  BV=3.20
    {7774,6144,1024, 0, 0, 0},  //TV = 7.01(197 lines)  AV=2.97  SV=6.87  BV=3.11
    {8326,6144,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.87  BV=3.01
    {8326,6144,1096, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.97  BV=2.91
    {8326,6144,1176, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.07  BV=2.81
    {8326,6144,1256, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.17  BV=2.71
    {8326,6144,1344, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.26  BV=2.61
    {8326,6144,1440, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.36  BV=2.52
    {8326,6144,1544, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.46  BV=2.41
    {8326,6144,1656, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.57  BV=2.31
    {8326,6144,1776, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.67  BV=2.21
    {8326,6144,1920, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.78  BV=2.10
    {16652,6144,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.87  BV=2.01
    {16652,6144,1096, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=6.97  BV=1.91
    {16652,6144,1176, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.07  BV=1.81
    {16652,6144,1264, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.18  BV=1.70
    {16652,6144,1352, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.27  BV=1.61
    {16652,6144,1448, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.37  BV=1.51
    {25017,6144,1032, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=6.88  BV=1.41
    {25017,6144,1104, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=6.98  BV=1.31
    {25017,6144,1184, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=7.08  BV=1.21
    {25017,6144,1272, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=7.18  BV=1.11
    {33343,6144,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=6.87  BV=1.01
    {33343,6144,1096, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=6.97  BV=0.91
    {33343,6144,1176, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=7.07  BV=0.81
    {33343,6144,1256, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=7.17  BV=0.71
    {41669,6144,1080, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=6.95  BV=0.61
    {41669,6144,1160, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=7.05  BV=0.50
    {49994,6144,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.88  BV=0.41
    {49994,6144,1104, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.98  BV=0.31
    {49994,6144,1184, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=7.08  BV=0.21
    {58319,6144,1088, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=6.96  BV=0.11
    {66645,6144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.88  BV=-0.00
    {66645,6144,1104, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.98  BV=-0.10
    {75011,6144,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.91  BV=-0.20
    {75011,6144,1128, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=7.01  BV=-0.30
    {83336,6144,1088, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.96  BV=-0.40
    {91661,6144,1056, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.92  BV=-0.50
    {99987,6144,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.89  BV=-0.60
    {108313,6144,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.87  BV=-0.69
    {116678,6144,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.87  BV=-0.80
    {125004,6144,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.87  BV=-0.90
    {133330,6144,1024, 0, 0, 0},  //TV = 2.91(3379 lines)  AV=2.97  SV=6.87  BV=-0.99
    {141655,6144,1040, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=6.89  BV=-1.10
    {149980,6144,1048, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.91  BV=-1.20
    {158346,6144,1064, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=6.93  BV=-1.30
    {174997,6144,1032, 0, 0, 0},  //TV = 2.51(4435 lines)  AV=2.97  SV=6.88  BV=-1.40
    {183322,6144,1056, 0, 0, 0},  //TV = 2.45(4646 lines)  AV=2.97  SV=6.92  BV=-1.50
    {199974,6144,1040, 0, 0, 0},  //TV = 2.32(5068 lines)  AV=2.97  SV=6.89  BV=-1.60
    {216665,6144,1024, 0, 0, 0},  //TV = 2.21(5491 lines)  AV=2.97  SV=6.87  BV=-1.69
    {233316,6144,1024, 0, 0, 0},  //TV = 2.10(5913 lines)  AV=2.97  SV=6.87  BV=-1.80
    {250007,6016,1040, 0, 0, 0},  //TV = 2.00(6336 lines)  AV=2.97  SV=6.86  BV=-1.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO800PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {197,2144,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.36  BV=9.92
    {197,2240,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.47  BV=9.81
    {197,2448,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.57  BV=9.71
    {197,2656,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.67  BV=9.61
    {197,2752,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.77  BV=9.51
    {197,3072,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.87  BV=9.41
    {197,3200,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.98  BV=9.31
    {197,3456,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.06  BV=9.22
    {197,3712,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.17  BV=9.11
    {197,3968,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.27  BV=9.01
    {197,4352,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.37  BV=8.91
    {197,4608,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.47  BV=8.81
    {197,4992,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.57  BV=8.71
    {197,5248,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.67  BV=8.61
    {197,5632,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.77  BV=8.51
    {197,6144,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.87  BV=8.41
    {198,6144,1096, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=6.97  BV=8.30
    {198,6144,1168, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=7.06  BV=8.21
    {237,6144,1048, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=6.91  BV=8.11
    {237,6144,1120, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.00  BV=8.01
    {277,6144,1032, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=6.88  BV=7.91
    {277,6144,1104, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=6.98  BV=7.81
    {316,6144,1032, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.88  BV=7.72
    {316,6144,1112, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=6.99  BV=7.61
    {356,6144,1056, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=6.92  BV=7.51
    {356,6144,1128, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=7.01  BV=7.42
    {395,6144,1096, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=6.97  BV=7.31
    {435,6144,1064, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=6.93  BV=7.21
    {474,6144,1048, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=6.91  BV=7.11
    {513,6144,1040, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=6.89  BV=7.01
    {553,6144,1032, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=6.88  BV=6.91
    {592,6144,1032, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=6.88  BV=6.81
    {632,6144,1032, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=6.88  BV=6.72
    {671,6144,1048, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=6.91  BV=6.61
    {711,6144,1056, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=6.92  BV=6.51
    {790,6016,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=6.86  BV=6.41
    {829,6144,1040, 0, 0, 0},  //TV = 10.24(21 lines)  AV=2.97  SV=6.89  BV=6.31
    {869,6144,1064, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=6.93  BV=6.21
    {947,6144,1048, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=6.91  BV=6.11
    {1026,6144,1040, 0, 0, 0},  //TV = 9.93(26 lines)  AV=2.97  SV=6.89  BV=6.01
    {1105,6144,1032, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=6.88  BV=5.91
    {1184,6144,1032, 0, 0, 0},  //TV = 9.72(30 lines)  AV=2.97  SV=6.88  BV=5.81
    {1263,6144,1040, 0, 0, 0},  //TV = 9.63(32 lines)  AV=2.97  SV=6.89  BV=5.71
    {1342,6144,1040, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=6.89  BV=5.62
    {1460,6144,1032, 0, 0, 0},  //TV = 9.42(37 lines)  AV=2.97  SV=6.88  BV=5.51
    {1579,6144,1024, 0, 0, 0},  //TV = 9.31(40 lines)  AV=2.97  SV=6.87  BV=5.41
    {1658,6144,1040, 0, 0, 0},  //TV = 9.24(42 lines)  AV=2.97  SV=6.89  BV=5.31
    {1776,6144,1040, 0, 0, 0},  //TV = 9.14(45 lines)  AV=2.97  SV=6.89  BV=5.21
    {1934,6144,1032, 0, 0, 0},  //TV = 9.01(49 lines)  AV=2.97  SV=6.88  BV=5.10
    {2052,6144,1040, 0, 0, 0},  //TV = 8.93(52 lines)  AV=2.97  SV=6.89  BV=5.01
    {2210,6144,1032, 0, 0, 0},  //TV = 8.82(56 lines)  AV=2.97  SV=6.88  BV=4.91
    {2368,6144,1032, 0, 0, 0},  //TV = 8.72(60 lines)  AV=2.97  SV=6.88  BV=4.81
    {2565,6016,1040, 0, 0, 0},  //TV = 8.61(65 lines)  AV=2.97  SV=6.86  BV=4.71
    {2723,6144,1032, 0, 0, 0},  //TV = 8.52(69 lines)  AV=2.97  SV=6.88  BV=4.61
    {2920,6144,1032, 0, 0, 0},  //TV = 8.42(74 lines)  AV=2.97  SV=6.88  BV=4.51
    {3157,6144,1024, 0, 0, 0},  //TV = 8.31(80 lines)  AV=2.97  SV=6.87  BV=4.41
    {3354,6144,1032, 0, 0, 0},  //TV = 8.22(85 lines)  AV=2.97  SV=6.88  BV=4.31
    {3591,6144,1032, 0, 0, 0},  //TV = 8.12(91 lines)  AV=2.97  SV=6.88  BV=4.21
    {3867,6144,1024, 0, 0, 0},  //TV = 8.01(98 lines)  AV=2.97  SV=6.87  BV=4.11
    {4144,6144,1032, 0, 0, 0},  //TV = 7.91(105 lines)  AV=2.97  SV=6.88  BV=4.00
    {4459,6144,1024, 0, 0, 0},  //TV = 7.81(113 lines)  AV=2.97  SV=6.87  BV=3.91
    {4775,6144,1024, 0, 0, 0},  //TV = 7.71(121 lines)  AV=2.97  SV=6.87  BV=3.81
    {5130,6016,1040, 0, 0, 0},  //TV = 7.61(130 lines)  AV=2.97  SV=6.86  BV=3.71
    {5485,6144,1024, 0, 0, 0},  //TV = 7.51(139 lines)  AV=2.97  SV=6.87  BV=3.61
    {5880,6016,1040, 0, 0, 0},  //TV = 7.41(149 lines)  AV=2.97  SV=6.86  BV=3.52
    {6274,6144,1024, 0, 0, 0},  //TV = 7.32(159 lines)  AV=2.97  SV=6.87  BV=3.42
    {6787,6016,1040, 0, 0, 0},  //TV = 7.20(172 lines)  AV=2.97  SV=6.86  BV=3.31
    {7261,6144,1024, 0, 0, 0},  //TV = 7.11(184 lines)  AV=2.97  SV=6.87  BV=3.20
    {7774,6144,1024, 0, 0, 0},  //TV = 7.01(197 lines)  AV=2.97  SV=6.87  BV=3.11
    {8326,6144,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=6.87  BV=3.01
    {8918,6144,1024, 0, 0, 0},  //TV = 6.81(226 lines)  AV=2.97  SV=6.87  BV=2.91
    {9549,6144,1024, 0, 0, 0},  //TV = 6.71(242 lines)  AV=2.97  SV=6.87  BV=2.81
    {9983,6144,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=6.91  BV=2.71
    {9983,6144,1120, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.00  BV=2.62
    {9983,6144,1200, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.10  BV=2.52
    {9983,6144,1288, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.20  BV=2.41
    {9983,6144,1384, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.31  BV=2.31
    {9983,6144,1488, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.41  BV=2.21
    {9983,6144,1584, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.50  BV=2.12
    {9983,6144,1712, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.61  BV=2.00
    {9983,6144,1832, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.71  BV=1.91
    {9983,6144,1968, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.81  BV=1.80
    {20006,6144,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=6.91  BV=1.71
    {20006,6144,1128, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.01  BV=1.60
    {20006,6144,1208, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.11  BV=1.50
    {20006,6144,1296, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.21  BV=1.40
    {20006,6144,1384, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.31  BV=1.31
    {20006,6144,1488, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.41  BV=1.20
    {29989,6144,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=6.93  BV=1.10
    {29989,6144,1136, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.02  BV=1.01
    {29989,6144,1216, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.12  BV=0.91
    {29989,6144,1304, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.22  BV=0.81
    {40011,6144,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=6.91  BV=0.71
    {40011,6144,1120, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=7.00  BV=0.61
    {40011,6144,1208, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=7.11  BV=0.50
    {49994,6144,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.88  BV=0.41
    {49994,6144,1104, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.98  BV=0.31
    {49994,6144,1184, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=7.08  BV=0.21
    {60017,6144,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.92  BV=0.11
    {60017,6144,1144, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=7.03  BV=-0.00
    {69999,6144,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.91  BV=-0.10
    {69999,6144,1128, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=7.01  BV=-0.20
    {79982,6144,1056, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.92  BV=-0.30
    {79982,6144,1128, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=7.01  BV=-0.40
    {90005,6144,1080, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.95  BV=-0.50
    {99987,6144,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.89  BV=-0.60
    {99987,6144,1112, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.99  BV=-0.70
    {110010,6144,1088, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.96  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {130015,6144,1056, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=6.92  BV=-1.00
    {139997,6144,1048, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=6.91  BV=-1.10
    {149980,6144,1048, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.91  BV=-1.20
    {160003,6144,1056, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.92  BV=-1.30
    {169986,6144,1064, 0, 0, 0},  //TV = 2.56(4308 lines)  AV=2.97  SV=6.93  BV=-1.40
    {180008,6144,1080, 0, 0, 0},  //TV = 2.47(4562 lines)  AV=2.97  SV=6.95  BV=-1.50
    {200014,6144,1040, 0, 0, 0},  //TV = 2.32(5069 lines)  AV=2.97  SV=6.89  BV=-1.60
    {209996,6144,1056, 0, 0, 0},  //TV = 2.25(5322 lines)  AV=2.97  SV=6.92  BV=-1.69
    {230002,6144,1040, 0, 0, 0},  //TV = 2.12(5829 lines)  AV=2.97  SV=6.89  BV=-1.80
    {250007,6016,1040, 0, 0, 0},  //TV = 2.00(6336 lines)  AV=2.97  SV=6.86  BV=-1.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO800Table =
{
    AETABLE_CAPTURE_ISO800,    //eAETableID
    129,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -19,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_800, //ISO SPEED
    sCaptureISO800PLineTable_60Hz,
    sCaptureISO800PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO1600PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {197,2144,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.36  BV=9.92
    {197,2240,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.47  BV=9.81
    {197,2448,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.57  BV=9.71
    {197,2656,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.67  BV=9.61
    {197,2752,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.77  BV=9.51
    {197,3072,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.87  BV=9.41
    {197,3200,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.98  BV=9.31
    {197,3456,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.06  BV=9.22
    {197,3712,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.17  BV=9.11
    {197,3968,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.27  BV=9.01
    {197,4352,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.37  BV=8.91
    {197,4608,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.47  BV=8.81
    {197,4992,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.57  BV=8.71
    {197,5248,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.67  BV=8.61
    {197,5632,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.77  BV=8.51
    {197,6144,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.87  BV=8.41
    {197,6144,1096, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.97  BV=8.31
    {197,6144,1176, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.07  BV=8.21
    {197,6144,1264, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.18  BV=8.10
    {197,6144,1352, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.27  BV=8.01
    {197,6144,1448, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.37  BV=7.91
    {197,6144,1552, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.47  BV=7.81
    {197,6144,1664, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.57  BV=7.71
    {198,6144,1776, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=7.67  BV=7.61
    {198,6144,1904, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=7.77  BV=7.51
    {237,6144,1704, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.61  BV=7.41
    {237,6144,1824, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.70  BV=7.31
    {237,6144,1952, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.80  BV=7.21
    {277,6144,1792, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=7.68  BV=7.11
    {277,6144,1920, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=7.78  BV=7.01
    {316,6144,1800, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=7.69  BV=6.91
    {356,6144,1720, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=7.62  BV=6.81
    {356,6144,1840, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=7.72  BV=6.71
    {395,6144,1776, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=7.67  BV=6.61
    {435,6144,1728, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=7.63  BV=6.51
    {474,6144,1704, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=7.61  BV=6.41
    {474,6144,1824, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=7.70  BV=6.31
    {513,6144,1800, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=7.69  BV=6.21
    {553,6144,1800, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=7.69  BV=6.11
    {592,6144,1792, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=7.68  BV=6.01
    {632,6144,1800, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=7.69  BV=5.91
    {711,6144,1712, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=7.61  BV=5.82
    {750,6144,1752, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=7.65  BV=5.71
    {790,6144,1776, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=7.67  BV=5.61
    {869,6144,1728, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=7.63  BV=5.51
    {947,6144,1704, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=7.61  BV=5.41
    {987,6144,1752, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=7.65  BV=5.31
    {1066,6144,1736, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=7.63  BV=5.21
    {1145,6144,1728, 0, 0, 0},  //TV = 9.77(29 lines)  AV=2.97  SV=7.63  BV=5.11
    {1224,6144,1744, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=7.64  BV=5.01
    {1342,6144,1704, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=7.61  BV=4.91
    {1421,6144,1720, 0, 0, 0},  //TV = 9.46(36 lines)  AV=2.97  SV=7.62  BV=4.81
    {1539,6144,1712, 0, 0, 0},  //TV = 9.34(39 lines)  AV=2.97  SV=7.61  BV=4.70
    {1618,6144,1736, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=7.63  BV=4.61
    {1737,6144,1728, 0, 0, 0},  //TV = 9.17(44 lines)  AV=2.97  SV=7.63  BV=4.51
    {1894,6144,1704, 0, 0, 0},  //TV = 9.04(48 lines)  AV=2.97  SV=7.61  BV=4.41
    {2013,6144,1720, 0, 0, 0},  //TV = 8.96(51 lines)  AV=2.97  SV=7.62  BV=4.31
    {2171,6144,1712, 0, 0, 0},  //TV = 8.85(55 lines)  AV=2.97  SV=7.61  BV=4.20
    {2329,6144,1704, 0, 0, 0},  //TV = 8.75(59 lines)  AV=2.97  SV=7.61  BV=4.11
    {2486,6144,1712, 0, 0, 0},  //TV = 8.65(63 lines)  AV=2.97  SV=7.61  BV=4.01
    {2644,6144,1720, 0, 0, 0},  //TV = 8.56(67 lines)  AV=2.97  SV=7.62  BV=3.91
    {2841,6144,1728, 0, 0, 0},  //TV = 8.46(72 lines)  AV=2.97  SV=7.63  BV=3.80
    {3078,6144,1704, 0, 0, 0},  //TV = 8.34(78 lines)  AV=2.97  SV=7.61  BV=3.71
    {3276,6144,1712, 0, 0, 0},  //TV = 8.25(83 lines)  AV=2.97  SV=7.61  BV=3.61
    {3512,6144,1712, 0, 0, 0},  //TV = 8.15(89 lines)  AV=2.97  SV=7.61  BV=3.51
    {3788,6144,1712, 0, 0, 0},  //TV = 8.04(96 lines)  AV=2.97  SV=7.61  BV=3.40
    {4065,6144,1704, 0, 0, 0},  //TV = 7.94(103 lines)  AV=2.97  SV=7.61  BV=3.31
    {4341,6144,1712, 0, 0, 0},  //TV = 7.85(110 lines)  AV=2.97  SV=7.61  BV=3.21
    {4657,6144,1704, 0, 0, 0},  //TV = 7.75(118 lines)  AV=2.97  SV=7.61  BV=3.11
    {4972,6144,1712, 0, 0, 0},  //TV = 7.65(126 lines)  AV=2.97  SV=7.61  BV=3.01
    {5367,6144,1704, 0, 0, 0},  //TV = 7.54(136 lines)  AV=2.97  SV=7.61  BV=2.91
    {5761,6144,1704, 0, 0, 0},  //TV = 7.44(146 lines)  AV=2.97  SV=7.61  BV=2.80
    {6156,6144,1704, 0, 0, 0},  //TV = 7.34(156 lines)  AV=2.97  SV=7.61  BV=2.71
    {6590,6144,1704, 0, 0, 0},  //TV = 7.25(167 lines)  AV=2.97  SV=7.61  BV=2.61
    {7063,6144,1704, 0, 0, 0},  //TV = 7.15(179 lines)  AV=2.97  SV=7.61  BV=2.51
    {7576,6144,1704, 0, 0, 0},  //TV = 7.04(192 lines)  AV=2.97  SV=7.61  BV=2.41
    {8089,6144,1712, 0, 0, 0},  //TV = 6.95(205 lines)  AV=2.97  SV=7.61  BV=2.31
    {8326,6144,1776, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.67  BV=2.21
    {8326,6144,1904, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.77  BV=2.11
    {8326,6144,2040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.87  BV=2.01
    {8326,6144,2200, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=7.98  BV=1.90
    {8326,6144,2360, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=8.08  BV=1.80
    {8326,6144,2528, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=8.18  BV=1.70
    {8326,6144,2712, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=8.28  BV=1.60
    {8326,6144,2904, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=8.38  BV=1.50
    {8326,6144,3112, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=8.48  BV=1.40
    {8326,6144,3328, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=8.57  BV=1.31
    {16652,6144,1784, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.67  BV=1.21
    {16652,6144,1912, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.77  BV=1.11
    {16652,6144,2048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.87  BV=1.01
    {16652,6144,2192, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=7.97  BV=0.91
    {16652,6144,2352, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=8.07  BV=0.81
    {16652,6144,2520, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=8.17  BV=0.71
    {25017,6144,1800, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=7.69  BV=0.61
    {25017,6144,1928, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=7.78  BV=0.51
    {25017,6144,2064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=7.88  BV=0.41
    {25017,6144,2216, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=7.99  BV=0.31
    {33343,6144,1776, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=7.67  BV=0.21
    {33343,6144,1904, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=7.77  BV=0.11
    {33343,6144,2056, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=7.88  BV=-0.00
    {41669,6144,1760, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=7.65  BV=-0.10
    {41669,6144,1888, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=7.75  BV=-0.20
    {41669,6144,2024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=7.85  BV=-0.30
    {49994,6144,1808, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=7.69  BV=-0.40
    {49994,6144,1936, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=7.79  BV=-0.50
    {58319,6144,1784, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=7.67  BV=-0.60
    {58319,6144,1912, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=7.77  BV=-0.70
    {66645,6144,1792, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=7.68  BV=-0.80
    {75011,6144,1704, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=7.61  BV=-0.90
    {75011,6144,1824, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=7.70  BV=-1.00
    {83336,6144,1760, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=7.65  BV=-1.10
    {91661,6144,1720, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=7.62  BV=-1.20
    {91661,6144,1840, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=7.72  BV=-1.30
    {99987,6144,1808, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=7.69  BV=-1.40
    {108313,6144,1792, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=7.68  BV=-1.50
    {116678,6144,1776, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=7.67  BV=-1.60
    {125004,6144,1776, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=7.67  BV=-1.70
    {133330,6144,1792, 0, 0, 0},  //TV = 2.91(3379 lines)  AV=2.97  SV=7.68  BV=-1.80
    {149980,6144,1704, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=7.61  BV=-1.90
    {158346,6144,1728, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=7.63  BV=-2.00
    {166672,6144,1760, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=7.65  BV=-2.10
    {183322,6144,1712, 0, 0, 0},  //TV = 2.45(4646 lines)  AV=2.97  SV=7.61  BV=-2.19
    {191648,6144,1760, 0, 0, 0},  //TV = 2.38(4857 lines)  AV=2.97  SV=7.65  BV=-2.30
    {208339,6144,1736, 0, 0, 0},  //TV = 2.26(5280 lines)  AV=2.97  SV=7.63  BV=-2.40
    {224990,6144,1720, 0, 0, 0},  //TV = 2.15(5702 lines)  AV=2.97  SV=7.62  BV=-2.50
    {241641,6144,1720, 0, 0, 0},  //TV = 2.05(6124 lines)  AV=2.97  SV=7.62  BV=-2.60
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO1600PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {197,1140,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.48  BV=10.81
    {197,1216,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.57  BV=10.71
    {197,1328,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.66  BV=10.62
    {197,1424,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.76  BV=10.52
    {197,1424,1104, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.87  BV=10.41
    {197,1632,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=4.97  BV=10.31
    {197,1728,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.06  BV=10.22
    {197,1840,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.18  BV=10.10
    {197,1936,1072, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.27  BV=10.01
    {197,2144,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.36  BV=9.92
    {197,2240,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.47  BV=9.81
    {197,2448,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.57  BV=9.71
    {197,2656,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.67  BV=9.61
    {197,2752,1064, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.77  BV=9.51
    {197,3072,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.87  BV=9.41
    {197,3200,1056, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=5.98  BV=9.31
    {197,3456,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.06  BV=9.22
    {197,3712,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.17  BV=9.11
    {197,3968,1048, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.27  BV=9.01
    {197,4352,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.37  BV=8.91
    {197,4608,1032, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.47  BV=8.81
    {197,4992,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.57  BV=8.71
    {197,5248,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.67  BV=8.61
    {197,5632,1040, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.77  BV=8.51
    {197,6144,1024, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.87  BV=8.41
    {197,6144,1096, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=6.97  BV=8.31
    {197,6144,1176, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.07  BV=8.21
    {197,6144,1264, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.18  BV=8.10
    {197,6144,1352, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.27  BV=8.01
    {197,6144,1448, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.37  BV=7.91
    {197,6144,1552, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.47  BV=7.81
    {197,6144,1664, 0, 0, 0},  //TV = 12.31(4 lines)  AV=2.97  SV=7.57  BV=7.71
    {198,6144,1776, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=7.67  BV=7.61
    {198,6144,1904, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=7.77  BV=7.51
    {237,6144,1704, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.61  BV=7.41
    {237,6144,1824, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.70  BV=7.31
    {237,6144,1952, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=7.80  BV=7.21
    {277,6144,1792, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=7.68  BV=7.11
    {277,6144,1920, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=7.78  BV=7.01
    {316,6144,1800, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=7.69  BV=6.91
    {356,6144,1720, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=7.62  BV=6.81
    {356,6144,1840, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=7.72  BV=6.71
    {395,6144,1776, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=7.67  BV=6.61
    {435,6144,1728, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=7.63  BV=6.51
    {474,6144,1704, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=7.61  BV=6.41
    {474,6144,1824, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=7.70  BV=6.31
    {513,6144,1800, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=7.69  BV=6.21
    {553,6144,1800, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=7.69  BV=6.11
    {592,6144,1792, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=7.68  BV=6.01
    {632,6144,1800, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=7.69  BV=5.91
    {711,6144,1712, 0, 0, 0},  //TV = 10.46(18 lines)  AV=2.97  SV=7.61  BV=5.82
    {750,6144,1752, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=7.65  BV=5.71
    {790,6144,1776, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=7.67  BV=5.61
    {869,6144,1728, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=7.63  BV=5.51
    {947,6144,1704, 0, 0, 0},  //TV = 10.04(24 lines)  AV=2.97  SV=7.61  BV=5.41
    {987,6144,1752, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=7.65  BV=5.31
    {1066,6144,1736, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=7.63  BV=5.21
    {1145,6144,1728, 0, 0, 0},  //TV = 9.77(29 lines)  AV=2.97  SV=7.63  BV=5.11
    {1224,6144,1744, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=7.64  BV=5.01
    {1342,6144,1704, 0, 0, 0},  //TV = 9.54(34 lines)  AV=2.97  SV=7.61  BV=4.91
    {1421,6144,1720, 0, 0, 0},  //TV = 9.46(36 lines)  AV=2.97  SV=7.62  BV=4.81
    {1539,6144,1712, 0, 0, 0},  //TV = 9.34(39 lines)  AV=2.97  SV=7.61  BV=4.70
    {1618,6144,1736, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=7.63  BV=4.61
    {1737,6144,1728, 0, 0, 0},  //TV = 9.17(44 lines)  AV=2.97  SV=7.63  BV=4.51
    {1894,6144,1704, 0, 0, 0},  //TV = 9.04(48 lines)  AV=2.97  SV=7.61  BV=4.41
    {2013,6144,1720, 0, 0, 0},  //TV = 8.96(51 lines)  AV=2.97  SV=7.62  BV=4.31
    {2171,6144,1712, 0, 0, 0},  //TV = 8.85(55 lines)  AV=2.97  SV=7.61  BV=4.20
    {2329,6144,1704, 0, 0, 0},  //TV = 8.75(59 lines)  AV=2.97  SV=7.61  BV=4.11
    {2486,6144,1712, 0, 0, 0},  //TV = 8.65(63 lines)  AV=2.97  SV=7.61  BV=4.01
    {2644,6144,1720, 0, 0, 0},  //TV = 8.56(67 lines)  AV=2.97  SV=7.62  BV=3.91
    {2841,6144,1728, 0, 0, 0},  //TV = 8.46(72 lines)  AV=2.97  SV=7.63  BV=3.80
    {3078,6144,1704, 0, 0, 0},  //TV = 8.34(78 lines)  AV=2.97  SV=7.61  BV=3.71
    {3276,6144,1712, 0, 0, 0},  //TV = 8.25(83 lines)  AV=2.97  SV=7.61  BV=3.61
    {3512,6144,1712, 0, 0, 0},  //TV = 8.15(89 lines)  AV=2.97  SV=7.61  BV=3.51
    {3788,6144,1712, 0, 0, 0},  //TV = 8.04(96 lines)  AV=2.97  SV=7.61  BV=3.40
    {4065,6144,1704, 0, 0, 0},  //TV = 7.94(103 lines)  AV=2.97  SV=7.61  BV=3.31
    {4341,6144,1712, 0, 0, 0},  //TV = 7.85(110 lines)  AV=2.97  SV=7.61  BV=3.21
    {4657,6144,1704, 0, 0, 0},  //TV = 7.75(118 lines)  AV=2.97  SV=7.61  BV=3.11
    {4972,6144,1712, 0, 0, 0},  //TV = 7.65(126 lines)  AV=2.97  SV=7.61  BV=3.01
    {5367,6144,1704, 0, 0, 0},  //TV = 7.54(136 lines)  AV=2.97  SV=7.61  BV=2.91
    {5761,6144,1704, 0, 0, 0},  //TV = 7.44(146 lines)  AV=2.97  SV=7.61  BV=2.80
    {6156,6144,1704, 0, 0, 0},  //TV = 7.34(156 lines)  AV=2.97  SV=7.61  BV=2.71
    {6590,6144,1704, 0, 0, 0},  //TV = 7.25(167 lines)  AV=2.97  SV=7.61  BV=2.61
    {7063,6144,1704, 0, 0, 0},  //TV = 7.15(179 lines)  AV=2.97  SV=7.61  BV=2.51
    {7576,6144,1704, 0, 0, 0},  //TV = 7.04(192 lines)  AV=2.97  SV=7.61  BV=2.41
    {8089,6144,1712, 0, 0, 0},  //TV = 6.95(205 lines)  AV=2.97  SV=7.61  BV=2.31
    {8681,6144,1704, 0, 0, 0},  //TV = 6.85(220 lines)  AV=2.97  SV=7.61  BV=2.21
    {9313,6144,1704, 0, 0, 0},  //TV = 6.75(236 lines)  AV=2.97  SV=7.61  BV=2.11
    {9944,6144,1712, 0, 0, 0},  //TV = 6.65(252 lines)  AV=2.97  SV=7.61  BV=2.01
    {9983,6144,1840, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.72  BV=1.90
    {9983,6144,1968, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.81  BV=1.80
    {9983,6144,2104, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=7.91  BV=1.71
    {9983,6144,2256, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=8.01  BV=1.61
    {9983,6144,2416, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=8.11  BV=1.51
    {9983,6144,2592, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=8.21  BV=1.41
    {9983,6144,2776, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=8.31  BV=1.31
    {9983,6144,2976, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=8.41  BV=1.21
    {9983,6144,3192, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=8.51  BV=1.11
    {20006,6144,1704, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.61  BV=1.01
    {20006,6144,1824, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.70  BV=0.91
    {20006,6144,1960, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.81  BV=0.81
    {20006,6144,2096, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=7.91  BV=0.71
    {20006,6144,2248, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=8.01  BV=0.61
    {20006,6144,2408, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=8.11  BV=0.51
    {29989,6144,1720, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.62  BV=0.41
    {29989,6144,1848, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.72  BV=0.31
    {29989,6144,1976, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.82  BV=0.21
    {29989,6144,2120, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=7.92  BV=0.11
    {40011,6144,1712, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=7.61  BV=0.00
    {40011,6144,1840, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=7.72  BV=-0.10
    {40011,6144,1968, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=7.81  BV=-0.20
    {40011,6144,2112, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=7.92  BV=-0.30
    {49994,6144,1808, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=7.69  BV=-0.40
    {49994,6144,1936, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=7.79  BV=-0.50
    {60017,6144,1728, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=7.63  BV=-0.60
    {60017,6144,1856, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=7.73  BV=-0.70
    {69999,6144,1704, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=7.61  BV=-0.80
    {69999,6144,1824, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=7.70  BV=-0.90
    {79982,6144,1712, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=7.61  BV=-1.00
    {79982,6144,1832, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=7.71  BV=-1.10
    {90005,6144,1752, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=7.65  BV=-1.20
    {90005,6144,1872, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=7.74  BV=-1.30
    {99987,6144,1808, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=7.69  BV=-1.40
    {110010,6144,1760, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=7.65  BV=-1.50
    {119992,6144,1728, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=7.63  BV=-1.60
    {130015,6144,1712, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=7.61  BV=-1.70
    {130015,6144,1832, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=7.71  BV=-1.80
    {139997,6144,1824, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=7.70  BV=-1.90
    {160003,6144,1712, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=7.61  BV=-2.00
    {169986,6144,1728, 0, 0, 0},  //TV = 2.56(4308 lines)  AV=2.97  SV=7.63  BV=-2.10
    {180008,6144,1744, 0, 0, 0},  //TV = 2.47(4562 lines)  AV=2.97  SV=7.64  BV=-2.20
    {189991,6144,1776, 0, 0, 0},  //TV = 2.40(4815 lines)  AV=2.97  SV=7.67  BV=-2.30
    {209996,6144,1720, 0, 0, 0},  //TV = 2.25(5322 lines)  AV=2.97  SV=7.62  BV=-2.40
    {220019,6144,1760, 0, 0, 0},  //TV = 2.18(5576 lines)  AV=2.97  SV=7.65  BV=-2.50
    {239984,6144,1728, 0, 0, 0},  //TV = 2.06(6082 lines)  AV=2.97  SV=7.63  BV=-2.60
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO1600Table =
{
    AETABLE_CAPTURE_ISO1600,    //eAETableID
    136,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -26,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_1600, //ISO SPEED
    sCaptureISO1600PLineTable_60Hz,
    sCaptureISO1600PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureStrobePLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureStrobePLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_StrobeTable =
{
    AETABLE_STROBE,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCaptureStrobePLineTable_60Hz,
    sCaptureStrobePLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene1PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene1PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable1 =
{
    AETABLE_SCENE_INDEX1,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    -10,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene1PLineTable_60Hz,
    sAEScene1PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene2PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene2PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable2 =
{
    AETABLE_SCENE_INDEX2,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene2PLineTable_60Hz,
    sAEScene2PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene3PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene3PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable3 =
{
    AETABLE_SCENE_INDEX3,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene3PLineTable_60Hz,
    sAEScene3PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene4PLineTable_60Hz =
{
{
    {250007,1632,1072, 0, 0, 0},  //TV = 2.00(6336 lines)  AV=2.97  SV=5.03  BV=-0.05
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene4PLineTable_50Hz =
{
{
    {250007,1632,1072, 0, 0, 0},  //TV = 2.00(6336 lines)  AV=2.97  SV=5.03  BV=-0.05
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable4 =
{
    AETABLE_SCENE_INDEX4,    //eAETableID
    1,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    0,    //i4MaxBV
    0,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene4PLineTable_60Hz,
    sAEScene4PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene5PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene5PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable5 =
{
    AETABLE_SCENE_INDEX5,    //eAETableID
    81,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    29,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene5PLineTable_60Hz,
    sAEScene5PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene6PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {58319,2144,1024, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.35  BV=1.72
    {58319,2240,1056, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.46  BV=1.61
    {66645,2144,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.36  BV=1.51
    {66645,2240,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.47  BV=1.41
    {66645,2448,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.57  BV=1.31
    {66645,2656,1032, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.67  BV=1.21
    {66645,2752,1064, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.77  BV=1.11
    {66645,2960,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.86  BV=1.02
    {66645,3200,1048, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.96  BV=0.91
    {66645,3456,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.06  BV=0.81
    {66645,3712,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.17  BV=0.71
    {66645,3968,1040, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.26  BV=0.61
    {66645,4352,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=6.37  BV=0.50
    {75011,4096,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.30  BV=0.41
    {75011,4352,1040, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.40  BV=0.31
    {83336,4224,1040, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.35  BV=0.20
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {108313,4352,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.37  BV=-0.20
    {116678,4352,1024, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.37  BV=-0.30
    {125004,4352,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.37  BV=-0.40
    {141655,4096,1032, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=6.30  BV=-0.51
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {158346,4224,1024, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=6.33  BV=-0.70
    {166672,4224,1048, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.36  BV=-0.81
    {166672,4608,1024, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.46  BV=-0.90
    {166672,4864,1040, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.56  BV=-1.00
    {166672,5248,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.66  BV=-1.10
    {166672,5632,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.76  BV=-1.20
    {166672,6016,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.85  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene6PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,1936,1080, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.28  BV=2.01
    {49994,2144,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.38  BV=1.92
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {60017,2048,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.32  BV=1.71
    {60017,2240,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.42  BV=1.61
    {60017,2352,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.52  BV=1.51
    {60017,2560,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.62  BV=1.41
    {60017,2752,1032, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.72  BV=1.30
    {60017,2960,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.82  BV=1.21
    {60017,3072,1056, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.92  BV=1.11
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {60017,3584,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.12  BV=0.91
    {60017,3840,1040, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.22  BV=0.81
    {60017,4096,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.32  BV=0.71
    {60017,4480,1024, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.42  BV=0.61
    {69999,4096,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.30  BV=0.51
    {69999,4352,1040, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.40  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.41  BV=0.21
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4224,1024, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {130015,4096,1048, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=6.32  BV=-0.41
    {139997,4096,1040, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=6.31  BV=-0.50
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {160003,4096,1048, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.32  BV=-0.71
    {160003,4480,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.42  BV=-0.80
    {160003,4736,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.52  BV=-0.90
    {160003,5120,1032, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.62  BV=-1.01
    {160003,5504,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.71  BV=-1.10
    {160003,5888,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.81  BV=-1.20
    {160003,6144,1056, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.92  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable6 =
{
    AETABLE_SCENE_INDEX6,    //eAETableID
    123,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -13,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene6PLineTable_60Hz,
    sAEScene6PLineTable_50Hz,
    NULL,
};

static strEvPline sAESceneReservePLineTable =
{
{
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};
static strAETable g_AE_ReserveSceneTable =
{
	AETABLE_SCENE_MAX,    //eAETableID
	0,    //u4TotalIndex
	20,    //u4StrobeTrigerBV
	0,    //i4MaxBV
	0,    //i4MinBV
	90,    //i4EffectiveMaxBV
	0,      //i4EffectiveMinBV
	LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
	sAESceneReservePLineTable,
	sAESceneReservePLineTable,
	NULL,
};
static strAESceneMapping g_AEScenePLineMapping = 
{
{
    {LIB3A_AE_SCENE_AUTO, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX5}},
    {LIB3A_AE_SCENE_NIGHT, {AETABLE_SCENE_INDEX6, AETABLE_SCENE_INDEX6, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_ACTION, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_BEACH, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_CANDLELIGHT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_FIREWORKS, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX4, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX4}},
    {LIB3A_AE_SCENE_LANDSCAPE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_PORTRAIT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_NIGHT_PORTRAIT, {AETABLE_SCENE_INDEX6, AETABLE_SCENE_INDEX6, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_PARTY, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_SNOW, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_SPORTS, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_STEADYPHOTO, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_SUNSET, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_THEATRE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_ISO_ANTI_SHAKE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_ISO100, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO100, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO100}},
    {LIB3A_AE_SCENE_ISO200, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO200, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO200}},
    {LIB3A_AE_SCENE_ISO400, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO400, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO400}},
    {LIB3A_AE_SCENE_ISO800, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO800, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO800}},
    {LIB3A_AE_SCENE_ISO1600 , {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO1600, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO1600}},
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

static strAEPLineTable g_strAEPlineTable =
{
{
// PLINE Table
g_AE_PreviewAutoTable,
g_AE_CaptureAutoTable,
g_AE_VideoAutoTable,
g_AE_Video1AutoTable,
g_AE_Video2AutoTable,
g_AE_Custom1AutoTable,
g_AE_Custom2AutoTable,
g_AE_Custom3AutoTable,
g_AE_Custom4AutoTable,
g_AE_Custom5AutoTable,
g_AE_VideoNightTable,
g_AE_CaptureISO100Table,
g_AE_CaptureISO200Table,
g_AE_CaptureISO400Table,
g_AE_CaptureISO800Table,
g_AE_CaptureISO1600Table,
g_AE_StrobeTable,
g_AE_SceneTable1,
g_AE_SceneTable2,
g_AE_SceneTable3,
g_AE_SceneTable4,
g_AE_SceneTable5,
g_AE_SceneTable6,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
},
};

static strAEPLineNumInfo g_strAEPreviewAutoPLineInfo =
{
    AETABLE_RPEVIEW_AUTO,
    90,
    -10,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureAutoPLineInfo =
{
    AETABLE_CAPTURE_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideoAutoPLineInfo =
{
    AETABLE_VIDEO_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideo1AutoPLineInfo =
{
    AETABLE_VIDEO1_AUTO,
    90,
    0,
    {
        {1,5000,120,1136,1136},
        {2,120,120,1136,6144},
        {3,0,0,1136,1136},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideo2AutoPLineInfo =
{
    AETABLE_VIDEO2_AUTO,
    90,
    0,
    {
        {1,5000,120,1136,1136},
        {2,120,120,1136,6144},
        {3,0,0,1136,1136},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom1AutoPLineInfo =
{
    AETABLE_CUSTOM1_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom2AutoPLineInfo =
{
    AETABLE_CUSTOM2_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom3AutoPLineInfo =
{
    AETABLE_CUSTOM3_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom4AutoPLineInfo =
{
    AETABLE_CUSTOM4_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom5AutoPLineInfo =
{
    AETABLE_CUSTOM5_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideoNightPLineInfo =
{
    AETABLE_VIDEO_NIGHT,
    90,
    -10,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,6,4096,4096},
        {6,6,6,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO100PLineInfo =
{
    AETABLE_CAPTURE_ISO100,
    90,
    0,
    {
        {1,5000,4,1136,1136},
        {2,0,0,1136,1136},
        {3,0,0,0,0},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO200PLineInfo =
{
    AETABLE_CAPTURE_ISO200,
    90,
    0,
    {
        {1,5000,5000,1136,1136},
        {2,5000,5000,1136,2048},
        {3,5000,4,2048,2048},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO400PLineInfo =
{
    AETABLE_CAPTURE_ISO400,
    90,
    0,
    {
        {1,5000,5000,1136,1136},
        {2,5000,5000,1136,4096},
        {3,5000,4,4096,4096},
        {4,0,0,1136,1136},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO800PLineInfo =
{
    AETABLE_CAPTURE_ISO800,
    90,
    0,
    {
        {1,5000,5000,1136,1136},
        {2,5000,5000,1136,6144},
        {3,5000,4,6144,6144},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO1600PLineInfo =
{
    AETABLE_CAPTURE_ISO1600,
    90,
    0,
    {
        {1,5000,5000,1136,1136},
        {2,5000,5000,1136,10240},
        {3,5000,4,10240,10240},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEStrobePLineInfo =
{
    AETABLE_STROBE,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene1PLineInfo =
{
    AETABLE_SCENE_INDEX1,
    90,
    -10,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene2PLineInfo =
{
    AETABLE_SCENE_INDEX2,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene3PLineInfo =
{
    AETABLE_SCENE_INDEX3,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene4PLineInfo =
{
    AETABLE_SCENE_INDEX4,
    90,
    0,
    {
        {1,4,4,1706,1706},
        {2,0,0,1136,1136},
        {3,0,0,0,0},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene5PLineInfo =
{
    AETABLE_SCENE_INDEX5,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,0,0,1136,1136},
        {3,0,0,0,0},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene6PLineInfo =
{
    AETABLE_SCENE_INDEX6,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,6,4096,4096},
        {6,6,6,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene7PLineInfo =
{
    AETABLE_SCENE_INDEX7,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene8PLineInfo =
{
    AETABLE_SCENE_INDEX8,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene9PLineInfo =
{
    AETABLE_SCENE_INDEX9,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene10PLineInfo =
{
    AETABLE_SCENE_INDEX10,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene11PLineInfo =
{
    AETABLE_SCENE_INDEX11,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,2048},
        {3,20,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};























static strAEPLineNumInfo g_strAENoScenePLineInfo =
{
    AETABLE_SCENE_MAX,
    90,
    0,
    {
        {1,0,0,0,0},
        {2,0,0,0,0},
        {3,0,0,0,0},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};
static strAEPLineInfomation g_strAEPlineInfo =
{
MTRUE,   // FALSE mean the sampling
{
// PLINE Info
g_strAEPreviewAutoPLineInfo,
g_strAECaptureAutoPLineInfo,
g_strAEVideoAutoPLineInfo,
g_strAEVideo1AutoPLineInfo,
g_strAEVideo2AutoPLineInfo,
g_strAECustom1AutoPLineInfo,
g_strAECustom2AutoPLineInfo,
g_strAECustom3AutoPLineInfo,
g_strAECustom4AutoPLineInfo,
g_strAECustom5AutoPLineInfo,
g_strAEVideoNightPLineInfo,
g_strAECaptureISO100PLineInfo,
g_strAECaptureISO200PLineInfo,
g_strAECaptureISO400PLineInfo,
g_strAECaptureISO800PLineInfo,
g_strAECaptureISO1600PLineInfo,
g_strAEStrobePLineInfo,
g_strAEScene1PLineInfo,
g_strAEScene2PLineInfo,
g_strAEScene3PLineInfo,
g_strAEScene4PLineInfo,
g_strAEScene5PLineInfo,
g_strAEScene6PLineInfo,
g_strAEScene7PLineInfo,
g_strAEScene8PLineInfo,
g_strAEScene9PLineInfo,
g_strAEScene10PLineInfo,
g_strAEScene11PLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
},
};

static strAEPLineGainList g_strAEGainList =
{
71,
{
{1140,100},
{1216,100},
{1328,100},
{1424,100},
{1536,100},
{1632,100},
{1728,100},
{1840,100},
{1936,100},
{2048,100},
{2144,100},
{2240,100},
{2352,100},
{2448,100},
{2560,100},
{2656,100},
{2752,100},
{2864,100},
{2960,100},
{3072,100},
{3200,100},
{3328,100},
{3456,100},
{3584,100},
{3712,100},
{3840,100},
{3968,100},
{4096,100},
{4224,100},
{4352,100},
{4480,100},
{4608,100},
{4736,100},
{4864,100},
{4992,100},
{5120,100},
{5248,100},
{5376,100},
{5504,100},
{5632,100},
{5760,100},
{5888,100},
{6016,100},
{6144,100},
{6240,100},
{6384,100},
{6544,100},
{6704,100},
{6896,100},
{7072,100},
{7280,100},
{7472,100},
{7696,100},
{7936,100},
{8192,100},
{8448,100},
{8720,100},
{9040,100},
{9344,100},
{9696,100},
{10080,100},
{10480,100},
{10912,100},
{11392,100},
{11904,100},
{12480,100},
{13104,100},
{13792,100},
{14560,100},
{15408,100},
{16384,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
}
};

static AE_PLINETABLE_T g_PlineTableMapping =
{
g_AEScenePLineMapping,
g_strAEPlineTable,
g_strAEPlineInfo,
g_strAEGainList
};
#endif


#if 0 //Save the P-line info to file for debug
MinGain,1136 
MaxGain,6144 
MiniISOGain,61 
GainStepUnitInTotalRange,128 
PreviewExposureLineUnit,39458 
PreviewMaxFrameRate,20 
VideoExposureLineUnit,39458 
VideoMaxFrameRate,20 
VideoToPreviewSensitivityRatio,1024 
CaptureExposureLineUnit,39458 
CaptureMaxFrameRate,20 
CaptureToPreviewSensitivityRatio,1024 
Video1ExposureLineUnit,39458 
Video1MaxFrameRate,20 
Video1ToPreviewSensitivityRatio,1024 
Video2ExposureLineUnit,39458 
Video2MaxFrameRate,20 
Video2ToPreviewSensitivityRatio,1024 
Custom1ExposureLineUnit,39458 
Custom1MaxFrameRate,20 
Custom1ToPreviewSensitivityRatio,1024 
Custom2ExposureLineUnit,39458 
Custom2MaxFrameRate,20 
Custom2ToPreviewSensitivityRatio,1024 
Custom3ExposureLineUnit,39458 
Custom3MaxFrameRate,20 
Custom3ToPreviewSensitivityRatio,1024 
Custom4ExposureLineUnit,39458 
Custom4MaxFrameRate,20 
Custom4ToPreviewSensitivityRatio,1024 
Custom5ExposureLineUnit,39458 
Custom5MaxFrameRate,20 
Custom5ToPreviewSensitivityRatio,1024 
FocusLength,350 
Fno,28 

// Preview table -- Use preview sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_RPEVIEW_AUTO
90,-10
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Capture table -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video table -- Use video sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video1 table -- Use Video1 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO1_AUTO
90,0
1,5000,120,1136,1136
2,120,120,1136,6144
AETABLE_END

// Video2 table -- Use Video2 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO2_AUTO
90,0
1,5000,120,1136,1136
2,120,120,1136,6144
AETABLE_END

// Custom1 table -- Use Custom1 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM1_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom2 table -- Use Custom2 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM2_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom3 table -- Use Custom3 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM3_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom4 table -- Use Custom4 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM4_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom5 table -- Use Custom5 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM5_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video Night table -- Use video sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO_NIGHT
90,-10
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,6,4096,4096
6,6,6,4096,6144
AETABLE_END

// Capture ISO100 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO100
90,0
1,5000,4,1136,1136
AETABLE_END

// Capture ISO200 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO200
90,0
1,5000,5000,1136,1136
2,5000,5000,1136,2048
3,5000,4,2048,2048
AETABLE_END

// Capture ISO400 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO400
90,0
1,5000,5000,1136,1136
2,5000,5000,1136,4096
3,5000,4,4096,4096
AETABLE_END

// Capture ISO800 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO800
90,0
1,5000,5000,1136,1136
2,5000,5000,1136,6144
3,5000,4,6144,6144
AETABLE_END

// Capture ISO1600 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO1600
90,0
1,5000,5000,1136,1136
2,5000,5000,1136,10240
3,5000,4,10240,10240
AETABLE_END

// Strobe table -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_STROBE
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END


//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX1
90,-10
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END


//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX2
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table1 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX3
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table2 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX4
90,0
1,4,4,1706,1706
AETABLE_END

// Table3 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX5
90,0
1,5000,20,1136,1136
AETABLE_END

// Table4 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX6
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,6,4096,4096
6,6,6,4096,6144
AETABLE_END

// Table5 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX7
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table6 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX8
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table7 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX9
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table8 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX10
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table9 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX11
90,0
1,5000,20,1136,1136
2,20,20,1136,2048
3,20,15,2048,2048
4,15,15,2048,4096
5,15,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table10 PLine -- Use capture sensor setting

// Table11 PLine -- Use capture sensor setting

// Table12 PLine -- Use capture sensor setting

// Table13 PLine -- Use capture sensor setting

// Table14 PLine -- Use capture sensor setting

// Table15 PLine -- Use capture sensor setting

// Table16 PLine -- Use capture sensor setting

// Table17 PLine -- Use capture sensor setting

// Table18 PLine -- Use capture sensor setting

// Table19 PLine -- Use capture sensor setting

// Table20 PLine -- Use capture sensor setting

// Table21 PLine -- Use capture sensor setting

// Table22 PLine -- Use capture sensor setting

// Table23 PLine -- Use capture sensor setting

// Table24 PLine -- Use capture sensor setting

// Table25 PLine -- Use capture sensor setting

// Table26 PLine -- Use capture sensor setting

// Table27 PLine -- Use capture sensor setting

// Table28 PLine -- Use capture sensor setting

// Table29 PLine -- Use capture sensor setting

// Table30 PLine -- Use capture sensor setting

// Table31 PLine -- Use capture sensor setting

AE_SCENE_AUTO,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX5
AE_SCENE_NIGHT,AETABLE_SCENE_INDEX6,AETABLE_SCENE_INDEX6,AETABLE_VIDEO_NIGHT,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_ACTION,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_BEACH,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_CANDLELIGHT,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX1,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_FIREWORKS,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX4,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX4
AE_SCENE_LANDSCAPE,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_PORTRAIT,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_NIGHT_PORTRAIT,AETABLE_SCENE_INDEX6,AETABLE_SCENE_INDEX6,AETABLE_VIDEO_NIGHT,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_PARTY,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX1,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_SNOW,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_SPORTS,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_STEADYPHOTO,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_SUNSET,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_THEATRE,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX1,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_ISO_ANTI_SHAKE,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_ISO100,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO100,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO100
AE_SCENE_ISO200,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO200,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO200
AE_SCENE_ISO400,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO400,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO400
AE_SCENE_ISO800,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO800,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO800
AE_SCENE_ISO1600 ,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO1600,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO1600
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
#endif
