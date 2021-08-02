/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
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
#ifndef __AIALGO_H__
#define __AIALGO_H__

// AINR Core Lib
#include <mtkcam3/feature/ainr/IAiAlgo.h>

// 3A
#include <mtkcam/aaa/IHal3A.h> // setIsp ...
// STL
#include <memory> //std::shared_ptr
#include <condition_variable>
#include <thread>
#include <future>
// AOSP
#include <utils/RefBase.h>

/* AINR Algo */
#include <MTKNRUtils.h>
#include <MTKNRCore.h>
#include <MTKHG.h>

/* Algo DNG */
#include <MTKDngOp.h>


using android::sp;
using namespace NS3Av3;

namespace ainr {

class AiAlgo : public IAiAlgo {

public:
    AiAlgo();

/* implementation from IAiAlgo */
public:
    virtual void init(std::shared_ptr<IAinrNvram> nvram);
    virtual enum AinrErr doHG(const AiHgParam &params);
    virtual enum AinrErr refBufUpk(const AiParam &params, const AiRefPackage &bufPackage);
    virtual enum AinrErr baseBufUpk(const AiParam &params, const AiBasePackage &bufPackage);
    virtual enum AinrErr doNrCore(const AiParam &params, const AiCoreParam &coreParam);
    virtual enum AinrErr doDRC(NSCam::IImageBuffer* rawBuf, NSCam::IMetadata* appMeta
                                    , NSCam::IMetadata* halMeta, NSCam::IMetadata* dynamicMeta);
    virtual ~AiAlgo(void);
protected:
    std::shared_ptr<IAinrNvram>  mNvram;
};
}; /* namespace ainr */
#endif//__AIALGO_H__
