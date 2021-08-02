/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*
 * Copyright (C) 2007-2008 Esmertec AG.
 * Copyright (C) 2007-2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.mediatek.op18.mms;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.TextView;

import com.mediatek.op18.mms.LevelControlLayout.OnScrollToScreenListener;
import com.mediatek.op18.mms.R;
//import com.mediatek.mms.ipmessage.IpSharePanel;

public class SharePanel extends LinearLayout {
    private static final String TAG = "Mms/Op18SharePanel";
    private Handler mHandler;
    private Context mContext;
    private View mConvertView;
    private LevelControlLayout mScrollLayout;
    private LinearLayout mSharePanelMain;
    private RadioButton mDotFirst;
    private RadioButton mDotSec;

    private int mOrientation;
    private int[] mColumnArray;
    private int mScreenIndex;
    // add for ipmessage
//    private IpSharePanel mIpSharePanel;

    /// M: MMS
    public static final int TAKE_PICTURE            = 0;
    public static final int RECORD_VIDEO            = 1;
    public static final int RECORD_SOUND            = 2;
    public static final int ADD_VCARD               = 3;
    public static final int ADD_IMAGE               = 4;
    public static final int ADD_VIDEO               = 5;
    public static final int ADD_SOUND               = 6;
    public static final int ADD_VCALENDAR           = 7;
    public static final int ADD_SLIDESHOW           = 8;

    private static final int[] MMS_ACTIONS = {
        TAKE_PICTURE, RECORD_VIDEO, RECORD_SOUND, ADD_VCARD,
        ADD_IMAGE, ADD_VIDEO, ADD_SOUND, ADD_VCALENDAR,
        ADD_SLIDESHOW};

    public static final int ACTION_SHARE = 1;

    public static final int[] shareIconArr = {
            R.drawable.ipmsg_take_a_photo, R.drawable.ipmsg_record_a_video,
            R.drawable.ipmsg_record_an_audio, R.drawable.ipmsg_share_contact,
            R.drawable.ipmsg_choose_a_photo, R.drawable.ipmsg_choose_a_video,
            R.drawable.ipmsg_choose_an_audio, R.drawable.ipmsg_share_calendar,
            R.drawable.ipmsg_add_slideshow
    };

    public static final String SHARE_ACTION = "shareAction";
    private String[] mSource;

