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
 * MediaTek Inc. (C) 2018. All rights reserved.
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

#ifndef _MTK_CAMERA_CAPTURE_FEATURE_PIPE_ROOT_NODE_H_
#define _MTK_CAMERA_CAPTURE_FEATURE_PIPE_ROOT_NODE_H_

#include "CaptureFeatureNode.h"
#include <utils/KeyedVector.h>

#ifdef SUPPORT_MFNR
// ALGORITHM
#include <MTKBss.h>
#include <mtkcam/utils/hw/IFDContainer.h>
#endif

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {
namespace NSCapture {

class RootNode : public CaptureFeatureNode
{
#ifdef SUPPORT_MFNR
    /*
     *  Tuning Param for EIS.
     *  Should not be configure by customer
     */
    static const int MFC_GMV_CONFX_TH = 25;
    static const int MFC_GMV_CONFY_TH = 25;
    static const int MAX_GMV_CNT = MAX_FRAME_NUM;

    struct GMV{
        MINT32 x = 0;
        MINT32 y = 0;
    };
#endif
public:
    RootNode(NodeID_T nid, const char *name, MINT32 policy = SCHED_NORMAL, MINT32 priority = DEFAULT_CAMTHREAD_PRIORITY);
    virtual ~RootNode();

public:
    virtual MBOOL onData(DataID id, const RequestPtr& pRequest);
    virtual MBOOL onAbort(RequestPtr &data);
    virtual MERROR evaluate(NodeID_T nodeId, CaptureFeatureInferenceData& rInference);

protected:
    virtual MBOOL onInit();
    virtual MBOOL onThreadStart();
    virtual MBOOL onThreadLoop();
    virtual MBOOL onThreadStop();

    MBOOL onRequestProcess(RequestPtr&);
    MVOID onRequestFinish(const RequestPtr&);
private:

#ifdef SUPPORT_MFNR
    MBOOL retrieveGmvInfo(IMetadata* pMetadata, int& x, int& y, MSize& size) const;

    GMV   calMotionVector(IMetadata* pMetadata, MBOOL isMain) const;

    MVOID updateBssProcInfo(IImageBuffer* pBuf, BSS_PARAM_STRUCT& p, MINT32 frameNum) const;

    MVOID updateBssIOInfo(IImageBuffer* pBuf, BSS_INPUT_DATA_G& bss_input) const;

    std::shared_ptr<char> getNvramChunk(size_t *bufferSize) const;

    MBOOL appendBSSInput(Vector<RequestPtr>& rvRequests, BSS_INPUT_DATA_G& bss_input, vector<FD_DATATYPE>& bss_fddata) const;

    MBOOL getForceBss(void* param_addr, size_t param_size) const;

    MVOID dumpBssInputData2File(RequestPtr& firstRequest, BSS_PARAM_STRUCT& bss_param, BSS_INPUT_DATA_G& bss_input, BSS_OUTPUT_DATA& bssOutData) const;

    MBOOL doBss(Vector<RequestPtr>& rvToDoRequests);

    MVOID collectPreBSSExifData(
        Vector<RequestPtr>& rvToDoRequests,
        BSS_PARAM_STRUCT& bss_param,
        BSS_INPUT_DATA_G& bss_input) const;

    MVOID collectPostBSSExifData(
        Vector<RequestPtr>& rvReadyRequests,
        Vector<MINT32>& vNewIndex,
        BSS_OUTPUT_DATA& bss_output);

    MVOID updateMetadata(Vector<RequestPtr>& rvOrderedRequests, size_t i4SkipFrmCnt, MetadataID_T metaId);
#endif
    MVOID reorder(Vector<RequestPtr>& rvRequests, Vector<RequestPtr>& rvOrderedRequests, size_t i4SkipFrmCnt);

private:

    mutable Mutex                       mLock;
    mutable Condition                   mWaitCondition;
    bool                                mbWait = false;
    WaitQueue<RequestPtr>               mRequests;
    Vector<RequestPtr>                  mvPendingRequests;
    Vector<RequestPtr>                  mvRestoredRequests;
    Vector<RequestPtr>                  mvBypassBSSRequest;

    class FDContainerWraper;
    UniquePtr<FDContainerWraper>        mFDContainerWraperPtr;

    mutable Mutex                       mNvramChunkLock;
    MINT32                              mDebugLevel;
    MINT32                              mMfnrQueryIndex;
    MINT32                              mMfnrDecisionIso;
    MINT32                              mDebugDump;
    MINT32                              mEnableBSSOrdering;
    MINT32                              mDebugDrop;
    MINT32                              mDebugLoadIn;
};

} // NSCapture
} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam

#endif // _MTK_CAMERA_CAPTURE_FEATURE_PIPE_ROOT_NODE_H_
