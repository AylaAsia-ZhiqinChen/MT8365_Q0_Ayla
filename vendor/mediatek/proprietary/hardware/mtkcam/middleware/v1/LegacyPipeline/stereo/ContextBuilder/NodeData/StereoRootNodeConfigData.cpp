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
#define LOG_TAG "ContextBuilder/StereoRootNodeData"
//
#include "StereoRootNodeConfigData.h"
//
#include <utils/StrongPointer.h>
//
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/middleware/v1/LegacyPipeline/stereo/ContextBuilder/MetaStreamManager.h>
#include <mtkcam/middleware/v1/LegacyPipeline/stereo/ContextBuilder/ImageStreamManager.h>
//
#include <mtkcam/pipeline/pipeline/PipelineContext.h>
//
#include <mtkcam/pipeline/hwnode/StereoRootNode.h>
//
#include <mtkcam/middleware/v1/LegacyPipeline/StreamId.h>
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
//
using namespace android;
using namespace NSCam;
using namespace v3;
using namespace NSCam::v3::NSPipelineContext;
//
typedef StereoRootNode                  NodeT;
typedef NodeActor< NodeT >              MyNodeActorT;
//
StereoRootNodeConfigData::
StereoRootNodeConfigData(
    MINT32 openId,
    NodeId_T nodeId,
    char const* nodeName)
    : INodeConfigBase(openId, nodeId, nodeName)
{
    initParam.openId = mOpenId;
    initParam.nodeId = mNodeId;
    initParam.nodeName = mNodeName;
}
//
StereoRootNodeConfigData::
~StereoRootNodeConfigData()
{
    MY_LOGD("dcot(%x)", this);
}
//
sp<INodeActor>
StereoRootNodeConfigData::
getNode()
{
    MY_LOGD("+");
    sp<MyNodeActorT> pNode = new MyNodeActorT( NodeT::createInstance() );
    //
    pNode->setInitParam(initParam);
    pNode->setConfigParam(cfgParam);
    //
    MY_LOGD("-");
    return pNode;
}
//
void
StereoRootNodeConfigData::
configNode(
    MetaStreamManager* metaManager,
    ImageStreamManager* imageManager,
    StereoBasicParameters* userParams,
    PipelineContext* pipeContext)
{
    if(metaManager != nullptr)
    {
        addStreamBegin(MTRUE);
        cfgParam.pInAppMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_APP_CONTROL);
        addStream(cfgParam.pInAppMeta, MTRUE);
        cfgParam.pOutAppMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_HAL_DYNAMIC_STEREOROOT);
        addStream(cfgParam.pOutAppMeta, MFALSE);

        cfgParam.pInHalMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_HAL_CONTROL);
        addStream(cfgParam.pInHalMeta, MFALSE);
        cfgParam.pOutHalMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_HAL_DYNAMIC_P1);
        addStream(cfgParam.pOutHalMeta, MFALSE);
        cfgParam.pOutHalMeta_Main2 = metaManager->getStreamInfoByStreamId(eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2);
        addStream(cfgParam.pOutHalMeta_Main2, MFALSE);

        addStreamEnd(MTRUE);
    }
    /*if(imageManager != nullptr)
    {
        addStreamBegin(MFALSE);
        cfgParam.pInResizeImageInfo = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
        addStream(cfgParam.pInResizeImageInfo, MTRUE);
        addStreamEnd(MFALSE);
    }*/
}
//
void
StereoRootNodeConfigData::
dump()
{

}
//
void
StereoRootNodeConfigData::
destroy()
{
    cfgParam.pInAppMeta = nullptr;
    cfgParam.pOutAppMeta = nullptr;
    cfgParam.pInHalMeta = nullptr;
    cfgParam.pOutHalMeta = nullptr;
    cfgParam.pOutHalMeta_Main2 = nullptr;
    //cfgParam.pInResizeImageInfo = nullptr;
}