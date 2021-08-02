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

#include "BufferLayer.h"

#include <cutils/properties.h>

#include <ui/gralloc_extra.h>

namespace android {

using ui::Dataspace;

#ifdef MTK_HDR_DISPLAY_SUPPORT
bool BufferLayer::isHdrHwSource() const
{
    static bool propertyLoaded = false;
    static bool hdrEnable = false;
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    if (!propertyLoaded) {
        char value[PROPERTY_VALUE_MAX] = {};
        property_get("vendor.debug.sf.hdr_enable", value, "0");

        hdrEnable = atoi(value);
        propertyLoaded = true;
    }

    if (!hdrEnable) {
        return false;
    }

    Dataspace standard = static_cast<Dataspace>(mCurrentDataSpace & Dataspace::STANDARD_MASK);
    if (standard != Dataspace::STANDARD_BT2020) {
        return false;
    }

    // only BT2020 case check hw video by gralloc_extra_query
    sp<GraphicBuffer> buf = mActiveBuffer;
    gralloc_extra_ion_sf_info_t extInfo;
    gralloc_extra_query(buf->handle, GRALLOC_EXTRA_GET_IOCTL_ION_SF_INFO, &extInfo);

    return ((extInfo.status & GRALLOC_EXTRA_MASK_TYPE) == GRALLOC_EXTRA_BIT_TYPE_VIDEO);
}
#endif

}; // namespace android
