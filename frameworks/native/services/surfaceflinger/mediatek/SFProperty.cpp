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

#include <cutils/properties.h>
#include <log/log.h>
#include "SFProperty.h"

namespace android {

using base::StringAppendF;

#ifdef MTK_SF_DEBUG_SUPPORT

ANDROID_SINGLETON_STATIC_INSTANCE(SFProperty);

PropertiesState* SFProperty::getPropState()
{
    return &sPropertiesState;
}

void SFProperty::setMTKProperties(int& debugRegion) {
    std::string result;
    setMTKProperties(result, debugRegion);
    ALOGD("%s", result.c_str());
}

void SFProperty::setMTKProperties(std::string& result, int& debugRegion) {
    const size_t SIZE = 4096;
    char buffer[SIZE];

    char value[PROPERTY_VALUE_MAX];

    snprintf(buffer, sizeof(buffer), "[%s]\n", __func__);
    StringAppendF(&result, "%s", buffer);
    StringAppendF(&result, "========================================================================\n");

    property_get("debug.sf.showupdates", value, "0");
    debugRegion = atoi(value);
    snprintf(buffer, sizeof(buffer), "    debug.sf.showupdates (mDebugRegion): %d\n", debugRegion);
    StringAppendF(&result, "%s", buffer);

    property_get("debug.sf.ddms", value, "0");
    int debugDDMS = atoi(value);
    snprintf(buffer, sizeof(buffer), "    debug.sf.ddms (mDebugDDMS): %d\n", debugDDMS);
    StringAppendF(&result, "%s", buffer);

    if (0 != debugDDMS) {
        // FIX-ME:  Why remove DdmConnection.cpp from Android.mk
        //DdmConnection::start(getServiceName());
        ALOGI("DDMS debugging not supported");
    }

    StringAppendF(&result,"[MediaTek SF]\n");

    property_get("vendor.debug.sf.slowmotion", value, "0");
    sPropertiesState.mDelayTime = atoi(value);
    snprintf(buffer, sizeof(buffer), "    vendor.debug.sf.slowmotion (mDelayTime): %d\n", sPropertiesState.mDelayTime);
    StringAppendF(&result, "%s", buffer);

    // for internal screen composition update
    property_get("vendor.debug.sf.log_repaint", value, "0");
    sPropertiesState.mLogRepaint = atoi(value);
    snprintf(buffer, sizeof(buffer), "    vendor.debug.sf.log_repaint (mLogRepaint): %d\n", sPropertiesState.mLogRepaint);
    StringAppendF(&result, "%s", buffer);

    property_get("vendor.debug.sf.log_transaction", value, "0");
    sPropertiesState.mLogTransaction = atoi(value);
    snprintf(buffer, sizeof(buffer), "    vendor.debug.sf.log_transaction (mLogTransaction): %d\n", sPropertiesState.mLogTransaction);
    StringAppendF(&result, "%s", buffer);

    // debug utils
    property_get("vendor.debug.sf.line_g3d", value, "0");
    sPropertiesState.mLineG3D = atoi(value);
    snprintf(buffer, sizeof(buffer), "    vendor.debug.sf.line_g3d (mLineG3D): %d\n", sPropertiesState.mLineG3D);
    StringAppendF(&result, "%s", buffer);


    StringAppendF(&result, "[MediaTek GUI]\n");
    // just get and print, real switches should be in libgui

    property_get("vendor.debug.bq.dump", value, "NULL");
    snprintf(buffer, sizeof(buffer), "    vendor.debug.bq.dump: %s\n", value);
    StringAppendF(&result, "%s", buffer);

    property_get("vendor.debug.bq.line", value, "0");
    snprintf(buffer, sizeof(buffer), "    vendor.debug.bq.line: %s\n", value);
    StringAppendF(&result, "%s", buffer);

    snprintf(buffer, sizeof(buffer), "    *** dynamic modification ***\n");
    StringAppendF(&result, "%s", buffer);

    property_get("vendor.debug.bq.line_p", value, "0");
    snprintf(buffer, sizeof(buffer), "        vendor.debug.bq.line_p (set fixed index): %s\n", value);
    StringAppendF(&result, "%s", buffer);

    property_get("vendor.debug.bq.line_g", value, "0");
    snprintf(buffer, sizeof(buffer), "        vendor.debug.bq.line_g (set drawing grid, ex: 16:1 / 1:16): %s\n", value);
    StringAppendF(&result,"%s", buffer);

    property_get("vendor.debug.bq.line_c", value, "0");
    snprintf(buffer, sizeof(buffer), "        vendor.debug.bq.line_c (set drawing color): %s\n", value);
    StringAppendF(&result, "%s", buffer);

    property_get("vendor.debug.bq.ext_service", value, "NULL");
    snprintf(buffer, sizeof(buffer), "        vendor.debug.bq.ext_service (dlopen libgui_exit.so): %s\n", value);
    StringAppendF(&result, "%s", buffer);

    StringAppendF(&result, "========================================================================\n\n");
}

#endif

}; // namespace android
