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

// Standard C header file

// Android system/core header file

// mtkcam custom header file

// mtkcam global header file
#include <mtkcam/utils/metadata/IMetadata.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>
// Module header file

// Local header file
#include "RootNode.h"

// Logging
#undef PIPE_CLASS_TAG
#undef PIPE_MODULE_TAG
#define PIPE_MODULE_TAG "IspPipe"
#define PIPE_CLASS_TAG "RootNode"
#include <featurePipe/core/include/PipeLog.h>

class scoped_tracer
{
public:
    scoped_tracer(const char* functionName)
    : mFunctionName(functionName)
    {
        CAM_LOGD("[%s] +", mFunctionName);
    }
    ~scoped_tracer()
    {
        CAM_LOGD("[%s] -", mFunctionName);
    }
private:
    const char* const mFunctionName;
};
#define SCOPED_TRACER() scoped_tracer ___scoped_tracer(__FUNCTION__ );
/*******************************************************************************
* Namespace start.
********************************************************************************/
namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {
namespace DualCamThirdParty {

/*******************************************************************************
* Class Definition
********************************************************************************/
namespace {
/**
 * @class DCMFFrature
 * @brief the mode of Dual-Camera Multi-Frame(DCMF)
 */
enum DCMFFrature
{
    DCMF_INVALID_MODE       = -1,
    DCMF_BOKEH_MODE         = MTK_DCMF_FEATURE_BOKEH,
    DCMF_MFNR_BOKEH_MODE    = MTK_DCMF_FEATURE_MFNR_BOKEH,
    DCMF_HDR_BOKEH_MODE     = MTK_DCMF_FEATURE_HDR_BOKEH
};

/**
 * @class RootNodeHelper
 * @brief Helper class for RootNode
 */
class RootNodeHelper
{
public:

    /**
     * @brief try to get the DCMF feature mode
     * @param [in] request target request
     * @param [in] nodeID the node ID
     * @param [out] mode the DCMF feature mode of the arugment request is
     * @return
     * - execute result
     */
    static MBOOL tryGetDCMFFeatureMode(const IspPipeRequestPtr& request, MINT32 nodeID, MINT32& mode);

    /**
     * @brief try to get the feature warning
     * @param [in] request target request
     * @param [in] nodeID the node ID
     * @param [out] mode the feature warning of the arugment request is
     * @return
     * - execute result
     */
    static MBOOL tryGetFeatureWarning(const IspPipeRequestPtr& request, MINT32 nodeID, MINT32& mode);

    /**
     * @brief determine the request is output request or not
     * @param [in] requestPtr target request
     * @return
     * - execute result
     */
    static MBOOL getIsOutputRequest(const IspPipeRequestPtr& requestPtr);

    /**
     * @brief convert DCMFFratureMode to the corresponding name
     * @param [in] mode DCMF frature mode
     * @return
     * - execute result
     */
    static const char* featureModeToName(MINT32 mode);
};

} // anonymous namespace
/*******************************************************************************
* Const Definition
********************************************************************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  RootNodeHelper Instantiation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
RootNodeHelper::
tryGetDCMFFeatureMode(const IspPipeRequestPtr& request, MINT32 nodeID, MINT32& mode)
{
    MBOOL bRet = MFALSE;

    IMetadata* main1HalMeta = request->getBufferHandler()->requestMetadata(nodeID, BID_META_IN_HAL_MAIN1);
    if(main1HalMeta == nullptr)
    {
        MY_LOGE("Failed to get main1 hal meta");
    }
    else
    {
        if(IMetadata::getEntry<MINT32>(main1HalMeta, MTK_STEREO_DCMF_FEATURE_MODE, mode))
        {
            bRet = MTRUE;
        }
        else
        {
            MY_LOGW("Failed to get app meta: MTK_STEREO_DCMF_FEATURE_MODE, main1HalMeta:%p", main1HalMeta);
        }
    }
    return bRet;
}

MBOOL
RootNodeHelper::
tryGetFeatureWarning(const IspPipeRequestPtr& request, MINT32 nodeID, MINT32& mode)
{
    MBOOL bRet = MFALSE;

    IMetadata* inAppMeta = request->getBufferHandler()->requestMetadata(nodeID, BID_META_IN_APP);
    if(inAppMeta == nullptr)
    {
        MY_LOGE("Failed to get in-app meta");
    }
    else
    {
        if(IMetadata::getEntry<MINT32>(inAppMeta, MTK_STEREO_FEATURE_WARNING, mode))
        {
            bRet = MTRUE;
        }
        else
        {
            MY_LOGD("Failed to get app meta: MTK_STEREO_FEATURE_WARNING, appMeta:%p", inAppMeta);
        }
    }
    return bRet;
}

MBOOL
RootNodeHelper::
getIsOutputRequest(const IspPipeRequestPtr& requestPtr)
{
    // Node:
    // if there are new output appended, please modify this function
    MBOOL hasDepthMap = requestPtr->isRequestBuffer(BID_OUT_DEPTHMAP);
    MBOOL hasCleanFSYUV = requestPtr->isRequestBuffer(BID_OUT_CLEAN_FSYUV);
    MBOOL hasBOKEHFSYUV = requestPtr->isRequestBuffer(BID_OUT_BOKEH_FSYUV);

    MY_LOGD("determine output request, hasDepthMap:%d, hasCleanFSYUV:%d, hasBOKEHFSYUV:%d",
        hasDepthMap, hasCleanFSYUV, hasBOKEHFSYUV);

    return (hasDepthMap && hasCleanFSYUV && hasBOKEHFSYUV);
}

const char*
RootNodeHelper::
featureModeToName(MINT32 mode)
{
    #define MAKE_NAME_CASE(name) \
      case name: return #name
    {
      switch(mode)
      {
        MAKE_NAME_CASE(DCMF_INVALID_MODE);
        MAKE_NAME_CASE(DCMF_BOKEH_MODE);
        MAKE_NAME_CASE(DCMF_MFNR_BOKEH_MODE);
        MAKE_NAME_CASE(DCMF_HDR_BOKEH_MODE);
      };
    }
    #undef MAKE_NAME_CASE

    return "UNKNOWN";
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  RootNode Instantiation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
RootNode::
RootNode(
    const char *name,
    IspPipeNodeID nodeId,
    const PipeNodeConfigs& config
)
: RootNode(name, nodeId, config, SCHED_NORMAL, DEFAULT_CAMTHREAD_PRIORITY)
{

}

RootNode::
RootNode(
    const char *name,
    IspPipeNodeID nodeId,
    const PipeNodeConfigs& config,
    int policy,
    int priority
)
: IspPipeNode(name, nodeId, config, policy, priority)
, mProcessingFeatureMode(DCMF_INVALID_MODE)
{
    SCOPED_TRACER();

    this->addWaitQueue(&mRequestQue);
}

RootNode::
~RootNode()
{
    SCOPED_TRACER();
}

MBOOL
RootNode::
onInit()
{
    SCOPED_TRACER();

    return MTRUE;
}

MBOOL
RootNode::
onUninit()
{
    SCOPED_TRACER();

    return MTRUE;
}

MBOOL
RootNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
RootNode::
onThreadStop()
{
    return MTRUE;
}

MBOOL
RootNode::
onData(DataID dataID, const IspPipeRequestPtr& pRequest)
{
    SCOPED_TRACER();

    MY_LOGD("reqID=%d", pRequest->getRequestNo());

    MBOOL ret = MTRUE;
    switch(dataID)
    {
        case ROOT_ENQUE:
            mRequestQue.enque(pRequest);
            break;
        default:
            MY_LOGW("Un-recognized dataID ID, id=%s reqID=%d", ID2Name(dataID), pRequest->getRequestNo());
            ret = MFALSE;
            break;
    }
    return ret;
}

MVOID
RootNode::
onFlush()
{
    SCOPED_TRACER();

    MY_LOGD("extDep=%d", this->getExtThreadDependency());

    IspPipeRequestPtr pRequest;
    while( mRequestQue.deque(pRequest) )
    {
        sp<PipeBufferHandler> pBufferHandler = pRequest->getBufferHandler();
        pBufferHandler->onProcessDone(getNodeId());
    }
    IspPipeNode::onFlush();
}

MBOOL
RootNode::
onThreadLoop()
{
    SCOPED_TRACER();

    IspPipeRequestPtr pRequest;
    if( !waitAllQueue() )
    {
        return MFALSE;
    }
    if( !mRequestQue.deque(pRequest) )
    {
        MY_LOGE("mRequestQue.deque() failed");
        return MFALSE;
    }

    MBOOL bRet = MTRUE;
    // mark on-going-request start
    this->incExtThreadDependency();
    {
        AutoProfileLogging profile("RootNode::threadLoop", pRequest->getRequestNo());

        MINT32 dcmfFeatureMode = -1;
        if(!RootNodeHelper::tryGetDCMFFeatureMode(pRequest, getNodeId(), dcmfFeatureMode))
        {
            MY_LOGE("Failed to get the DCMF feature mode");

            bRet = MFALSE;
        }
        else
        {
            MY_LOGD("get the DCMF feature mode:%d(%s)",
                dcmfFeatureMode, RootNodeHelper::featureModeToName(dcmfFeatureMode));

            if(mProcessingFeatureMode == DCMF_INVALID_MODE)
            {
                 MY_LOGD("set processingFeatureMode, new:%d(%s), old:%d(%s)",
                    dcmfFeatureMode, RootNodeHelper::featureModeToName(dcmfFeatureMode), mProcessingFeatureMode, RootNodeHelper::featureModeToName(mProcessingFeatureMode));

                 mProcessingFeatureMode = dcmfFeatureMode;
            }

            if(dcmfFeatureMode != mProcessingFeatureMode)
            {
                MY_LOGE("the wrong flow, the currentFeatureMode(%d) is different with processingFeatureMode(%d)",
                    dcmfFeatureMode, mProcessingFeatureMode);

                bRet = MFALSE;
            }
            else
            {
                const MINT32 featureWarningforEnableDepthAndBokeh = 0;
                MINT32 featureWarning = featureWarningforEnableDepthAndBokeh;
                if(RootNodeHelper::tryGetFeatureWarning(pRequest, getNodeId(), featureWarning) && (featureWarning != featureWarningforEnableDepthAndBokeh))
                {
                    MY_LOGW("disableDepthAndBokeh, featureWarning:%d", featureWarning);
                    pRequest->getRequestAttr().isDisableDepth = MTRUE;
                    pRequest->getRequestAttr().isDisableBokeh = MTRUE;
                }

                switch(mProcessingFeatureMode)
                {
                    case DCMF_BOKEH_MODE:
                    {
                        pRequest->getRequestAttr().isPureBokeh = MTRUE;

                        this->handleDataAndDump(ROOT_TO_FD, pRequest);
                        break;
                    }
                    case DCMF_MFNR_BOKEH_MODE:
                    {
                        this->handleDataAndDump(ROOT_TO_MFNR, pRequest);
                        this->handleDataAndDump(ROOT_TO_DEPTH, pRequest);
                        this->handleDataAndDump(ROOT_TO_FD, pRequest);
                        break;
                    }
                    case DCMF_HDR_BOKEH_MODE:
                    {
                        this->handleDataAndDump(ROOT_TO_HDR, pRequest);
                        this->handleDataAndDump(ROOT_TO_DEPTH, pRequest);
                        this->handleDataAndDump(ROOT_TO_FD, pRequest);
                        break;
                    }
                    default:
                    {
                        MY_LOGE("unknown feature mode, req#:%d, featureMode: %d", pRequest->getRequestNo(), mProcessingFeatureMode);

                        bRet = MFALSE;
                        break;
                    }
                }

                if(RootNodeHelper::getIsOutputRequest(pRequest))
                {
                    MY_LOGD("current request is output request, reset processingFeatureMode into invalid, req#:%d, processingFeatureMode:%d(%s)",
                        pRequest->getRequestNo(), mProcessingFeatureMode, RootNodeHelper::featureModeToName(mProcessingFeatureMode));

                    mProcessingFeatureMode = DCMF_INVALID_MODE;
                }
            }
        }

        if(!bRet)
        {
            MY_LOGE("error occure, dispatch to error notify, req#:%d, featureMode: %d, processingFeatureMode:%d",
                pRequest->getRequestNo(), dcmfFeatureMode, mProcessingFeatureMode);

            this->handleData(ERROR_OCCUR_NOTIFY, pRequest);
        }
        // launch onProcessDone
        pRequest->getBufferHandler()->onProcessDone(getNodeId());
    }
    // mark on-going-request end
    this->decExtThreadDependency();
    return bRet;
}


}; // DualCamThirdParty
}; // NSFeaturePipe
}; // NSCamFeature
}; // NSCam

