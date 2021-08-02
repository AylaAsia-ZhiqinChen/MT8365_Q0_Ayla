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

#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_PIPELINE_INFLIGHTREQUEST_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_PIPELINE_INFLIGHTREQUEST_H_
//
#include <list>
#include <string>
#include <vector>

#include <utils/Condition.h>
#include <utils/Printer.h>

#include <mtkcam3/pipeline/pipeline/IPipelineBufferSetFrameControl.h>
#include <mtkcam3/pipeline/pipeline/IPipelineNode.h>


/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace v3 {


/******************************************************************************
 *
 ******************************************************************************/
class InFlightRequest
    : public IPipelineFrameListener
{
public:
                                    InFlightRequest();

public:

    /**
     * Dump debugging state.
     */
    virtual MVOID                   dumpState(
                                        android::Printer& printer,
                                        const std::vector<std::string>& options
                                    ) const;

    /* register for a new request */
    virtual MVOID                   registerRequest(
                                        android::sp<IPipelineFrame>const& pFrame
                                    );

    /* wait until all inflight request done */
    virtual MVOID                   waitUntilDrained();

    /* wait until specified node has done all requests*/
    virtual MVOID                   waitUntilNodeDrained(NodeId_T id);

    /* wait until specified node has done all requests*/
    virtual MVOID                   waitUntilNodeImageDrained(NodeId_T id);

    /* wait until specified node has done all requests*/
    virtual MVOID                   waitUntilNodeMetaDrained(NodeId_T id);

    virtual MVOID                   abort();

/******************************************************************************
 *   IPipelineFrameListener Interface
 ******************************************************************************/
    virtual MVOID                   onPipelineFrame(
                                        MUINT32 const frameNo,
                                        MUINT32 const message,
                                        MVOID*const pCookie
                                    );

    virtual MVOID                   onPipelineFrame(
                                        MUINT32 const frameNo,
                                        NodeId_T const nodeId,
                                        MUINT32 const message,
                                        MVOID*const pCookie
                                    );

protected:   /// data members
    mutable android::Mutex                   mLock;
    android::Condition                       mRequestCond;
    using FrameListT = std::list<std::pair<MUINT32, android::wp<IPipelineFrame>>>;
    FrameListT                               mRequest;
    typedef android::List<MUINT32>           RequestList;
    android::DefaultKeyedVector<NodeId_T, RequestList>  mRequestMap_meta;//pair<Node, request_list> for meta
    android::DefaultKeyedVector<NodeId_T, RequestList>  mRequestMap_image;//pair<Node, request_list> for image
    MINT32                                   mLogLevel;
};


/******************************************************************************
 *
 ******************************************************************************/
};  //namespace v3
};  //namespace NSCam
#endif  //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_PIPELINE_INFLIGHTREQUEST_H_

