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
#ifndef _MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_PIPELINE_PLUGIN_PIPELINEPLUGIN_SWNRPLUGIN_H_
#define _MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_PIPELINE_PLUGIN_PIPELINEPLUGIN_SWNRPLUGIN_H_

#include <stdlib.h>
#include <sys/prctl.h>
#include <utils/KeyedVector.h>
#include <utils/Mutex.h>
#include <utils/Condition.h>
#include <utils/Errors.h>
#include <utils/List.h>
#include <utils/RefBase.h>
#include <future>
#include <map>
//
#include <mtkcam3/3rdparty/plugin/PipelinePlugin.h>
#include <mtkcam3/3rdparty/plugin/PipelinePluginType.h>

/* SWNR */
#include <mtkcam/aaa/ICaptureNR.h>
/* LPCNR */
#include <mtkcam3/feature/lpcnr/ILpcnr.h>

using namespace lpcnr;

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace NSPipelinePlugin {

/******************************************************************************
*
******************************************************************************/
class SwnrPluginProviderImp : public YuvPlugin::IProvider
{
    typedef YuvPlugin::Property Property;
    typedef YuvPlugin::Selection Selection;
    typedef YuvPlugin::Request::Ptr RequestPtr;
    typedef YuvPlugin::RequestCallback::Ptr RequestCallbackPtr;

public:

    SwnrPluginProviderImp();

    virtual ~SwnrPluginProviderImp();

    virtual const Property&        property() override;

    virtual MERROR                 negotiate(Selection& sel) override;

    virtual void                   init() override;

    virtual MERROR                 process(RequestPtr pRequest,
                                           RequestCallbackPtr pCallback = nullptr) override;

    virtual void                   abort(std::vector<RequestPtr>& pRequests) override;

    virtual void                   uninit() override;

    virtual void                   set(MINT32 iOpenId, MINT32 iOpenId2) override;


protected:

    bool                           onDequeRequest();

    bool                           onProcessFuture();

    int32_t                        doSwnr(RequestPtr const req);

    void*                          getTuningFromNvram(MUINT32 openId, MINT32 magicNo, MINT64 featureCombination) const;

    bool                           queryNrThreshold(MINT64 const featureCombination, int& hw_threshold, int& swnr_threshold);

    void                           waitForIdle();

private:

    MINT32                                          mOpenId = 0;
    MINT32                                          muDumpBuffer = 0;
    MINT32                                          mEnable = -1;
    //
    ISwNR*                                          mpSwnr = nullptr;
    //
    //
    std::shared_ptr<ILpcnr>                         mLpcnr = nullptr;
    bool                                            mbLpcnrEn = false;
    //
    mutable android::Mutex                          mFutureLock;
    mutable android::Condition                      mFutureCond;
    std::map<RequestPtr, std::future<int32_t> >     mvFutures;
    std::future<void>                               mThread;
    volatile MBOOL                                  mbRequestExit = MFALSE;
    //
    MUINT8                                          mIspProfile = 0;
    bool                                            mbTenBitts = false;

};
/******************************************************************************
*
******************************************************************************/
};  //namespace NSPipelinePlugin
};  //namespace NSCam
#endif //_MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_PIPELINE_PLUGIN_PIPELINEPLUGIN_SWNRPLUGIN_H_


