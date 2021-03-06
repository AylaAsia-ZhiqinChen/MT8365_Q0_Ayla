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

package android.app.stubs;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.test.AndroidTestCase;
import android.test.PerformanceTestCase;

public class ActivityTestsBase extends AndroidTestCase implements PerformanceTestCase,
        LaunchpadActivity.CallingTest {
    public static final String PERMISSION_GRANTED = "android.app.cts.permission.TEST_GRANTED";
    public static final String PERMISSION_DENIED = "android.app.cts.permission.TEST_DENIED";

    private static final int TIMEOUT_MS = 60 * 1000;

    protected Intent mIntent;

    private PerformanceTestCase.Intermediates mIntermediates;
    private String mExpecting;

    // Synchronization of activity result.
    private boolean mFinished;
    private int mResultCode = 0;
    private Intent mData;
    private Activity mActivity;
    private RuntimeException mResultStack = null;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mIntent = new Intent(mContext, LaunchpadActivity.class);
        mIntermediates = null;
    }

    @Override
    protected void tearDown() throws Exception {
        mIntermediates = null;
        super.tearDown();
    }

    public boolean isPerformanceOnly() {
        return false;
    }

    public void setInternalIterations(int count) {
    }

    public void startTiming(boolean realTime) {
        if (mIntermediates != null) {
            mIntermediates.startTiming(realTime);
        }
    }

    public void addIntermediate(String name) {
        if (mIntermediates != null) {
            mIntermediates.addIntermediate(name);
        }
    }

    public void addIntermediate(String name, long timeInNS) {
        if (mIntermediates != null) {
            mIntermediates.addIntermediate(name, timeInNS);
        }
    }

    public void finishTiming(boolean realTime) {
        if (mIntermediates != null) {
            mIntermediates.finishTiming(realTime);
        }
    }

    public void activityRunning(Activity activity) {
        finishWithActivity(activity);
    }

    public void activityFinished(int resultCode, Intent data, RuntimeException where) {
        finishWithResult(resultCode, data, null, where);
    }

    public Intent editIntent() {
        return mIntent;
    }

    @Override
    public Context getContext() {
        return mContext;
    }

    public int startPerformance(Intermediates intermediates) {
        mIntermediates = intermediates;
        return 1;
    }

    public void finishGood() {
        finishWithResult(Activity.RESULT_OK, null);
    }

    public void finishBad(String error) {
        finishWithResult(Activity.RESULT_CANCELED, new Intent().setAction(error));
    }

    public void finishWithActivity(Activity activity) {
        final RuntimeException where = new RuntimeException("Original error was here");
        where.fillInStackTrace();
        finishWithResult(Activity.RESULT_OK, null, activity, where);

    }

    public void finishWithResult(int resultCode, Intent data) {
        final RuntimeException where = new RuntimeException("Original error was here");
        where.fillInStackTrace();
        finishWithResult(resultCode, data, null, where);
    }

    public void finishWithResult(int resultCode, Intent data, Activity activity,
            RuntimeException where) {
        synchronized (this) {
            mResultCode = resultCode;
            mData = data;
            mActivity = activity;
            mResultStack = where;
            mFinished = true;
            notifyAll();
        }
    }

    public int runLaunchpad(String action) {
        startLaunchpadActivity(action);
        return waitForResultOrThrow(TIMEOUT_MS);
    }

    private void startLaunchpadActivity(String action) {
        LaunchpadActivity.setCallingTest(this);

        synchronized (this) {
            mIntent.setAction(action);
            mFinished = false;
            mIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            mContext.startActivity(mIntent);
        }
    }

    public int waitForResultOrThrow(int timeoutMs) {
        return waitForResultOrThrow(timeoutMs, null);
    }

    public int waitForResultOrThrow(int timeoutMs, String expected) {
        final int res = waitForResult(timeoutMs, expected);

        if (res == Activity.RESULT_CANCELED) {
            if (mResultStack != null) {
                throw new RuntimeException(mData != null ? mData.toString() : "Unable to launch",
                        mResultStack);
            } else {
                throw new RuntimeException(mData != null ? mData.toString() : "Unable to launch");
            }
        }
        return res;
    }

    public int waitForResult(int timeoutMs, String expected) {
        mExpecting = expected;

        final long endTime = System.currentTimeMillis() + timeoutMs;

        boolean timeout = false;
        synchronized (this) {
            while (!mFinished) {
                final long delay = endTime - System.currentTimeMillis();
                if (delay < 0) {
                    timeout = true;
                    break;
                }

                try {
                    wait(delay);
                } catch (final java.lang.InterruptedException e) {
                    // do nothing
                }
            }
        }

        mFinished = false;

        if (timeout) {
            mResultCode = Activity.RESULT_CANCELED;
            onTimeout();
        }
        return mResultCode;
    }


    public int getResultCode() {
        return mResultCode;
    }

    public Intent getResultData() {
        return mData;
    }

    public RuntimeException getResultStack() {
        return mResultStack;
    }

    public Activity getRunningActivity() {
        return mActivity;
    }

    public void onTimeout() {
        final String msg = mExpecting == null ? "Timeout" : "Timeout while expecting " + mExpecting;
        finishWithResult(Activity.RESULT_CANCELED, new Intent().setAction(msg));
    }
}
