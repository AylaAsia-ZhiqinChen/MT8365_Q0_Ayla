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

#include <dlfcn.h>
#include <log/log.h>

#include "DispSyncEnhancementApiLoader.h"

namespace android {

#ifdef MTK_VSYNC_ENHANCEMENT_SUPPORT

ANDROID_SINGLETON_STATIC_INSTANCE(DispSyncEnhancementApiLoader);

DispSyncEnhancementApiLoader::DispSyncEnhancementApiLoader()
    : mEnhancementHandle(nullptr)
    , mEnhancement(nullptr)
{
    typedef DispSyncEnhancementApi* (*createDispSyncPrototype)();
    mEnhancementHandle = dlopen("libvsync_enhance.so", RTLD_LAZY);
    if (mEnhancementHandle) {
        createDispSyncPrototype creatPtr = reinterpret_cast<createDispSyncPrototype>(dlsym(mEnhancementHandle, "createDispSyncEnhancement"));
        if (creatPtr) {
            mEnhancement = creatPtr();
        } else {
            ALOGW("Failed to get function: createDispSyncEnhancement");
        }
    } else {
        ALOGW("Failed to load libvsync_enhance.so");
    }
}

DispSyncEnhancementApiLoader::~DispSyncEnhancementApiLoader()
{
    if (mEnhancement) {
        delete mEnhancement;
    }
    if (mEnhancementHandle) {
        dlclose(mEnhancementHandle);
    }
}

void DispSyncEnhancementApiLoader::registerFunction(struct DispSyncEnhancementFunctionList* list) {
    if (mEnhancement) {
        mEnhancement->registerFunction(list);
    }
}

bool DispSyncEnhancementApiLoader::obeyResync() {
    if (mEnhancement) {
        return mEnhancement->obeyResync();
    }
    return true;
}

bool DispSyncEnhancementApiLoader::addPresentFence(bool* res) {
    if (mEnhancement) {
        return mEnhancement->addPresentFence(res);
    }
    return false;
}

bool DispSyncEnhancementApiLoader::addResyncSample(bool* res, nsecs_t timestamp, nsecs_t* period,
                                  nsecs_t* phase, nsecs_t* referenceTime) {
    if (mEnhancement) {
        return mEnhancement->addResyncSample(res, timestamp, period, phase, referenceTime);
    }
    return false;
}

void DispSyncEnhancementApiLoader::dump(std::string& result) const {
    if (mEnhancement) {
        mEnhancement->dump(result);
    }
}

status_t DispSyncEnhancementApiLoader::setVSyncMode(int32_t mode, int32_t fps, nsecs_t* period,
                                   nsecs_t* phase, nsecs_t* referenceTime) {
    status_t res = NO_ERROR;
    if (mEnhancement) {
        res = mEnhancement->setVSyncMode(mode, fps, period, phase, referenceTime);
    }
    return res;
}

void DispSyncEnhancementApiLoader::registerSfCallback(struct SurfaceFlingerCallbackList* list) {
    if (mEnhancement) {
        mEnhancement->registerSfCallback(list);
    }
}
#endif

}; // namespace android
