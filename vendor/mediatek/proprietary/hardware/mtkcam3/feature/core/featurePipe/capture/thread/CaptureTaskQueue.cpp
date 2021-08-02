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
#include "CaptureTaskQueue.h"

#define PIPE_CLASS_TAG "Task"
#define PIPE_TRACE TRACE_CAPTURE_FEATURE_NODE

#include <featurePipe/core/include/PipeLog.h>

CAM_ULOG_DECLARE_MODULE_ID(MOD_FPIPE_CAPTURE);

using namespace NSCam::NSCamFeature::NSFeaturePipe;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {
namespace NSCapture {

CaptureTaskQueue::CaptureTaskQueue(MUINT8 max)
    : mStop(MFALSE)
    , mThreadMax(max)
    , mThreadCount(0)
{
}

CaptureTaskQueue::~CaptureTaskQueue()
{
    mStop = MTRUE;

    mTaskCond.notify_all();

    for (std::thread &t : mThreads) {
        try {
            t.join();
        }
        catch (std::exception&) {
            MY_LOGW("thread's join throws exception");
        }
    }
    mThreads.clear();
}

 MVOID CaptureTaskQueue::addTask(std::function<void()>& func)
{
    std::unique_lock<std::mutex> lock(mTaskLock);

    mTasks.push_back(std::move(func));

    if (mThreadMax > mThreadCount && mTasks.size() > mThreadCount)
        addThread();

    mTaskCond.notify_one();
}

MVOID CaptureTaskQueue::addThread() {

    std::thread t([this]() {
        while (!mStop) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mTaskLock);
                if(mTasks.empty()) {
                    mTaskCond.wait(lock);
                    continue;
                }
                task = std::move(mTasks.front());
                mTasks.pop_front();
            }
            task();
        }
    });
    mThreads.push_back(std::move(t));
    mThreadCount++;
}


} // NSCapture
} // NSFeaturePipe
} // NSCamFeature
} // NSCam
