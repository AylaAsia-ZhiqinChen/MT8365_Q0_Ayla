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

/*******************************************************************************
*   ISP_NVRAM_PCA_STRUCT
********************************************************************************/

.COLOR_Method= 1,  //0:dynamic PCA,  1:smooth PCA

.Config={
    .set={//00
        0x00000000, 0x00000000
    }
},

.COLOR_CT_Env={
    3650, 3800, 5600, 5800, 7000
},

.rPcaParam=
{
    //SCENE_00_NORMAL
    {16, 16, 16, 30, 50, 100, 120, 0, 4, 4, 4, 30, 0, 100, 120, 4, 16},

    //SCENE_01_HDR
    {16, 16, 16, 30, 50, 100, 120, 0, 4, 4, 4, 30, 0, 100, 120, 4, 16},

    //SCENE_02_FLASH
    {16, 16, 16, 30, 50, 100, 120, 0, 4, 4, 4, 30, 0, 100, 120, 4, 16},

    //SCENE_03_OUTDOOR
    {16, 16, 16, 30, 50, 100, 120, 0, 4, 4, 4, 30, 0, 100, 120, 4, 16}

},

.PCA_LUT=
{
    {//SCENE_00_NORMAL
        {.lut={//CT_00 3320
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 18 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 236, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 232, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 234, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 244, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 240, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 238, .PCA_HUE_SHIFT = 238, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 232, .PCA_HUE_SHIFT = 240, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 226, .PCA_HUE_SHIFT = 244, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 234, .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 248, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 248, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 244, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 12 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 16 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 18 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 18 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 12 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}} //
        }},
        {.lut={//CT_01 3800
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 248, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 236, .PCA_HUE_SHIFT = 242, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 234, .PCA_HUE_SHIFT = 240, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 232, .PCA_HUE_SHIFT = 238, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 230, .PCA_HUE_SHIFT = 236, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 228, .PCA_HUE_SHIFT = 234, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 224, .PCA_HUE_SHIFT = 232, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 224, .PCA_HUE_SHIFT = 232, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 224, .PCA_HUE_SHIFT = 232, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 230, .PCA_HUE_SHIFT = 234, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 234, .PCA_HUE_SHIFT = 236, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 240, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 12 , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 14 , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 18 , .PCA_HUE_SHIFT = 16 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 14 , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 10 , .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 10 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}} //
        }},
        {.lut={//CT_02 5600
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 248, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 236, .PCA_HUE_SHIFT = 242, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 234, .PCA_HUE_SHIFT = 240, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 232, .PCA_HUE_SHIFT = 238, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 230, .PCA_HUE_SHIFT = 236, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 228, .PCA_HUE_SHIFT = 234, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 224, .PCA_HUE_SHIFT = 232, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 224, .PCA_HUE_SHIFT = 232, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 224, .PCA_HUE_SHIFT = 232, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 230, .PCA_HUE_SHIFT = 234, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 234, .PCA_HUE_SHIFT = 236, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 240, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 12 , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 14 , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 18 , .PCA_HUE_SHIFT = 16 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 14 , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 10 , .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 10 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 6  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}} //
        }},
        {.lut={//CT_03 5800
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 248, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 244, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 10 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 12 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 14 , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}} //
        }},
        {.lut={//CT_04 7000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 240, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 248, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 244, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 246, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 250, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 252, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 248, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 246, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 242, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 244, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 250, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 2  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 8  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 12 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 14 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 10 , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 6  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 4  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 2  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 4  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 10 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 12 , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 14 , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 8  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 254, .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 0  , .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 254, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0  , .PCA_HUE_SHIFT = 252, .rsv_24 = 0}} //
        }}
    },
    {//SCENE_01_HDR
        {.lut={//CT_00
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_01
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_02
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_03
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_04
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }}
    },
    {//SCENE_02_FLASH
        {.lut={//CT_00
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_01
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_02
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_03
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_04
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }}
    },
    {//SCENE_03_OUTDOOR
        {.lut={//CT_00
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_01
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_02
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_03
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }},
        {.lut={//CT_04
            //y_gain sat_gain hue_shift reserved
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //000
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //010
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //020
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //030
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //040
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //050
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //060
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //070
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //080
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //090
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //100
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //110
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //120
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //130
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //140
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //150
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //160
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //170
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}}, //
            {.bits= {.PCA_LUMA_GAIN = 0, .PCA_SAT_GAIN = 0, .PCA_HUE_SHIFT = 0, .rsv_24 = 0}} //
        }}
    }
}

