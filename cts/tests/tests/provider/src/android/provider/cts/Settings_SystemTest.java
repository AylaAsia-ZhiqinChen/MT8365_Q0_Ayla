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

package android.provider.cts;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import android.content.ContentResolver;
import android.content.res.Configuration;
import android.database.Cursor;
import android.net.Uri;
import android.os.SystemClock;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.provider.Settings.System;

import androidx.test.InstrumentationRegistry;
import androidx.test.runner.AndroidJUnit4;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class Settings_SystemTest {
    private static final String INT_FIELD = Settings.System.SCREEN_BRIGHTNESS;
    private static final String LONG_FIELD = Settings.System.SCREEN_OFF_TIMEOUT;
    private static final String FLOAT_FIELD = Settings.System.FONT_SCALE;
    private static final String STRING_FIELD = Settings.System.NEXT_ALARM_FORMATTED;

    @BeforeClass
    public static void setUp() throws Exception {
        final String packageName = InstrumentationRegistry.getTargetContext().getPackageName();
        InstrumentationRegistry.getInstrumentation().getUiAutomation().executeShellCommand(
                "appops set " + packageName + " android:write_settings allow");

        // Wait a beat to persist the change
        SystemClock.sleep(500);
    }

    @AfterClass
    public static void tearDown() throws Exception {
        final String packageName = InstrumentationRegistry.getTargetContext().getPackageName();
        InstrumentationRegistry.getInstrumentation().getUiAutomation().executeShellCommand(
                "appops set " + packageName + " android:write_settings default");
    }

    @Test
    public void testSystemSettings() throws SettingNotFoundException {
        final ContentResolver cr = InstrumentationRegistry.getTargetContext().getContentResolver();

        /**
         * first query the exist settings in System table, and then insert five
         * rows: an int, a long, a float, a String, and a ShowGTalkServiceStatus.
         * Get these six rows to check whether insert succeeded and then delete them.
         */

        // first query exist rows
        Cursor c = cr.query(System.CONTENT_URI, null, null, null, null);

        // backup fontScale
        Configuration cfg = new Configuration();
        System.getConfiguration(cr, cfg);
        float store = cfg.fontScale;

        try {
            assertNotNull(c);
            c.close();

            String stringValue = "cts";

            // insert 4 rows, and update 1 rows
            assertTrue(System.putInt(cr, INT_FIELD, 10));
            assertTrue(System.putLong(cr, LONG_FIELD, 20l));
            assertTrue(System.putFloat(cr, FLOAT_FIELD, 30.0f));
            assertTrue(System.putString(cr, STRING_FIELD, stringValue));

            c = cr.query(System.CONTENT_URI, null, null, null, null);
            assertNotNull(c);
            c.close();

            // get these rows to assert
            assertEquals(10, System.getInt(cr, INT_FIELD));
            assertEquals(20l, System.getLong(cr, LONG_FIELD));
            assertEquals(30.0f, System.getFloat(cr, FLOAT_FIELD), 0.001);

            assertEquals(stringValue, System.getString(cr, STRING_FIELD));

            c = cr.query(System.CONTENT_URI, null, null, null, null);
            assertNotNull(c);

            // update fontScale row
            cfg = new Configuration();
            cfg.fontScale = 1.2f;
            assertTrue(System.putConfiguration(cr, cfg));

            System.getConfiguration(cr, cfg);
            assertEquals(1.2f, cfg.fontScale, 0.001);
        } finally {
            // TODO should clean up more better
            c.close();

            // restore the fontScale
            try {
                // Delay helps ActivityManager in completing its previous font-change processing.
                Thread.sleep(1000);
            } catch (Exception e){}

            cfg.fontScale = store;
            assertTrue(System.putConfiguration(cr, cfg));
        }
    }

    @Test
    public void testGetDefaultValues() {
        final ContentResolver cr = InstrumentationRegistry.getTargetContext().getContentResolver();

        assertEquals(10, System.getInt(cr, "int", 10));
        assertEquals(20, System.getLong(cr, "long", 20l));
        assertEquals(30.0f, System.getFloat(cr, "float", 30.0f), 0.001);
    }

    @Test
    public void testGetUriFor() {
        String name = "table";

        Uri uri = System.getUriFor(name);
        assertNotNull(uri);
        assertEquals(Uri.withAppendedPath(System.CONTENT_URI, name), uri);
    }
}
