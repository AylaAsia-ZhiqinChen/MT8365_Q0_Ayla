/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_NODEID_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_NODEID_H_
//


/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {


/******************************************************************************
 * Legacy Pipeline Node ID
 ******************************************************************************/
//  Notes:
//  Eveny node Id should belong to the global namespace, not to a specific
//  pipeline's namespace, so that we can easily reuse node's instances.
enum
{
    eNODEID_P1Node          = 0x01,
    eNODEID_P2Node          = 0x02,
    eNODEID_FDNode          = 0x03,
    eNODEID_HdrNode         = 0x04,
    eNODEID_MfllNode        = 0x05,
    //
    eNODEID_P2Node_VSS      = 0x12,
    eNODEID_JpegNode        = 0x13,
    //
    eNODEID_RAW16Out        = 0x22,
    // VSDOF
    eNODEID_P1Node_main2    = 0x31,
    eNODEID_StereoRootNode  = 0x32,
    eNODEID_DualITNode      = 0x33,
    eNODEID_JpsNode         = 0x34,
    eNODEID_DepthMapNode    = 0x35,
    eNODEID_BokehNode       = 0x36,
    eNODEID_JpegNode_Bokeh  = 0x37,
    eNODEID_JpegNode_JPS    = 0x38,
    // Denoise
    eNODEID_BMPreProcessNode= 0x39,
    eNODEID_BMDeNoiseNode   = 0x40,
    // Features
    eNODEID_P2aNode         = 0x41,
    //
    eNODEID_DualYuvNode     = 0x42,
    eNODEID_JpegNode_Clean  = 0x43,
    //
    eNODEID_SWBokehNode     = 0x45,
    eNODEID_HWBokehNode     = 0x46,
    eNODEID_PostViewNode     = 0x47,
    // 3rd party
    eNODEID_TPNode          = 0x48,
    eNODEID_JpegNode_TP     = 0x49,
    eNODEID_JpegNode_TP0    = 0x50,
    eNODEID_P2Node_main2    = 0x51,
    eNODEID_Dual3rdPartyNode= 0x52,
    eNODEID_VendorBokehNode = 0x53,
};


/******************************************************************************
 *
 ******************************************************************************/
};  //namespace NSCam
#endif  //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_NODEID_H_

