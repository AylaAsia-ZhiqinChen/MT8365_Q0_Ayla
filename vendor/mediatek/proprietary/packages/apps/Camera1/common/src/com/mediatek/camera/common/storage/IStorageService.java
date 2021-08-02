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
 * MediaTek Inc. (C) 2016. All rights reserved.
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

package com.mediatek.camera.common.storage;

import android.content.Intent;
/**
 * The interface IStorageService.
 * used for those who want know about storage info
 */
public interface IStorageService {
    int STORAGE_IS_OUT_OF_SERVICES = 0;
    int STORAGE_IS_IN_SERVICES = 1;
    int STORAGE_IS_PREPARING = 2;
    /**
     * The interface IStorageStateListener.
     * use for those who want monitor storage
     * state and do some thing for it.
     * this is mainly used for sd card.
     */
    interface IStorageStateListener {
        /**
         * Use for response storage state change.
         * @param storageState its values are
         *  STORAGE_IS_OUT_OF_SERVICES/STORAGE_IS_IN_SERVICES/STORAGE_IS_PREPARING
         *  means storage three different states.
         *  @param intent sd card info get from broadcast and feature can use this
         *  do some thing.
         */
        void onStateChanged(int storageState, Intent intent);
    }

    /**
     * Get the storage space for record.
     * the space has subtract storage threshold to
     * make sure video can save success.
     * @return left storage space for record and unit is byte.
     */
    long getRecordStorageSpace();
    /**
     * Get the storage space for capture.
     * the space has subtract storage threshold to
     * make sure capture can save success.
     * @return left storage space for capture and unit is byte.
     */
    long getCaptureStorageSpace();
    /**
     * Get file storage directory.
     * @return the storage directory.
     * Sdk version is bigger M and mediatek internal apk maybe have
     * the permission to write data to sd card,so in this case and
     * system default path is sd card will return sd card path. otherwise
     * will return internal path.
     *
     */
    String getFileDirectory();
    /**
     * Register listener to monitor the storage state change.
     * @param listener the use listener.
     */
    void registerStorageStateListener(IStorageStateListener listener);

    /**
     * Remove the listener to monitor the storage state change.
     * @param listener the use listener.
     */
    void unRegisterStorageStateListener(IStorageStateListener listener);
}