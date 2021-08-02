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

#include "SFWatchDogAPILoader.h"

namespace android {

#ifdef MTK_SF_WATCHDOG_SUPPORT

ANDROID_SINGLETON_STATIC_INSTANCE(SFWatchDogAPILoader);

SFWatchDogAPILoader::SFWatchDogAPILoader()
    : mSoHandle(nullptr)
    , mMessageWDT(nullptr)
{
    typedef SFWatchDogAPI *(*createSFWDPrototype)();
    mSoHandle = dlopen("libsf_debug.so", RTLD_NOW);
    if (mSoHandle) {
        createSFWDPrototype createSFWDPtr =
            reinterpret_cast<createSFWDPrototype>(dlsym(mSoHandle, "createInstance"));
        if (createSFWDPtr == nullptr) {
            ALOGD("Can't load func mCreateSFWDPtr");
        } else if ((mMessageWDT = createSFWDPtr()) == nullptr) {
            ALOGD("Can't get SFWD");
        }
    } else {
        ALOGD("Can't load libsf_debug");
    }
}

SFWatchDogAPILoader::~SFWatchDogAPILoader()
{
    if (mMessageWDT != nullptr) {
        delete mMessageWDT;
    }
    if (mSoHandle != nullptr) {
        dlclose(mSoHandle);
    }
}

bool SFWatchDogAPILoader::setAnchor(const String8& msg)
{
    if (mMessageWDT != nullptr) {
        return mMessageWDT->setAnchor(msg);
    } else {
        return false;
    }
}

bool SFWatchDogAPILoader::delAnchor()
{
    if (mMessageWDT != nullptr) {
        return mMessageWDT->delAnchor();
    } else {
        return false;
    }
}

void SFWatchDogAPILoader::setSuspend()
{
    if (mMessageWDT != nullptr) {
        mMessageWDT->setSuspend();
    } else {
        return;
    }
}

void SFWatchDogAPILoader::setResume()
{
    if (mMessageWDT != nullptr) {
        mMessageWDT->setResume();
    } else {
        return;
    }
}

void SFWatchDogAPILoader::setThreshold(const SWWatchDog::msecs_t& threshold)
{
    if (mMessageWDT != nullptr) {
        mMessageWDT->setThreshold(threshold);
    } else {
        return;
    }
}

#endif

}; // namespace android
