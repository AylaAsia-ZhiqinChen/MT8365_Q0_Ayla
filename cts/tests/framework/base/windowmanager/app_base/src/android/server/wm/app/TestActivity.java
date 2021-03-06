/*
 * Copyright (C) 2015 The Android Open Source Project
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

package android.server.wm.app;

import static android.server.wm.app.Components.TestActivity.EXTRA_CONFIG_ASSETS_SEQ;
import static android.server.wm.app.Components.TestActivity.EXTRA_FIXED_ORIENTATION;
import static android.server.wm.app.Components.TestActivity.TEST_ACTIVITY_ACTION_FINISH_SELF;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;

public class TestActivity extends AbstractLifecycleLogActivity {

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent != null && TEST_ACTIVITY_ACTION_FINISH_SELF.equals(intent.getAction())) {
                finish();
            }
        }
    };

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        // Set the fixed orientation if requested
        if (getIntent().hasExtra(EXTRA_FIXED_ORIENTATION)) {
            final int ori = Integer.parseInt(getIntent().getStringExtra(EXTRA_FIXED_ORIENTATION));
            setRequestedOrientation(ori);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        registerReceiver(mReceiver, new IntentFilter(TEST_ACTIVITY_ACTION_FINISH_SELF));
    }

    @Override
    protected void onResume() {
        super.onResume();
        final Configuration configuration = getResources().getConfiguration();
        dumpConfiguration(configuration);
        dumpAssetSeqNumber(configuration);
        dumpConfigInfo();
    }

    @Override
    protected void onStop() {
        super.onStop();
        unregisterReceiver(mReceiver);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        dumpConfiguration(newConfig);
        dumpAssetSeqNumber(newConfig);
        dumpConfigInfo();
    }

    private void dumpAssetSeqNumber(Configuration newConfig) {
        withTestJournalClient(client -> {
            final Bundle extras = new Bundle();
            extras.putInt(EXTRA_CONFIG_ASSETS_SEQ, newConfig.assetsSeq);
            client.putExtras(extras);
        });
    }
}
