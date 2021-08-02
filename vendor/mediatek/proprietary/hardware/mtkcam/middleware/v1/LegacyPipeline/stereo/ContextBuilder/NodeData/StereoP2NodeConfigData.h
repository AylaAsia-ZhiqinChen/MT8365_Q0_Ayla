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
 * MediaTek Inc. (C) 2017. All rights reserved.
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
#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_STEREO_P2_NODE_CONFIG_DATA_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_STEREO_P2_NODE_CONFIG_DATA_H_
//
#include <utils/StrongPointer.h>
//
#include <mtkcam/middleware/v1/LegacyPipeline/stereo/ContextBuilder/INodeConfigBase.h>
//
#include <mtkcam/pipeline/hwnode/P2Node.h>
//
using namespace android;
//
namespace NSCam
{
namespace v3
{
class StereoP2NodeConfigData: public INodeConfigBase
{
private:

    typedef enum{
        EImgInfo_ResizeRaw = 0,
        EImgInfo_FullRaw,

        EImgInfo_MaxNum
    }EImgInfoId;

public:
    StereoP2NodeConfigData(
                                MINT32 openId,
                                NodeId_T nodeId,
                                char const* nodeName);
    virtual ~StereoP2NodeConfigData();
    //
    virtual void configNode( MetaStreamManager* metaManager,
                                ImageStreamManager* imageManager,
                                StereoBasicParameters* userParams,
                                PipelineContext* pipeContext);

    //
    virtual sp<INodeActor>                          getNode();
    //
    virtual void                                    dump();
    virtual void                                    destroy();

private:
    P2Node::InitParams initParam;
    P2Node::ConfigParams cfgParam;
};
};
};
#endif //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_STEREO_P2_NODE_CONFIG_DATA_H_