    public SharePanel(Context context) {
        super(context);

        LayoutInflater inflater = LayoutInflater.from(context);
        mConvertView = inflater.inflate(R.layout.share_common_panel, this, true);
        mContext = context;
        init();
    }

//    public SharePanel(Context context, AttributeSet attrs) {
//        super(context, attrs);
//        LayoutInflater inflater = LayoutInflater.from(context);
//        mConvertView = inflater.inflate(R.layout.share_common_panel, this, true);
//        mContext = context;
////        // add for ipmessage
////        mIpSharePanel = IpMessageUtils.getIpMessagePlugin(context).getIpSharePanel();
//    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();
        init();
    }

    private void init() {
//        mScrollLayout = (LevelControlLayout) mConvertView.findViewById(R.id.share_panel_zone);
        mSharePanelMain = (LinearLayout) mConvertView.findViewById(R.id.share_panel_main);
        mScrollLayout = new LevelControlLayout(mContext, null);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                ViewGroup.LayoutParams.FILL_PARENT, 0, 1.0f);
        mScrollLayout.setLayoutParams(params);
        mSharePanelMain.addView(mScrollLayout);
        LayoutInflater.from(mContext).inflate(R.layout.share_panel_radio, mSharePanelMain, true);
        mDotFirst = (RadioButton) mSharePanelMain.findViewById(R.id.rb_dot_first);
        mDotSec = (RadioButton) mSharePanelMain.findViewById(R.id.rb_dot_sec);
        resetShareItem();
    }

    public void resetShareItem() {
        Log.d(TAG, "thiss resetShareItem");
        mOrientation = getResources().getConfiguration().orientation;
        if (mScrollLayout.getChildCount() != 0) {
            mScrollLayout.removeAllViews();
        }
        addSharePage(0);
        mDotFirst.setVisibility(View.VISIBLE);
        mDotFirst.setChecked(true);
        int pageNumber = computePageNumber();
        if (pageNumber == 2) {
            //total two pages
            addSharePage(1);
            mDotSec.setVisibility(View.VISIBLE);
        } else if (pageNumber == 1) {
            mDotSec.setVisibility(View.GONE);
            mDotFirst.setVisibility(View.GONE);
        } else if (pageNumber > 2) {
            //TODO: not support more than two pages
        }
        mScrollLayout.setOnScrollToScreen(new OnScrollToScreenListener() {
            @Override
            public void doAction(int whichScreen) {
                mScreenIndex = whichScreen;
                if (whichScreen == 0) {
                    mDotFirst.setChecked(true);
                } else {
                    mDotSec.setChecked(true);
                }
            }
        });
        mScrollLayout.setDefaultScreen(mScreenIndex);
        mScrollLayout.autoRecovery();
    }

    private void addSharePage(int index) {
        mColumnArray = getResources().getIntArray(R.array.share_column);
        View v = LayoutInflater.from(mContext)
                .inflate(R.layout.share_flipper, mScrollLayout, false);
        GridView gridView = (GridView) v.findViewById(R.id.gv_share_gridview);
        ViewGroup.LayoutParams params = mSharePanelMain.getLayoutParams();
        if (params == null) {
            params = new ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.FILL_PARENT,
                    ViewGroup.LayoutParams.FILL_PARENT);
        }
        if (mOrientation != Configuration.ORIENTATION_LANDSCAPE) {
            params.height = mContext.getResources()
                  .getDimensionPixelOffset(R.dimen.share_panel_port_height);
        } else {
              ///@M debug fix ALPS00867457;
              //if (!getResources().getBoolean(R.bool.isTablet))
                  params.height = mContext.getResources()
                      .getDimensionPixelOffset(R.dimen.share_panel_common_lan_height);
             /* else
               {
                  params.height = mContext.getResources()
                       .getDimensionPixelOffset(R.dimen.share_panel_lan_height_tablet);
              }*/
                ///@
        }
        mSharePanelMain.setLayoutParams(params);
        if (mOrientation == Configuration.ORIENTATION_PORTRAIT) {
            gridView.setNumColumns(mColumnArray[0]);
        } else {
            gridView.setNumColumns(mColumnArray[1]);
        }
        ShareAdapter adapter = new ShareAdapter(getLableArray(index), getIconArray(index));
        adapter.setIndex(index);
        gridView.setAdapter(adapter);
        gridView.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Message msg = mHandler.obtainMessage();
                Bundle bundle = new Bundle();
                int actionPosition = getActionId(position);
                // add for ipmessage
//                if (!mIpSharePanel.onIpGridViewItemClick(actionPosition, bundle)) {
                    if (actionPosition < 0 || actionPosition >= MMS_ACTIONS.length) {
                        return;
                    }
                    bundle.putInt(SHARE_ACTION, MMS_ACTIONS[actionPosition]);
