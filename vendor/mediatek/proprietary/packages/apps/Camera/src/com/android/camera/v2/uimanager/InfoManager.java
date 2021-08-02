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
 * MediaTek Inc. (C) 2014. All rights reserved.
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
package com.android.camera.v2.uimanager;

import android.app.Activity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.android.camera.R;
import com.android.camera.v2.ui.UiUtil;
import com.mediatek.camera.debug.LogHelper;
import com.mediatek.camera.debug.LogHelper.Tag;

public class InfoManager extends AbstractUiManager {
    private static final Tag TAG = new Tag(InfoManager.class.getSimpleName());
    private TextView                             mInfoView;
    private CharSequence                         mInfoText;
    private ViewGroup                            mParentView;
    private View                                 mInfoLayout;

    public InfoManager(Activity activity, ViewGroup parent) {
        super(activity, parent);
        mParentView = parent;
    }

    @Override
    protected View getView() {
        View view = inflate(R.layout.onscreen_info_v2);
        mInfoLayout = view;
        mInfoView = (TextView) view.findViewById(R.id.info_view);
        return view;
    }

    @Override
    protected void onRefresh() {
        LogHelper.d(TAG, "onRefresh() mInfoView=" + mInfoView + ", mInfoText=" + mInfoText);
        if (mInfoView != null) {
            mInfoView.setText(mInfoText);
            int visibility = mInfoText != null ? View.VISIBLE : View.INVISIBLE;
            mInfoView.setVisibility(visibility);
        }
    }

    public void showText(CharSequence text) {
        LogHelper.d(TAG, "showText(" + text + ")");
        mInfoText = text;
        show();
        int orientation = (Integer) mParentView.getTag();
        UiUtil.setOrientation(mInfoLayout, orientation, false);
    }
}
