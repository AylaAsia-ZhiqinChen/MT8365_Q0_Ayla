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
#include <inttypes.h>

#include <utils/Log.h>
#include <renderengine/RenderEngine.h>
#include <compositionengine/Display.h>
#include <compositionengine/RenderSurface.h>

#include "SurfaceFlinger.h"
#include "Scheduler/PhaseOffsets.h"

#ifdef MTK_VSYNC_ENHANCEMENT_SUPPORT
#include "DispSyncEnhancementApiLoader.h"
#endif

#ifdef MTK_SF_DEBUG_SUPPORT
#include "mediatek/SFProperty.h"
#include "mediatek/PropertiesState.h"
#endif

namespace android {
using base::StringAppendF;

#ifdef MTK_BOOT_PROF
void SurfaceFlinger::bootProf(int start) {
    int fd         = open("/proc/bootprof", O_RDWR);
    int fd_nand    = open("/proc/driver/nand", O_RDWR);

    if (fd == -1) {
        ALOGE("fail to open /proc/bootproffile : %s", strerror(errno));
        return;
    }

    const size_t BUF_SIZE = 64;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    if (1 == start) {
        strncpy(buf,"BOOT_Animation:START", BUF_SIZE - 1);
        buf[BUF_SIZE - 1] = '\0';
        if (fd > 0) {
            write(fd, buf, 32);
            close(fd);
        }
        if (fd_nand > 0) {
            close(fd_nand);
        }
    } else {
        strncpy(buf, "BOOT_Animation:END", BUF_SIZE - 1);
        buf[BUF_SIZE - 1] = '\0';
        if (fd > 0) {
            write(fd, buf, 32);
            close(fd);
        }
        if (fd_nand > 0) {
            write(fd_nand, "I1", 2);
            close(fd_nand);
        }
    }
}
#endif

#ifdef MTK_SF_DEBUG_SUPPORT
status_t SurfaceFlinger::getProcessName(int pid, String8& name)
{
    FILE *fp = fopen(String8::format("/proc/%d/cmdline", pid), "r");
    if (NULL != fp) {
        const size_t size = 64;
        char proc_name[size];
        fgets(proc_name, size, fp);
        fclose(fp);

        name = proc_name;
        return NO_ERROR;
    }

    return INVALID_OPERATION;
}

status_t SurfaceFlinger::dumpLock(bool& dumpLocked, nsecs_t& start, std::string& result)
{
    status_t err = mDumpLock.timedLock(s2ns(1));
    dumpLocked = (err == NO_ERROR);
    if (!dumpLocked) {
        StringAppendF(&result,
                "SurfaceFlinger appears to be unresponsive (%s [%d]), "
                "dumping anyways (no locks held)\n", strerror(-err), err);
    }

    err = mStateLock.timedLock(s2ns(1));
    start = systemTime(SYSTEM_TIME_MONOTONIC);
    ALOGD("start SF dump");

    return err;
}

void SurfaceFlinger::dumpUnlock(bool dumpLocked, nsecs_t start) NO_THREAD_SAFETY_ANALYSIS
{
    if (dumpLocked) {
        mDumpLock.unlock();
    }

    nsecs_t end = systemTime(SYSTEM_TIME_MONOTONIC);
    ALOGD("end SF dump [%" PRId64 "ns]", end - start);
}

void SurfaceFlinger::mtkDump(std::string& result)
{
    // for run-time enable property
    SFProperty::getInstance().setMTKProperties(result, mDebugRegion);
}
#endif

#ifdef MTK_VSYNC_ENHANCEMENT_SUPPORT
status_t SurfaceFlinger::checkMtkTransactCodeCredentials(uint32_t code) {
    if (code > 10000 && code <= 10002) {
        return OK;
    }
    return BAD_VALUE;
}

status_t SurfaceFlinger::onMtkTransact(uint32_t code, const Parcel& data, Parcel* /*reply*/, uint32_t /*flags*/) {
    int n;
    switch (code) {
        case 10001: {
            n = data.readInt32();
            adjustSwVsyncPeriod(n);
            return NO_ERROR;
        }
        case 10002: {
            n = data.readInt32();
            adjustSwVsyncOffset(static_cast<nsecs_t>(n));
            return NO_ERROR;
        }
    }

    return UNKNOWN_TRANSACTION;
}

void SurfaceFlinger::adjustSwVsyncPeriod(int32_t fps) {
    ALOGI("Get request of changing vsync fps: %d", fps);
    if (fps == DS_DEFAULT_FPS || fps <= 0) {
        mScheduler->setVSyncMode(VSYNC_MODE_CALIBRATED_SW_VSYNC, fps);
    } else {
        mScheduler->setVSyncMode(VSYNC_MODE_INTERNAL_SW_VSYNC, fps);
    }
}

void SurfaceFlinger::adjustSwVsyncOffset(nsecs_t offset) {
    ALOGI("Adjust vsync offset: %" PRId64, offset);
    mPhaseOffsets->setLatePhaseOffsets(offset, offset, offset, offset);
    const auto [early, gl, late] = mPhaseOffsets->getCurrentOffsets();
    mVsyncModulator.setPhaseOffsets(early, gl, late);
}

void SurfaceFlinger::initVsyncEnhancement() {
    struct SurfaceFlingerCallbackList list;
    list.onVSyncOffsetChange = std::bind(&SurfaceFlinger::adjustSwVsyncOffset, this,
                                      std::placeholders::_1);
    DispSyncEnhancementApiLoader::getInstance().registerSfCallback(&list);
}
#endif

#ifdef MTK_SF_DEBUG_SUPPORT
void SurfaceFlinger::drawDebugLine(const sp<DisplayDevice>& displayDevice) {
    const uint32_t steps = 32;
    static bool enableLineG3D = false;
    if (CC_UNLIKELY(SFProperty::getInstance().getPropState()->mLineG3D != enableLineG3D)) {
        enableLineG3D = SFProperty::getInstance().getPropState()->mLineG3D;
        auto& re = getRenderEngine();
        re.enableDebugLine(enableLineG3D);
    }
    if (enableLineG3D) {
        auto& re = getRenderEngine();
        auto display = displayDevice->getCompositionDisplay();
        auto* rs = display->getRenderSurface();
        uint32_t color = 0;
        if (displayDevice->isPrimary()) {
            color = 0xFF0000FF;
        } else if (displayDevice->isVirtual()) {
            color = 0xFFFF0000;
        } else {
            color = 0xFFFFFFFF;
        }
        re.setDebugLineConfig(displayDevice->getWidth(), displayDevice->getHeight(),
                rs->getPageFlipCount(), color, steps);
    }
}
#endif

}; // namespace android
