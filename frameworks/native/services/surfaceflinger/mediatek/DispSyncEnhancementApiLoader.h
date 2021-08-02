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

#ifndef ANDROID_DISP_SYNC_ENHANCEMENT_API_LOADER_H
#define ANDROID_DISP_SYNC_ENHANCEMENT_API_LOADER_H

#include <utils/String8.h>
#include <utils/Singleton.h>
#include <vsync_enhance/DispSyncEnhancementApi.h>

namespace android {
// ---------------------------------------------------------------------------
#ifdef MTK_VSYNC_ENHANCEMENT_SUPPORT
class DispSyncEnhancementApiLoader : public Singleton<DispSyncEnhancementApiLoader>
{
public:
    DispSyncEnhancementApiLoader();
    ~DispSyncEnhancementApiLoader();

    // get some function pointer from DispSync
    void registerFunction(struct DispSyncEnhancementFunctionList* list);

    // used to change the VSync mode and fps
    status_t setVSyncMode(int32_t mode, int32_t fps, nsecs_t* period,
                                  nsecs_t* phase, nsecs_t* referenceTime);

    // used to add present fence for calibration
    bool addPresentFence(bool* res);

    // used to add the sample of hw vsync
    bool addResyncSample(bool* res, nsecs_t timestamp, nsecs_t* period,
                                 nsecs_t* phase, nsecs_t* referenceTime);

    // notify caller that we do not want to clear parameter of DispSync
    bool obeyResync();

    // dump the information of enhancement
    void dump(std::string& result) const;

    // get some function pointer from SurfaceFlinger
    void registerSfCallback(struct SurfaceFlingerCallbackList* list);

private:
    void* mEnhancementHandle;
    DispSyncEnhancementApi* mEnhancement;
};
#endif
// ---------------------------------------------------------------------------
}; // namespace android

#endif // ANDROID_DISP_SYNC_ENHANCEMENT_API_LOADER_H
