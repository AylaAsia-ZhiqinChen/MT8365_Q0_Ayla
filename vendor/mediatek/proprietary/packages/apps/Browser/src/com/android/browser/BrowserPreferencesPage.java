/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2008 The Android Open Source Project
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

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.preference.PreferenceActivity;

import com.android.browser.preferences.AccessibilityPreferencesFragment;
import com.android.browser.preferences.AdvancedPreferencesFragment;
import com.android.browser.preferences.BandwidthPreferencesFragment;
import com.android.browser.preferences.DebugPreferencesFragment;
import com.android.browser.preferences.GeneralPreferencesFragment;
import com.android.browser.preferences.LabPreferencesFragment;
import com.android.browser.preferences.PrivacySecurityPreferencesFragment;
import com.android.browser.preferences.WebsiteSettingsFragment;
import com.android.browser.preferences.SearchEngineSettings;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class BrowserPreferencesPage extends PreferenceActivity {

    public static final String CURRENT_PAGE = "currentPage";
    private List<Header> mHeaders;

    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            actionBar.setDisplayOptions(
                    ActionBar.DISPLAY_HOME_AS_UP, ActionBar.DISPLAY_HOME_AS_UP);
        }
    }

    /**
     * Populate the activity with the top-level headers.
     */
    @Override
    public void onBuildHeaders(List<Header> target) {
        loadHeadersFromResource(R.xml.preference_headers, target);

        if (BrowserSettings.getInstance().isDebugEnabled()) {
            Header debug = new Header();
            debug.title = getText(R.string.pref_development_title);
            debug.fragment = DebugPreferencesFragment.class.getName();
            target.add(debug);
        }
        mHeaders = target;
    }

    @Override
    public Header onGetInitialHeader() {
        String action = getIntent().getAction();
        if (Intent.ACTION_MANAGE_NETWORK_USAGE.equals(action)) {
            String fragName = BandwidthPreferencesFragment.class.getName();
            for (Header h : mHeaders) {
                if (fragName.equals(h.fragment)) {
                    return h;
                }
            }
        }
        return super.onGetInitialHeader();
    }

    @Override
    public boolean onSearchRequested() {
        // TODO Auto-generated method stub
        // return super.onSearchRequested();
        return false;
    }

    @Override
    public Intent onBuildStartFragmentIntent(String fragmentName, Bundle args,
            int titleRes, int shortTitleRes) {
        Intent intent = super.onBuildStartFragmentIntent(fragmentName, args,
                titleRes, shortTitleRes);
        String url = getIntent().getStringExtra(CURRENT_PAGE);
        intent.putExtra(CURRENT_PAGE, url);
        return intent;
    }

    @Override
    protected boolean isValidFragment(String fragmentName) {
        return AccessibilityPreferencesFragment.class.getName().equals(fragmentName) ||
                AdvancedPreferencesFragment.class.getName().equals(fragmentName) ||
                BandwidthPreferencesFragment.class.getName().equals(fragmentName) ||
                DebugPreferencesFragment.class.getName().equals(fragmentName) ||
                GeneralPreferencesFragment.class.getName().equals(fragmentName) ||
                LabPreferencesFragment.class.getName().equals(fragmentName) ||
                PrivacySecurityPreferencesFragment.class.getName().equals(fragmentName) ||
                WebsiteSettingsFragment.class.getName().equals(fragmentName) ||
                SearchEngineSettings.class.getName().equals(fragmentName) ||
                "com.android.browser.search.SearchEnginePreference".equals(fragmentName);

    }
}
