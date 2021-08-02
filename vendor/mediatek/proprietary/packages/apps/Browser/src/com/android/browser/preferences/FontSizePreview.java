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

package com.android.browser.preferences;

import android.content.Context;
import android.content.res.Resources;
import android.util.AttributeSet;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;

import com.android.browser.BrowserSettings;
import com.android.browser.Extensions;
import com.android.browser.R;

/// M: Add for Browser setting plugin. @{
import com.mediatek.browser.ext.IBrowserSettingExt;
/// @}

public class FontSizePreview extends WebViewPreview {

    static final String HTML_FORMAT = "<!DOCTYPE html><html><head>" +
        "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">" +
        "<style type=\"text/css\">p { margin: 2px auto;}</style><body>" +
        "<p style=\"font-size: 4pt\">%s</p><p style=\"font-size: 8pt\">%s</p>" +
        "<p style=\"font-size: 10pt\">%s</p><p style=\"font-size: 14pt\">%s</p>" +
        "<p style=\"font-size: 18pt\">%s</p></body></html>";
    /// M: Add for Browser setting plugin
    private Context mContext = null;

    String mHtml;

    public FontSizePreview(
            Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public FontSizePreview(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public FontSizePreview(Context context) {
        super(context);
    }

    @Override
    protected void init(Context context) {
        super.init(context);
        /// M: Add for Browser setting plugin
        mContext = context;
        Resources res = context.getResources();
        Object[] visualNames = res.getStringArray(R.array.pref_text_size_choices);
        mHtml = String.format(HTML_FORMAT, visualNames);
    }

    @Override
    protected void updatePreview(boolean forceReload) {
        if (mWebView == null) return;

        WebSettings ws = mWebView.getSettings();
        BrowserSettings bs = BrowserSettings.getInstance();
        ws.setMinimumFontSize(bs.getMinimumFontSize());
        ws.setTextZoom(bs.getTextZoom());
        /// M: Set standard font family to the preview. @{
        IBrowserSettingExt browserSettingExt = Extensions.getSettingPlugin(mContext);
        browserSettingExt.setStandardFontFamily(ws, bs.getPreferences());
        /// @}
        mWebView.loadDataWithBaseURL(null, mHtml, "text/html", "utf-8", null);
    }

    @Override
    protected void setupWebView(WebView view) {
        super.setupWebView(view);
        view.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
    }

}
