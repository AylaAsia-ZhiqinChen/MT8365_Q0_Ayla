/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2016. All rights reserved.
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
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef _CAMERA_AE_PLINETABLE_GC2375RAWRAW_H
#define _CAMERA_AE_PLINETABLE_GC2375RAWRAW_H

#include <custom/aaa/AEPlinetable.h>
static constexpr strEvPline sPreviewPLineTable_60Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8321,1088,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.62  BV=4.26
    {8321,1152,1040, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.73  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {16668,1088,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.62  BV=3.25
    {16668,1152,1040, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {24987,1024,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.55  BV=2.75
    {24987,1088,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.63  BV=2.66
    {24987,1184,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.75  BV=2.55
    {24987,1248,1040, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.84  BV=2.45
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {33335,1088,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.62  BV=2.25
    {33335,1152,1040, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.73  BV=2.15
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1664,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.24  BV=1.64
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33335,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33335,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33335,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33335,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33335,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {41654,2816,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.01  BV=0.55
    {41654,3040,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.11  BV=0.45
    {41654,3264,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.21  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {58321,2848,1032, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.02  BV=0.05
    {58321,3072,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.12  BV=-0.05
    {66668,2880,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.03  BV=-0.15
    {66668,3072,1032, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.13  BV=-0.25
    {66668,3296,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.22  BV=-0.34
    {66668,3552,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.33  BV=-0.45
    {66668,3808,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.43  BV=-0.55
    {66668,4064,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.52  BV=-0.65
    {66668,4352,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.62  BV=-0.75
    {66668,4672,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.73  BV=-0.85
    {66668,4992,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.82  BV=-0.94
    {66668,5408,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.94  BV=-1.06
    {66668,5792,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.04  BV=-1.16
    {66668,6176,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.13  BV=-1.25
    {66668,6624,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.23  BV=-1.35
    {66668,7104,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.33  BV=-1.45
    {74988,6784,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.26  BV=-1.56
    {74988,7264,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.36  BV=-1.65
    {83335,7008,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.31  BV=-1.76
    {83335,7488,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.41  BV=-1.85
    {91655,7296,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=8.37  BV=-1.95
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sPreviewPLineTable_50Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8934,1024,1024, 0, 0, 0},  //TV = 6.81(335 lines)  AV=2.97  SV=5.54  BV=4.24
    {9574,1024,1024, 0, 0, 0},  //TV = 6.71(359 lines)  AV=2.97  SV=5.54  BV=4.14
    {10001,1024,1048, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.57  BV=4.05
    {10001,1120,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.67  BV=3.95
    {10001,1184,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.76  BV=3.86
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {20001,1024,1048, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.57  BV=3.05
    {20001,1120,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {30001,1056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.58  BV=2.45
    {30001,1120,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.68  BV=2.35
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1280,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.88  BV=2.15
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1696,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.28  BV=1.75
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1952,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.48  BV=1.55
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2272,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.69  BV=1.34
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.89  BV=1.14
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,2976,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.08  BV=0.95
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {40001,2752,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.96  BV=0.65
    {40001,2944,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.06  BV=0.56
    {40001,3168,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.17  BV=0.45
    {40001,3392,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.26  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {50001,3328,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.24  BV=0.06
    {60001,2976,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.08  BV=-0.05
    {60001,3200,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.18  BV=-0.15
    {70001,2944,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.06  BV=-0.25
    {70001,3136,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.15  BV=-0.34
    {70001,3392,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.26  BV=-0.46
    {70001,3616,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.36  BV=-0.55
    {70001,3872,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.45  BV=-0.65
    {70001,4160,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.56  BV=-0.75
    {70001,4448,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.65  BV=-0.85
    {70001,4768,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.76  BV=-0.95
    {70001,5152,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.87  BV=-1.06
    {70001,5504,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.96  BV=-1.15
    {70001,5888,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.06  BV=-1.25
    {70001,6304,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.16  BV=-1.35
    {70001,6752,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.26  BV=-1.45
    {70001,7264,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.36  BV=-1.56
    {80001,6816,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.27  BV=-1.66
    {80001,7296,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.37  BV=-1.75
    {90002,6944,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.30  BV=-1.85
    {90002,7456,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.40  BV=-1.96
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_PreviewAutoTable =
{
    AETABLE_RPEVIEW_AUTO, //eAETableID
    137, //u4TotalIndex
    20, //i4StrobeTrigerBV
    108, //i4MaxBV
    -29, //i4MinBV
    90, //i4EffectiveMaxBV
    -50, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sPreviewPLineTable_60Hz,
    sPreviewPLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCapturePLineTable_60Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8321,1088,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.62  BV=4.26
    {8321,1152,1040, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.73  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {16668,1088,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.62  BV=3.25
    {16668,1152,1040, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {24987,1024,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.55  BV=2.75
    {24987,1088,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.63  BV=2.66
    {24987,1184,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.75  BV=2.55
    {24987,1248,1040, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.84  BV=2.45
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {33335,1088,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.62  BV=2.25
    {33335,1152,1040, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.73  BV=2.15
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1664,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.24  BV=1.64
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33335,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33335,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33335,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33335,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33335,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {41654,2816,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.01  BV=0.55
    {41654,3040,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.11  BV=0.45
    {41654,3264,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.21  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {58321,2848,1032, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.02  BV=0.05
    {58321,3072,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.12  BV=-0.05
    {66668,2880,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.03  BV=-0.15
    {66668,3072,1032, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.13  BV=-0.25
    {66668,3296,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.22  BV=-0.34
    {66668,3552,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.33  BV=-0.45
    {66668,3808,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.43  BV=-0.55
    {66668,4064,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.52  BV=-0.65
    {66668,4352,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.62  BV=-0.75
    {66668,4672,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.73  BV=-0.85
    {66668,4992,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.82  BV=-0.94
    {66668,5408,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.94  BV=-1.06
    {66668,5792,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.04  BV=-1.16
    {66668,6176,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.13  BV=-1.25
    {66668,6624,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.23  BV=-1.35
    {66668,7104,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.33  BV=-1.45
    {74988,6784,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.26  BV=-1.56
    {74988,7264,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.36  BV=-1.65
    {83335,7008,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.31  BV=-1.76
    {83335,7488,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.41  BV=-1.85
    {91655,7296,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=8.37  BV=-1.95
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCapturePLineTable_50Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8934,1024,1024, 0, 0, 0},  //TV = 6.81(335 lines)  AV=2.97  SV=5.54  BV=4.24
    {9574,1024,1024, 0, 0, 0},  //TV = 6.71(359 lines)  AV=2.97  SV=5.54  BV=4.14
    {10001,1024,1048, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.57  BV=4.05
    {10001,1120,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.67  BV=3.95
    {10001,1184,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.76  BV=3.86
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {20001,1024,1048, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.57  BV=3.05
    {20001,1120,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {30001,1056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.58  BV=2.45
    {30001,1120,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.68  BV=2.35
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1280,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.88  BV=2.15
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1696,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.28  BV=1.75
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1952,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.48  BV=1.55
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2272,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.69  BV=1.34
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.89  BV=1.14
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,2976,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.08  BV=0.95
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {40001,2752,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.96  BV=0.65
    {40001,2944,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.06  BV=0.56
    {40001,3168,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.17  BV=0.45
    {40001,3392,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.26  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {50001,3328,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.24  BV=0.06
    {60001,2976,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.08  BV=-0.05
    {60001,3200,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.18  BV=-0.15
    {70001,2944,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.06  BV=-0.25
    {70001,3136,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.15  BV=-0.34
    {70001,3392,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.26  BV=-0.46
    {70001,3616,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.36  BV=-0.55
    {70001,3872,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.45  BV=-0.65
    {70001,4160,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.56  BV=-0.75
    {70001,4448,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.65  BV=-0.85
    {70001,4768,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.76  BV=-0.95
    {70001,5152,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.87  BV=-1.06
    {70001,5504,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.96  BV=-1.15
    {70001,5888,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.06  BV=-1.25
    {70001,6304,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.16  BV=-1.35
    {70001,6752,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.26  BV=-1.45
    {70001,7264,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.36  BV=-1.56
    {80001,6816,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.27  BV=-1.66
    {80001,7296,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.37  BV=-1.75
    {90002,6944,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.30  BV=-1.85
    {90002,7456,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.40  BV=-1.96
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_CaptureAutoTable =
{
    AETABLE_CAPTURE_AUTO, //eAETableID
    137, //u4TotalIndex
    20, //i4StrobeTrigerBV
    108, //i4MaxBV
    -29, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCapturePLineTable_60Hz,
    sCapturePLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sVideoPLineTable_60Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8321,1088,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.62  BV=4.26
    {8321,1152,1040, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.73  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {16668,1088,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.62  BV=3.25
    {16668,1152,1040, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {24987,1024,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.55  BV=2.75
    {24987,1088,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.63  BV=2.66
    {24987,1184,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.75  BV=2.55
    {24987,1248,1040, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.84  BV=2.45
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {33228,1088,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=5.62  BV=2.26
    {33228,1152,1040, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=5.73  BV=2.15
    {33228,1248,1032, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=5.83  BV=2.05
    {33228,1344,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=5.93  BV=1.95
    {33228,1440,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.03  BV=1.85
    {33228,1536,1032, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.13  BV=1.75
    {33228,1664,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.24  BV=1.65
    {33228,1760,1032, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.33  BV=1.55
    {33228,1920,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.44  BV=1.44
    {33228,2048,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.54  BV=1.35
    {33228,2176,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.62  BV=1.26
    {33228,2336,1032, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.74  BV=1.15
    {33228,2496,1032, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.83  BV=1.05
    {33228,2688,1024, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=6.93  BV=0.95
    {33228,2880,1032, 0, 0, 0},  //TV = 4.91(1246 lines)  AV=2.97  SV=7.04  BV=0.84
    {33254,3104,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.14  BV=0.75
    {33254,3296,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.22  BV=0.66
    {41654,2816,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.01  BV=0.55
    {41654,3040,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.11  BV=0.45
    {41654,3264,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.21  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {58321,2848,1032, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.02  BV=0.05
    {58321,3072,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.12  BV=-0.05
    {66588,2880,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.03  BV=-0.15
    {66588,3072,1032, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.13  BV=-0.25
    {66588,3296,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.22  BV=-0.34
    {66588,3552,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.33  BV=-0.45
    {66588,3808,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.43  BV=-0.55
    {66588,4064,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.52  BV=-0.65
    {66588,4384,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.63  BV=-0.75
    {66588,4672,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.73  BV=-0.85
    {66588,5024,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.83  BV=-0.95
    {66588,5408,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=7.94  BV=-1.06
    {66588,5792,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=8.04  BV=-1.16
    {66588,6208,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=8.14  BV=-1.26
    {66588,6624,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=8.23  BV=-1.35
    {66588,7104,1024, 0, 0, 0},  //TV = 3.91(2497 lines)  AV=2.97  SV=8.33  BV=-1.45
    {74988,6784,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.26  BV=-1.56
    {74988,7264,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.36  BV=-1.65
    {83335,7008,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.31  BV=-1.76
    {83335,7488,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.41  BV=-1.85
    {91655,7296,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=8.37  BV=-1.95
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sVideoPLineTable_50Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8934,1024,1024, 0, 0, 0},  //TV = 6.81(335 lines)  AV=2.97  SV=5.54  BV=4.24
    {9574,1024,1024, 0, 0, 0},  //TV = 6.71(359 lines)  AV=2.97  SV=5.54  BV=4.14
    {10001,1024,1048, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.57  BV=4.05
    {10001,1120,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.67  BV=3.95
    {10001,1184,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.76  BV=3.86
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {20001,1024,1048, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.57  BV=3.05
    {20001,1120,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {30001,1056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.58  BV=2.45
    {30001,1120,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.68  BV=2.35
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1280,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.88  BV=2.15
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1696,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.28  BV=1.75
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1952,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.48  BV=1.55
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2272,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.69  BV=1.34
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.89  BV=1.14
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,2976,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.08  BV=0.95
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {40001,2752,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.96  BV=0.65
    {40001,2944,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.06  BV=0.56
    {40001,3168,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.17  BV=0.45
    {40001,3392,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.26  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {50001,3328,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.24  BV=0.06
    {60001,2976,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.08  BV=-0.05
    {60001,3200,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.18  BV=-0.15
    {70001,2944,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.06  BV=-0.25
    {70001,3136,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.15  BV=-0.34
    {70001,3392,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.26  BV=-0.46
    {70001,3616,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.36  BV=-0.55
    {70001,3872,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.45  BV=-0.65
    {70001,4160,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.56  BV=-0.75
    {70001,4448,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.65  BV=-0.85
    {70001,4768,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.76  BV=-0.95
    {70001,5152,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.87  BV=-1.06
    {70001,5504,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.96  BV=-1.15
    {70001,5888,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.06  BV=-1.25
    {70001,6304,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.16  BV=-1.35
    {70001,6752,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.26  BV=-1.45
    {70001,7264,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.36  BV=-1.56
    {80001,6816,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.27  BV=-1.66
    {80001,7296,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.37  BV=-1.75
    {90002,6944,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.30  BV=-1.85
    {90002,7456,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.40  BV=-1.96
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_VideoAutoTable =
{
    AETABLE_VIDEO_AUTO, //eAETableID
    137, //u4TotalIndex
    20, //i4StrobeTrigerBV
    108, //i4MaxBV
    -29, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sVideoPLineTable_60Hz,
    sVideoPLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sVideo1PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8241,1248,1040, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=5.84  BV=4.05
    {8241,1344,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=5.94  BV=3.95
    {8241,1440,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.04  BV=3.85
    {8241,1568,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.15  BV=3.74
    {8241,1664,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.25  BV=3.65
    {8241,1792,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.34  BV=3.55
    {8241,1920,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.44  BV=3.45
    {8241,2048,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.55  BV=3.35
    {8241,2208,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.64  BV=3.25
    {8241,2368,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.75  BV=3.15
    {8241,2528,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.84  BV=3.05
    {8241,2720,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.95  BV=2.95
    {8241,2912,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.04  BV=2.85
    {8241,3104,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.14  BV=2.76
    {8241,3360,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.25  BV=2.64
    {8241,3584,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.34  BV=2.55
    {8241,3840,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.44  BV=2.45
    {8241,4128,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.55  BV=2.35
    {8241,4416,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.64  BV=2.25
    {8241,4736,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.75  BV=2.15
    {8241,5056,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.84  BV=2.05
    {8241,5440,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.95  BV=1.95
    {8241,5856,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.05  BV=1.84
    {8241,6272,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.15  BV=1.74
    {8241,6688,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.24  BV=1.65
    {8241,7168,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.34  BV=1.55
    {8241,7680,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.44  BV=1.45
    {8241,8224,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.54  BV=1.35
    {8241,8832,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.64  BV=1.25
    {8241,9440,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.74  BV=1.15
    {8241,10144,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.84  BV=1.05
    {8241,10912,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.95  BV=0.94
    {8241,11712,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.05  BV=0.84
    {8241,12288,1040, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.14  BV=0.75
    {8241,12288,1120, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.25  BV=0.64
    {8241,12288,1200, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.35  BV=0.54
    {8241,12288,1280, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.44  BV=0.45
    {8241,12288,1376, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.55  BV=0.35
    {8241,12288,1472, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.64  BV=0.25
    {8241,12288,1576, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.74  BV=0.15
    {8241,12288,1688, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.84  BV=0.05
    {8241,12288,1816, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.95  BV=-0.05
    {8241,12288,1816, 0, 0, 0},  /* TV = 6.92(309 lines)  AV=2.97  SV=9.95  BV=-0.05 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sVideo1PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8241,1248,1040, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=5.84  BV=4.05
    {8241,1344,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=5.94  BV=3.95
    {8241,1440,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.04  BV=3.85
    {8241,1568,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.15  BV=3.74
    {8241,1664,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.25  BV=3.65
    {8241,1792,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.34  BV=3.55
    {8241,1920,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.44  BV=3.45
    {8241,2048,1032, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.55  BV=3.35
    {8241,2208,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.64  BV=3.25
    {8241,2368,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.75  BV=3.15
    {8241,2528,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.84  BV=3.05
    {8241,2720,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=6.95  BV=2.95
    {8241,2912,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.04  BV=2.85
    {8241,3104,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.14  BV=2.76
    {8241,3360,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.25  BV=2.64
    {8241,3584,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.34  BV=2.55
    {8241,3840,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.44  BV=2.45
    {8241,4128,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.55  BV=2.35
    {8241,4416,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.64  BV=2.25
    {8241,4736,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.75  BV=2.15
    {8241,5056,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.84  BV=2.05
    {8241,5440,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=7.95  BV=1.95
    {8241,5856,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.05  BV=1.84
    {8241,6272,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.15  BV=1.74
    {8241,6688,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.24  BV=1.65
    {8241,7168,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.34  BV=1.55
    {8241,7680,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.44  BV=1.45
    {8241,8224,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.54  BV=1.35
    {8241,8832,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.64  BV=1.25
    {8241,9440,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.74  BV=1.15
    {8241,10144,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.84  BV=1.05
    {8241,10912,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=8.95  BV=0.94
    {8241,11712,1024, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.05  BV=0.84
    {8241,12288,1040, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.14  BV=0.75
    {8241,12288,1120, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.25  BV=0.64
    {8241,12288,1200, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.35  BV=0.54
    {8241,12288,1280, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.44  BV=0.45
    {8241,12288,1376, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.55  BV=0.35
    {8241,12288,1472, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.64  BV=0.25
    {8241,12288,1576, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.74  BV=0.15
    {8241,12288,1688, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.84  BV=0.05
    {8241,12288,1816, 0, 0, 0},  //TV = 6.92(309 lines)  AV=2.97  SV=9.95  BV=-0.05
    {8241,12288,1816, 0, 0, 0},  /* TV = 6.92(309 lines)  AV=2.97  SV=9.95  BV=-0.05 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Video1AutoTable =
{
    AETABLE_VIDEO1_AUTO, //eAETableID
    108, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -1, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sVideo1PLineTable_60Hz,
    sVideo1PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sVideo2PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {16668,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16668,2016,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.52  BV=2.35
    {16668,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16668,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16668,2496,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.82  BV=2.06
    {16668,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16668,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {16667,3072,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.13  BV=1.75
    {16667,3296,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.22  BV=1.66
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {24987,2912,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.04  BV=1.25
    {24987,3136,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.15  BV=1.14
    {24987,3360,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.25  BV=1.04
    {33254,2688,1032, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=6.94  BV=0.94
    {33254,2880,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.03  BV=0.85
    {33254,3104,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.14  BV=0.75
    {33254,3296,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.22  BV=0.66
    {33254,3552,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.33  BV=0.55
    {33254,3808,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.43  BV=0.45
    {33254,4096,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.54  BV=0.35
    {33254,4384,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.63  BV=0.25
    {33254,4672,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.73  BV=0.16
    {33254,5056,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.84  BV=0.04
    {33254,5408,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=7.94  BV=-0.06
    {33254,5792,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.04  BV=-0.15
    {33254,6208,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.14  BV=-0.25
    {33254,6656,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.24  BV=-0.36
    {33254,7104,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33254,7616,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33254,8160,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33254,8736,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33254,9376,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33254,10112,1024, 0, 0, 0},  //TV = 4.91(1247 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33254,10240,1024, 0, 0, 0},  /* TV = 4.91(1247 lines)  AV=2.97  SV=8.86  BV=-0.98 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sVideo2PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2944,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.06  BV=2.55
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3616,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.36  BV=2.26
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4192,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.57  BV=2.05
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3616,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.36  BV=1.26
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.99  BV=1.04
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {30001,5984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.19  BV=-0.16
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7360,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.38  BV=-0.35
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10240,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Video2AutoTable =
{
    AETABLE_VIDEO2_AUTO, //eAETableID
    117, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sVideo2PLineTable_60Hz,
    sVideo2PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCustom1PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {16668,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16668,2016,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.52  BV=2.35
    {16668,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16668,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16668,2496,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.82  BV=2.06
    {16668,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16668,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {16667,3072,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.13  BV=1.75
    {16667,3296,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.22  BV=1.66
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {24987,2912,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.04  BV=1.25
    {24987,3136,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.15  BV=1.14
    {24987,3360,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.25  BV=1.04
    {33334,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33334,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33334,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33334,3808,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.43  BV=0.45
    {33334,4064,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.52  BV=0.35
    {33334,4352,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.62  BV=0.25
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,4992,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.82  BV=0.06
    {33334,5408,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.94  BV=-0.06
    {33334,5792,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.04  BV=-0.16
    {33334,6176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.13  BV=-0.25
    {33334,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33334,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33334,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33334,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10080,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33334,10240,1024, 0, 0, 0},  /* TV = 4.91(1250 lines)  AV=2.97  SV=8.86  BV=-0.98 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCustom1PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2944,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.06  BV=2.55
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3616,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.36  BV=2.26
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4192,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.57  BV=2.05
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3616,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.36  BV=1.26
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.99  BV=1.04
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {30001,5984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.19  BV=-0.16
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7360,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.38  BV=-0.35
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10240,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Custom1AutoTable =
{
    AETABLE_CUSTOM1_AUTO, //eAETableID
    117, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCustom1PLineTable_60Hz,
    sCustom1PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCustom2PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {16668,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16668,2016,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.52  BV=2.35
    {16668,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16668,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16668,2496,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.82  BV=2.06
    {16668,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16668,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {16667,3072,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.13  BV=1.75
    {16667,3296,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.22  BV=1.66
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {24987,2912,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.04  BV=1.25
    {24987,3136,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.15  BV=1.14
    {24987,3360,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.25  BV=1.04
    {33334,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33334,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33334,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33334,3808,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.43  BV=0.45
    {33334,4064,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.52  BV=0.35
    {33334,4352,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.62  BV=0.25
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,4992,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.82  BV=0.06
    {33334,5408,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.94  BV=-0.06
    {33334,5792,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.04  BV=-0.16
    {33334,6176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.13  BV=-0.25
    {33334,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33334,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33334,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33334,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10080,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33334,10240,1024, 0, 0, 0},  /* TV = 4.91(1250 lines)  AV=2.97  SV=8.86  BV=-0.98 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCustom2PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2944,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.06  BV=2.55
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3616,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.36  BV=2.26
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4192,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.57  BV=2.05
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3616,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.36  BV=1.26
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.99  BV=1.04
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {30001,5984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.19  BV=-0.16
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7360,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.38  BV=-0.35
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10240,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Custom2AutoTable =
{
    AETABLE_CUSTOM2_AUTO, //eAETableID
    117, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCustom2PLineTable_60Hz,
    sCustom2PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCustom3PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {16668,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16668,2016,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.52  BV=2.35
    {16668,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16668,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16668,2496,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.82  BV=2.06
    {16668,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16668,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {16667,3072,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.13  BV=1.75
    {16667,3296,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.22  BV=1.66
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {24987,2912,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.04  BV=1.25
    {24987,3136,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.15  BV=1.14
    {24987,3360,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.25  BV=1.04
    {33334,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33334,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33334,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33334,3808,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.43  BV=0.45
    {33334,4064,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.52  BV=0.35
    {33334,4352,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.62  BV=0.25
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,4992,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.82  BV=0.06
    {33334,5408,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.94  BV=-0.06
    {33334,5792,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.04  BV=-0.16
    {33334,6176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.13  BV=-0.25
    {33334,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33334,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33334,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33334,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10080,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33334,10240,1024, 0, 0, 0},  /* TV = 4.91(1250 lines)  AV=2.97  SV=8.86  BV=-0.98 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCustom3PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2944,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.06  BV=2.55
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3616,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.36  BV=2.26
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4192,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.57  BV=2.05
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3616,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.36  BV=1.26
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.99  BV=1.04
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {30001,5984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.19  BV=-0.16
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7360,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.38  BV=-0.35
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10240,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Custom3AutoTable =
{
    AETABLE_CUSTOM3_AUTO, //eAETableID
    117, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCustom3PLineTable_60Hz,
    sCustom3PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCustom4PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {16668,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16668,2016,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.52  BV=2.35
    {16668,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16668,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16668,2496,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.82  BV=2.06
    {16668,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16668,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {16667,3072,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.13  BV=1.75
    {16667,3296,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.22  BV=1.66
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {24987,2912,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.04  BV=1.25
    {24987,3136,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.15  BV=1.14
    {24987,3360,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.25  BV=1.04
    {33334,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33334,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33334,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33334,3808,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.43  BV=0.45
    {33334,4064,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.52  BV=0.35
    {33334,4352,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.62  BV=0.25
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,4992,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.82  BV=0.06
    {33334,5408,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.94  BV=-0.06
    {33334,5792,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.04  BV=-0.16
    {33334,6176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.13  BV=-0.25
    {33334,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33334,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33334,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33334,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10080,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33334,10240,1024, 0, 0, 0},  /* TV = 4.91(1250 lines)  AV=2.97  SV=8.86  BV=-0.98 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCustom4PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2944,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.06  BV=2.55
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3616,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.36  BV=2.26
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4192,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.57  BV=2.05
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3616,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.36  BV=1.26
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.99  BV=1.04
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {30001,5984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.19  BV=-0.16
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7360,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.38  BV=-0.35
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10240,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Custom4AutoTable =
{
    AETABLE_CUSTOM4_AUTO, //eAETableID
    117, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCustom4PLineTable_60Hz,
    sCustom4PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCustom5PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {16668,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16668,2016,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.52  BV=2.35
    {16668,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16668,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16668,2496,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.82  BV=2.06
    {16668,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16668,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {16667,3072,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.13  BV=1.75
    {16667,3296,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.22  BV=1.66
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {24987,2912,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.04  BV=1.25
    {24987,3136,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.15  BV=1.14
    {24987,3360,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.25  BV=1.04
    {33334,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33334,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33334,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33334,3808,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.43  BV=0.45
    {33334,4064,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.52  BV=0.35
    {33334,4352,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.62  BV=0.25
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,4992,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.82  BV=0.06
    {33334,5408,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.94  BV=-0.06
    {33334,5792,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.04  BV=-0.16
    {33334,6176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.13  BV=-0.25
    {33334,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33334,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33334,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33334,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10080,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33334,10240,1024, 0, 0, 0},  /* TV = 4.91(1250 lines)  AV=2.97  SV=8.86  BV=-0.98 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCustom5PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2944,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.06  BV=2.55
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3616,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.36  BV=2.26
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4192,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.57  BV=2.05
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3616,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.36  BV=1.26
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.99  BV=1.04
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {30001,5984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.19  BV=-0.16
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7360,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.38  BV=-0.35
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10240,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {30001,10240,1024, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=8.86  BV=-0.83 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_Custom5AutoTable =
{
    AETABLE_CUSTOM5_AUTO, //eAETableID
    117, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCustom5PLineTable_60Hz,
    sCustom5PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sVideoNightPLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {24987,1280,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.86  BV=2.44
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {24987,1440,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.04  BV=2.25
    {24987,1568,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.15  BV=2.14
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1632,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.22  BV=1.66
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33335,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33335,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33335,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33335,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33335,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33335,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33335,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33335,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33335,3776,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.42  BV=0.46
    {33335,4096,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.54  BV=0.34
    {33335,4384,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.63  BV=0.24
    {33335,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33335,5024,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.83  BV=0.05
    {33335,5376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.93  BV=-0.05
    {33335,5760,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.03  BV=-0.15
    {33335,6208,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.14  BV=-0.26
    {33335,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33335,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33335,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33335,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33335,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33335,9376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33335,10016,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.83  BV=-0.95
    {33335,10752,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.93  BV=-1.05
    {33335,11520,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.03  BV=-1.15
    {33334,12288,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.12  BV=-1.24
    {33334,12288,1104, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.23  BV=-1.35
    {41654,11360,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.01  BV=-1.45
    {41654,12160,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.11  BV=-1.55
    {41654,12288,1088, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.21  BV=-1.65
    {50001,11648,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.04  BV=-1.75
    {50001,12288,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.14  BV=-1.85
    {58321,11456,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.02  BV=-1.95
    {58321,12288,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.12  BV=-2.05
    {66668,11520,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.03  BV=-2.15
    {66668,12288,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.12  BV=-2.24
    {66668,12288,1104, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.23  BV=-2.35
    {66668,12288,1176, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.32  BV=-2.44
    {66668,12288,1264, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.42  BV=-2.55
    {66668,12288,1352, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.52  BV=-2.64
    {66668,12288,1456, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.63  BV=-2.75
    {66668,12288,1560, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.73  BV=-2.85
    {66668,12288,1664, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.82  BV=-2.94
    {66668,12288,1800, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.93  BV=-3.06
    {66668,12288,1928, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.03  BV=-3.16
    {66668,12288,2064, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.13  BV=-3.25
    {66668,12288,2216, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.23  BV=-3.36
    {66668,12288,2376, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.34  BV=-3.46
    {66668,12288,2544, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.43  BV=-3.56
    {66668,12288,2728, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.53  BV=-3.66
    {66668,12288,2920, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.63  BV=-3.76
    {74988,12288,2768, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=10.56  BV=-3.85
    {74988,12288,2968, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=10.66  BV=-3.95
    {83335,12288,2864, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=10.60  BV=-4.05
    {83335,12288,3072, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=10.71  BV=-4.15
    {91655,12288,2992, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=10.67  BV=-4.25
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sVideoNightPLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.57  BV=3.04
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {20001,1568,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.16  BV=2.45
    {20001,1696,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.28  BV=2.34
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1312,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.89  BV=2.14
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.29  BV=1.74
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.49  BV=1.54
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2240,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.68  BV=1.35
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4544,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.69  BV=0.34
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.14
    {30001,5568,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.98  BV=0.05
    {30001,5952,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.18  BV=-0.15
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7392,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.39  BV=-0.36
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.88  BV=-0.85
    {30001,11136,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.98  BV=-0.95
    {30001,11936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.08  BV=-1.05
    {30001,12288,1064, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.18  BV=-1.15
    {30001,12288,1144, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.28  BV=-1.25
    {40001,11040,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=8.97  BV=-1.35
    {40001,11840,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.07  BV=-1.45
    {40001,12288,1056, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.17  BV=-1.55
    {40001,12288,1128, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.26  BV=-1.65
    {50001,11648,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.04  BV=-1.75
    {50001,12288,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.14  BV=-1.85
    {50001,12288,1112, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.24  BV=-1.95
    {60001,11936,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.08  BV=-2.05
    {60001,12288,1064, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.18  BV=-2.15
    {70001,11776,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.06  BV=-2.25
    {70001,12288,1048, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.15  BV=-2.35
    {70001,12288,1120, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.25  BV=-2.44
    {70001,12288,1208, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.36  BV=-2.55
    {70001,12288,1288, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.45  BV=-2.64
    {70001,12288,1384, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.56  BV=-2.75
    {70001,12288,1480, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.65  BV=-2.85
    {70001,12288,1592, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.76  BV=-2.95
    {70001,12288,1712, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.86  BV=-3.06
    {70001,12288,1840, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.97  BV=-3.16
    {70001,12288,1968, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.06  BV=-3.26
    {70001,12288,2112, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.17  BV=-3.36
    {70001,12288,2264, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.27  BV=-3.46
    {70001,12288,2424, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.36  BV=-3.56
    {70001,12288,2600, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.47  BV=-3.66
    {70001,12288,2784, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.56  BV=-3.76
    {70001,12288,2968, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.66  BV=-3.85
    {80001,12288,2784, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=10.56  BV=-3.95
    {80001,12288,2984, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=10.66  BV=-4.05
    {90002,12288,2840, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=10.59  BV=-4.15
    {90002,12288,3048, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=10.69  BV=-4.25
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_VideoNightTable =
{
    AETABLE_VIDEO_NIGHT, //eAETableID
    149, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -43, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sVideoNightPLineTable_60Hz,
    sVideoNightPLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCaptureISO100PLineTable_60Hz =
{
{
    {81,1504,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.10  BV=10.46
    {81,1568,1024, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.15  BV=10.41 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.90  BV=10.26
    {107,1440,1040, 0, 0, 0},  /* TV = 13.19(4 lines)  AV=2.97  SV=6.05  BV=10.11 */
    {134,1216,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.81  BV=10.03
    {134,1312,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.89  BV=9.94
    {134,1408,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.00  BV=9.84
    {161,1248,1040, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.84  BV=9.73
    {161,1344,1040, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.95  BV=9.62
    {187,1248,1040, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.84  BV=9.51
    {187,1312,1032, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45 */
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {241,1344,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.94  BV=9.05
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {507,1280,1024, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.86  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {587,1280,1024, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.86  BV=7.85
    {641,1248,1024, 0, 0, 0},  //TV = 10.61(24 lines)  AV=2.97  SV=5.82  BV=7.76
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {801,1216,1040, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=5.81  BV=7.45
    {854,1248,1024, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=5.82  BV=7.34
    {907,1248,1024, 0, 0, 0},  //TV = 10.11(34 lines)  AV=2.97  SV=5.82  BV=7.26
    {987,1216,1040, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=5.81  BV=7.15
    {1041,1248,1024, 0, 0, 0},  //TV = 9.91(39 lines)  AV=2.97  SV=5.82  BV=7.06
    {1121,1248,1024, 0, 0, 0},  //TV = 9.80(42 lines)  AV=2.97  SV=5.82  BV=6.95
    {1227,1216,1032, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.80  BV=6.85
    {1307,1216,1032, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=5.80  BV=6.76
    {1387,1248,1024, 0, 0, 0},  //TV = 9.49(52 lines)  AV=2.97  SV=5.82  BV=6.64
    {1494,1216,1040, 0, 0, 0},  //TV = 9.39(56 lines)  AV=2.97  SV=5.81  BV=6.55
    {1601,1248,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=5.82  BV=6.44
    {1707,1248,1024, 0, 0, 0},  //TV = 9.19(64 lines)  AV=2.97  SV=5.82  BV=6.34
    {1841,1216,1040, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=5.81  BV=6.25
    {1974,1216,1040, 0, 0, 0},  //TV = 8.98(74 lines)  AV=2.97  SV=5.81  BV=6.15
    {2107,1216,1040, 0, 0, 0},  //TV = 8.89(79 lines)  AV=2.97  SV=5.81  BV=6.06
    {2267,1216,1040, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=5.81  BV=5.95
    {2427,1216,1040, 0, 0, 0},  //TV = 8.69(91 lines)  AV=2.97  SV=5.81  BV=5.85
    {2614,1216,1032, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=5.80  BV=5.76
    {2801,1216,1032, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=5.80  BV=5.66
    {2987,1216,1040, 0, 0, 0},  //TV = 8.39(112 lines)  AV=2.97  SV=5.81  BV=5.55
    {3227,1216,1032, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=5.80  BV=5.45
    {3441,1216,1040, 0, 0, 0},  //TV = 8.18(129 lines)  AV=2.97  SV=5.81  BV=5.35
    {3681,1216,1040, 0, 0, 0},  //TV = 8.09(138 lines)  AV=2.97  SV=5.81  BV=5.25
    {3974,1216,1032, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=5.80  BV=5.15
    {4214,1216,1040, 0, 0, 0},  //TV = 7.89(158 lines)  AV=2.97  SV=5.81  BV=5.06
    {4561,1216,1032, 0, 0, 0},  //TV = 7.78(171 lines)  AV=2.97  SV=5.80  BV=4.95
    {4881,1216,1032, 0, 0, 0},  //TV = 7.68(183 lines)  AV=2.97  SV=5.80  BV=4.85
    {5227,1216,1032, 0, 0, 0},  //TV = 7.58(196 lines)  AV=2.97  SV=5.80  BV=4.76
    {5601,1216,1032, 0, 0, 0},  //TV = 7.48(210 lines)  AV=2.97  SV=5.80  BV=4.66
    {6001,1216,1032, 0, 0, 0},  //TV = 7.38(225 lines)  AV=2.97  SV=5.80  BV=4.56
    {6427,1216,1032, 0, 0, 0},  //TV = 7.28(241 lines)  AV=2.97  SV=5.80  BV=4.46
    {6907,1216,1040, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=5.81  BV=4.34
    {7414,1216,1032, 0, 0, 0},  //TV = 7.08(278 lines)  AV=2.97  SV=5.80  BV=4.25
    {7947,1216,1032, 0, 0, 0},  //TV = 6.98(298 lines)  AV=2.97  SV=5.80  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2016,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.52  BV=3.36
    {8321,2176,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.63  BV=3.25
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {24987,1248,1040, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.84  BV=2.45
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {24987,1440,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.04  BV=2.25
    {24987,1568,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.15  BV=2.14
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {41654,1216,1040, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=5.81  BV=1.75
    {41654,1312,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=5.90  BV=1.65
    {41654,1408,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.01  BV=1.55
    {50001,1248,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=5.84  BV=1.45
    {50001,1344,1032, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=5.94  BV=1.35
    {58321,1248,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=5.82  BV=1.25
    {58321,1344,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=5.93  BV=1.14
    {66668,1248,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=5.82  BV=1.06
    {66668,1344,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=5.93  BV=0.95
    {74988,1280,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=5.86  BV=0.85
    {83335,1216,1040, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=5.81  BV=0.75
    {83335,1312,1032, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=5.90  BV=0.65
    {91655,1280,1032, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=5.87  BV=0.55
    {100002,1248,1040, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=5.84  BV=0.45
    {108322,1248,1024, 0, 0, 0},  //TV = 3.21(4062 lines)  AV=2.97  SV=5.82  BV=0.36
    {116669,1248,1024, 0, 0, 0},  //TV = 3.10(4375 lines)  AV=2.97  SV=5.82  BV=0.25
    {124989,1248,1024, 0, 0, 0},  //TV = 3.00(4687 lines)  AV=2.97  SV=5.82  BV=0.15
    {133336,1248,1024, 0, 0, 0},  //TV = 2.91(5000 lines)  AV=2.97  SV=5.82  BV=0.06
    {141656,1248,1040, 0, 0, 0},  //TV = 2.82(5312 lines)  AV=2.97  SV=5.84  BV=-0.05
    {150003,1280,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=5.86  BV=-0.15
    {166670,1216,1040, 0, 0, 0},  //TV = 2.58(6250 lines)  AV=2.97  SV=5.81  BV=-0.25
    {174989,1248,1032, 0, 0, 0},  //TV = 2.51(6562 lines)  AV=2.97  SV=5.83  BV=-0.35
    {191656,1216,1040, 0, 0, 0},  //TV = 2.38(7187 lines)  AV=2.97  SV=5.81  BV=-0.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCaptureISO100PLineTable_50Hz =
{
{
    {81,1504,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.10  BV=10.46
    {81,1568,1024, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.15  BV=10.41 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.90  BV=10.26
    {107,1440,1040, 0, 0, 0},  /* TV = 13.19(4 lines)  AV=2.97  SV=6.05  BV=10.11 */
    {134,1216,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.81  BV=10.03
    {134,1312,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.89  BV=9.94
    {134,1408,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.00  BV=9.84
    {161,1248,1040, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.84  BV=9.73
    {161,1344,1040, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.95  BV=9.62
    {187,1248,1040, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.84  BV=9.51
    {187,1312,1032, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45 */
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {241,1344,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.94  BV=9.05
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {507,1280,1024, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.86  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {587,1280,1024, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.86  BV=7.85
    {641,1248,1024, 0, 0, 0},  //TV = 10.61(24 lines)  AV=2.97  SV=5.82  BV=7.76
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {801,1216,1040, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=5.81  BV=7.45
    {854,1248,1024, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=5.82  BV=7.34
    {907,1248,1024, 0, 0, 0},  //TV = 10.11(34 lines)  AV=2.97  SV=5.82  BV=7.26
    {987,1216,1040, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=5.81  BV=7.15
    {1041,1248,1024, 0, 0, 0},  //TV = 9.91(39 lines)  AV=2.97  SV=5.82  BV=7.06
    {1121,1248,1024, 0, 0, 0},  //TV = 9.80(42 lines)  AV=2.97  SV=5.82  BV=6.95
    {1227,1216,1032, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.80  BV=6.85
    {1307,1216,1032, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=5.80  BV=6.76
    {1387,1248,1024, 0, 0, 0},  //TV = 9.49(52 lines)  AV=2.97  SV=5.82  BV=6.64
    {1494,1216,1040, 0, 0, 0},  //TV = 9.39(56 lines)  AV=2.97  SV=5.81  BV=6.55
    {1601,1248,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=5.82  BV=6.44
    {1707,1248,1024, 0, 0, 0},  //TV = 9.19(64 lines)  AV=2.97  SV=5.82  BV=6.34
    {1841,1216,1040, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=5.81  BV=6.25
    {1974,1216,1040, 0, 0, 0},  //TV = 8.98(74 lines)  AV=2.97  SV=5.81  BV=6.15
    {2107,1216,1040, 0, 0, 0},  //TV = 8.89(79 lines)  AV=2.97  SV=5.81  BV=6.06
    {2267,1216,1040, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=5.81  BV=5.95
    {2427,1216,1040, 0, 0, 0},  //TV = 8.69(91 lines)  AV=2.97  SV=5.81  BV=5.85
    {2614,1216,1032, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=5.80  BV=5.76
    {2801,1216,1032, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=5.80  BV=5.66
    {2987,1216,1040, 0, 0, 0},  //TV = 8.39(112 lines)  AV=2.97  SV=5.81  BV=5.55
    {3227,1216,1032, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=5.80  BV=5.45
    {3441,1216,1040, 0, 0, 0},  //TV = 8.18(129 lines)  AV=2.97  SV=5.81  BV=5.35
    {3681,1216,1040, 0, 0, 0},  //TV = 8.09(138 lines)  AV=2.97  SV=5.81  BV=5.25
    {3974,1216,1032, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=5.80  BV=5.15
    {4214,1216,1040, 0, 0, 0},  //TV = 7.89(158 lines)  AV=2.97  SV=5.81  BV=5.06
    {4561,1216,1032, 0, 0, 0},  //TV = 7.78(171 lines)  AV=2.97  SV=5.80  BV=4.95
    {4881,1216,1032, 0, 0, 0},  //TV = 7.68(183 lines)  AV=2.97  SV=5.80  BV=4.85
    {5227,1216,1032, 0, 0, 0},  //TV = 7.58(196 lines)  AV=2.97  SV=5.80  BV=4.76
    {5601,1216,1032, 0, 0, 0},  //TV = 7.48(210 lines)  AV=2.97  SV=5.80  BV=4.66
    {6001,1216,1032, 0, 0, 0},  //TV = 7.38(225 lines)  AV=2.97  SV=5.80  BV=4.56
    {6427,1216,1032, 0, 0, 0},  //TV = 7.28(241 lines)  AV=2.97  SV=5.80  BV=4.46
    {6907,1216,1040, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=5.81  BV=4.34
    {7414,1216,1032, 0, 0, 0},  //TV = 7.08(278 lines)  AV=2.97  SV=5.80  BV=4.25
    {7947,1216,1032, 0, 0, 0},  //TV = 6.98(298 lines)  AV=2.97  SV=5.80  BV=4.15
    {8507,1216,1032, 0, 0, 0},  //TV = 6.88(319 lines)  AV=2.97  SV=5.80  BV=4.05
    {9121,1216,1032, 0, 0, 0},  //TV = 6.78(342 lines)  AV=2.97  SV=5.80  BV=3.95
    {9761,1216,1032, 0, 0, 0},  //TV = 6.68(366 lines)  AV=2.97  SV=5.80  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.28  BV=3.34
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.57  BV=3.04
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.96  BV=2.65
    {20001,1472,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.07  BV=2.54
    {20001,1568,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.16  BV=2.45
    {20001,1696,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.26  BV=2.35
    {20001,1824,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.37  BV=2.25
    {30001,1280,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.88  BV=2.15
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1504,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.09  BV=1.94
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {40001,1280,1032, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=5.87  BV=1.75
    {40001,1376,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=5.96  BV=1.65
    {40001,1472,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.06  BV=1.56
    {50001,1248,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=5.84  BV=1.45
    {50001,1344,1032, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=5.94  BV=1.35
    {50001,1440,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.05  BV=1.24
    {60002,1280,1040, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=5.88  BV=1.15
    {60002,1376,1040, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=5.98  BV=1.05
    {70002,1280,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=5.86  BV=0.95
    {70002,1376,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=5.96  BV=0.84
    {80002,1280,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=5.86  BV=0.76
    {80002,1376,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=5.96  BV=0.65
    {90002,1312,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=5.89  BV=0.55
    {100002,1248,1040, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=5.84  BV=0.45
    {110002,1216,1040, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=5.81  BV=0.35
    {110002,1312,1032, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=5.90  BV=0.25
    {120002,1280,1040, 0, 0, 0},  //TV = 3.06(4500 lines)  AV=2.97  SV=5.88  BV=0.15
    {130002,1280,1024, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=5.86  BV=0.06
    {140003,1280,1024, 0, 0, 0},  //TV = 2.84(5250 lines)  AV=2.97  SV=5.86  BV=-0.05
    {150003,1280,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=5.86  BV=-0.15
    {160003,1280,1024, 0, 0, 0},  //TV = 2.64(6000 lines)  AV=2.97  SV=5.86  BV=-0.24
    {170003,1280,1040, 0, 0, 0},  //TV = 2.56(6375 lines)  AV=2.97  SV=5.88  BV=-0.35
    {190003,1248,1024, 0, 0, 0},  //TV = 2.40(7125 lines)  AV=2.97  SV=5.82  BV=-0.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_CaptureISO100Table =
{
    AETABLE_CAPTURE_ISO100, //eAETableID
    111, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -5, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_100, //ISO
    sCaptureISO100PLineTable_60Hz,
    sCaptureISO100PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCaptureISO200PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {534,2432,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=6.78  BV=7.06
    {561,2496,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=6.82  BV=6.95
    {614,2432,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=6.78  BV=6.86
    {667,2400,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=6.76  BV=6.76
    {694,2496,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=6.82  BV=6.64
    {747,2464,1024, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=6.80  BV=6.55
    {801,2464,1024, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=6.80  BV=6.45
    {854,2464,1032, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=6.81  BV=6.35
    {934,2432,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=6.78  BV=6.25
    {987,2464,1024, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=6.80  BV=6.15
    {1067,2432,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=6.78  BV=6.06
    {1147,2432,1032, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=6.80  BV=5.94
    {1227,2432,1032, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=6.80  BV=5.85
    {1307,2464,1024, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=6.80  BV=5.75
    {1414,2432,1024, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=6.78  BV=5.65
    {1494,2464,1024, 0, 0, 0},  //TV = 9.39(56 lines)  AV=2.97  SV=6.80  BV=5.55
    {1601,2464,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=6.80  BV=5.45
    {1734,2432,1024, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=6.78  BV=5.36
    {1841,2464,1024, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=6.80  BV=5.25
    {1974,2464,1024, 0, 0, 0},  //TV = 8.98(74 lines)  AV=2.97  SV=6.80  BV=5.15
    {2134,2464,1024, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=6.80  BV=5.04
    {2267,2464,1024, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=6.80  BV=4.95
    {2427,2464,1024, 0, 0, 0},  //TV = 8.69(91 lines)  AV=2.97  SV=6.80  BV=4.85
    {2614,2464,1024, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=6.80  BV=4.75
    {2801,2464,1024, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=6.80  BV=4.65
    {3014,2432,1032, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=6.80  BV=4.55
    {3227,2432,1032, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=6.80  BV=4.45
    {3467,2464,1024, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=6.80  BV=4.34
    {3707,2464,1024, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=6.80  BV=4.24
    {3974,2464,1024, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=6.80  BV=4.14
    {4241,2464,1024, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=6.80  BV=4.05
    {4561,2432,1032, 0, 0, 0},  //TV = 7.78(171 lines)  AV=2.97  SV=6.80  BV=3.95
    {4907,2464,1024, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=6.80  BV=3.84
    {5254,2464,1024, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=6.80  BV=3.74
    {5627,2464,1024, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=6.80  BV=3.64
    {6027,2464,1024, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=6.80  BV=3.54
    {6454,2464,1024, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=6.80  BV=3.44
    {6907,2464,1024, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=6.80  BV=3.35
    {7387,2464,1024, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=6.80  BV=3.25
    {7921,2464,1024, 0, 0, 0},  //TV = 6.98(297 lines)  AV=2.97  SV=6.80  BV=3.15
    {8321,2496,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.82  BV=3.06
    {8321,2688,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.94  BV=2.94
    {8321,2880,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.04  BV=2.84
    {8321,3104,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.14  BV=2.74
    {8321,3328,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.24  BV=2.64
    {8321,3552,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.33  BV=2.55
    {16667,1888,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.43  BV=2.45
    {16667,2048,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.54  BV=2.34
    {16667,2176,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.62  BV=2.25
    {16667,2336,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.73  BV=2.15
    {16667,2496,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.83  BV=2.05
    {16667,2688,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.93  BV=1.95
    {16667,2880,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.03  BV=1.85
    {24987,2048,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.55  BV=1.75
    {24987,2208,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.64  BV=1.65
    {24987,2368,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.75  BV=1.55
    {24987,2528,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.84  BV=1.45
    {24987,2720,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.95  BV=1.35
    {33334,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33334,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33334,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33334,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {41654,2304,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.71  BV=0.85
    {41654,2464,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.80  BV=0.75
    {41654,2624,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.90  BV=0.65
    {50001,2368,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.75  BV=0.55
    {50001,2528,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.84  BV=0.45
    {58321,2304,1032, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=6.72  BV=0.35
    {58321,2496,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=6.82  BV=0.25
    {66668,2336,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.73  BV=0.15
    {66668,2496,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.82  BV=0.06
    {74988,2368,1032, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=6.76  BV=-0.05
    {74988,2560,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=6.86  BV=-0.15
    {83335,2464,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=6.80  BV=-0.25
    {91655,2400,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=6.76  BV=-0.35
    {100002,2368,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=6.75  BV=-0.45
    {100002,2528,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=6.84  BV=-0.55
    {108322,2528,1024, 0, 0, 0},  //TV = 3.21(4062 lines)  AV=2.97  SV=6.84  BV=-0.66
    {116669,2496,1024, 0, 0, 0},  //TV = 3.10(4375 lines)  AV=2.97  SV=6.82  BV=-0.75
    {124989,2496,1024, 0, 0, 0},  //TV = 3.00(4687 lines)  AV=2.97  SV=6.82  BV=-0.85
    {133335,2528,1024, 0, 0, 0},  //TV = 2.91(5000 lines)  AV=2.97  SV=6.84  BV=-0.96
    {150002,2400,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=6.76  BV=-1.06
    {158322,2432,1024, 0, 0, 0},  //TV = 2.66(5937 lines)  AV=2.97  SV=6.78  BV=-1.15
    {166669,2464,1024, 0, 0, 0},  //TV = 2.58(6250 lines)  AV=2.97  SV=6.80  BV=-1.25
    {183336,2400,1032, 0, 0, 0},  //TV = 2.45(6875 lines)  AV=2.97  SV=6.78  BV=-1.36
    {191656,2464,1024, 0, 0, 0},  //TV = 2.38(7187 lines)  AV=2.97  SV=6.80  BV=-1.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCaptureISO200PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {534,2432,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=6.78  BV=7.06
    {561,2496,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=6.82  BV=6.95
    {614,2432,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=6.78  BV=6.86
    {667,2400,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=6.76  BV=6.76
    {694,2496,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=6.82  BV=6.64
    {747,2464,1024, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=6.80  BV=6.55
    {801,2464,1024, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=6.80  BV=6.45
    {854,2464,1032, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=6.81  BV=6.35
    {934,2432,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=6.78  BV=6.25
    {987,2464,1024, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=6.80  BV=6.15
    {1067,2432,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=6.78  BV=6.06
    {1147,2432,1032, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=6.80  BV=5.94
    {1227,2432,1032, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=6.80  BV=5.85
    {1307,2464,1024, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=6.80  BV=5.75
    {1414,2432,1024, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=6.78  BV=5.65
    {1494,2464,1024, 0, 0, 0},  //TV = 9.39(56 lines)  AV=2.97  SV=6.80  BV=5.55
    {1601,2464,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=6.80  BV=5.45
    {1734,2432,1024, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=6.78  BV=5.36
    {1841,2464,1024, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=6.80  BV=5.25
    {1974,2464,1024, 0, 0, 0},  //TV = 8.98(74 lines)  AV=2.97  SV=6.80  BV=5.15
    {2134,2464,1024, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=6.80  BV=5.04
    {2267,2464,1024, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=6.80  BV=4.95
    {2427,2464,1024, 0, 0, 0},  //TV = 8.69(91 lines)  AV=2.97  SV=6.80  BV=4.85
    {2614,2464,1024, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=6.80  BV=4.75
    {2801,2464,1024, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=6.80  BV=4.65
    {3014,2432,1032, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=6.80  BV=4.55
    {3227,2432,1032, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=6.80  BV=4.45
    {3467,2464,1024, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=6.80  BV=4.34
    {3707,2464,1024, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=6.80  BV=4.24
    {3974,2464,1024, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=6.80  BV=4.14
    {4241,2464,1024, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=6.80  BV=4.05
    {4561,2432,1032, 0, 0, 0},  //TV = 7.78(171 lines)  AV=2.97  SV=6.80  BV=3.95
    {4907,2464,1024, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=6.80  BV=3.84
    {5254,2464,1024, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=6.80  BV=3.74
    {5627,2464,1024, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=6.80  BV=3.64
    {6027,2464,1024, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=6.80  BV=3.54
    {6454,2464,1024, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=6.80  BV=3.44
    {6907,2464,1024, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=6.80  BV=3.35
    {7387,2464,1024, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=6.80  BV=3.25
    {7921,2464,1024, 0, 0, 0},  //TV = 6.98(297 lines)  AV=2.97  SV=6.80  BV=3.15
    {8481,2464,1024, 0, 0, 0},  //TV = 6.88(318 lines)  AV=2.97  SV=6.80  BV=3.05
    {9094,2464,1024, 0, 0, 0},  //TV = 6.78(341 lines)  AV=2.97  SV=6.80  BV=2.95
    {9814,2464,1024, 0, 0, 0},  //TV = 6.67(368 lines)  AV=2.97  SV=6.80  BV=2.84
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.97  BV=2.64
    {10001,2944,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.07  BV=2.54
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3648,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.37  BV=2.25
    {20001,1952,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.47  BV=2.15
    {20001,2080,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.56  BV=2.06
    {20001,2240,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.67  BV=1.95
    {20001,2400,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.76  BV=1.85
    {20001,2560,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.86  BV=1.76
    {20001,2752,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.96  BV=1.65
    {20001,2944,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.06  BV=1.56
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2240,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.68  BV=1.35
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {40001,2240,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.67  BV=0.95
    {40001,2400,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.76  BV=0.85
    {40001,2560,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.86  BV=0.76
    {40001,2752,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.96  BV=0.65
    {50001,2368,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.75  BV=0.55
    {50001,2528,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.84  BV=0.45
    {60001,2240,1032, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=6.68  BV=0.35
    {60001,2432,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=6.78  BV=0.25
    {60001,2592,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=6.88  BV=0.15
    {70001,2368,1032, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.76  BV=0.05
    {70001,2560,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.86  BV=-0.05
    {80001,2400,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=6.76  BV=-0.15
    {80001,2560,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=6.86  BV=-0.24
    {90002,2464,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=6.80  BV=-0.36
    {100002,2368,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=6.75  BV=-0.45
    {100002,2528,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=6.84  BV=-0.55
    {110002,2464,1024, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=6.80  BV=-0.65
    {120002,2432,1024, 0, 0, 0},  //TV = 3.06(4500 lines)  AV=2.97  SV=6.78  BV=-0.75
    {130002,2400,1024, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=6.76  BV=-0.85
    {140002,2400,1024, 0, 0, 0},  //TV = 2.84(5250 lines)  AV=2.97  SV=6.76  BV=-0.96
    {150002,2368,1032, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=6.76  BV=-1.05
    {160002,2400,1024, 0, 0, 0},  //TV = 2.64(6000 lines)  AV=2.97  SV=6.76  BV=-1.15
    {170003,2400,1032, 0, 0, 0},  //TV = 2.56(6375 lines)  AV=2.97  SV=6.78  BV=-1.25
    {180003,2464,1024, 0, 0, 0},  //TV = 2.47(6750 lines)  AV=2.97  SV=6.80  BV=-1.36
    {190003,2464,1032, 0, 0, 0},  //TV = 2.40(7125 lines)  AV=2.97  SV=6.81  BV=-1.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_CaptureISO200Table =
{
    AETABLE_CAPTURE_ISO200, //eAETableID
    121, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -15, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_200, //ISO
    sCaptureISO200PLineTable_60Hz,
    sCaptureISO200PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCaptureISO400PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {481,2720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.95  BV=7.05
    {481,2912,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.04  BV=6.95
    {481,3136,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.15  BV=6.84
    {481,3328,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.24  BV=6.76
    {481,3584,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.34  BV=6.65
    {481,3840,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.44  BV=6.55
    {481,4128,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.55  BV=6.45
    {481,4416,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.64  BV=6.35
    {481,4736,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.75  BV=6.25
    {481,5056,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.84  BV=6.15
    {534,4896,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=7.79  BV=6.05
    {561,4992,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=7.82  BV=5.95
    {614,4864,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=7.78  BV=5.86
    {667,4800,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=7.76  BV=5.76
    {694,4960,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=7.81  BV=5.65
    {747,4928,1024, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=7.80  BV=5.55
    {801,4960,1024, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=7.81  BV=5.44
    {854,4960,1024, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=7.81  BV=5.35
    {934,4864,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=7.78  BV=5.25
    {987,4928,1024, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=7.80  BV=5.15
    {1067,4896,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=7.79  BV=5.05
    {1147,4896,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=7.79  BV=4.95
    {1227,4896,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=7.79  BV=4.85
    {1307,4928,1024, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=7.80  BV=4.75
    {1414,4864,1024, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=7.78  BV=4.65
    {1521,4864,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=7.78  BV=4.55
    {1601,4928,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=7.80  BV=4.45
    {1734,4896,1024, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=7.79  BV=4.35
    {1841,4928,1024, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=7.80  BV=4.25
    {2001,4896,1024, 0, 0, 0},  //TV = 8.97(75 lines)  AV=2.97  SV=7.79  BV=4.14
    {2134,4896,1024, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=7.79  BV=4.05
    {2267,4928,1024, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=7.80  BV=3.95
    {2427,4928,1024, 0, 0, 0},  //TV = 8.69(91 lines)  AV=2.97  SV=7.80  BV=3.85
    {2614,4928,1024, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=7.80  BV=3.75
    {2801,4928,1024, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=7.80  BV=3.65
    {3014,4896,1024, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=7.79  BV=3.55
    {3227,4928,1024, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=7.80  BV=3.44
    {3467,4896,1024, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=7.79  BV=3.35
    {3707,4896,1024, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=7.79  BV=3.25
    {3974,4896,1024, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=7.79  BV=3.15
    {4241,4928,1024, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=7.80  BV=3.05
    {4587,4896,1024, 0, 0, 0},  //TV = 7.77(172 lines)  AV=2.97  SV=7.79  BV=2.95
    {4907,4896,1024, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=7.79  BV=2.85
    {5254,4896,1024, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=7.79  BV=2.75
    {5627,4896,1024, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=7.79  BV=2.65
    {6027,4896,1024, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=7.79  BV=2.55
    {6454,4896,1024, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=7.79  BV=2.45
    {6907,4896,1024, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=7.79  BV=2.36
    {7387,4928,1024, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=7.80  BV=2.25
    {7974,4896,1024, 0, 0, 0},  //TV = 6.97(299 lines)  AV=2.97  SV=7.79  BV=2.15
    {8321,5056,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.84  BV=2.04
    {8321,5408,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.94  BV=1.94
    {8321,5792,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.04  BV=1.84
    {8321,6208,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.14  BV=1.74
    {8321,6656,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.24  BV=1.64
    {8321,7104,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {16667,4064,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.52  BV=1.35
    {16667,4352,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.62  BV=1.25
    {16667,4672,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.73  BV=1.15
    {16667,5024,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.83  BV=1.05
    {16667,5376,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.93  BV=0.95
    {16667,5760,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.03  BV=0.85
    {24987,4096,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.54  BV=0.76
    {24987,4416,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.64  BV=0.65
    {24987,4736,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.75  BV=0.55
    {24987,5056,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.84  BV=0.45
    {24987,5472,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.95  BV=0.34
    {33334,4352,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.62  BV=0.25
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,4992,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.82  BV=0.06
    {33334,5376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.93  BV=-0.05
    {41654,4640,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.72  BV=-0.16
    {41654,4960,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.81  BV=-0.26
    {41654,5312,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.91  BV=-0.35
    {50001,4736,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.75  BV=-0.45
    {50001,5088,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.85  BV=-0.56
    {58321,4672,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.73  BV=-0.66
    {58321,4992,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.82  BV=-0.75
    {66668,4704,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.74  BV=-0.86
    {66668,5024,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.83  BV=-0.95
    {74988,4800,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=7.76  BV=-1.06
    {74988,5152,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=7.87  BV=-1.16
    {83335,4960,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=7.81  BV=-1.26
    {91655,4832,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=7.77  BV=-1.36
    {100002,4736,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.75  BV=-1.45
    {100002,5088,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.85  BV=-1.56
    {108322,5024,1024, 0, 0, 0},  //TV = 3.21(4062 lines)  AV=2.97  SV=7.83  BV=-1.65
    {116669,4992,1024, 0, 0, 0},  //TV = 3.10(4375 lines)  AV=2.97  SV=7.82  BV=-1.75
    {124989,4992,1024, 0, 0, 0},  //TV = 3.00(4687 lines)  AV=2.97  SV=7.82  BV=-1.85
    {133335,5024,1024, 0, 0, 0},  //TV = 2.91(5000 lines)  AV=2.97  SV=7.83  BV=-1.95
    {150002,4800,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=7.76  BV=-2.06
    {158322,4864,1024, 0, 0, 0},  //TV = 2.66(5937 lines)  AV=2.97  SV=7.78  BV=-2.15
    {166669,4960,1024, 0, 0, 0},  //TV = 2.58(6250 lines)  AV=2.97  SV=7.81  BV=-2.26
    {183336,4832,1024, 0, 0, 0},  //TV = 2.45(6875 lines)  AV=2.97  SV=7.77  BV=-2.36
    {191656,4960,1024, 0, 0, 0},  //TV = 2.38(7187 lines)  AV=2.97  SV=7.81  BV=-2.46
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCaptureISO400PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {481,2720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.95  BV=7.05
    {481,2912,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.04  BV=6.95
    {481,3136,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.15  BV=6.84
    {481,3328,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.24  BV=6.76
    {481,3584,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.34  BV=6.65
    {481,3840,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.44  BV=6.55
    {481,4128,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.55  BV=6.45
    {481,4416,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.64  BV=6.35
    {481,4736,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.75  BV=6.25
    {481,5056,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.84  BV=6.15
    {534,4896,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=7.79  BV=6.05
    {561,4992,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=7.82  BV=5.95
    {614,4864,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=7.78  BV=5.86
    {667,4800,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=7.76  BV=5.76
    {694,4960,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=7.81  BV=5.65
    {747,4928,1024, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=7.80  BV=5.55
    {801,4960,1024, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=7.81  BV=5.44
    {854,4960,1024, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=7.81  BV=5.35
    {934,4864,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=7.78  BV=5.25
    {987,4928,1024, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=7.80  BV=5.15
    {1067,4896,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=7.79  BV=5.05
    {1147,4896,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=7.79  BV=4.95
    {1227,4896,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=7.79  BV=4.85
    {1307,4928,1024, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=7.80  BV=4.75
    {1414,4864,1024, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=7.78  BV=4.65
    {1521,4864,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=7.78  BV=4.55
    {1601,4928,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=7.80  BV=4.45
    {1734,4896,1024, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=7.79  BV=4.35
    {1841,4928,1024, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=7.80  BV=4.25
    {2001,4896,1024, 0, 0, 0},  //TV = 8.97(75 lines)  AV=2.97  SV=7.79  BV=4.14
    {2134,4896,1024, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=7.79  BV=4.05
    {2267,4928,1024, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=7.80  BV=3.95
    {2427,4928,1024, 0, 0, 0},  //TV = 8.69(91 lines)  AV=2.97  SV=7.80  BV=3.85
    {2614,4928,1024, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=7.80  BV=3.75
    {2801,4928,1024, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=7.80  BV=3.65
    {3014,4896,1024, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=7.79  BV=3.55
    {3227,4928,1024, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=7.80  BV=3.44
    {3467,4896,1024, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=7.79  BV=3.35
    {3707,4896,1024, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=7.79  BV=3.25
    {3974,4896,1024, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=7.79  BV=3.15
    {4241,4928,1024, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=7.80  BV=3.05
    {4587,4896,1024, 0, 0, 0},  //TV = 7.77(172 lines)  AV=2.97  SV=7.79  BV=2.95
    {4907,4896,1024, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=7.79  BV=2.85
    {5254,4896,1024, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=7.79  BV=2.75
    {5627,4896,1024, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=7.79  BV=2.65
    {6027,4896,1024, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=7.79  BV=2.55
    {6454,4896,1024, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=7.79  BV=2.45
    {6907,4896,1024, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=7.79  BV=2.36
    {7387,4928,1024, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=7.80  BV=2.25
    {7974,4896,1024, 0, 0, 0},  //TV = 6.97(299 lines)  AV=2.97  SV=7.79  BV=2.15
    {8534,4928,1024, 0, 0, 0},  //TV = 6.87(320 lines)  AV=2.97  SV=7.80  BV=2.04
    {9147,4896,1024, 0, 0, 0},  //TV = 6.77(343 lines)  AV=2.97  SV=7.79  BV=1.95
    {9814,4896,1024, 0, 0, 0},  //TV = 6.67(368 lines)  AV=2.97  SV=7.79  BV=1.85
    {10001,5152,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.87  BV=1.75
    {10001,5536,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.97  BV=1.64
    {10001,5920,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.07  BV=1.55
    {10001,6336,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.17  BV=1.45
    {10001,6784,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.26  BV=1.35
    {10001,7296,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.37  BV=1.25
    {20001,3904,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.47  BV=1.15
    {20001,4160,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.56  BV=1.06
    {20001,4480,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.67  BV=0.95
    {20001,4800,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.76  BV=0.85
    {20001,5152,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.87  BV=0.75
    {20001,5504,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.96  BV=0.65
    {20001,5888,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.06  BV=0.56
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5184,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.15
    {30001,5600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.99  BV=0.04
    {40001,4480,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.67  BV=-0.05
    {40001,4800,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.76  BV=-0.15
    {40001,5120,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.86  BV=-0.24
    {50001,4416,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.64  BV=-0.35
    {50001,4736,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.75  BV=-0.45
    {50001,5088,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.85  BV=-0.56
    {60001,4544,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.69  BV=-0.66
    {60001,4864,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.78  BV=-0.75
    {60001,5216,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.88  BV=-0.86
    {70001,4800,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.76  BV=-0.96
    {70001,5120,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.86  BV=-1.05
    {80001,4800,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=7.76  BV=-1.15
    {80001,5152,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=7.87  BV=-1.25
    {90002,4928,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=7.80  BV=-1.36
    {100002,4736,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.75  BV=-1.45
    {100002,5088,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.85  BV=-1.56
    {110002,4960,1024, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=7.81  BV=-1.66
    {120002,4864,1024, 0, 0, 0},  //TV = 3.06(4500 lines)  AV=2.97  SV=7.78  BV=-1.75
    {130002,4800,1024, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=7.76  BV=-1.85
    {140002,4800,1024, 0, 0, 0},  //TV = 2.84(5250 lines)  AV=2.97  SV=7.76  BV=-1.96
    {150002,4800,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=7.76  BV=-2.06
    {160002,4800,1024, 0, 0, 0},  //TV = 2.64(6000 lines)  AV=2.97  SV=7.76  BV=-2.15
    {170003,4864,1024, 0, 0, 0},  //TV = 2.56(6375 lines)  AV=2.97  SV=7.78  BV=-2.26
    {180003,4928,1024, 0, 0, 0},  //TV = 2.47(6750 lines)  AV=2.97  SV=7.80  BV=-2.36
    {190003,4992,1024, 0, 0, 0},  //TV = 2.40(7125 lines)  AV=2.97  SV=7.82  BV=-2.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_CaptureISO400Table =
{
    AETABLE_CAPTURE_ISO400, //eAETableID
    131, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -25, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_400, //ISO
    sCaptureISO400PLineTable_60Hz,
    sCaptureISO400PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCaptureISO800PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {481,2720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.95  BV=7.05
    {481,2912,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.04  BV=6.95
    {481,3136,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.15  BV=6.84
    {481,3328,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.24  BV=6.76
    {481,3584,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.34  BV=6.65
    {481,3840,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.44  BV=6.55
    {481,4128,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.55  BV=6.45
    {481,4416,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.64  BV=6.35
    {481,4736,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.75  BV=6.25
    {481,5056,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.84  BV=6.15
    {481,5440,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.95  BV=6.05
    {481,5824,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.04  BV=5.95
    {481,6240,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.14  BV=5.85
    {481,6720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.25  BV=5.74
    {481,7200,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.35  BV=5.64
    {481,7712,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.45  BV=5.54
    {481,8256,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.55  BV=5.45
    {481,8832,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.64  BV=5.35
    {481,9472,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.75  BV=5.25
    {481,10144,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.84  BV=5.15
    {534,9824,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=8.80  BV=5.04
    {561,9984,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=8.82  BV=4.95
    {614,9760,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=8.79  BV=4.85
    {667,9632,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=8.77  BV=4.75
    {694,9952,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=8.82  BV=4.65
    {747,9888,1024, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=8.81  BV=4.55
    {801,9888,1024, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=8.81  BV=4.45
    {854,9920,1024, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=8.81  BV=4.35
    {934,9760,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=8.79  BV=4.25
    {987,9888,1024, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=8.81  BV=4.15
    {1067,9824,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=8.80  BV=4.04
    {1147,9792,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=8.79  BV=3.95
    {1227,9792,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=8.79  BV=3.85
    {1307,9888,1024, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=8.81  BV=3.74
    {1414,9760,1024, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=8.79  BV=3.65
    {1521,9760,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=8.79  BV=3.54
    {1601,9888,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=8.81  BV=3.45
    {1734,9824,1024, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=8.80  BV=3.34
    {1841,9888,1024, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=8.81  BV=3.25
    {2001,9792,1024, 0, 0, 0},  //TV = 8.97(75 lines)  AV=2.97  SV=8.79  BV=3.14
    {2134,9824,1024, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=8.80  BV=3.04
    {2267,9888,1024, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=8.81  BV=2.95
    {2454,9824,1024, 0, 0, 0},  //TV = 8.67(92 lines)  AV=2.97  SV=8.80  BV=2.84
    {2614,9856,1024, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=8.80  BV=2.75
    {2801,9856,1024, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=8.80  BV=2.65
    {3014,9792,1024, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=8.79  BV=2.55
    {3227,9856,1024, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=8.80  BV=2.44
    {3467,9824,1024, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=8.80  BV=2.34
    {3707,9824,1024, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=8.80  BV=2.25
    {3974,9824,1024, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=8.80  BV=2.15
    {4241,9856,1024, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=8.80  BV=2.05
    {4587,9824,1024, 0, 0, 0},  //TV = 7.77(172 lines)  AV=2.97  SV=8.80  BV=1.94
    {4907,9824,1024, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=8.80  BV=1.84
    {5254,9824,1024, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=8.80  BV=1.75
    {5627,9824,1024, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=8.80  BV=1.65
    {6027,9824,1024, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=8.80  BV=1.55
    {6454,9824,1024, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=8.80  BV=1.45
    {6907,9824,1024, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=8.80  BV=1.35
    {7387,9856,1024, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=8.80  BV=1.25
    {7974,9824,1024, 0, 0, 0},  //TV = 6.97(299 lines)  AV=2.97  SV=8.80  BV=1.14
    {8321,10080,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.84  BV=1.04
    {8321,10816,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.94  BV=0.94
    {8321,11584,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=9.04  BV=0.84
    {8321,12288,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=9.13  BV=0.75
    {8321,12288,1104, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=9.23  BV=0.65
    {8321,12288,1184, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=9.33  BV=0.55
    {16667,7616,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.43  BV=0.45
    {16667,8160,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.53  BV=0.35
    {16667,8736,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.63  BV=0.25
    {16667,9376,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.73  BV=0.15
    {16667,10048,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.83  BV=0.05
    {16667,10752,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=8.93  BV=-0.05
    {16667,11520,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.03  BV=-0.15
    {24987,8224,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=8.54  BV=-0.25
    {24987,8832,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=8.64  BV=-0.35
    {24987,9440,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=8.74  BV=-0.45
    {24987,10144,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=8.84  BV=-0.55
    {24987,10848,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=8.94  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10080,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.84  BV=-0.96
    {33334,10816,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.94  BV=-1.06
    {41654,9280,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=8.72  BV=-1.16
    {41654,9920,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=8.81  BV=-1.26
    {41654,10656,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=8.92  BV=-1.36
    {50001,9504,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=8.75  BV=-1.46
    {50001,10176,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=8.85  BV=-1.56
    {58321,9344,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=8.73  BV=-1.66
    {58321,10016,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=8.83  BV=-1.76
    {66668,9408,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.74  BV=-1.86
    {66668,10080,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.84  BV=-1.96
    {74988,9600,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.76  BV=-2.06
    {74988,10272,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.86  BV=-2.15
    {83335,9920,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.81  BV=-2.26
    {91655,9664,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=8.77  BV=-2.36
    {100002,9504,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.46
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {108322,10080,1024, 0, 0, 0},  //TV = 3.21(4062 lines)  AV=2.97  SV=8.84  BV=-2.66
    {116669,10016,1024, 0, 0, 0},  //TV = 3.10(4375 lines)  AV=2.97  SV=8.83  BV=-2.76
    {124989,10016,1024, 0, 0, 0},  //TV = 3.00(4687 lines)  AV=2.97  SV=8.83  BV=-2.86
    {133335,10080,1024, 0, 0, 0},  //TV = 2.91(5000 lines)  AV=2.97  SV=8.84  BV=-2.96
    {150002,9600,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=8.76  BV=-3.06
    {158322,9728,1024, 0, 0, 0},  //TV = 2.66(5937 lines)  AV=2.97  SV=8.78  BV=-3.15
    {166669,9920,1024, 0, 0, 0},  //TV = 2.58(6250 lines)  AV=2.97  SV=8.81  BV=-3.26
    {183336,9664,1024, 0, 0, 0},  //TV = 2.45(6875 lines)  AV=2.97  SV=8.77  BV=-3.36
    {191656,9920,1024, 0, 0, 0},  //TV = 2.38(7187 lines)  AV=2.97  SV=8.81  BV=-3.46
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCaptureISO800PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {481,2720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.95  BV=7.05
    {481,2912,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.04  BV=6.95
    {481,3136,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.15  BV=6.84
    {481,3328,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.24  BV=6.76
    {481,3584,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.34  BV=6.65
    {481,3840,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.44  BV=6.55
    {481,4128,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.55  BV=6.45
    {481,4416,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.64  BV=6.35
    {481,4736,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.75  BV=6.25
    {481,5056,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.84  BV=6.15
    {481,5440,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.95  BV=6.05
    {481,5824,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.04  BV=5.95
    {481,6240,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.14  BV=5.85
    {481,6720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.25  BV=5.74
    {481,7200,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.35  BV=5.64
    {481,7712,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.45  BV=5.54
    {481,8256,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.55  BV=5.45
    {481,8832,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.64  BV=5.35
    {481,9472,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.75  BV=5.25
    {481,10144,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.84  BV=5.15
    {534,9824,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=8.80  BV=5.04
    {561,9984,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=8.82  BV=4.95
    {614,9760,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=8.79  BV=4.85
    {667,9632,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=8.77  BV=4.75
    {694,9952,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=8.82  BV=4.65
    {747,9888,1024, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=8.81  BV=4.55
    {801,9888,1024, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=8.81  BV=4.45
    {854,9920,1024, 0, 0, 0},  //TV = 10.19(32 lines)  AV=2.97  SV=8.81  BV=4.35
    {934,9760,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=8.79  BV=4.25
    {987,9888,1024, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=8.81  BV=4.15
    {1067,9824,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=8.80  BV=4.04
    {1147,9792,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=8.79  BV=3.95
    {1227,9792,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=8.79  BV=3.85
    {1307,9888,1024, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=8.81  BV=3.74
    {1414,9760,1024, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=8.79  BV=3.65
    {1521,9760,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=8.79  BV=3.54
    {1601,9888,1024, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=8.81  BV=3.45
    {1734,9824,1024, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=8.80  BV=3.34
    {1841,9888,1024, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=8.81  BV=3.25
    {2001,9792,1024, 0, 0, 0},  //TV = 8.97(75 lines)  AV=2.97  SV=8.79  BV=3.14
    {2134,9824,1024, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=8.80  BV=3.04
    {2267,9888,1024, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=8.81  BV=2.95
    {2454,9824,1024, 0, 0, 0},  //TV = 8.67(92 lines)  AV=2.97  SV=8.80  BV=2.84
    {2614,9856,1024, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=8.80  BV=2.75
    {2801,9856,1024, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=8.80  BV=2.65
    {3014,9792,1024, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=8.79  BV=2.55
    {3227,9856,1024, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=8.80  BV=2.44
    {3467,9824,1024, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=8.80  BV=2.34
    {3707,9824,1024, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=8.80  BV=2.25
    {3974,9824,1024, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=8.80  BV=2.15
    {4241,9856,1024, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=8.80  BV=2.05
    {4587,9824,1024, 0, 0, 0},  //TV = 7.77(172 lines)  AV=2.97  SV=8.80  BV=1.94
    {4907,9824,1024, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=8.80  BV=1.84
    {5254,9824,1024, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=8.80  BV=1.75
    {5627,9824,1024, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=8.80  BV=1.65
    {6027,9824,1024, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=8.80  BV=1.55
    {6454,9824,1024, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=8.80  BV=1.45
    {6907,9824,1024, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=8.80  BV=1.35
    {7387,9856,1024, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=8.80  BV=1.25
    {7974,9824,1024, 0, 0, 0},  //TV = 6.97(299 lines)  AV=2.97  SV=8.80  BV=1.14
    {8534,9856,1024, 0, 0, 0},  //TV = 6.87(320 lines)  AV=2.97  SV=8.80  BV=1.04
    {9147,9824,1024, 0, 0, 0},  //TV = 6.77(343 lines)  AV=2.97  SV=8.80  BV=0.95
    {9814,9824,1024, 0, 0, 0},  //TV = 6.67(368 lines)  AV=2.97  SV=8.80  BV=0.84
    {10001,10336,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.87  BV=0.74
    {10001,11072,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.97  BV=0.64
    {10001,11840,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=9.07  BV=0.55
    {10001,12288,1056, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=9.17  BV=0.45
    {10001,12288,1128, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=9.26  BV=0.35
    {10001,12288,1216, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=9.37  BV=0.25
    {20001,7808,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.47  BV=0.15
    {20001,8352,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.56  BV=0.05
    {20001,8960,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.67  BV=-0.05
    {20001,9600,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.76  BV=-0.15
    {20001,10304,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.87  BV=-0.25
    {20001,11040,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=8.97  BV=-0.35
    {20001,11808,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.06  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9696,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.88  BV=-0.85
    {30001,11136,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.98  BV=-0.95
    {40001,8992,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=8.67  BV=-1.06
    {40001,9664,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=8.77  BV=-1.16
    {40001,10336,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=8.87  BV=-1.26
    {50001,8864,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=8.65  BV=-1.36
    {50001,9504,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=8.75  BV=-1.46
    {50001,10176,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=8.85  BV=-1.56
    {60001,9088,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=8.69  BV=-1.66
    {60001,9760,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=8.79  BV=-1.76
    {60001,10432,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=8.88  BV=-1.86
    {70001,9600,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.76  BV=-1.96
    {70001,10272,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.86  BV=-2.06
    {80001,9632,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.77  BV=-2.15
    {80001,10336,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.87  BV=-2.26
    {90002,9856,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.80  BV=-2.36
    {100002,9504,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.46
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {110002,9920,1024, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=8.81  BV=-2.66
    {120002,9728,1024, 0, 0, 0},  //TV = 3.06(4500 lines)  AV=2.97  SV=8.78  BV=-2.75
    {130002,9632,1024, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=8.77  BV=-2.86
    {140002,9600,1024, 0, 0, 0},  //TV = 2.84(5250 lines)  AV=2.97  SV=8.76  BV=-2.96
    {150002,9600,1024, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=8.76  BV=-3.06
    {160002,9632,1024, 0, 0, 0},  //TV = 2.64(6000 lines)  AV=2.97  SV=8.77  BV=-3.15
    {170003,9728,1024, 0, 0, 0},  //TV = 2.56(6375 lines)  AV=2.97  SV=8.78  BV=-3.26
    {180003,9824,1024, 0, 0, 0},  //TV = 2.47(6750 lines)  AV=2.97  SV=8.80  BV=-3.35
    {190003,9984,1024, 0, 0, 0},  //TV = 2.40(7125 lines)  AV=2.97  SV=8.82  BV=-3.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_CaptureISO800Table =
{
    AETABLE_CAPTURE_ISO800, //eAETableID
    141, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -35, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_800, //ISO
    sCaptureISO800PLineTable_60Hz,
    sCaptureISO800PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCaptureISO1600PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {481,2720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.95  BV=7.05
    {481,2912,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.04  BV=6.95
    {481,3136,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.15  BV=6.84
    {481,3328,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.24  BV=6.76
    {481,3584,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.34  BV=6.65
    {481,3840,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.44  BV=6.55
    {481,4128,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.55  BV=6.45
    {481,4416,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.64  BV=6.35
    {481,4736,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.75  BV=6.25
    {481,5056,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.84  BV=6.15
    {481,5440,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.95  BV=6.05
    {481,5824,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.04  BV=5.95
    {481,6240,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.14  BV=5.85
    {481,6720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.25  BV=5.74
    {481,7200,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.35  BV=5.64
    {481,7712,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.45  BV=5.54
    {481,8256,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.55  BV=5.45
    {481,8832,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.64  BV=5.35
    {481,9472,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.75  BV=5.25
    {481,10144,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.84  BV=5.15
    {481,10880,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.95  BV=5.05
    {481,11648,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.04  BV=4.95
    {481,12288,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.14  BV=4.85
    {481,12288,1112, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.24  BV=4.75
    {481,12288,1192, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.34  BV=4.65
    {481,12288,1288, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.45  BV=4.54
    {481,12288,1376, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.55  BV=4.45
    {481,12288,1472, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.64  BV=4.35
    {481,12288,1584, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.75  BV=4.24
    {481,12288,1696, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.85  BV=4.14
    {534,12288,1632, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=9.79  BV=4.05
    {561,12288,1656, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=9.81  BV=3.96
    {614,12288,1624, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=9.79  BV=3.85
    {667,12288,1608, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=9.77  BV=3.75
    {694,12288,1656, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=9.81  BV=3.65
    {747,12288,1648, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=9.81  BV=3.55
    {801,12288,1648, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=9.81  BV=3.45
    {881,12288,1608, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=9.77  BV=3.35
    {934,12288,1624, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=9.79  BV=3.25
    {987,12288,1648, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=9.81  BV=3.15
    {1067,12288,1640, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=9.80  BV=3.04
    {1147,12288,1632, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=9.79  BV=2.95
    {1227,12288,1632, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=9.79  BV=2.85
    {1307,12288,1640, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=9.80  BV=2.75
    {1414,12288,1624, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=9.79  BV=2.65
    {1521,12288,1624, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=9.79  BV=2.55
    {1601,12288,1648, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=9.81  BV=2.45
    {1734,12288,1632, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=9.79  BV=2.35
    {1841,12288,1648, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=9.81  BV=2.25
    {2001,12288,1632, 0, 0, 0},  //TV = 8.97(75 lines)  AV=2.97  SV=9.79  BV=2.14
    {2134,12288,1632, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=9.79  BV=2.05
    {2267,12288,1648, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=9.81  BV=1.95
    {2454,12288,1632, 0, 0, 0},  //TV = 8.67(92 lines)  AV=2.97  SV=9.79  BV=1.85
    {2614,12288,1640, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=9.80  BV=1.75
    {2801,12288,1640, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=9.80  BV=1.65
    {3014,12288,1632, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=9.79  BV=1.55
    {3227,12288,1640, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=9.80  BV=1.45
    {3467,12288,1632, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=9.79  BV=1.35
    {3707,12288,1640, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=9.80  BV=1.25
    {3974,12288,1632, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=9.79  BV=1.15
    {4241,12288,1640, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=9.80  BV=1.05
    {4587,12288,1632, 0, 0, 0},  //TV = 7.77(172 lines)  AV=2.97  SV=9.79  BV=0.95
    {4907,12288,1632, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=9.79  BV=0.85
    {5254,12288,1632, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=9.79  BV=0.75
    {5627,12288,1632, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=9.79  BV=0.65
    {6027,12288,1632, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=9.79  BV=0.55
    {6454,12288,1632, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=9.79  BV=0.45
    {6907,12288,1632, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=9.79  BV=0.36
    {7387,12288,1640, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=9.80  BV=0.25
    {7974,12288,1640, 0, 0, 0},  //TV = 6.97(299 lines)  AV=2.97  SV=9.80  BV=0.14
    {8321,12288,1680, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=9.84  BV=0.04
    {8321,12288,1800, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=9.93  BV=-0.05
    {8321,12288,1928, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=10.03  BV=-0.15
    {8321,12288,2064, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=10.13  BV=-0.25
    {8321,12288,2216, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=10.23  BV=-0.35
    {8321,12288,2376, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=10.34  BV=-0.46
    {16667,12288,1264, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.42  BV=-0.55
    {16667,12288,1360, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.53  BV=-0.65
    {16667,12288,1456, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.63  BV=-0.75
    {16667,12288,1560, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.73  BV=-0.85
    {16667,12288,1672, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.83  BV=-0.95
    {16667,12288,1792, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=9.93  BV=-1.05
    {16667,12288,1920, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=10.03  BV=-1.15
    {24987,12288,1368, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=9.54  BV=-1.25
    {24987,12288,1472, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=9.64  BV=-1.35
    {24987,12288,1576, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=9.74  BV=-1.45
    {24987,12288,1688, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=9.84  BV=-1.55
    {24987,12288,1808, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=9.94  BV=-1.65
    {33334,12288,1448, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.62  BV=-1.74
    {33334,12288,1552, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.72  BV=-1.84
    {33334,12288,1664, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.82  BV=-1.94
    {33334,12288,1800, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.93  BV=-2.06
    {41654,12288,1544, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.71  BV=-2.16
    {41654,12288,1656, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.81  BV=-2.26
    {41654,12288,1776, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.92  BV=-2.36
    {50001,12288,1584, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.75  BV=-2.46
    {50001,12288,1696, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.85  BV=-2.56
    {58321,12288,1560, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.73  BV=-2.66
    {58321,12288,1672, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.83  BV=-2.76
    {66668,12288,1568, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.74  BV=-2.86
    {66668,12288,1680, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.84  BV=-2.96
    {74988,12288,1600, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=9.76  BV=-3.06
    {74988,12288,1712, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=9.86  BV=-3.15
    {83335,12288,1648, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=9.81  BV=-3.25
    {91655,12288,1608, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=9.77  BV=-3.35
    {100002,12288,1584, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.75  BV=-3.46
    {100002,12288,1696, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.85  BV=-3.56
    {108322,12288,1680, 0, 0, 0},  //TV = 3.21(4062 lines)  AV=2.97  SV=9.84  BV=-3.66
    {116669,12288,1664, 0, 0, 0},  //TV = 3.10(4375 lines)  AV=2.97  SV=9.82  BV=-3.75
    {124989,12288,1672, 0, 0, 0},  //TV = 3.00(4687 lines)  AV=2.97  SV=9.83  BV=-3.86
    {133335,12288,1680, 0, 0, 0},  //TV = 2.91(5000 lines)  AV=2.97  SV=9.84  BV=-3.96
    {150002,12288,1600, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=9.76  BV=-4.06
    {158322,12288,1624, 0, 0, 0},  //TV = 2.66(5937 lines)  AV=2.97  SV=9.79  BV=-4.16
    {166669,12288,1648, 0, 0, 0},  //TV = 2.58(6250 lines)  AV=2.97  SV=9.81  BV=-4.25
    {183336,12288,1608, 0, 0, 0},  //TV = 2.45(6875 lines)  AV=2.97  SV=9.77  BV=-4.35
    {191656,12288,1648, 0, 0, 0},  //TV = 2.38(7187 lines)  AV=2.97  SV=9.81  BV=-4.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCaptureISO1600PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {481,1248,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.84  BV=8.15
    {481,1344,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.94  BV=8.05
    {481,1440,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.04  BV=7.95
    {481,1536,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.14  BV=7.85
    {481,1664,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.24  BV=7.76
    {481,1792,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.34  BV=7.65
    {481,1920,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.44  BV=7.55
    {481,2048,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.55  BV=7.45
    {481,2208,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.64  BV=7.35
    {481,2368,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.75  BV=7.25
    {481,2528,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.84  BV=7.15
    {481,2720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=6.95  BV=7.05
    {481,2912,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.04  BV=6.95
    {481,3136,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.15  BV=6.84
    {481,3328,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.24  BV=6.76
    {481,3584,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.34  BV=6.65
    {481,3840,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.44  BV=6.55
    {481,4128,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.55  BV=6.45
    {481,4416,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.64  BV=6.35
    {481,4736,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.75  BV=6.25
    {481,5056,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.84  BV=6.15
    {481,5440,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=7.95  BV=6.05
    {481,5824,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.04  BV=5.95
    {481,6240,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.14  BV=5.85
    {481,6720,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.25  BV=5.74
    {481,7200,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.35  BV=5.64
    {481,7712,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.45  BV=5.54
    {481,8256,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.55  BV=5.45
    {481,8832,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.64  BV=5.35
    {481,9472,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.75  BV=5.25
    {481,10144,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.84  BV=5.15
    {481,10880,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=8.95  BV=5.05
    {481,11648,1024, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.04  BV=4.95
    {481,12288,1040, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.14  BV=4.85
    {481,12288,1112, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.24  BV=4.75
    {481,12288,1192, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.34  BV=4.65
    {481,12288,1288, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.45  BV=4.54
    {481,12288,1376, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.55  BV=4.45
    {481,12288,1472, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.64  BV=4.35
    {481,12288,1584, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.75  BV=4.24
    {481,12288,1696, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=9.85  BV=4.14
    {534,12288,1632, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=9.79  BV=4.05
    {561,12288,1656, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=9.81  BV=3.96
    {614,12288,1624, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=9.79  BV=3.85
    {667,12288,1608, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=9.77  BV=3.75
    {694,12288,1656, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=9.81  BV=3.65
    {747,12288,1648, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=9.81  BV=3.55
    {801,12288,1648, 0, 0, 0},  //TV = 10.29(30 lines)  AV=2.97  SV=9.81  BV=3.45
    {881,12288,1608, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=9.77  BV=3.35
    {934,12288,1624, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=9.79  BV=3.25
    {987,12288,1648, 0, 0, 0},  //TV = 9.98(37 lines)  AV=2.97  SV=9.81  BV=3.15
    {1067,12288,1640, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=9.80  BV=3.04
    {1147,12288,1632, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=9.79  BV=2.95
    {1227,12288,1632, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=9.79  BV=2.85
    {1307,12288,1640, 0, 0, 0},  //TV = 9.58(49 lines)  AV=2.97  SV=9.80  BV=2.75
    {1414,12288,1624, 0, 0, 0},  //TV = 9.47(53 lines)  AV=2.97  SV=9.79  BV=2.65
    {1521,12288,1624, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=9.79  BV=2.55
    {1601,12288,1648, 0, 0, 0},  //TV = 9.29(60 lines)  AV=2.97  SV=9.81  BV=2.45
    {1734,12288,1632, 0, 0, 0},  //TV = 9.17(65 lines)  AV=2.97  SV=9.79  BV=2.35
    {1841,12288,1648, 0, 0, 0},  //TV = 9.09(69 lines)  AV=2.97  SV=9.81  BV=2.25
    {2001,12288,1632, 0, 0, 0},  //TV = 8.97(75 lines)  AV=2.97  SV=9.79  BV=2.14
    {2134,12288,1632, 0, 0, 0},  //TV = 8.87(80 lines)  AV=2.97  SV=9.79  BV=2.05
    {2267,12288,1648, 0, 0, 0},  //TV = 8.78(85 lines)  AV=2.97  SV=9.81  BV=1.95
    {2454,12288,1632, 0, 0, 0},  //TV = 8.67(92 lines)  AV=2.97  SV=9.79  BV=1.85
    {2614,12288,1640, 0, 0, 0},  //TV = 8.58(98 lines)  AV=2.97  SV=9.80  BV=1.75
    {2801,12288,1640, 0, 0, 0},  //TV = 8.48(105 lines)  AV=2.97  SV=9.80  BV=1.65
    {3014,12288,1632, 0, 0, 0},  //TV = 8.37(113 lines)  AV=2.97  SV=9.79  BV=1.55
    {3227,12288,1640, 0, 0, 0},  //TV = 8.28(121 lines)  AV=2.97  SV=9.80  BV=1.45
    {3467,12288,1632, 0, 0, 0},  //TV = 8.17(130 lines)  AV=2.97  SV=9.79  BV=1.35
    {3707,12288,1640, 0, 0, 0},  //TV = 8.08(139 lines)  AV=2.97  SV=9.80  BV=1.25
    {3974,12288,1632, 0, 0, 0},  //TV = 7.98(149 lines)  AV=2.97  SV=9.79  BV=1.15
    {4241,12288,1640, 0, 0, 0},  //TV = 7.88(159 lines)  AV=2.97  SV=9.80  BV=1.05
    {4587,12288,1632, 0, 0, 0},  //TV = 7.77(172 lines)  AV=2.97  SV=9.79  BV=0.95
    {4907,12288,1632, 0, 0, 0},  //TV = 7.67(184 lines)  AV=2.97  SV=9.79  BV=0.85
    {5254,12288,1632, 0, 0, 0},  //TV = 7.57(197 lines)  AV=2.97  SV=9.79  BV=0.75
    {5627,12288,1632, 0, 0, 0},  //TV = 7.47(211 lines)  AV=2.97  SV=9.79  BV=0.65
    {6027,12288,1632, 0, 0, 0},  //TV = 7.37(226 lines)  AV=2.97  SV=9.79  BV=0.55
    {6454,12288,1632, 0, 0, 0},  //TV = 7.28(242 lines)  AV=2.97  SV=9.79  BV=0.45
    {6907,12288,1632, 0, 0, 0},  //TV = 7.18(259 lines)  AV=2.97  SV=9.79  BV=0.36
    {7387,12288,1640, 0, 0, 0},  //TV = 7.08(277 lines)  AV=2.97  SV=9.80  BV=0.25
    {7974,12288,1640, 0, 0, 0},  //TV = 6.97(299 lines)  AV=2.97  SV=9.80  BV=0.14
    {8534,12288,1640, 0, 0, 0},  //TV = 6.87(320 lines)  AV=2.97  SV=9.80  BV=0.04
    {9147,12288,1640, 0, 0, 0},  //TV = 6.77(343 lines)  AV=2.97  SV=9.80  BV=-0.06
    {9814,12288,1632, 0, 0, 0},  //TV = 6.67(368 lines)  AV=2.97  SV=9.79  BV=-0.15
    {10001,12288,1720, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=9.87  BV=-0.25
    {10001,12288,1840, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=9.97  BV=-0.35
    {10001,12288,1976, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=10.07  BV=-0.45
    {10001,12288,2112, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=10.17  BV=-0.55
    {10001,12288,2264, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=10.27  BV=-0.65
    {10001,12288,2424, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=10.36  BV=-0.75
    {20001,12288,1296, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.46  BV=-0.85
    {20001,12288,1392, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.56  BV=-0.95
    {20001,12288,1488, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.66  BV=-1.05
    {20001,12288,1600, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.76  BV=-1.15
    {20001,12288,1712, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.86  BV=-1.25
    {20001,12288,1832, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=9.96  BV=-1.35
    {20001,12288,1968, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=10.06  BV=-1.45
    {30001,12288,1408, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.58  BV=-1.55
    {30001,12288,1504, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.68  BV=-1.65
    {30001,12288,1616, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.78  BV=-1.75
    {30001,12288,1728, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.88  BV=-1.85
    {30001,12288,1864, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.99  BV=-1.96
    {40001,12288,1488, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.66  BV=-2.05
    {40001,12288,1608, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.77  BV=-2.16
    {40001,12288,1720, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.87  BV=-2.25
    {50001,12288,1472, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.64  BV=-2.35
    {50001,12288,1584, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.75  BV=-2.46
    {50001,12288,1696, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.85  BV=-2.56
    {60001,12288,1512, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.68  BV=-2.65
    {60001,12288,1624, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.79  BV=-2.76
    {60001,12288,1736, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.88  BV=-2.85
    {70001,12288,1600, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.76  BV=-2.96
    {70001,12288,1712, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.86  BV=-3.06
    {80001,12288,1608, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=9.77  BV=-3.16
    {80001,12288,1720, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=9.87  BV=-3.25
    {90002,12288,1640, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=9.80  BV=-3.36
    {100002,12288,1584, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.75  BV=-3.46
    {100002,12288,1696, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.85  BV=-3.56
    {110002,12288,1648, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=9.81  BV=-3.65
    {120002,12288,1624, 0, 0, 0},  //TV = 3.06(4500 lines)  AV=2.97  SV=9.79  BV=-3.76
    {130002,12288,1608, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=9.77  BV=-3.86
    {140002,12288,1600, 0, 0, 0},  //TV = 2.84(5250 lines)  AV=2.97  SV=9.76  BV=-3.96
    {150002,12288,1600, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=9.76  BV=-4.06
    {160002,12288,1608, 0, 0, 0},  //TV = 2.64(6000 lines)  AV=2.97  SV=9.77  BV=-4.16
    {170003,12288,1616, 0, 0, 0},  //TV = 2.56(6375 lines)  AV=2.97  SV=9.78  BV=-4.25
    {180003,12288,1640, 0, 0, 0},  //TV = 2.47(6750 lines)  AV=2.97  SV=9.80  BV=-4.36
    {190003,12288,1664, 0, 0, 0},  //TV = 2.40(7125 lines)  AV=2.97  SV=9.82  BV=-4.45
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_CaptureISO1600Table =
{
    AETABLE_CAPTURE_ISO1600, //eAETableID
    151, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -45, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_1600, //ISO
    sCaptureISO1600PLineTable_60Hz,
    sCaptureISO1600PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sCaptureStrobePLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {24987,1280,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.86  BV=2.44
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {24987,1440,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.04  BV=2.25
    {24987,1568,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.15  BV=2.14
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1632,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.22  BV=1.66
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33335,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33335,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33335,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33335,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33335,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33335,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33335,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33335,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33335,3776,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.42  BV=0.46
    {33335,4096,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.54  BV=0.34
    {33335,4384,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.63  BV=0.24
    {33335,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33335,5024,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.83  BV=0.05
    {33335,5376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.93  BV=-0.05
    {33335,5760,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.03  BV=-0.15
    {33335,6208,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.14  BV=-0.26
    {33335,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33335,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33335,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33335,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33335,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33335,9376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33335,10016,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.83  BV=-0.95
    {33335,10752,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.93  BV=-1.05
    {33335,11520,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.03  BV=-1.15
    {33334,12288,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.12  BV=-1.24
    {33334,12288,1104, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.23  BV=-1.35
    {41654,11360,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.01  BV=-1.45
    {41654,12160,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.11  BV=-1.55
    {41654,12288,1088, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.21  BV=-1.65
    {50001,11648,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.04  BV=-1.75
    {50001,12288,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.14  BV=-1.85
    {58321,11456,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.02  BV=-1.95
    {58321,12288,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.12  BV=-2.05
    {66668,11520,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.03  BV=-2.15
    {66668,12288,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.12  BV=-2.24
    {66668,12288,1104, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.23  BV=-2.35
    {66668,12288,1176, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.32  BV=-2.44
    {66668,12288,1264, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.42  BV=-2.55
    {66668,12288,1352, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.52  BV=-2.64
    {66668,12288,1456, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.63  BV=-2.75
    {66668,12288,1560, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.73  BV=-2.85
    {66668,12288,1664, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.82  BV=-2.94
    {66668,12288,1800, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.93  BV=-3.06
    {66668,12288,1928, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.03  BV=-3.16
    {66668,12288,2064, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.13  BV=-3.25
    {66668,12288,2216, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.23  BV=-3.36
    {66668,12288,2376, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.34  BV=-3.46
    {66668,12288,2544, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.43  BV=-3.56
    {66668,12288,2728, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.53  BV=-3.66
    {66668,12288,2920, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.63  BV=-3.76
    {66668,12288,3128, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.73  BV=-3.85
    {66668,12288,3360, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.84  BV=-3.96
    {66668,12288,3600, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.93  BV=-4.06
    {66668,12288,3840, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=11.03  BV=-4.15
    {66668,12288,3840, 0, 0, 0},  /* TV = 3.91(2500 lines)  AV=2.97  SV=11.03  BV=-4.15 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sCaptureStrobePLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.57  BV=3.04
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {20001,1568,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.16  BV=2.45
    {20001,1696,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.28  BV=2.34
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1312,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.89  BV=2.14
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.29  BV=1.74
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.49  BV=1.54
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2240,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.68  BV=1.35
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4544,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.69  BV=0.34
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.14
    {30001,5568,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.98  BV=0.05
    {30001,5952,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.18  BV=-0.15
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7392,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.39  BV=-0.36
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.88  BV=-0.85
    {30001,11136,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.98  BV=-0.95
    {30001,11936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.08  BV=-1.05
    {30001,12288,1064, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.18  BV=-1.15
    {30001,12288,1144, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.28  BV=-1.25
    {40001,11040,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=8.97  BV=-1.35
    {40001,11840,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.07  BV=-1.45
    {40001,12288,1056, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.17  BV=-1.55
    {40001,12288,1128, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.26  BV=-1.65
    {50001,11648,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.04  BV=-1.75
    {50001,12288,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.14  BV=-1.85
    {50001,12288,1112, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.24  BV=-1.95
    {60001,11936,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.08  BV=-2.05
    {60001,12288,1064, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.18  BV=-2.15
    {70001,11776,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.06  BV=-2.25
    {70001,12288,1048, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.15  BV=-2.35
    {70001,12288,1120, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.25  BV=-2.44
    {70001,12288,1208, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.36  BV=-2.55
    {70001,12288,1288, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.45  BV=-2.64
    {70001,12288,1384, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.56  BV=-2.75
    {70001,12288,1480, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.65  BV=-2.85
    {70001,12288,1592, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.76  BV=-2.95
    {70001,12288,1712, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.86  BV=-3.06
    {70001,12288,1840, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.97  BV=-3.16
    {70001,12288,1968, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.06  BV=-3.26
    {70001,12288,2112, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.17  BV=-3.36
    {70001,12288,2264, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.27  BV=-3.46
    {70001,12288,2424, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.36  BV=-3.56
    {70001,12288,2600, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.47  BV=-3.66
    {70001,12288,2784, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.56  BV=-3.76
    {70001,12288,2984, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.66  BV=-3.86
    {70001,12288,3200, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.76  BV=-3.96
    {70001,12288,3424, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.86  BV=-4.06
    {70001,12288,3672, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.96  BV=-4.16
    {70001,12288,3840, 0, 0, 0},  /* TV = 3.84(2625 lines)  AV=2.97  SV=11.03  BV=-4.22 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_StrobeTable =
{
    AETABLE_STROBE, //eAETableID
    149, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -43, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sCaptureStrobePLineTable_60Hz,
    sCaptureStrobePLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sAEScene1PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {24987,1280,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.86  BV=2.44
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {24987,1440,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.04  BV=2.25
    {24987,1568,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.15  BV=2.14
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1632,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.22  BV=1.66
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33335,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33335,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33335,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33335,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33335,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33335,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33335,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {33335,3552,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.33  BV=0.55
    {33335,3776,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.42  BV=0.46
    {33335,4096,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.54  BV=0.34
    {33335,4384,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.63  BV=0.24
    {33335,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33335,5024,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.83  BV=0.05
    {33335,5376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.93  BV=-0.05
    {33335,5760,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.03  BV=-0.15
    {33335,6208,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.14  BV=-0.26
    {33335,6624,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.23  BV=-0.35
    {33335,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33335,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33335,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33335,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33335,9376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33335,10016,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.83  BV=-0.95
    {33335,10752,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.93  BV=-1.05
    {33335,11520,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.03  BV=-1.15
    {33334,12288,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.12  BV=-1.24
    {33334,12288,1104, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.23  BV=-1.35
    {41654,11360,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.01  BV=-1.45
    {41654,12160,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.11  BV=-1.55
    {41654,12288,1088, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=9.21  BV=-1.65
    {50001,11648,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.04  BV=-1.75
    {50001,12288,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.14  BV=-1.85
    {58321,11456,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.02  BV=-1.95
    {58321,12288,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=9.12  BV=-2.05
    {66668,11520,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.03  BV=-2.15
    {66668,12288,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.12  BV=-2.24
    {66668,12288,1104, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.23  BV=-2.35
    {66668,12288,1176, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.32  BV=-2.44
    {66668,12288,1264, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.42  BV=-2.55
    {66668,12288,1352, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.52  BV=-2.64
    {66668,12288,1456, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.63  BV=-2.75
    {66668,12288,1560, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.73  BV=-2.85
    {66668,12288,1664, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.82  BV=-2.94
    {66668,12288,1800, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=9.93  BV=-3.06
    {66668,12288,1928, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.03  BV=-3.16
    {66668,12288,2064, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.13  BV=-3.25
    {66668,12288,2216, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.23  BV=-3.36
    {66668,12288,2376, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.34  BV=-3.46
    {66668,12288,2544, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.43  BV=-3.56
    {66668,12288,2728, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.53  BV=-3.66
    {66668,12288,2920, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.63  BV=-3.76
    {66668,12288,3128, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.73  BV=-3.85
    {66668,12288,3360, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.84  BV=-3.96
    {66668,12288,3600, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=10.93  BV=-4.06
    {66668,12288,3840, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=11.03  BV=-4.15
    {74988,12288,3656, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=10.96  BV=-4.25
    {74988,12288,3944, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=11.07  BV=-4.36
    {83335,12288,3800, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=11.01  BV=-4.46
    {91655,12288,3704, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=10.98  BV=-4.56
    {91655,12288,3976, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=11.08  BV=-4.66
    {100002,12288,3904, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=11.05  BV=-4.76
    {108322,12288,3864, 0, 0, 0},  //TV = 3.21(4062 lines)  AV=2.97  SV=11.04  BV=-4.86
    {116669,12288,3840, 0, 0, 0},  //TV = 3.10(4375 lines)  AV=2.97  SV=11.03  BV=-4.96
    {124989,12288,3840, 0, 0, 0},  //TV = 3.00(4687 lines)  AV=2.97  SV=11.03  BV=-5.06
    {133335,12288,3864, 0, 0, 0},  //TV = 2.91(5000 lines)  AV=2.97  SV=11.04  BV=-5.16
    {141656,12288,3896, 0, 0, 0},  //TV = 2.82(5312 lines)  AV=2.97  SV=11.05  BV=-5.26
    {150002,12288,3944, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=11.07  BV=-5.36
    {166669,12288,3800, 0, 0, 0},  //TV = 2.58(6250 lines)  AV=2.97  SV=11.01  BV=-5.46
    {174989,12288,3880, 0, 0, 0},  //TV = 2.51(6562 lines)  AV=2.97  SV=11.04  BV=-5.56
    {191656,12288,3800, 0, 0, 0},  //TV = 2.38(7187 lines)  AV=2.97  SV=11.01  BV=-5.66
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sAEScene1PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.57  BV=3.04
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {20001,1568,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.16  BV=2.45
    {20001,1696,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.28  BV=2.34
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1312,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.89  BV=2.14
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.29  BV=1.74
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.49  BV=1.54
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2240,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.68  BV=1.35
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.88  BV=1.15
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,3008,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.09  BV=0.94
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4544,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.69  BV=0.34
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.14
    {30001,5568,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.98  BV=0.05
    {30001,5952,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.18  BV=-0.15
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7392,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.39  BV=-0.36
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8448,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.58  BV=-0.55
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.88  BV=-0.85
    {30001,11136,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.98  BV=-0.95
    {30001,11936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.08  BV=-1.05
    {30001,12288,1064, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.18  BV=-1.15
    {30001,12288,1144, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.28  BV=-1.25
    {40001,11040,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=8.97  BV=-1.35
    {40001,11840,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.07  BV=-1.45
    {40001,12288,1056, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.17  BV=-1.55
    {40001,12288,1128, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=9.26  BV=-1.65
    {50001,11648,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.04  BV=-1.75
    {50001,12288,1040, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.14  BV=-1.85
    {50001,12288,1112, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=9.24  BV=-1.95
    {60001,11936,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.08  BV=-2.05
    {60001,12288,1064, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=9.18  BV=-2.15
    {70001,11776,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.06  BV=-2.25
    {70001,12288,1048, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.15  BV=-2.35
    {70001,12288,1120, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.25  BV=-2.44
    {70001,12288,1208, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.36  BV=-2.55
    {70001,12288,1288, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.45  BV=-2.64
    {70001,12288,1384, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.56  BV=-2.75
    {70001,12288,1480, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.65  BV=-2.85
    {70001,12288,1592, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.76  BV=-2.95
    {70001,12288,1712, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.86  BV=-3.06
    {70001,12288,1840, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=9.97  BV=-3.16
    {70001,12288,1968, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.06  BV=-3.26
    {70001,12288,2112, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.17  BV=-3.36
    {70001,12288,2264, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.27  BV=-3.46
    {70001,12288,2424, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.36  BV=-3.56
    {70001,12288,2600, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.47  BV=-3.66
    {70001,12288,2784, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.56  BV=-3.76
    {70001,12288,2984, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.66  BV=-3.86
    {70001,12288,3200, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.76  BV=-3.96
    {70001,12288,3424, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.86  BV=-4.06
    {70001,12288,3656, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=10.96  BV=-4.15
    {70001,12288,3912, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=11.05  BV=-4.25
    {80001,12288,3696, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=10.97  BV=-4.36
    {80001,12288,3960, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=11.07  BV=-4.46
    {90002,12288,3776, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=11.00  BV=-4.56
    {90002,12288,4048, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=11.10  BV=-4.66
    {100002,12288,3904, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=11.05  BV=-4.76
    {110002,12288,3800, 0, 0, 0},  //TV = 3.18(4125 lines)  AV=2.97  SV=11.01  BV=-4.86
    {120002,12288,3736, 0, 0, 0},  //TV = 3.06(4500 lines)  AV=2.97  SV=10.99  BV=-4.96
    {130002,12288,3696, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=10.97  BV=-5.06
    {130002,12288,3960, 0, 0, 0},  //TV = 2.94(4875 lines)  AV=2.97  SV=11.07  BV=-5.16
    {140002,12288,3944, 0, 0, 0},  //TV = 2.84(5250 lines)  AV=2.97  SV=11.07  BV=-5.26
    {150002,12288,3944, 0, 0, 0},  //TV = 2.74(5625 lines)  AV=2.97  SV=11.07  BV=-5.36
    {170003,12288,3728, 0, 0, 0},  //TV = 2.56(6375 lines)  AV=2.97  SV=10.99  BV=-5.46
    {180003,12288,3776, 0, 0, 0},  //TV = 2.47(6750 lines)  AV=2.97  SV=11.00  BV=-5.56
    {190003,12288,3832, 0, 0, 0},  //TV = 2.40(7125 lines)  AV=2.97  SV=11.02  BV=-5.66
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_SceneTable1 =
{
    AETABLE_SCENE_INDEX1, //eAETableID
    163, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -57, //i4MinBV
    90, //i4EffectiveMaxBV
    -50, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sAEScene1PLineTable_60Hz,
    sAEScene1PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sAEScene2PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {8321,2496,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.83  BV=3.05
    {8321,2688,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.93  BV=2.95
    {8321,2880,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.04  BV=2.84
    {8321,3104,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.14  BV=2.74
    {8321,3296,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.22  BV=2.66
    {8321,3552,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.33  BV=2.55
    {8321,3808,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.43  BV=2.45
    {8321,4096,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.54  BV=2.34
    {8321,4384,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.63  BV=2.25
    {8321,4672,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.73  BV=2.15
    {8321,5024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.83  BV=2.05
    {8321,5376,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=7.93  BV=1.95
    {8321,5760,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=8.03  BV=1.85
    {16667,3072,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.12  BV=1.76
    {16667,3328,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.24  BV=1.64
    {16667,3552,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.33  BV=1.55
    {16667,3808,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.43  BV=1.45
    {16667,4096,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.54  BV=1.34
    {16667,4384,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.63  BV=1.24
    {16667,4672,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.73  BV=1.15
    {16667,5024,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=7.83  BV=1.05
    {24987,3584,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.34  BV=0.95
    {24987,3840,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.44  BV=0.85
    {24987,4128,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.55  BV=0.75
    {24987,4416,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.64  BV=0.65
    {24987,4736,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=7.75  BV=0.55
    {33334,3808,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.43  BV=0.45
    {33334,4064,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.52  BV=0.35
    {33334,4384,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.63  BV=0.24
    {33334,4672,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.73  BV=0.15
    {33334,5024,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.83  BV=0.05
    {33334,5376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.93  BV=-0.05
    {33334,5760,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.03  BV=-0.15
    {33334,6208,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.14  BV=-0.26
    {33334,6656,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.24  BV=-0.36
    {33334,7104,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.33  BV=-0.45
    {33334,7616,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.43  BV=-0.55
    {33334,8160,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.53  BV=-0.65
    {33334,8736,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.63  BV=-0.75
    {33334,9376,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.73  BV=-0.85
    {33334,10048,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.83  BV=-0.95
    {33334,10752,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=8.93  BV=-1.05
    {33334,11520,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.03  BV=-1.15
    {33334,12288,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.12  BV=-1.24
    {33334,12288,1096, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.22  BV=-1.34
    {33334,12288,1184, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.33  BV=-1.45
    {33334,12288,1272, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.43  BV=-1.56
    {33334,12288,1360, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=9.53  BV=-1.65
    {33334,12288,1360, 0, 0, 0},  /* TV = 4.91(1250 lines)  AV=2.97  SV=9.53  BV=-1.65 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sAEScene2PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.76  BV=3.86
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.97  BV=3.64
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.56  BV=3.06
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {10001,2400,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.76  BV=2.85
    {10001,2560,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.87  BV=2.75
    {10001,2752,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.96  BV=2.65
    {10001,2976,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.08  BV=2.54
    {10001,3168,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.17  BV=2.45
    {10001,3392,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.26  BV=2.35
    {10001,3648,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.37  BV=2.25
    {10001,3904,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.47  BV=2.15
    {10001,4160,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.56  BV=2.06
    {10001,4480,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.67  BV=1.95
    {10001,4800,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.76  BV=1.85
    {10001,5120,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.86  BV=1.76
    {10001,5536,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=7.97  BV=1.64
    {10001,5920,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=8.07  BV=1.55
    {20001,3168,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.17  BV=1.45
    {20001,3392,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.26  BV=1.35
    {20001,3648,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.37  BV=1.25
    {20001,3904,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.47  BV=1.15
    {20001,4192,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.57  BV=1.05
    {20001,4480,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.67  BV=0.95
    {20001,4800,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=7.76  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {30001,3680,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.38  BV=0.65
    {30001,3936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.48  BV=0.55
    {30001,4224,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.58  BV=0.45
    {30001,4512,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.68  BV=0.35
    {30001,4864,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.78  BV=0.25
    {30001,5216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.88  BV=0.14
    {30001,5568,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.98  BV=0.05
    {30001,5952,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.08  BV=-0.05
    {30001,6400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.18  BV=-0.15
    {30001,6880,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.28  BV=-0.25
    {30001,7392,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.39  BV=-0.36
    {30001,7904,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.48  BV=-0.45
    {30001,8480,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.59  BV=-0.56
    {30001,9056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.68  BV=-0.65
    {30001,9728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.78  BV=-0.75
    {30001,10400,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.88  BV=-0.85
    {30001,11136,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=8.98  BV=-0.95
    {30001,11936,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.08  BV=-1.05
    {30001,12288,1064, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.18  BV=-1.15
    {30001,12288,1136, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.27  BV=-1.24
    {30001,12288,1232, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.39  BV=-1.36
    {30001,12288,1320, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=9.49  BV=-1.46
    {30001,12288,1360, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=9.53  BV=-1.50 */
    {30001,12288,1360, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=9.53  BV=-1.50 */
    {30001,12288,1360, 0, 0, 0},  /* TV = 5.06(1125 lines)  AV=2.97  SV=9.53  BV=-1.50 */
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_SceneTable2 =
{
    AETABLE_SCENE_INDEX2, //eAETableID
    124, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -17, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sAEScene2PLineTable_60Hz,
    sAEScene2PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sAEScene3PLineTable_60Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {8321,2176,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.62  BV=3.26
    {8321,2336,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {16668,1536,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.12  BV=2.76
    {16668,1632,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.22  BV=2.66
    {16668,1760,1032, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.33  BV=2.55
    {24987,1280,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.86  BV=2.44
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {24987,1440,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.04  BV=2.25
    {24987,1568,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=6.15  BV=2.14
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1632,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.22  BV=1.66
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33334,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {41654,1856,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.41  BV=1.15
    {41654,2016,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.51  BV=1.04
    {41654,2144,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=6.60  BV=0.95
    {50001,1920,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.44  BV=0.85
    {50001,2048,1032, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.55  BV=0.75
    {50001,2208,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.64  BV=0.65
    {58321,2016,1032, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=6.52  BV=0.55
    {58321,2176,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=6.62  BV=0.45
    {66668,2048,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.54  BV=0.34
    {66668,2176,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.62  BV=0.25
    {66668,2336,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.73  BV=0.15
    {66668,2496,1032, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.83  BV=0.05
    {66668,2688,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=6.93  BV=-0.05
    {66668,2880,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.03  BV=-0.15
    {66668,3072,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.12  BV=-0.24
    {66668,3328,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.24  BV=-0.36
    {66668,3552,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.33  BV=-0.45
    {66668,3808,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.43  BV=-0.55
    {66668,4064,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.52  BV=-0.65
    {74988,3904,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=7.47  BV=-0.76
    {74988,4160,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=7.56  BV=-0.85
    {83335,4032,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=7.51  BV=-0.96
    {91655,3904,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=7.47  BV=-1.05
    {91655,4192,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=7.57  BV=-1.15
    {100002,4096,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.54  BV=-1.24
    {100002,4416,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.64  BV=-1.35
    {100002,4704,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.74  BV=-1.44
    {100002,5088,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.85  BV=-1.56
    {100002,5440,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.95  BV=-1.65
    {100002,5824,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.04  BV=-1.75
    {100002,6240,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.14  BV=-1.85
    {100002,6688,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.24  BV=-1.95
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8224,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.54  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9504,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.46
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10912,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.66
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sAEScene3PLineTable_50Hz =
{
{
    {81,1472,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=6.06  BV=10.50
    {81,1504,1040, 0, 0, 0},  /* TV = 13.59(3 lines)  AV=2.97  SV=6.11  BV=10.45 */
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {107,1312,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.92  BV=10.24
    {107,1408,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=6.01  BV=10.15
    {134,1248,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.82  BV=10.01 */
    {134,1312,1032, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.90  BV=9.93
    {134,1408,1040, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=6.02  BV=9.82
    {161,1280,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.86  BV=9.71
    {161,1376,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.96  BV=9.61
    {187,1216,1040, 0, 0, 0},  /* TV = 12.38(7 lines)  AV=2.97  SV=5.81  BV=9.55 */
    {187,1312,1032, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.90  BV=9.45
    {214,1216,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.81  BV=9.35
    {214,1312,1032, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.90  BV=9.26
    {241,1248,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.83  BV=9.16
    {267,1216,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.78  BV=9.06
    {267,1312,1024, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.89  BV=8.95
    {294,1280,1024, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.86  BV=8.84
    {321,1248,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.82  BV=8.75
    {347,1248,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.82  BV=8.64
    {374,1216,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.81  BV=8.55
    {401,1216,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.81  BV=8.45
    {427,1248,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.82  BV=8.34
    {454,1248,1024, 0, 0, 0},  //TV = 11.11(17 lines)  AV=2.97  SV=5.82  BV=8.25
    {507,1184,1040, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.77  BV=8.15
    {534,1216,1024, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.78  BV=8.06
    {561,1248,1024, 0, 0, 0},  //TV = 10.80(21 lines)  AV=2.97  SV=5.82  BV=7.95
    {614,1216,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.78  BV=7.86
    {667,1184,1040, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.77  BV=7.75
    {694,1248,1024, 0, 0, 0},  //TV = 10.49(26 lines)  AV=2.97  SV=5.82  BV=7.64
    {747,1216,1040, 0, 0, 0},  //TV = 10.39(28 lines)  AV=2.97  SV=5.81  BV=7.55
    {827,1184,1040, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.77  BV=7.44
    {881,1184,1040, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.77  BV=7.35
    {934,1216,1024, 0, 0, 0},  //TV = 10.06(35 lines)  AV=2.97  SV=5.78  BV=7.25
    {1014,1184,1040, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.77  BV=7.15
    {1067,1216,1024, 0, 0, 0},  //TV = 9.87(40 lines)  AV=2.97  SV=5.78  BV=7.06
    {1147,1216,1024, 0, 0, 0},  //TV = 9.77(43 lines)  AV=2.97  SV=5.78  BV=6.95
    {1227,1216,1024, 0, 0, 0},  //TV = 9.67(46 lines)  AV=2.97  SV=5.78  BV=6.86
    {1334,1184,1040, 0, 0, 0},  //TV = 9.55(50 lines)  AV=2.97  SV=5.77  BV=6.75
    {1441,1184,1032, 0, 0, 0},  //TV = 9.44(54 lines)  AV=2.97  SV=5.76  BV=6.65
    {1521,1216,1024, 0, 0, 0},  //TV = 9.36(57 lines)  AV=2.97  SV=5.78  BV=6.55
    {1654,1184,1040, 0, 0, 0},  //TV = 9.24(62 lines)  AV=2.97  SV=5.77  BV=6.44
    {1761,1184,1040, 0, 0, 0},  //TV = 9.15(66 lines)  AV=2.97  SV=5.77  BV=6.35
    {1894,1184,1040, 0, 0, 0},  //TV = 9.04(71 lines)  AV=2.97  SV=5.77  BV=6.25
    {2027,1184,1040, 0, 0, 0},  //TV = 8.95(76 lines)  AV=2.97  SV=5.77  BV=6.15
    {2161,1216,1024, 0, 0, 0},  //TV = 8.85(81 lines)  AV=2.97  SV=5.78  BV=6.04
    {2321,1184,1040, 0, 0, 0},  //TV = 8.75(87 lines)  AV=2.97  SV=5.77  BV=5.95
    {2507,1184,1032, 0, 0, 0},  //TV = 8.64(94 lines)  AV=2.97  SV=5.76  BV=5.85
    {2667,1184,1040, 0, 0, 0},  //TV = 8.55(100 lines)  AV=2.97  SV=5.77  BV=5.75
    {2854,1184,1040, 0, 0, 0},  //TV = 8.45(107 lines)  AV=2.97  SV=5.77  BV=5.66
    {3067,1184,1040, 0, 0, 0},  //TV = 8.35(115 lines)  AV=2.97  SV=5.77  BV=5.55
    {3281,1184,1040, 0, 0, 0},  //TV = 8.25(123 lines)  AV=2.97  SV=5.77  BV=5.45
    {3547,1184,1032, 0, 0, 0},  //TV = 8.14(133 lines)  AV=2.97  SV=5.76  BV=5.35
    {3787,1184,1040, 0, 0, 0},  //TV = 8.04(142 lines)  AV=2.97  SV=5.77  BV=5.25
    {4054,1184,1040, 0, 0, 0},  //TV = 7.95(152 lines)  AV=2.97  SV=5.77  BV=5.15
    {4374,1184,1032, 0, 0, 0},  //TV = 7.84(164 lines)  AV=2.97  SV=5.76  BV=5.05
    {4641,1184,1040, 0, 0, 0},  //TV = 7.75(174 lines)  AV=2.97  SV=5.77  BV=4.95
    {5014,1184,1032, 0, 0, 0},  //TV = 7.64(188 lines)  AV=2.97  SV=5.76  BV=4.85
    {5361,1184,1040, 0, 0, 0},  //TV = 7.54(201 lines)  AV=2.97  SV=5.77  BV=4.75
    {5761,1184,1032, 0, 0, 0},  //TV = 7.44(216 lines)  AV=2.97  SV=5.76  BV=4.65
    {6161,1184,1032, 0, 0, 0},  //TV = 7.34(231 lines)  AV=2.97  SV=5.76  BV=4.56
    {6641,1184,1032, 0, 0, 0},  //TV = 7.23(249 lines)  AV=2.97  SV=5.76  BV=4.45
    {7121,1184,1032, 0, 0, 0},  //TV = 7.13(267 lines)  AV=2.97  SV=5.76  BV=4.35
    {7627,1184,1032, 0, 0, 0},  //TV = 7.03(286 lines)  AV=2.97  SV=5.76  BV=4.25
    {8161,1184,1032, 0, 0, 0},  //TV = 6.94(306 lines)  AV=2.97  SV=5.76  BV=4.15
    {8747,1184,1032, 0, 0, 0},  //TV = 6.84(328 lines)  AV=2.97  SV=5.76  BV=4.05
    {9361,1184,1032, 0, 0, 0},  //TV = 6.74(351 lines)  AV=2.97  SV=5.76  BV=3.95
    {10001,1184,1040, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.77  BV=3.85
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {10001,2080,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.57  BV=3.04
    {10001,2240,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {20001,1568,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.16  BV=2.45
    {20001,1696,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.28  BV=2.34
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1312,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.89  BV=2.14
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1728,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.29  BV=1.74
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1984,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.49  BV=1.54
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2240,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.68  BV=1.35
    {40001,1824,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.37  BV=1.25
    {40001,1952,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.47  BV=1.15
    {40001,2080,1032, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.57  BV=1.05
    {40001,2240,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.67  BV=0.95
    {50001,1920,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.44  BV=0.85
    {50001,2048,1032, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.55  BV=0.75
    {50001,2208,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=6.64  BV=0.65
    {60001,1952,1032, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=6.48  BV=0.55
    {60001,2112,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=6.58  BV=0.45
    {70001,1952,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.47  BV=0.34
    {70001,2080,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.56  BV=0.25
    {70001,2208,1032, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.66  BV=0.15
    {70001,2400,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.76  BV=0.04
    {70001,2560,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.86  BV=-0.05
    {70001,2752,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=6.96  BV=-0.15
    {70001,2944,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.06  BV=-0.25
    {70001,3168,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.17  BV=-0.36
    {70001,3392,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.26  BV=-0.46
    {70001,3616,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.36  BV=-0.55
    {70001,3872,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.45  BV=-0.65
    {70001,4160,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.56  BV=-0.75
    {80001,3904,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=7.47  BV=-0.85
    {80001,4192,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=7.57  BV=-0.95
    {90002,4000,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=7.50  BV=-1.06
    {90002,4288,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=7.60  BV=-1.16
    {100002,4096,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.54  BV=-1.24
    {100002,4416,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.64  BV=-1.35
    {100002,4704,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.74  BV=-1.44
    {100002,5088,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.85  BV=-1.56
    {100002,5440,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=7.95  BV=-1.65
    {100002,5824,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.04  BV=-1.75
    {100002,6240,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.14  BV=-1.85
    {100002,6688,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.24  BV=-1.95
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8224,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.54  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9504,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.46
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10912,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.66
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_SceneTable3 =
{
    AETABLE_SCENE_INDEX3, //eAETableID
    135, //u4TotalIndex
    20, //i4StrobeTrigerBV
    105, //i4MaxBV
    -29, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sAEScene3PLineTable_60Hz,
    sAEScene3PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sAEScene4PLineTable_60Hz =
{
{
    {200003,1696,1032, 0, 0, 0},  //TV = 2.32(7500 lines)  AV=2.97  SV=6.28  BV=-0.98
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sAEScene4PLineTable_50Hz =
{
{
    {200003,1696,1032, 0, 0, 0},  //TV = 2.32(7500 lines)  AV=2.97  SV=6.28  BV=-0.98
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_SceneTable4 =
{
    AETABLE_SCENE_INDEX4, //eAETableID
    1, //u4TotalIndex
    20, //i4StrobeTrigerBV
    -9, //i4MaxBV
    -10, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sAEScene4PLineTable_60Hz,
    sAEScene4PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sAEScene5PLineTable_60Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8321,1088,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.62  BV=4.26
    {8321,1152,1040, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.73  BV=4.15
    {8321,1248,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.82  BV=4.06
    {8321,1344,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.93  BV=3.95
    {8321,1440,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.03  BV=3.85
    {8321,1536,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.13  BV=3.75
    {8321,1664,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.24  BV=3.64
    {8321,1760,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.33  BV=3.55
    {8321,1888,1032, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.43  BV=3.45
    {8321,2048,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=6.54  BV=3.34
    {16668,1088,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.62  BV=3.25
    {16668,1152,1040, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.73  BV=3.15
    {16668,1248,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.82  BV=3.06
    {16668,1344,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=5.93  BV=2.95
    {16668,1440,1024, 0, 0, 0},  //TV = 5.91(625 lines)  AV=2.97  SV=6.03  BV=2.85
    {24987,1024,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.55  BV=2.75
    {24987,1088,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.63  BV=2.66
    {24987,1184,1024, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.75  BV=2.55
    {24987,1248,1040, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.84  BV=2.45
    {24987,1344,1032, 0, 0, 0},  //TV = 5.32(937 lines)  AV=2.97  SV=5.94  BV=2.35
    {33335,1088,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.62  BV=2.25
    {33335,1152,1040, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.73  BV=2.15
    {33335,1248,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.82  BV=2.06
    {33335,1344,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=5.93  BV=1.95
    {33335,1440,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.03  BV=1.85
    {33335,1536,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.12  BV=1.76
    {33335,1664,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.24  BV=1.64
    {33335,1760,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.33  BV=1.55
    {33335,1888,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.43  BV=1.45
    {33335,2016,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.52  BV=1.35
    {33335,2176,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.62  BV=1.25
    {33335,2336,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.73  BV=1.15
    {33335,2496,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.82  BV=1.06
    {33335,2688,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=6.93  BV=0.95
    {33335,2880,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.03  BV=0.85
    {33334,3072,1032, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.13  BV=0.75
    {33334,3296,1024, 0, 0, 0},  //TV = 4.91(1250 lines)  AV=2.97  SV=7.22  BV=0.66
    {41654,2816,1032, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.01  BV=0.55
    {41654,3040,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.11  BV=0.45
    {41654,3264,1024, 0, 0, 0},  //TV = 4.59(1562 lines)  AV=2.97  SV=7.21  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {58321,2848,1032, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.02  BV=0.05
    {58321,3072,1024, 0, 0, 0},  //TV = 4.10(2187 lines)  AV=2.97  SV=7.12  BV=-0.05
    {66668,2880,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.03  BV=-0.15
    {66668,3072,1032, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.13  BV=-0.25
    {66668,3296,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.22  BV=-0.34
    {66668,3552,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.33  BV=-0.45
    {66668,3808,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.43  BV=-0.55
    {66668,4064,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.52  BV=-0.65
    {66668,4352,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.62  BV=-0.75
    {66668,4672,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.73  BV=-0.85
    {66668,4992,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.82  BV=-0.94
    {66668,5408,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=7.94  BV=-1.06
    {66668,5792,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.04  BV=-1.16
    {66668,6176,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.13  BV=-1.25
    {66668,6624,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.23  BV=-1.35
    {66668,7104,1024, 0, 0, 0},  //TV = 3.91(2500 lines)  AV=2.97  SV=8.33  BV=-1.45
    {74988,6784,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.26  BV=-1.56
    {74988,7264,1024, 0, 0, 0},  //TV = 3.74(2812 lines)  AV=2.97  SV=8.36  BV=-1.65
    {83335,7008,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.31  BV=-1.76
    {83335,7488,1024, 0, 0, 0},  //TV = 3.58(3125 lines)  AV=2.97  SV=8.41  BV=-1.85
    {91655,7296,1024, 0, 0, 0},  //TV = 3.45(3437 lines)  AV=2.97  SV=8.37  BV=-1.95
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strEvPline sAEScene5PLineTable_50Hz =
{
{
    {81,1248,1032, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.83  BV=10.73
    {81,1344,1024, 0, 0, 0},  //TV = 13.59(3 lines)  AV=2.97  SV=5.93  BV=10.63
    {107,1088,1032, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.63  BV=10.53
    {107,1152,1040, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.73  BV=10.43
    {107,1248,1024, 0, 0, 0},  //TV = 13.19(4 lines)  AV=2.97  SV=5.82  BV=10.34
    {134,1056,1024, 0, 0, 0},  //TV = 12.87(5 lines)  AV=2.97  SV=5.58  BV=10.26
    {134,1152,1040, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.73  BV=10.11 */
    {134,1216,1024, 0, 0, 0},  /* TV = 12.87(5 lines)  AV=2.97  SV=5.78  BV=10.05 */
    {161,1088,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.62  BV=9.95
    {161,1184,1024, 0, 0, 0},  //TV = 12.60(6 lines)  AV=2.97  SV=5.75  BV=9.83
    {187,1088,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.62  BV=9.73
    {187,1152,1024, 0, 0, 0},  //TV = 12.38(7 lines)  AV=2.97  SV=5.71  BV=9.65
    {214,1056,1040, 0, 0, 0},  //TV = 12.19(8 lines)  AV=2.97  SV=5.60  BV=9.56
    {241,1024,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.55  BV=9.44
    {241,1088,1032, 0, 0, 0},  //TV = 12.02(9 lines)  AV=2.97  SV=5.63  BV=9.35
    {267,1056,1032, 0, 0, 0},  //TV = 11.87(10 lines)  AV=2.97  SV=5.59  BV=9.25
    {294,1024,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.56  BV=9.14
    {294,1088,1040, 0, 0, 0},  //TV = 11.73(11 lines)  AV=2.97  SV=5.65  BV=9.06
    {321,1088,1024, 0, 0, 0},  //TV = 11.61(12 lines)  AV=2.97  SV=5.62  BV=8.95
    {347,1088,1024, 0, 0, 0},  //TV = 11.49(13 lines)  AV=2.97  SV=5.62  BV=8.84
    {374,1056,1040, 0, 0, 0},  //TV = 11.38(14 lines)  AV=2.97  SV=5.60  BV=8.75
    {401,1056,1040, 0, 0, 0},  //TV = 11.28(15 lines)  AV=2.97  SV=5.60  BV=8.65
    {427,1088,1024, 0, 0, 0},  //TV = 11.19(16 lines)  AV=2.97  SV=5.62  BV=8.54
    {481,1024,1032, 0, 0, 0},  //TV = 11.02(18 lines)  AV=2.97  SV=5.55  BV=8.45
    {507,1024,1048, 0, 0, 0},  //TV = 10.95(19 lines)  AV=2.97  SV=5.57  BV=8.35
    {534,1056,1032, 0, 0, 0},  //TV = 10.87(20 lines)  AV=2.97  SV=5.59  BV=8.25
    {587,1024,1040, 0, 0, 0},  //TV = 10.73(22 lines)  AV=2.97  SV=5.56  BV=8.15
    {614,1056,1024, 0, 0, 0},  //TV = 10.67(23 lines)  AV=2.97  SV=5.58  BV=8.06
    {667,1056,1024, 0, 0, 0},  //TV = 10.55(25 lines)  AV=2.97  SV=5.58  BV=7.94
    {721,1024,1048, 0, 0, 0},  //TV = 10.44(27 lines)  AV=2.97  SV=5.57  BV=7.84
    {774,1024,1040, 0, 0, 0},  //TV = 10.34(29 lines)  AV=2.97  SV=5.56  BV=7.75
    {827,1024,1048, 0, 0, 0},  //TV = 10.24(31 lines)  AV=2.97  SV=5.57  BV=7.64
    {881,1024,1048, 0, 0, 0},  //TV = 10.15(33 lines)  AV=2.97  SV=5.57  BV=7.55
    {961,1024,1032, 0, 0, 0},  //TV = 10.02(36 lines)  AV=2.97  SV=5.55  BV=7.45
    {1014,1024,1048, 0, 0, 0},  //TV = 9.95(38 lines)  AV=2.97  SV=5.57  BV=7.35
    {1094,1024,1040, 0, 0, 0},  //TV = 9.84(41 lines)  AV=2.97  SV=5.56  BV=7.25
    {1174,1024,1040, 0, 0, 0},  //TV = 9.73(44 lines)  AV=2.97  SV=5.56  BV=7.15
    {1281,1024,1024, 0, 0, 0},  //TV = 9.61(48 lines)  AV=2.97  SV=5.54  BV=7.04
    {1361,1024,1032, 0, 0, 0},  //TV = 9.52(51 lines)  AV=2.97  SV=5.55  BV=6.94
    {1467,1024,1032, 0, 0, 0},  //TV = 9.41(55 lines)  AV=2.97  SV=5.55  BV=6.84
    {1574,1024,1024, 0, 0, 0},  //TV = 9.31(59 lines)  AV=2.97  SV=5.54  BV=6.75
    {1681,1024,1032, 0, 0, 0},  //TV = 9.22(63 lines)  AV=2.97  SV=5.55  BV=6.64
    {1787,1024,1040, 0, 0, 0},  //TV = 9.13(67 lines)  AV=2.97  SV=5.56  BV=6.54
    {1921,1024,1032, 0, 0, 0},  //TV = 9.02(72 lines)  AV=2.97  SV=5.55  BV=6.45
    {2081,1024,1024, 0, 0, 0},  //TV = 8.91(78 lines)  AV=2.97  SV=5.54  BV=6.34
    {2187,1024,1040, 0, 0, 0},  //TV = 8.84(82 lines)  AV=2.97  SV=5.56  BV=6.25
    {2374,1024,1032, 0, 0, 0},  //TV = 8.72(89 lines)  AV=2.97  SV=5.55  BV=6.14
    {2534,1024,1032, 0, 0, 0},  //TV = 8.62(95 lines)  AV=2.97  SV=5.55  BV=6.05
    {2721,1024,1032, 0, 0, 0},  //TV = 8.52(102 lines)  AV=2.97  SV=5.55  BV=5.95
    {2934,1024,1024, 0, 0, 0},  //TV = 8.41(110 lines)  AV=2.97  SV=5.54  BV=5.85
    {3121,1024,1032, 0, 0, 0},  //TV = 8.32(117 lines)  AV=2.97  SV=5.55  BV=5.75
    {3361,1024,1032, 0, 0, 0},  //TV = 8.22(126 lines)  AV=2.97  SV=5.55  BV=5.64
    {3601,1024,1032, 0, 0, 0},  //TV = 8.12(135 lines)  AV=2.97  SV=5.55  BV=5.54
    {3867,1024,1024, 0, 0, 0},  //TV = 8.01(145 lines)  AV=2.97  SV=5.54  BV=5.45
    {4134,1024,1032, 0, 0, 0},  //TV = 7.92(155 lines)  AV=2.97  SV=5.55  BV=5.34
    {4427,1024,1032, 0, 0, 0},  //TV = 7.82(166 lines)  AV=2.97  SV=5.55  BV=5.24
    {4774,1024,1024, 0, 0, 0},  //TV = 7.71(179 lines)  AV=2.97  SV=5.54  BV=5.15
    {5121,1024,1024, 0, 0, 0},  //TV = 7.61(192 lines)  AV=2.97  SV=5.54  BV=5.04
    {5467,1024,1032, 0, 0, 0},  //TV = 7.52(205 lines)  AV=2.97  SV=5.55  BV=4.94
    {5867,1024,1024, 0, 0, 0},  //TV = 7.41(220 lines)  AV=2.97  SV=5.54  BV=4.85
    {6294,1024,1024, 0, 0, 0},  //TV = 7.31(236 lines)  AV=2.97  SV=5.54  BV=4.75
    {6774,1024,1024, 0, 0, 0},  //TV = 7.21(254 lines)  AV=2.97  SV=5.54  BV=4.64
    {7254,1024,1024, 0, 0, 0},  //TV = 7.11(272 lines)  AV=2.97  SV=5.54  BV=4.54
    {7734,1024,1024, 0, 0, 0},  //TV = 7.01(290 lines)  AV=2.97  SV=5.54  BV=4.45
    {8321,1024,1024, 0, 0, 0},  //TV = 6.91(312 lines)  AV=2.97  SV=5.54  BV=4.34
    {8934,1024,1024, 0, 0, 0},  //TV = 6.81(335 lines)  AV=2.97  SV=5.54  BV=4.24
    {9574,1024,1024, 0, 0, 0},  //TV = 6.71(359 lines)  AV=2.97  SV=5.54  BV=4.14
    {10001,1024,1048, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.57  BV=4.05
    {10001,1120,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.67  BV=3.95
    {10001,1184,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.76  BV=3.86
    {10001,1280,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.86  BV=3.76
    {10001,1376,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=5.96  BV=3.65
    {10001,1472,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.06  BV=3.55
    {10001,1568,1032, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.16  BV=3.45
    {10001,1696,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.26  BV=3.35
    {10001,1824,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.37  BV=3.25
    {10001,1952,1024, 0, 0, 0},  //TV = 6.64(375 lines)  AV=2.97  SV=6.47  BV=3.15
    {20001,1024,1048, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.57  BV=3.05
    {20001,1120,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.67  BV=2.95
    {20001,1184,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.76  BV=2.86
    {20001,1280,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.86  BV=2.76
    {20001,1376,1032, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=5.97  BV=2.64
    {20001,1472,1024, 0, 0, 0},  //TV = 5.64(750 lines)  AV=2.97  SV=6.06  BV=2.56
    {30001,1056,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.58  BV=2.45
    {30001,1120,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.68  BV=2.35
    {30001,1216,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.78  BV=2.25
    {30001,1280,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.88  BV=2.15
    {30001,1376,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=5.98  BV=2.05
    {30001,1472,1040, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.08  BV=1.95
    {30001,1600,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.18  BV=1.85
    {30001,1696,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.28  BV=1.75
    {30001,1824,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.38  BV=1.65
    {30001,1952,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.48  BV=1.55
    {30001,2112,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.58  BV=1.45
    {30001,2272,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.69  BV=1.34
    {30001,2432,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.78  BV=1.25
    {30001,2592,1032, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.89  BV=1.14
    {30001,2784,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=6.98  BV=1.05
    {30001,2976,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.08  BV=0.95
    {30001,3200,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.18  BV=0.85
    {30001,3424,1024, 0, 0, 0},  //TV = 5.06(1125 lines)  AV=2.97  SV=7.28  BV=0.75
    {40001,2752,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=6.96  BV=0.65
    {40001,2944,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.06  BV=0.56
    {40001,3168,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.17  BV=0.45
    {40001,3392,1024, 0, 0, 0},  //TV = 4.64(1500 lines)  AV=2.97  SV=7.26  BV=0.35
    {50001,2912,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.04  BV=0.25
    {50001,3104,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.14  BV=0.16
    {50001,3328,1024, 0, 0, 0},  //TV = 4.32(1875 lines)  AV=2.97  SV=7.24  BV=0.06
    {60001,2976,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.08  BV=-0.05
    {60001,3200,1024, 0, 0, 0},  //TV = 4.06(2250 lines)  AV=2.97  SV=7.18  BV=-0.15
    {70001,2944,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.06  BV=-0.25
    {70001,3136,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.15  BV=-0.34
    {70001,3392,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.26  BV=-0.46
    {70001,3616,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.36  BV=-0.55
    {70001,3872,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.45  BV=-0.65
    {70001,4160,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.56  BV=-0.75
    {70001,4448,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.65  BV=-0.85
    {70001,4768,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.76  BV=-0.95
    {70001,5152,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.87  BV=-1.06
    {70001,5504,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=7.96  BV=-1.15
    {70001,5888,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.06  BV=-1.25
    {70001,6304,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.16  BV=-1.35
    {70001,6752,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.26  BV=-1.45
    {70001,7264,1024, 0, 0, 0},  //TV = 3.84(2625 lines)  AV=2.97  SV=8.36  BV=-1.56
    {80001,6816,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.27  BV=-1.66
    {80001,7296,1024, 0, 0, 0},  //TV = 3.64(3000 lines)  AV=2.97  SV=8.37  BV=-1.75
    {90002,6944,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.30  BV=-1.85
    {90002,7456,1024, 0, 0, 0},  //TV = 3.47(3375 lines)  AV=2.97  SV=8.40  BV=-1.96
    {100002,7168,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.34  BV=-2.05
    {100002,7680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.44  BV=-2.15
    {100002,8256,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.55  BV=-2.25
    {100002,8864,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.65  BV=-2.36
    {100002,9472,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.75  BV=-2.45
    {100002,10176,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.85  BV=-2.56
    {100002,10880,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=8.95  BV=-2.65
    {100002,11680,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.05  BV=-2.76
    {100002,12288,1024, 0, 0, 0},  //TV = 3.32(3750 lines)  AV=2.97  SV=9.12  BV=-2.83
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
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

static constexpr strAETable g_AE_SceneTable5 =
{
    AETABLE_SCENE_INDEX5, //eAETableID
    137, //u4TotalIndex
    20, //i4StrobeTrigerBV
    108, //i4MaxBV
    -29, //i4MinBV
    90, //i4EffectiveMaxBV
    -30, //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO
    sAEScene5PLineTable_60Hz,
    sAEScene5PLineTable_50Hz,
    NULL,
};
static constexpr strEvPline sAESceneReservePLineTable =
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
static constexpr strAETable g_AE_ReserveSceneTable =
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

static constexpr strAESceneMapping g_AEScenePLineMapping = 
{
{
    {LIB3A_AE_SCENE_AUTO, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX5}},
    {LIB3A_AE_SCENE_NIGHT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_ACTION, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_BEACH, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_CANDLELIGHT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_FIREWORKS, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX4, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX4}},
    {LIB3A_AE_SCENE_LANDSCAPE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_PORTRAIT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_NIGHT_PORTRAIT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
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
    {LIB3A_AE_SCENE_ISO1600, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO1600, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO1600}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_AUTO}},
},
};

static constexpr strAEPLineTable g_strAEPlineTable =
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
    g_AE_ReserveSceneTable,
},
};

static constexpr strAEPLineNumInfo g_strAEPreviewAutoPLineInfo =
{
    AETABLE_RPEVIEW_AUTO,
    90,
    -50,
    {
        {1,10000,30,1024,1024},
        {2,30,30,1024,3072},
        {3,30,15,3072,3072},
        {4,15,15,3072,7168},
        {5,15,10,7168,7168},
        {6,10,10,7168,12288},
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

static constexpr strAEPLineNumInfo g_strAECaptureAutoPLineInfo =
{
    AETABLE_CAPTURE_AUTO,
    90,
    -30,
    {
        {1,10000,30,1024,1024},
        {2,30,30,1024,3072},
        {3,30,15,3072,3072},
        {4,15,15,3072,7168},
        {5,15,10,7168,7168},
        {6,10,10,7168,12288},
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

static constexpr strAEPLineNumInfo g_strAEVideoAutoPLineInfo =
{
    AETABLE_VIDEO_AUTO,
    90,
    -30,
    {
        {1,10000,30,1024,1024},
        {2,30,30,1024,3072},
        {3,30,15,3072,3072},
        {4,15,15,3072,7168},
        {5,15,10,7168,7168},
        {6,10,10,7168,12288},
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

static constexpr strAEPLineNumInfo g_strAEVideo1AutoPLineInfo =
{
    AETABLE_VIDEO1_AUTO,
    90,
    -30,
    {
        {1,10000,121,1195,1195},
        {2,121,121,1195,21845},
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

static constexpr strAEPLineNumInfo g_strAEVideo2AutoPLineInfo =
{
    AETABLE_VIDEO2_AUTO,
    90,
    -30,
    {
        {1,10000,60,1195,1195},
        {2,60,60,1195,3072},
        {3,60,30,3072,3072},
        {4,30,30,3072,10240},
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

static constexpr strAEPLineNumInfo g_strAECustom1AutoPLineInfo =
{
    AETABLE_CUSTOM1_AUTO,
    90,
    -30,
    {
        {1,10000,60,1195,1195},
        {2,60,60,1195,3072},
        {3,60,30,3072,3072},
        {4,30,30,3072,10240},
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

static constexpr strAEPLineNumInfo g_strAECustom2AutoPLineInfo =
{
    AETABLE_CUSTOM2_AUTO,
    90,
    -30,
    {
        {1,10000,60,1195,1195},
        {2,60,60,1195,3072},
        {3,60,30,3072,3072},
        {4,30,30,3072,10240},
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

static constexpr strAEPLineNumInfo g_strAECustom3AutoPLineInfo =
{
    AETABLE_CUSTOM3_AUTO,
    90,
    -30,
    {
        {1,10000,60,1195,1195},
        {2,60,60,1195,3072},
        {3,60,30,3072,3072},
        {4,30,30,3072,10240},
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

static constexpr strAEPLineNumInfo g_strAECustom4AutoPLineInfo =
{
    AETABLE_CUSTOM4_AUTO,
    90,
    -30,
    {
        {1,10000,60,1195,1195},
        {2,60,60,1195,3072},
        {3,60,30,3072,3072},
        {4,30,30,3072,10240},
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

static constexpr strAEPLineNumInfo g_strAECustom5AutoPLineInfo =
{
    AETABLE_CUSTOM5_AUTO,
    90,
    -30,
    {
        {1,10000,60,1195,1195},
        {2,60,60,1195,3072},
        {3,60,30,3072,3072},
        {4,30,30,3072,10240},
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

static constexpr strAEPLineNumInfo g_strAEVideoNightPLineInfo =
{
    AETABLE_VIDEO_NIGHT,
    90,
    -30,
    {
        {1,10000,30,1195,1195},
        {2,30,30,1195,12288},
        {3,30,15,12288,12288},
        {4,15,15,12288,35200},
        {5,15,10,35200,35200},
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

static constexpr strAEPLineNumInfo g_strAECaptureISO100PLineInfo =
{
    AETABLE_CAPTURE_ISO100,
    90,
    -30,
    {
        {1,10000,5,1230,1230},
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

static constexpr strAEPLineNumInfo g_strAECaptureISO200PLineInfo =
{
    AETABLE_CAPTURE_ISO200,
    90,
    -30,
    {
        {1,10000,2000,1195,1195},
        {2,2000,2000,1195,2460},
        {3,2000,5,2460,2460},
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

static constexpr strAEPLineNumInfo g_strAECaptureISO400PLineInfo =
{
    AETABLE_CAPTURE_ISO400,
    90,
    -30,
    {
        {1,10000,2000,1195,1195},
        {2,2000,2000,1195,4920},
        {3,2000,5,4920,4920},
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

static constexpr strAEPLineNumInfo g_strAECaptureISO800PLineInfo =
{
    AETABLE_CAPTURE_ISO800,
    90,
    -30,
    {
        {1,10000,2000,1195,1195},
        {2,2000,2000,1195,9840},
        {3,2000,5,9840,9840},
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

static constexpr strAEPLineNumInfo g_strAECaptureISO1600PLineInfo =
{
    AETABLE_CAPTURE_ISO1600,
    90,
    -30,
    {
        {1,10000,2000,1195,1195},
        {2,2000,2000,1195,19680},
        {3,2000,5,19680,19680},
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

static constexpr strAEPLineNumInfo g_strAEStrobePLineInfo =
{
    AETABLE_STROBE,
    90,
    -30,
    {
        {1,10000,30,1195,1195},
        {2,30,30,1195,12288},
        {3,30,15,12288,12288},
        {4,15,15,12288,46080},
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

static constexpr strAEPLineNumInfo g_strAEScene1PLineInfo =
{
    AETABLE_SCENE_INDEX1,
    90,
    -50,
    {
        {1,10000,30,1195,1195},
        {2,30,30,1195,12288},
        {3,30,15,12288,12288},
        {4,15,15,12288,46080},
        {5,15,5,46080,46080},
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

static constexpr strAEPLineNumInfo g_strAEScene2PLineInfo =
{
    AETABLE_SCENE_INDEX2,
    90,
    -30,
    {
        {1,10000,100,1195,1195},
        {2,100,100,1195,4096},
        {3,100,30,4096,4096},
        {4,30,30,4096,16384},
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

static constexpr strAEPLineNumInfo g_strAEScene3PLineInfo =
{
    AETABLE_SCENE_INDEX3,
    90,
    -30,
    {
        {1,10000,30,1195,1195},
        {2,30,30,1195,2048},
        {3,30,15,2048,2048},
        {4,15,15,2048,4096},
        {5,15,10,4096,4096},
        {6,10,10,4096,12288},
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

static constexpr strAEPLineNumInfo g_strAEScene4PLineInfo =
{
    AETABLE_SCENE_INDEX4,
    90,
    -30,
    {
        {1,5,5,1706,1706},
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

static constexpr strAEPLineNumInfo g_strAEScene5PLineInfo =
{
    AETABLE_SCENE_INDEX5,
    90,
    -30,
    {
        {1,10000,30,1024,1024},
        {2,30,30,1024,3072},
        {3,30,15,3072,3072},
        {4,15,15,3072,7168},
        {5,15,10,7168,7168},
        {6,10,10,7168,12288},
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

static constexpr strAEPLineNumInfo g_strAENoScenePLineInfo =
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
static constexpr strAEPLineInfomation g_strAEPlineInfo =
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
        g_strAENoScenePLineInfo,
        g_strAENoScenePLineInfo,
        g_strAENoScenePLineInfo,
        g_strAENoScenePLineInfo,
        g_strAENoScenePLineInfo,
        g_strAENoScenePLineInfo,
},
};

static constexpr strAEPLineGainList g_strAEGainList =
{
    0,
    {
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

static constexpr AE_PLINETABLE_T g_PlineTableMapping =
{
    g_AEScenePLineMapping,
    g_strAEPlineTable,
    g_strAEPlineInfo,
    g_strAEGainList
};
#endif

#if 0 //Save the P-line info to file for debug
MinGain,1024
MaxGain,12288
MiniISOGain,145
GainStepUnitInTotalRange,32
PreviewExposureLineUnit,26667
PreviewMaxFrameRate,30
VideoExposureLineUnit,26667
VideoMaxFrameRate,30
VideoToPreviewSensitivityRatio,1024
CaptureExposureLineUnit,26667
CaptureMaxFrameRate,30
CaptureToPreviewSensitivityRatio,1024
Video1ExposureLineUnit,26667
Video1MaxFrameRate,30
Video1ToPreviewSensitivityRatio,1024
Video2ExposureLineUnit,26667
Video2MaxFrameRate,30
Video2ToPreviewSensitivityRatio,1024
Custom1ExposureLineUnit,26667
Custom1MaxFrameRate,30
Custom1ToPreviewSensitivityRatio,1024
Custom2ExposureLineUnit,26667
Custom2MaxFrameRate,30
Custom2ToPreviewSensitivityRatio,1024
Custom3ExposureLineUnit,26667
Custom3MaxFrameRate,30
Custom3ToPreviewSensitivityRatio,1024
Custom4ExposureLineUnit,26667
Custom4MaxFrameRate,30
Custom4ToPreviewSensitivityRatio,1024
Custom5ExposureLineUnit,26667
Custom5MaxFrameRate,30
Custom5ToPreviewSensitivityRatio,1024
FocusLength,350
Fno,28

// Preview table -- Use preview sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_RPEVIEW_AUTO
90,-50
1,10000,30,1024,1024
2,30,30,1024,3072
3,30,15,3072,3072
4,15,15,3072,7168
5,15,10,7168,7168
6,10,10,7168,12288
AETABLE_END

// Capture table -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_AUTO
90,-30
1,10000,30,1024,1024
2,30,30,1024,3072
3,30,15,3072,3072
4,15,15,3072,7168
5,15,10,7168,7168
6,10,10,7168,12288
AETABLE_END

// Video table -- Use video sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO_AUTO
90,-30
1,10000,30,1024,1024
2,30,30,1024,3072
3,30,15,3072,3072
4,15,15,3072,7168
5,15,10,7168,7168
6,10,10,7168,12288
AETABLE_END

// Video1 table -- Use video1 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO1_AUTO
90,-30
1,10000,121,1195,1195
2,121,121,1195,21845
AETABLE_END

// Video2 table -- Use video2 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO2_AUTO
90,-30
1,10000,60,1195,1195
2,60,60,1195,3072
3,60,30,3072,3072
4,30,30,3072,10240
AETABLE_END

// Custom1 table -- Use custom1 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM1_AUTO
90,-30
1,10000,60,1195,1195
2,60,60,1195,3072
3,60,30,3072,3072
4,30,30,3072,10240
AETABLE_END

// Custom2 table -- Use custom2 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM2_AUTO
90,-30
1,10000,60,1195,1195
2,60,60,1195,3072
3,60,30,3072,3072
4,30,30,3072,10240
AETABLE_END

// Custom3 table -- Use custom3 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM3_AUTO
90,-30
1,10000,60,1195,1195
2,60,60,1195,3072
3,60,30,3072,3072
4,30,30,3072,10240
AETABLE_END

// Custom4 table -- Use custom4 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM4_AUTO
90,-30
1,10000,60,1195,1195
2,60,60,1195,3072
3,60,30,3072,3072
4,30,30,3072,10240
AETABLE_END

// Custom5 table -- Use custom5 sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM5_AUTO
90,-30
1,10000,60,1195,1195
2,60,60,1195,3072
3,60,30,3072,3072
4,30,30,3072,10240
AETABLE_END

// Video Night table -- Use video sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO_NIGHT
90,-30
1,10000,30,1195,1195
2,30,30,1195,12288
3,30,15,12288,12288
4,15,15,12288,35200
5,15,10,35200,35200
AETABLE_END

// Capture ISO100 -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO100
90,-30
1,10000,5,1230,1230
AETABLE_END

// Capture ISO200 -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO200
90,-30
1,10000,2000,1195,1195
2,2000,2000,1195,2460
3,2000,5,2460,2460
AETABLE_END

// Capture ISO400 -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO400
90,-30
1,10000,2000,1195,1195
2,2000,2000,1195,4920
3,2000,5,4920,4920
AETABLE_END

// Capture ISO800 -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO800
90,-30
1,10000,2000,1195,1195
2,2000,2000,1195,9840
3,2000,5,9840,9840
AETABLE_END

// Capture ISO1600 -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO1600
90,-30
1,10000,2000,1195,1195
2,2000,2000,1195,19680
3,2000,5,19680,19680
AETABLE_END

// Strobe -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_STROBE
90,-30
1,10000,30,1195,1195
2,30,30,1195,12288
3,30,15,12288,12288
4,15,15,12288,46080
AETABLE_END

// Table1 PLine -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX1
90,-50
1,10000,30,1195,1195
2,30,30,1195,12288
3,30,15,12288,12288
4,15,15,12288,46080
5,15,5,46080,46080
AETABLE_END

// Table2 PLine -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX2
90,-30
1,10000,100,1195,1195
2,100,100,1195,4096
3,100,30,4096,4096
4,30,30,4096,16384
AETABLE_END

// Table3 PLine -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX3
90,-30
1,10000,30,1195,1195
2,30,30,1195,2048
3,30,15,2048,2048
4,15,15,2048,4096
5,15,10,4096,4096
6,10,10,4096,12288
AETABLE_END

// Table4 PLine -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX4
90,-30
1,5,5,1706,1706
AETABLE_END

// Table5 PLine -- Use capture sensor setting
// Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX5
90,-30
1,10000,30,1024,1024
2,30,30,1024,3072
3,30,15,3072,3072
4,15,15,3072,7168
5,15,10,7168,7168
6,10,10,7168,12288
AETABLE_END

LIB3A_AE_SCENE_AUTO, AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX5
LIB3A_AE_SCENE_NIGHT, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1
LIB3A_AE_SCENE_ACTION, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2
LIB3A_AE_SCENE_BEACH, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3
LIB3A_AE_SCENE_CANDLELIGHT, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1
LIB3A_AE_SCENE_FIREWORKS, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX4, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX4
LIB3A_AE_SCENE_LANDSCAPE, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3
LIB3A_AE_SCENE_PORTRAIT, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2
LIB3A_AE_SCENE_NIGHT_PORTRAIT, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1
LIB3A_AE_SCENE_PARTY, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1
LIB3A_AE_SCENE_SNOW, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3
LIB3A_AE_SCENE_SPORTS, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2
LIB3A_AE_SCENE_STEADYPHOTO, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2
LIB3A_AE_SCENE_SUNSET, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3
LIB3A_AE_SCENE_THEATRE, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1
LIB3A_AE_SCENE_ISO_ANTI_SHAKE, AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2
LIB3A_AE_SCENE_ISO100, AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO100, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO100
LIB3A_AE_SCENE_ISO200, AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO200, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO200
LIB3A_AE_SCENE_ISO400, AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO400, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO400
LIB3A_AE_SCENE_ISO800, AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO800, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO800
LIB3A_AE_SCENE_ISO1600, AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO1600, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO1600
#endif
