/*
 * Copyright (C) 2016 The Android Open Source Project
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
 * limitations under the License
 */

package android.server.wm;

import static android.server.wm.ComponentNameUtils.getActivityName;
import static android.server.wm.app.Components.TEST_ACTIVITY;
import static android.view.Display.DEFAULT_DISPLAY;

import static androidx.test.platform.app.InstrumentationRegistry.getInstrumentation;

import static org.junit.Assert.assertEquals;
import static org.junit.Assume.assumeFalse;

import android.content.Context;
import android.content.res.Configuration;
import android.hardware.display.DisplayManager;
import android.platform.test.annotations.Presubmit;
import android.server.wm.ActivityManagerState.ActivityDisplay;
import android.util.Size;
import android.view.Display;
import androidx.test.filters.FlakyTest;

import org.junit.Test;

import java.util.List;

/**
 * Build/Install/Run:
 *     atest CtsWindowManagerDeviceTestCases:DisplayTests
 */
@Presubmit
public class DisplayTests extends MultiDisplayTestBase {

    /**
     * Tests that the global configuration is equal to the default display's override configuration.
     */
    @Test
    public void testDefaultDisplayOverrideConfiguration() throws Exception {
        final List<ActivityDisplay> reportedDisplays = getDisplaysStates();
        final ActivityDisplay primaryDisplay = getDisplayState(reportedDisplays, DEFAULT_DISPLAY);
        assertEquals("Primary display's configuration should be equal to global configuration.",
                primaryDisplay.mOverrideConfiguration, primaryDisplay.mFullConfiguration);
        assertEquals("Primary display's configuration should be equal to global configuration.",
                primaryDisplay.mOverrideConfiguration, primaryDisplay.mMergedOverrideConfiguration);
    }

    /**
     * Tests that secondary display has override configuration set.
     */
    @Test
    public void testCreateVirtualDisplayWithCustomConfig() throws Exception {
        try (final VirtualDisplaySession virtualDisplaySession = new VirtualDisplaySession()) {
            final ActivityDisplay newDisplay = virtualDisplaySession.createDisplay();

            // Find the density of created display.
            final int newDensityDpi = newDisplay.mFullConfiguration.densityDpi;
            assertEquals(CUSTOM_DENSITY_DPI, newDensityDpi);
        }
    }

    @Test
    @FlakyTest(bugId = 129521230)
    public void testNonDefaultDisplayResourcesConfiguration() throws Exception {
        final int smallDisplaySize = 1000;
        final int longDisplaySize = 1920;

        // Get land-sized display's resources configuration.
        final int smallestScreenWidthForLandDisplay = getDisplayResourcesConfiguration(
                longDisplaySize, smallDisplaySize).smallestScreenWidthDp;

        // Get port-sized display's resources configuration.
        final int smallestScreenWidthForPortDisplay = getDisplayResourcesConfiguration(
                smallDisplaySize, longDisplaySize).smallestScreenWidthDp;

        // Check whether smallestScreenWidthDp configuration is proper.
        assertEquals("smallestScreenWidthDp configuration should be set to smallest possible size.",
                smallestScreenWidthForLandDisplay, smallestScreenWidthForPortDisplay);
    }

    /**
     * Tests that launch on secondary display is not permitted if device has the feature disabled.
     * Activities requested to be launched on a secondary display in this case should land on the
     * default display.
     */
    @Test
    public void testMultiDisplayDisabled() throws Exception {
        // Only check devices with the feature disabled.
        assumeFalse(supportsMultiDisplay());

        try (final VirtualDisplaySession virtualDisplaySession = new VirtualDisplaySession()) {
            // Create new virtual display.
            final ActivityDisplay newDisplay = virtualDisplaySession.createDisplay();

            // Launch activity on new secondary display.
            launchActivityOnDisplay(TEST_ACTIVITY, newDisplay.mId);
            mAmWmState.computeState(TEST_ACTIVITY);

            mAmWmState.assertFocusedActivity("Launched activity must be focused",
                    TEST_ACTIVITY);

            // Check that activity is on the right display.
            final int frontStackId = mAmWmState.getAmState().getFrontStackId(DEFAULT_DISPLAY);
            final ActivityManagerState.ActivityStack frontStack =
                    mAmWmState.getAmState().getStackById(frontStackId);
            assertEquals("Launched activity must be resumed",
                    getActivityName(TEST_ACTIVITY), frontStack.mResumedActivity);
            assertEquals("Front stack must be on the default display",
                    DEFAULT_DISPLAY, frontStack.mDisplayId);
            mAmWmState.assertFocusedStack("Focus must be on the default display", frontStackId);
        }
    }

