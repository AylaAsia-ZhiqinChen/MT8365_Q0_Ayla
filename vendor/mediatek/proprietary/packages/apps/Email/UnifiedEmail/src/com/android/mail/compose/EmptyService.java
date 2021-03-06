/**
 * Copyright (c) 2012, Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.mail.compose;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import com.android.email.EmailApplication;
import com.android.mail.utils.LogTag;
import com.android.mail.utils.LogUtils;

/**
 * A Service that does nothing.  It is used to bump the adjustment of the BulkOperationHelper thread
 * so we don't get killed while background updates are happening.
 */
public class EmptyService extends Service
{
    private static final String TAG = LogTag.getLogTag();

    @Override
    public void onCreate() {
        // Nothing to see here...
        LogUtils.v(TAG, "onCreate()");
        EmailApplication.checkAndStartForeground(this, 14, "EmptyService");
    }

    @Override
    public IBinder onBind(Intent intent) {
        LogUtils.i(TAG, "onBind()");
        return null;
    }
    public void onDestroy() {
        LogUtils.d(TAG, "onDestroy");
        EmailApplication.checkAndStopForeground(this, "EmptyService, onDestroy");
    }
}
