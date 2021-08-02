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
 * MediaTek Inc. (C) 2016~2017. All rights reserved.
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

//- vim: set ts=4 sts=4 sw=4 et: --------------------------------------------

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#ifdef MTK_ANDROID_PLATFORM
#include <cutils/properties.h>
#endif

#include "boots.h"

//---------------------------------------------------------------------------
#define LOG_TAG "boots_osi"

//---------------------------------------------------------------------------
int osi_property_get(const char *key, char *value, const char *default_value)
{
    UNUSED(boots_btif);
#ifdef MTK_ANDROID_PLATFORM
    return property_get(key, value, default_value);
#else
    UNUSED(key);
    UNUSED(value);
    UNUSED(default_value);
    BPRINT_E("%s: Do not support this OSI currently", __func__);
    return -1;
#endif
}

//---------------------------------------------------------------------------
int osi_property_set(const char *key, const char *value)
{
#ifdef MTK_ANDROID_PLATFORM
    return property_set(key, value);
#else
    UNUSED(key);
    UNUSED(value);
    BPRINT_E("%s: Do not support this OSI currently", __func__);
    return -1;
#endif
}

int osi_system(const char *cmd)
{
    FILE *fp;
    int ret;

    if (cmd == NULL) {
        BPRINT_E("%s: cmd is NULL", __func__);
        return -1;
    }

    fp = popen(cmd, "w");
    if (fp == NULL) {
        BPRINT_E("%s: (%s) failed", __func__, cmd);
        return -1;
    }

    BPRINT_I("Command: %s\n", cmd);

    ret = pclose(fp);
    if (ret < 0) {
        BPRINT_E("%s: pclose ret = %d", __func__, ret);
    } else if (ret > 0) {
        BPRINT_I("%s: pclose ret = %d", __func__, ret);
    }
    return ret;
}

