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

#ifndef _MTK_HARDWARE_MTKCAM_LEGACYPIPELINE_PIPELINEBUILDERFEATUREPREVIEW_H_
#define _MTK_HARDWARE_MTKCAM_LEGACYPIPELINE_PIPELINEBUILDERFEATUREPREVIEW_H_
//
#define FEATURE_MODIFY

#include "PipelineBuilderBase.h"

using namespace android;

/******************************************************************************
 *
 ******************************************************************************/
//
namespace NSCam {
namespace v1 {
namespace NSLegacyPipeline {

class LegacyPipelinePreview;
class PipelineBuilderFeaturePreview
    : public PipelineBuilderBase
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
                                        PipelineBuilderFeaturePreview(
                                            MINT32 const openId,
                                            char const* pipeName,
                                            ConfigParams const & rParam
                                        );

    virtual                             ~PipelineBuilderFeaturePreview();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  LegacyPipelineBuilder Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Interface for setup PipelineContext.
    /**
     * create the ILegacyPipeline
     *
     * PipelineManager will hold the sp of this ILegacyPipeline.
     *
     */
     virtual sp<ILegacyPipeline>        create();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:
    MVOID                               buildStream(
                                            sp<PipelineContext> rpPipelineContext,
                                            const PipelineImageParam& params
                                        );

    MERROR                              configureP1Node(
                                            sp<PipelineContext> rpPipelineContext
                                        );

    MERROR                              configureP2FeatureNode(
                                            sp<PipelineContext> rpPipelineContext
                                        );

    MERROR                              configureDualFeatureNode(
                                            sp<PipelineContext> rpPipelineContext
                                        );

    MBOOL                               isImageRawOutput(void);

    #ifdef FEATURE_MODIFY
    MBOOL                               isVideoRecord() const;
    #endif // FEATURE_MODIFY

protected:     ////
    sp<LegacyPipelinePreview>           mpLegacyPipeline;

protected:
    sp<IMetaStreamInfo>                 mpControlMeta_App;
    sp<IMetaStreamInfo>                 mpControlMeta_Hal;
    sp<IMetaStreamInfo>                 mpResultMeta_P1_App;
    sp<IMetaStreamInfo>                 mpResultMeta_P1_Hal;
    sp<IMetaStreamInfo>                 mpResultMeta_P2_App;
    sp<IMetaStreamInfo>                 mpResultMeta_P2_Hal;

protected:
    sp<IImageStreamInfo>                mpImage_RrzoRaw;
    sp<IImageStreamInfo>                mpImage_ImgoRaw;
    sp<IImageStreamInfo>                mpImage_LcsoRaw;
    sp<IImageStreamInfo>                mpImage_RssoRaw;
    sp<IImageStreamInfo>                mpImage_Yuv00;
    sp<IImageStreamInfo>                mpImage_Yuv01;
    sp<IImageStreamInfo>                mpImage_YuvFd;
    // for dual cam using
    sp<IImageStreamInfo>                mpImage_Yuv02;
    sp<IImageStreamInfo>                mpImage_Y8_01;
};

};  //namespace NSLegacyPipeline
};  //namespace v1
};  //namespace NSCam
#endif  //_MTK_HARDWARE_MTKCAM_LEGACYPIPELINE_PIPELINEBUILDERFEATUREPREVIEW_H_