    @Test
    public void testCreateMultipleVirtualDisplays() throws Exception {
        final List<ActivityDisplay> originalDs = getDisplaysStates();
        try (final VirtualDisplaySession virtualDisplaySession = new VirtualDisplaySession()) {
            // Create new virtual displays
            virtualDisplaySession.createDisplays(3);
            getDisplayStateAfterChange(originalDs.size() + 3);
        }
        getDisplayStateAfterChange(originalDs.size());
    }

    /**
     * Test that display overrides apply correctly and won't be affected by display changes.
     * This sets overrides to display size and density, initiates a display changed event by locking
     * and unlocking the phone and verifies that overrides are kept.
     */
    @Test
    public void testForceDisplayMetrics() throws Exception {
        launchHomeActivity();

        try (final DisplayMetricsSession displayMetricsSession =
                     new DisplayMetricsSession(DEFAULT_DISPLAY);
             final LockScreenSession lockScreenSession = new LockScreenSession()) {
            // Read initial sizes.
            final ReportedDisplayMetrics originalDisplayMetrics =
                    displayMetricsSession.getInitialDisplayMetrics();

            // Apply new override values that don't match the physical metrics.
            final Size overrideSize = new Size(
                    (int) (originalDisplayMetrics.physicalSize.getWidth() * 1.5),
                    (int) (originalDisplayMetrics.physicalSize.getHeight() * 1.5));
            final Integer overrideDensity = (int) (originalDisplayMetrics.physicalDensity * 1.1);
            displayMetricsSession.overrideDisplayMetrics(overrideSize, overrideDensity);

            // Check if overrides applied correctly.
            ReportedDisplayMetrics displayMetrics = displayMetricsSession.getDisplayMetrics();
            assertEquals(overrideSize, displayMetrics.overrideSize);
            assertEquals(overrideDensity, displayMetrics.overrideDensity);

            // Lock and unlock device. This will cause a DISPLAY_CHANGED event to be triggered and
            // might update the metrics.
            lockScreenSession.sleepDevice()
                    .wakeUpDevice()
                    .unlockDevice();
            mAmWmState.waitForHomeActivityVisible();

            // Check if overrides are still applied.
            displayMetrics = displayMetricsSession.getDisplayMetrics();
            assertEquals(overrideSize, displayMetrics.overrideSize);
            assertEquals(overrideDensity, displayMetrics.overrideDensity);
        }
    }

    private Configuration getDisplayResourcesConfiguration(int displayWidth, int displayHeight)
            throws Exception  {
        final Context context = getInstrumentation().getContext();
        final DisplayManager displayManager =
                (DisplayManager) context.getSystemService(Context.DISPLAY_SERVICE);

        try (final VirtualDisplaySession virtualDisplaySession = new VirtualDisplaySession()) {
            final ActivityDisplay activityDisplay = virtualDisplaySession
                    .setSimulateDisplay(true)
                    .setSimulationDisplaySize(displayWidth, displayHeight)
                    .createDisplay();
            final Display display = displayManager.getDisplay(activityDisplay.mId);
            Configuration config = context.createDisplayContext(display)
                    .getResources().getConfiguration();
            return config;
        }
    }

    private static class DisplayMetricsSession implements AutoCloseable {

        private final ReportedDisplayMetrics mInitialDisplayMetrics;
        private final int mDisplayId;

        DisplayMetricsSession(int displayId) throws Exception {
            mDisplayId = displayId;
            mInitialDisplayMetrics = ReportedDisplayMetrics.getDisplayMetrics(mDisplayId);
        }

        ReportedDisplayMetrics getInitialDisplayMetrics() {
            return mInitialDisplayMetrics;
        }

        ReportedDisplayMetrics getDisplayMetrics() throws Exception {
            return ReportedDisplayMetrics.getDisplayMetrics(mDisplayId);
        }

        void overrideDisplayMetrics(final Size size, final int density) {
            mInitialDisplayMetrics.setDisplayMetrics(size, density);
        }

        @Override
        public void close() throws Exception {
            mInitialDisplayMetrics.restoreDisplayMetrics();
        }
    }
}