//                }

                msg.setData(bundle);
                msg.what = ACTION_SHARE;
                mHandler.sendMessage(msg);
            }
        });
        mScrollLayout.addView(v);
    }

    private String[] getLableArray(int index) {
        // add for ipmessage
//        mSource = mIpSharePanel.getIpLableArray(mContext);
        if (mSource == null) {
            mSource = getResources().getStringArray(R.array.share_string_array);
        }

        int onePage;
        if (mOrientation == Configuration.ORIENTATION_PORTRAIT) {
            onePage = mColumnArray[0] * 2;
        } else {
            onePage = mColumnArray[1];
        }
        if (index == 0) {
            String[] index0 = new String[onePage];
            int count = onePage > mSource.length ? mSource.length : onePage;
            for (int i = 0; i < count; i++) {
                index0[i] = mSource[i];
            }
            return index0;
        } else {
            int count = mSource.length - onePage;
            String[] index1 = new String[count];
            for (int i = 0; i < count; i++) {
                index1[i] = mSource[onePage + i];
            }
            return index1;
        }
    }

    private int[] getIconArray(int index) {
        int[] source = shareIconArr;

        int onePage;
        if (mOrientation == Configuration.ORIENTATION_PORTRAIT) {
            onePage = mColumnArray[0] * 2;
        } else {
            onePage = mColumnArray[1];
        }
        if (index == 0) {
            int[] index0 = new int[onePage];
            int count = onePage > source.length ? source.length : onePage;
            for (int i = 0; i < count; i++) {
                index0[i] = source[i];
            }
            return index0;
        } else {
            int count = source.length - onePage;
            int[] index1 = new int[count];
            for (int i = 0; i < count; i++) {
                index1[i] = source[onePage + i];
            }
            return index1;
        }
    }

    private int getActionId(int position) {
        int onePage;
        if (mOrientation == Configuration.ORIENTATION_PORTRAIT) {
            onePage = mColumnArray[0] * 2;
        } else {
            onePage = mColumnArray[1];
        }
        if (mScreenIndex == 0) {
            return position;
        } else {
            return onePage + position;
        }
    }

    public void setHandler(Handler handler) {
        mHandler = handler;
    }

    public void recycleView() {
        if (mScrollLayout != null && mScrollLayout.getChildCount() != 0) {
            mScrollLayout.removeAllViews();
        }
    }

    private int computePageNumber() {
        int numberArray[] = getResources().getIntArray(R.array.share_column);
        int onePage;
        if (mOrientation == Configuration.ORIENTATION_PORTRAIT) {
            onePage = numberArray[0] * 2;
        } else {
            onePage = numberArray[1];
        }
        int pages = mSource.length / onePage +
                (mSource.length % onePage == 0 ? 0 : 1);
        return pages;
    }

    private class ShareAdapter extends BaseAdapter {

        private String[] mStringArray;
        private int[] mIconArray;
        private int mIndex;

        public ShareAdapter(String[] stringArray, int[] iconArray) {
            mStringArray = stringArray;
            mIconArray = iconArray;
        }

        public void setIndex(int index) {
            mIndex = index;
        }

        @Override
        public int getCount() {
            int count = 0;
            if (mOrientation == Configuration.ORIENTATION_PORTRAIT) {
                count = mColumnArray[0] * 2;
            } else {
                count = mColumnArray[1];
            }
            return count;
        }

        @Override
        public Object getItem(int position) {
            return null;
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = LayoutInflater.from(mContext)
                          .inflate(R.layout.share_grid_common_item, null);
                convertView.setTag(convertView);
            } else {
                convertView = (View) convertView.getTag();
            }

            if (mOrientation == Configuration.ORIENTATION_LANDSCAPE) {
                ViewGroup.LayoutParams params = convertView.getLayoutParams();
                if (params == null) {
                    params = new ViewGroup.LayoutParams(
                            ViewGroup.LayoutParams.FILL_PARENT,
                            ViewGroup.LayoutParams.FILL_PARENT);
                }
                params.height = mContext.getResources()
                        .getDimensionPixelOffset(R.dimen.share_panel_lan_item_height);
                convertView.setLayoutParams(params);
            }

            TextView text = (TextView) convertView.findViewById(R.id.tv_share_name);
            ImageView img = (ImageView) convertView.findViewById(R.id.iv_share_icon);
            int actionPosition = mIndex * getCount() + position;
//            if (!mIpSharePanel.getIpView(actionPosition, text, img)) {
                if (position < mStringArray.length) {
                    text.setText(mStringArray[position]);
                    img.setImageResource(mIconArray[position]);
                }
//            }
            return convertView;
        }
    }
}