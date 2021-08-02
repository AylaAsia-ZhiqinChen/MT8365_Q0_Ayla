/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.browser;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupMenu;
import android.widget.PopupMenu.OnMenuItemClickListener;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.android.browser.NavTabScroller.OnLayoutListener;
import com.android.browser.NavTabScroller.OnRemoveListener;
import com.android.browser.preferences.GeneralPreferencesFragment;
import com.android.browser.TabControl.OnThumbnailUpdatedListener;
import com.android.browser.UI.ComboViews;

import java.util.HashMap;

public class NavScreen extends RelativeLayout
        implements OnClickListener, OnMenuItemClickListener, OnThumbnailUpdatedListener {

    private static final String LOGTAG = "NavScreen";
    /// M: add for debug @ {
    private static final boolean DEBUG = Browser.DEBUG;
    private static final String TAG = "browser";
    /// @ }
    UiController mUiController;
    PhoneUi mUi;
    Tab mTab;
    Activity mActivity;

    ImageButton mRefresh;
    ImageButton mForward;
    ImageButton mBookmarks;
    ImageButton mMore;
    ImageButton mNewTab;
    FrameLayout mHolder;

    TextView mTitle;
    ImageView mFavicon;
    ImageButton mCloseTab;

    NavTabScroller mScroller;
    TabAdapter mAdapter;
    int mOrientation;
    boolean mNeedsMenu;
    HashMap<Tab, View> mTabViews;

    PopupMenu mPopup;

    public NavScreen(Activity activity, UiController ctl, PhoneUi ui) {
        super(activity);
        mActivity = activity;
        mUiController = ctl;
        mUi = ui;
        mOrientation = activity.getResources().getConfiguration().orientation;
        init();
    }

    /**
     * M: For bug fix:ALPS000316857
     */
    public void reload() {
        int sv = mScroller.getScrollValue();
        removeAllViews();
        if (mPopup != null)
        {
            mPopup.dismiss();
        }
        mOrientation = mActivity.getResources().getConfiguration().orientation;
        init();
        mScroller.setScrollValue(sv);
        mAdapter.notifyDataSetChanged();
    }

    protected void showMenu() {
        mPopup = new PopupMenu(mContext, mMore);
        Menu menu = mPopup.getMenu();
        mPopup.getMenuInflater().inflate(R.menu.browser, menu);
        mUiController.updateMenuState(mUiController.getCurrentTab(), menu);
        mPopup.setOnMenuItemClickListener(this);
        mPopup.show();
    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        return mUiController.onOptionsItemSelected(item);
    }

    protected float getToolbarHeight() {
        return mActivity.getResources().getDimension(R.dimen.toolbar_height);
    }

    @Override
    protected void onConfigurationChanged(Configuration newconfig) {
        Log.d(LOGTAG, "NavScreen.onConfigurationChanged() new orientation = "
                       + newconfig.orientation +
            ", original orientation = " + mOrientation);
        if (newconfig.orientation != mOrientation) {
            int sv = mScroller.getScrollValue();
            removeAllViews();
            if (mPopup != null)
            {
                mPopup.dismiss();
            }
            mOrientation = newconfig.orientation;
            init();
            mScroller.setScrollValue(sv);
            mAdapter.notifyDataSetChanged();
        }
    }

    public void refreshAdapter() {
        mScroller.handleDataChanged(
                mUiController.getTabControl().getTabPosition(mUi.getActiveTab()));
    }

    private void init() {
        LayoutInflater.from(mContext).inflate(R.layout.nav_screen, this);
        setContentDescription(mContext.getResources().getString(
                R.string.accessibility_transition_navscreen));
        mBookmarks = (ImageButton) findViewById(R.id.bookmarks);
        mNewTab = (ImageButton) findViewById(R.id.newtab);
        mMore = (ImageButton) findViewById(R.id.more);
        mBookmarks.setOnClickListener(this);
        mNewTab.setOnClickListener(this);
        mMore.setOnClickListener(this);
        mScroller = (NavTabScroller) findViewById(R.id.scroller);
        TabControl tc = mUiController.getTabControl();
        mTabViews = new HashMap<Tab, View>(tc.getTabCount());
        mAdapter = new TabAdapter(mContext, tc);
        mScroller.setOrientation(mOrientation == Configuration.ORIENTATION_LANDSCAPE
                ? LinearLayout.HORIZONTAL : LinearLayout.VERTICAL);
        // update state for active tab
        mScroller.setAdapter(mAdapter,
                mUiController.getTabControl().getTabPosition(mUi.getActiveTab()));
        mScroller.setOnRemoveListener(new OnRemoveListener() {
            public void onRemovePosition(int pos) {
                Tab tab = mAdapter.getItem(pos);
                onCloseTab(tab);
                mNewTab.setClickable(true);
                updateBookMarkButton();
            }
        });
        mNeedsMenu = !ViewConfiguration.get(getContext()).hasPermanentMenuKey();
        if (!mNeedsMenu) {
            mMore.setVisibility(View.GONE);
        }
        updateBookMarkButton();
    }

    private void updateBookMarkButton() {
        if (mUiController.getTabControl().getTabCount() == 0) {
            mBookmarks.setVisibility(View.GONE);
            mNewTab.setVisibility(View.GONE);
        } else {
            mBookmarks.setVisibility(View.VISIBLE);
            mNewTab.setVisibility(View.VISIBLE);
        }
    }

    @Override
    public void onClick(View v) {
        if (mBookmarks == v) {
            mUiController.bookmarksOrHistoryPicker(ComboViews.Bookmarks);
        } else if (mNewTab == v) {
            openNewTab();
        } else if (mMore == v) {
            showMenu();
        }
    }

    private void onCloseTab(Tab tab) {
        if (DEBUG) {
            Log.d(TAG, "NavScreen.onCloseTab()--->tab : " + tab);
        }
        if (tab != null) {
            if (tab == mUiController.getCurrentTab()) {
                mUiController.closeCurrentTab();
            } else {
                mUiController.closeTab(tab);
            }

            /// M: fix CR:ALPS00488127 here must remove the tab from HashMap to release memory. @{
            ImageView image = (ImageView) mTabViews.get(tab);
            if (image != null) {
                image.setImageBitmap(null);
            }
            /// @}
            mTabViews.remove(tab);
        }
        updateBookMarkButton();
    }

    private void openNewTab() {
        // need to call openTab explicitely with setactive false
        final Tab tab = mUiController.openTab(GeneralPreferencesFragment.BLANK_URL,
                false, false, false);
        if (tab != null) {
            mUiController.setBlockEvents(true);
            final int tix = mUi.mTabControl.getTabPosition(tab);
            mScroller.setOnLayoutListener(new OnLayoutListener() {

                @Override
                public void onLayout(int l, int t, int r, int b) {
                    final int pos = mUi.mTabControl.getTabPosition(tab);
                    mUi.hideNavScreen(pos, true);
                    switchToTab(tab);
                }
            });
            mScroller.handleDataChanged(tix);
            mUiController.setBlockEvents(false);
        }
        updateBookMarkButton();
        if (DEBUG) {
            Log.d(TAG, "NavScreen.openNewTab()--->new tab is " + tab);
        }
    }

    private void switchToTab(Tab tab) {
        if (tab != mUi.getActiveTab()) {
            mUiController.setActiveTab(tab);
        }
    }

    protected void close(int position) {
        close(position, true);
    }

    protected void close(int position, boolean animate) {
        mUi.hideNavScreen(position, animate);
    }

    protected NavTabView getTabView(int pos) {
        return mScroller.getTabView(pos);
    }

    class TabAdapter extends BaseAdapter {

        Context context;
        TabControl tabControl;

        public TabAdapter(Context ctx, TabControl tc) {
            context = ctx;
            tabControl = tc;
        }

        @Override
        public int getCount() {
            return tabControl.getTabCount();
        }

        @Override
        public Tab getItem(int position) {
            return tabControl.getTab(position);
        }

        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            final NavTabView tabview = new NavTabView(mActivity);
            final Tab tab = getItem(position);
            tabview.setWebView(tab);
            mTabViews.put(tab, tabview.mImage);
            tabview.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (tabview.isClose(v)) {
                        mNewTab.setClickable(false);
                        mScroller.animateOut(tabview);
                        mTabViews.remove(tab);
                    } else if (tabview.isTitle(v)) {
                        switchToTab(tab);
                        mUi.getTitleBar().setSkipTitleBarAnimations(true);
                        close(position, false);
                        mUi.editUrl(false, true);
                        mUi.getTitleBar().setSkipTitleBarAnimations(false);
                    } else if (tabview.isWebView(v)) {
                        close(position);
                    }
                }
            });
            return tabview;
        }

    }

    @Override
    public void onThumbnailUpdated(Tab t) {
        View v = mTabViews.get(t);
        if (v != null) {
            v.invalidate();
        }
    }

}
