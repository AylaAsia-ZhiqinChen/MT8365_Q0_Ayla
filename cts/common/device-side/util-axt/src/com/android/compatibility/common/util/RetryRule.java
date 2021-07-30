/*
 * Copyright (C) 2017 The Android Open Source Project
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

package com.android.compatibility.common.util;

import android.util.Log;

import org.junit.rules.TestRule;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

/**
 * Custom JUnit4 rule that retry tests when they fail due to a {@link RetryableException}.
 */
public class RetryRule implements TestRule {

    private static final String TAG = "RetryRule";
    private final int mMaxAttempts;

    /**
     * Retries the underlying test when it catches a {@link RetryableException}.
     *
     * @param retries number of retries. Use {@code 0} to disable rule.
     *
     * @throws IllegalArgumentException if {@code retries} is less than {@code 0}.
     */
    public RetryRule(int retries) {
        if (retries < 0) {
            throw new IllegalArgumentException("retries must be more than 0");
        }
        mMaxAttempts = retries + 1;
    }

    @Override
    public Statement apply(Statement base, Description description) {
        return new Statement() {

            @Override
            public void evaluate() throws Throwable {
                if (mMaxAttempts <= 1) {
                    Log.v(TAG, "Executing " + description.getDisplayName()
                            + " right away because mMaxAttempts is " + mMaxAttempts);
                    base.evaluate();
                    return;
                }

                final String name = description.getDisplayName();
                Throwable caught = null;
                for (int i = 1; i <= mMaxAttempts; i++) {
                    try {
                        base.evaluate();
                        if (i == 1) {
                            Log.v(TAG, "Good News, Everyone! " + name + " passed right away");
                        } else {
                            Log.d(TAG,
                                    "Better late than never: " + name + " passed at attempt #" + i);
                        }
                        return;
                    } catch (RetryableException e) {
                        final Timeout timeout = e.getTimeout();
                        if (timeout != null) {
                            long before = timeout.ms();
                            timeout.increase();
                            Log.d(TAG, "Increased " + timeout.getName() + " from " + before + "ms"
                                    + " to " + timeout.ms() + "ms");
                        }
                        caught = e;
                    }
                    Log.w(TAG, "Arrrr! " + name + " failed at attempt " + i + "/" + mMaxAttempts
                            + ": " + caught);
                }
                Log.e(TAG, "D'OH! " + name + ": giving up after " + mMaxAttempts + " attempts");
                throw caught;
            }
        };
    }
}
