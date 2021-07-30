/*
 * Copyright (C) 2019 The Android Open Source Project
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
package android.telephony.ims.cts;

import static android.provider.Telephony.RcsColumns.IS_RCS_TABLE_SCHEMA_CODE_COMPLETE;

import static com.google.common.truth.Truth.assertThat;

import android.content.Context;
import android.telephony.ims.Rcs1To1Thread;
import android.telephony.ims.RcsManager;
import android.telephony.ims.RcsMessageStore;
import android.telephony.ims.RcsMessageStoreException;
import android.telephony.ims.RcsParticipant;

import androidx.test.InstrumentationRegistry;

import org.junit.AfterClass;
import org.junit.Assume;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class Rcs1To1ThreadTest {
    private RcsMessageStore mRcsMessageStore;
    private Context mContext;

    @BeforeClass
    public static void ensureDefaultSmsApp() {
        DefaultSmsAppHelper.ensureDefaultSmsApp();
    }

    @Before
    public void setupTestEnvironment() {
        // Used to skip tests for production builds without RCS tables, will be removed when
        // IS_RCS_TABLE_SCHEMA_CODE_COMPLETE flag is removed.
        Assume.assumeTrue(IS_RCS_TABLE_SCHEMA_CODE_COMPLETE);

        mContext = InstrumentationRegistry.getTargetContext();
        RcsManager rcsManager = mContext.getSystemService(RcsManager.class);
        mRcsMessageStore = rcsManager.getRcsMessageStore();

        cleanup();
    }

    @AfterClass
    public static void cleanup() {
        // TODO(b/123997749) should clean RCS message store here
    }

    @Test
    public void testRcs1To1Thread_isGroupReturnsFalse() throws RcsMessageStoreException {
        RcsParticipant participant = mRcsMessageStore.createRcsParticipant("+1234567890", "Alice");
        Rcs1To1Thread thread = mRcsMessageStore.createRcs1To1Thread(participant);

        assertThat(thread.isGroup()).isFalse();
    }

    @Test
    public void testRcs1To1Thread_fallbackThreadIdCanBeSet() throws RcsMessageStoreException {
        RcsParticipant participant = mRcsMessageStore.createRcsParticipant("+1234567890", "Alice");
        Rcs1To1Thread thread = mRcsMessageStore.createRcs1To1Thread(participant);

        thread.setFallbackThreadId(2);

        assertThat(thread.getFallbackThreadId()).isEqualTo(2);
    }
}
