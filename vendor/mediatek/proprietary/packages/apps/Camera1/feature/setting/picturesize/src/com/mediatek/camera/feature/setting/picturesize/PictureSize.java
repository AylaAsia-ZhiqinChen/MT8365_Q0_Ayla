/*
 * Copyright Statement:
 *
 *   This software/firmware and related documentation ("MediaTek Software") are
 *   protected under relevant copyright laws. The information contained herein is
 *   confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 *   the prior written permission of MediaTek inc. and/or its licensors, any
 *   reproduction, modification, use or disclosure of MediaTek Software, and
 *   information contained herein, in whole or in part, shall be strictly
 *   prohibited.
 *
 *   MediaTek Inc. (C) 2016. All rights reserved.
 *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *   THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *   RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 *   ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 *   WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 *   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 *   NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 *   RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *   INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 *   TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 *   RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 *   OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 *   SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 *   RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 *   STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 *   ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 *   RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 *   MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 *   CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *   The following software/firmware and/or related documentation ("MediaTek
 *   Software") have been modified by MediaTek Inc. All revisions are subject to
 *   any receiver's applicable license agreements with MediaTek Inc.
 */
package com.mediatek.camera.feature.setting.picturesize;

import com.mediatek.camera.common.ICameraContext;
import com.mediatek.camera.common.app.IApp;
import com.mediatek.camera.common.debug.LogHelper;
import com.mediatek.camera.common.debug.LogUtil;
import com.mediatek.camera.common.setting.ISettingManager.SettingController;
import com.mediatek.camera.common.setting.SettingBase;

import java.util.ArrayList;
import java.util.List;

import javax.annotation.Nonnull;

/**
 * Picture size setting item.
 *
 */
public class PictureSize extends SettingBase implements
        PictureSizeSettingView.OnValueChangeListener {
    private static final LogUtil.Tag TAG = new LogUtil.Tag(PictureSize.class.getSimpleName());
    private static final String KEY_PICTURE_SIZE = "key_picture_size";

    private ISettingChangeRequester mSettingChangeRequester;
    private PictureSizeSettingView mSettingView;

    @Override
    public void init(IApp app,
                     ICameraContext cameraContext,
                     SettingController settingController) {
        super.init(app, cameraContext, settingController);
    }

    @Override
    public void unInit() {

    }

    @Override
    public void addViewEntry() {
        if (mSettingView == null) {
            mSettingView = new PictureSizeSettingView(getKey());
            mSettingView.setOnValueChangeListener(this);
        }
        mAppUi.addSettingView(mSettingView);
    }

    @Override
    public void removeViewEntry() {
        mAppUi.removeSettingView(mSettingView);
    }

    @Override
    public void refreshViewEntry() {
        if (mSettingView != null) {
            mSettingView.setValue(getValue());
            mSettingView.setEntryValues(getEntryValues());
            mSettingView.setEnabled(getEntryValues().size() > 1);
        }
    }

    @Override
    public void postRestrictionAfterInitialized() {

    }

    @Override
    public SettingType getSettingType() {
        return SettingType.PHOTO;
    }

    @Override
    public String getKey() {
        return KEY_PICTURE_SIZE;
    }

    @Override
    public IParametersConfigure getParametersConfigure() {
        if (mSettingChangeRequester == null) {
            PictureSizeParametersConfig parametersConfig
                    = new PictureSizeParametersConfig(this, mSettingDeviceRequester);
            mSettingChangeRequester = parametersConfig;
        }
        return (PictureSizeParametersConfig) mSettingChangeRequester;
    }

    @Override
    public ICaptureRequestConfigure getCaptureRequestConfigure() {
        if (mSettingChangeRequester == null) {
            PictureSizeCaptureRequestConfig captureRequestConfig
                    = new PictureSizeCaptureRequestConfig(this, mSettingDevice2Requester);
            mSettingChangeRequester = captureRequestConfig;
        }
        return (PictureSizeCaptureRequestConfig) mSettingChangeRequester;
    }

    /**
     * Invoked after setting's all values are initialized.
     *
     * @param supportedPictureSize Picture sizes which is supported in current platform.
     */
    public void onValueInitialized(List<String> supportedPictureSize) {
        LogHelper.d(TAG, "[onValueInitialized], supportedPictureSize:" + supportedPictureSize);
        setSupportedPlatformValues(supportedPictureSize);
        setSupportedEntryValues(supportedPictureSize);
        setEntryValues(supportedPictureSize);

        double fullRatio = PictureSizeHelper.findFullScreenRatio(mActivity);
        List<Double> desiredAspectRatios = new ArrayList<>();
        desiredAspectRatios.add(fullRatio);
        desiredAspectRatios.add(PictureSizeHelper.RATIO_4_3);
        PictureSizeHelper.setDesiredAspectRatios(desiredAspectRatios);

        String valueInStore = mDataStore.getValue(getKey(), null, getStoreScope());
        if (valueInStore != null
                && !supportedPictureSize.contains(valueInStore)) {
            LogHelper.d(TAG, "[onValueInitialized], value:" + valueInStore
                    + " isn't supported in current platform");
            valueInStore = null;
            mDataStore.setValue(getKey(), null, getStoreScope(), false);
        }
        if (valueInStore == null) {
            // Default picture size is the max full-ratio size.
            List<String> entryValues = getEntryValues();
            for (String value : entryValues) {
                if (PictureSizeHelper.getStandardAspectRatio(value) == fullRatio) {
                    valueInStore = value;
                    break;
                }
            }
        }
        // If there is no full screen ratio picture size, use the first value in
        // entry values as the default value.
        if (valueInStore == null) {
            valueInStore = getEntryValues().get(0);
        }
        setValue(valueInStore);
    }

    @Override
    public void onValueChanged(String value) {
        LogHelper.i(TAG, "[onValueChanged], value:" + value);
        if (!getValue().equals(value)) {
            setValue(value);
            mDataStore.setValue(getKey(), value, getStoreScope(), false);
            mSettingChangeRequester.sendSettingChangeRequest();
        }
    }
}
