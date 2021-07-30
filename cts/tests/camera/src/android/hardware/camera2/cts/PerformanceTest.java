/*
 * Copyright (C) 2014 The Android Open Source Project
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

package android.hardware.camera2.cts;

import static com.android.ex.camera2.blocking.BlockingSessionCallback.SESSION_CLOSED;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import android.app.Instrumentation;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCaptureSession.CaptureCallback;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.cts.CameraTestUtils.SimpleCaptureCallback;
import android.hardware.camera2.cts.CameraTestUtils.SimpleImageReaderListener;
import android.hardware.camera2.cts.helpers.StaticMetadata;
import android.hardware.camera2.cts.helpers.StaticMetadata.CheckLevel;
import android.hardware.camera2.cts.testcases.Camera2AndroidTestCase;
import android.hardware.camera2.params.InputConfiguration;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.media.ImageWriter;
import android.os.ConditionVariable;
import android.os.SystemClock;
import android.util.Log;
import android.util.Pair;
import android.util.Range;
import android.util.Size;
import android.view.Surface;

import androidx.test.InstrumentationRegistry;

import com.android.compatibility.common.util.DeviceReportLog;
import com.android.compatibility.common.util.ResultType;
import com.android.compatibility.common.util.ResultUnit;
import com.android.compatibility.common.util.Stat;
import com.android.ex.camera2.blocking.BlockingSessionCallback;
import com.android.ex.camera2.exceptions.TimeoutRuntimeException;

import org.junit.Test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

/**
 * Test camera2 API use case performance KPIs, such as camera open time, session creation time,
 * shutter lag etc. The KPI data will be reported in cts results.
 */
public class PerformanceTest extends Camera2AndroidTestCase {
    private static final String TAG = "PerformanceTest";
    private static final String REPORT_LOG_NAME = "CtsCameraTestCases";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private static final int NUM_TEST_LOOPS = 10;
    private static final int NUM_MAX_IMAGES = 4;
    private static final int NUM_RESULTS_WAIT = 30;
    private static final int[] REPROCESS_FORMATS = {ImageFormat.YUV_420_888, ImageFormat.PRIVATE};
    private final int MAX_REPROCESS_IMAGES = 6;
    private final int MAX_JPEG_IMAGES = MAX_REPROCESS_IMAGES;
    private final int MAX_INPUT_IMAGES = MAX_REPROCESS_IMAGES;
    // ZSL queue depth should be bigger than the max simultaneous reprocessing capture request
    // count to maintain reasonable number of candidate image for the worse-case.
    private final int MAX_ZSL_IMAGES = MAX_REPROCESS_IMAGES * 3 / 2;
    private final double REPROCESS_STALL_MARGIN = 0.1;
    private static final int WAIT_FOR_RESULT_TIMEOUT_MS = 3000;
    private static final int NUM_RESULTS_WAIT_TIMEOUT = 100;
    private static final int NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY = 8;

    private DeviceReportLog mReportLog;

    // Used for reading camera output buffers.
    private ImageReader mCameraZslReader;
    private SimpleImageReaderListener mCameraZslImageListener;
    // Used for reprocessing (jpeg) output.
    private ImageReader mJpegReader;
    private SimpleImageReaderListener mJpegListener;
    // Used for reprocessing input.
    private ImageWriter mWriter;
    private SimpleCaptureCallback mZslResultListener;

    private Instrumentation mInstrumentation;

    private Surface mPreviewSurface;
    private SurfaceTexture mPreviewSurfaceTexture;

    @Override
    public void setUp() throws Exception {
        super.setUp();
        mInstrumentation = InstrumentationRegistry.getInstrumentation();
    }

    @Override
    public void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * Test camera launch KPI: the time duration between a camera device is
     * being opened and first preview frame is available.
     * <p>
     * It includes camera open time, session creation time, and sending first
     * preview request processing latency etc. For the SurfaceView based preview use
     * case, there is no way for client to know the exact preview frame
     * arrival time. To approximate this time, a companion YUV420_888 stream is
     * created. The first YUV420_888 Image coming out of the ImageReader is treated
     * as the first preview arrival time.</p>
     * <p>
     * For depth-only devices, timing is done with the DEPTH16 format instead.
     * </p>
     */
    public void testCameraLaunch() throws Exception {
        double[] avgCameraLaunchTimes = new double[mCameraIds.length];

        int counter = 0;
        for (String id : mCameraIds) {
            // Do NOT move these variables to outer scope
            // They will be passed to DeviceReportLog and their references will be stored
            String streamName = "test_camera_launch";
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
            double[] cameraOpenTimes = new double[NUM_TEST_LOOPS];
            double[] configureStreamTimes = new double[NUM_TEST_LOOPS];
            double[] startPreviewTimes = new double[NUM_TEST_LOOPS];
            double[] stopPreviewTimes = new double[NUM_TEST_LOOPS];
            double[] cameraCloseTimes = new double[NUM_TEST_LOOPS];
            double[] cameraLaunchTimes = new double[NUM_TEST_LOOPS];
            try {
                mStaticInfo = new StaticMetadata(mCameraManager.getCameraCharacteristics(id));
                if (mStaticInfo.isColorOutputSupported()) {
                    initializeImageReader(id, ImageFormat.YUV_420_888);
                } else {
                    assertTrue("Depth output must be supported if regular output isn't!",
                            mStaticInfo.isDepthOutputSupported());
                    initializeImageReader(id, ImageFormat.DEPTH16);
                }

                SimpleImageListener imageListener = null;
                long startTimeMs, openTimeMs, configureTimeMs, previewStartedTimeMs;
                for (int i = 0; i < NUM_TEST_LOOPS; i++) {
                    try {
                        // Need create a new listener every iteration to be able to wait
                        // for the first image comes out.
                        imageListener = new SimpleImageListener();
                        mReader.setOnImageAvailableListener(imageListener, mHandler);
                        startTimeMs = SystemClock.elapsedRealtime();

                        // Blocking open camera
                        simpleOpenCamera(id);
                        openTimeMs = SystemClock.elapsedRealtime();
                        cameraOpenTimes[i] = openTimeMs - startTimeMs;

                        // Blocking configure outputs.
                        configureReaderAndPreviewOutputs();
                        configureTimeMs = SystemClock.elapsedRealtime();
                        configureStreamTimes[i] = configureTimeMs - openTimeMs;

                        // Blocking start preview (start preview to first image arrives)
                        SimpleCaptureCallback resultListener =
                                new SimpleCaptureCallback();
                        blockingStartPreview(resultListener, imageListener);
                        previewStartedTimeMs = SystemClock.elapsedRealtime();
                        startPreviewTimes[i] = previewStartedTimeMs - configureTimeMs;
                        cameraLaunchTimes[i] = previewStartedTimeMs - startTimeMs;

                        // Let preview on for a couple of frames
                        CameraTestUtils.waitForNumResults(resultListener, NUM_RESULTS_WAIT,
                                WAIT_FOR_RESULT_TIMEOUT_MS);

                        // Blocking stop preview
                        startTimeMs = SystemClock.elapsedRealtime();
                        blockingStopPreview();
                        stopPreviewTimes[i] = SystemClock.elapsedRealtime() - startTimeMs;
                    }
                    finally {
                        // Blocking camera close
                        startTimeMs = SystemClock.elapsedRealtime();
                        closeDevice(id);
                        cameraCloseTimes[i] = SystemClock.elapsedRealtime() - startTimeMs;
                    }
                }

                avgCameraLaunchTimes[counter] = Stat.getAverage(cameraLaunchTimes);
                // Finish the data collection, report the KPIs.
                // ReportLog keys have to be lowercase underscored format.
                mReportLog.addValues("camera_open_time", cameraOpenTimes, ResultType.LOWER_BETTER,
                        ResultUnit.MS);
                mReportLog.addValues("camera_configure_stream_time", configureStreamTimes,
                        ResultType.LOWER_BETTER, ResultUnit.MS);
                mReportLog.addValues("camera_start_preview_time", startPreviewTimes,
                        ResultType.LOWER_BETTER, ResultUnit.MS);
                mReportLog.addValues("camera_camera_stop_preview", stopPreviewTimes,
                        ResultType.LOWER_BETTER, ResultUnit.MS);
                mReportLog.addValues("camera_camera_close_time", cameraCloseTimes,
                        ResultType.LOWER_BETTER, ResultUnit.MS);
                mReportLog.addValues("camera_launch_time", cameraLaunchTimes,
                        ResultType.LOWER_BETTER, ResultUnit.MS);
            }
            finally {
                closeDefaultImageReader();
                closePreviewSurface();
            }
            counter++;
            mReportLog.submit(mInstrumentation);

            if (VERBOSE) {
                Log.v(TAG, "Camera " + id + " device open times(ms): "
                        + Arrays.toString(cameraOpenTimes)
                        + ". Average(ms): " + Stat.getAverage(cameraOpenTimes)
                        + ". Min(ms): " + Stat.getMin(cameraOpenTimes)
                        + ". Max(ms): " + Stat.getMax(cameraOpenTimes));
                Log.v(TAG, "Camera " + id + " configure stream times(ms): "
                        + Arrays.toString(configureStreamTimes)
                        + ". Average(ms): " + Stat.getAverage(configureStreamTimes)
                        + ". Min(ms): " + Stat.getMin(configureStreamTimes)
                        + ". Max(ms): " + Stat.getMax(configureStreamTimes));
                Log.v(TAG, "Camera " + id + " start preview times(ms): "
                        + Arrays.toString(startPreviewTimes)
                        + ". Average(ms): " + Stat.getAverage(startPreviewTimes)
                        + ". Min(ms): " + Stat.getMin(startPreviewTimes)
                        + ". Max(ms): " + Stat.getMax(startPreviewTimes));
                Log.v(TAG, "Camera " + id + " stop preview times(ms): "
                        + Arrays.toString(stopPreviewTimes)
                        + ". Average(ms): " + Stat.getAverage(stopPreviewTimes)
                        + ". nMin(ms): " + Stat.getMin(stopPreviewTimes)
                        + ". nMax(ms): " + Stat.getMax(stopPreviewTimes));
                Log.v(TAG, "Camera " + id + " device close times(ms): "
                        + Arrays.toString(cameraCloseTimes)
                        + ". Average(ms): " + Stat.getAverage(cameraCloseTimes)
                        + ". Min(ms): " + Stat.getMin(cameraCloseTimes)
                        + ". Max(ms): " + Stat.getMax(cameraCloseTimes));
                Log.v(TAG, "Camera " + id + " camera launch times(ms): "
                        + Arrays.toString(cameraLaunchTimes)
                        + ". Average(ms): " + Stat.getAverage(cameraLaunchTimes)
                        + ". Min(ms): " + Stat.getMin(cameraLaunchTimes)
                        + ". Max(ms): " + Stat.getMax(cameraLaunchTimes));
            }
        }
        if (mCameraIds.length != 0) {
            String streamName = "test_camera_launch_average";
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            mReportLog.setSummary("camera_launch_average_time_for_all_cameras",
                    Stat.getAverage(avgCameraLaunchTimes), ResultType.LOWER_BETTER, ResultUnit.MS);
            mReportLog.submit(mInstrumentation);
        }
    }

    /**
     * Test camera capture KPI for YUV_420_888, PRIVATE, JPEG, RAW and RAW+JPEG
     * formats: the time duration between sending out a single image capture request
     * and receiving image data and capture result.
     * <p>
     * It enumerates the following metrics: capture latency, computed by
     * measuring the time between sending out the capture request and getting
     * the image data; partial result latency, computed by measuring the time
     * between sending out the capture request and getting the partial result;
     * capture result latency, computed by measuring the time between sending
     * out the capture request and getting the full capture result.
     * </p>
     */
    public void testSingleCapture() throws Exception {
        int[] YUV_FORMAT = {ImageFormat.YUV_420_888};
        testSingleCaptureForFormat(YUV_FORMAT, null, /*addPreviewDelay*/ false);
        int[] PRIVATE_FORMAT = {ImageFormat.PRIVATE};
        testSingleCaptureForFormat(PRIVATE_FORMAT, "private", /*addPreviewDelay*/ true);
        int[] JPEG_FORMAT = {ImageFormat.JPEG};
        testSingleCaptureForFormat(JPEG_FORMAT, "jpeg", /*addPreviewDelay*/ true);
        int[] RAW_FORMAT = {ImageFormat.RAW_SENSOR};
        testSingleCaptureForFormat(RAW_FORMAT, "raw", /*addPreviewDelay*/ true);
        int[] RAW_JPEG_FORMATS = {ImageFormat.RAW_SENSOR, ImageFormat.JPEG};
        testSingleCaptureForFormat(RAW_JPEG_FORMATS, "raw_jpeg", /*addPreviewDelay*/ true);
    }

    private String appendFormatDescription(String message, String formatDescription) {
        if (message == null) {
            return null;
        }

        String ret = message;
        if (formatDescription != null) {
            ret = String.format(ret + "_%s", formatDescription);
        }

        return ret;
    }

    private void testSingleCaptureForFormat(int[] formats, String formatDescription,
            boolean addPreviewDelay) throws Exception {
        double[] avgResultTimes = new double[mCameraIds.length];

        int counter = 0;
        for (String id : mCameraIds) {
            // Do NOT move these variables to outer scope
            // They will be passed to DeviceReportLog and their references will be stored
            String streamName = appendFormatDescription("test_single_capture", formatDescription);
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
            double[] captureTimes = new double[NUM_TEST_LOOPS];
            double[] getPartialTimes = new double[NUM_TEST_LOOPS];
            double[] getResultTimes = new double[NUM_TEST_LOOPS];
            ImageReader[] readers = null;
            try {
                if (!mAllStaticInfo.get(id).isColorOutputSupported()) {
                    Log.i(TAG, "Camera " + id + " does not support color outputs, skipping");
                    continue;
                }

                StreamConfigurationMap configMap = mAllStaticInfo.get(id).getCharacteristics().get(
                        CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                boolean formatsSupported = true;
                for (int format : formats) {
                    if (!configMap.isOutputSupportedFor(format)) {
                        Log.i(TAG, "Camera " + id + " does not support output format: " + format +
                                " skipping");
                        formatsSupported = false;
                        break;
                    }
                }
                if (!formatsSupported) {
                    continue;
                }

                openDevice(id);

                boolean partialsExpected = mStaticInfo.getPartialResultCount() > 1;
                long startTimeMs;
                boolean isPartialTimingValid = partialsExpected;
                for (int i = 0; i < NUM_TEST_LOOPS; i++) {

                    // setup builders and listeners
                    CaptureRequest.Builder previewBuilder =
                            mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                    CaptureRequest.Builder captureBuilder =
                            mCamera.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
                    SimpleCaptureCallback previewResultListener =
                            new SimpleCaptureCallback();
                    SimpleTimingResultListener captureResultListener =
                            new SimpleTimingResultListener();
                    SimpleImageListener[] imageListeners = new SimpleImageListener[formats.length];
                    Size[] imageSizes = new Size[formats.length];
                    for (int j = 0; j < formats.length; j++) {
                        imageSizes[j] = CameraTestUtils.getSortedSizesForFormat(
                                id, mCameraManager, formats[j], /*bound*/null).get(0);
                        imageListeners[j] = new SimpleImageListener();
                    }

                    readers = prepareStillCaptureAndStartPreview(previewBuilder, captureBuilder,
                            mOrderedPreviewSizes.get(0), imageSizes, formats,
                            previewResultListener, NUM_MAX_IMAGES, imageListeners,
                            false /*isHeic*/);

                    if (addPreviewDelay) {
                        Thread.sleep(500);
                    }

                    // Capture an image and get image data
                    startTimeMs = SystemClock.elapsedRealtime();
                    CaptureRequest request = captureBuilder.build();
                    mCameraSession.capture(request, captureResultListener, mHandler);

                    Pair<CaptureResult, Long> partialResultNTime = null;
                    if (partialsExpected) {
                        partialResultNTime = captureResultListener.getPartialResultNTimeForRequest(
                            request, NUM_RESULTS_WAIT);
                        // Even if maxPartials > 1, may not see partials for some devices
                        if (partialResultNTime == null) {
                            partialsExpected = false;
                            isPartialTimingValid = false;
                        }
                    }
                    Pair<CaptureResult, Long> captureResultNTime =
                            captureResultListener.getCaptureResultNTimeForRequest(
                                    request, NUM_RESULTS_WAIT);

                    double [] imageTimes = new double[formats.length];
                    for (int j = 0; j < formats.length; j++) {
                        imageListeners[j].waitForImageAvailable(
                                CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
                        imageTimes[j] = imageListeners[j].getTimeReceivedImage();
                    }

                    captureTimes[i] = Stat.getAverage(imageTimes) - startTimeMs;
                    if (partialsExpected) {
                        getPartialTimes[i] = partialResultNTime.second - startTimeMs;
                        if (getPartialTimes[i] < 0) {
                            isPartialTimingValid = false;
                        }
                    }
                    getResultTimes[i] = captureResultNTime.second - startTimeMs;

                    // simulate real scenario (preview runs a bit)
                    CameraTestUtils.waitForNumResults(previewResultListener, NUM_RESULTS_WAIT,
                            WAIT_FOR_RESULT_TIMEOUT_MS);

                    stopPreviewAndDrain();

                    CameraTestUtils.closeImageReaders(readers);
                    readers = null;
                }
                String message = appendFormatDescription("camera_capture_latency",
                        formatDescription);
                mReportLog.addValues(message, captureTimes, ResultType.LOWER_BETTER, ResultUnit.MS);
                // If any of the partial results do not contain AE and AF state, then no report
                if (isPartialTimingValid) {
                    message = appendFormatDescription("camera_partial_result_latency",
                            formatDescription);
                    mReportLog.addValues(message, getPartialTimes, ResultType.LOWER_BETTER,
                            ResultUnit.MS);
                }
                message = appendFormatDescription("camera_capture_result_latency",
                        formatDescription);
                mReportLog.addValues(message, getResultTimes, ResultType.LOWER_BETTER,
                        ResultUnit.MS);

                avgResultTimes[counter] = Stat.getAverage(getResultTimes);
            }
            finally {
                CameraTestUtils.closeImageReaders(readers);
                readers = null;
                closeDevice(id);
                closePreviewSurface();
            }
            counter++;
            mReportLog.submit(mInstrumentation);
        }

        // Result will not be reported in CTS report if no summary is printed.
        if (mCameraIds.length != 0) {
            String streamName = appendFormatDescription("test_single_capture_average",
                    formatDescription);
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            String message = appendFormatDescription(
                    "camera_capture_result_average_latency_for_all_cameras", formatDescription);
            mReportLog.setSummary(message, Stat.getAverage(avgResultTimes),
                    ResultType.LOWER_BETTER, ResultUnit.MS);
            mReportLog.submit(mInstrumentation);
        }
    }

    /**
     * Test multiple capture KPI for YUV_420_888 format: the average time duration
     * between sending out image capture requests and receiving capture results.
     * <p>
     * It measures capture latency, which is the time between sending out the capture
     * request and getting the full capture result, and the frame duration, which is the timestamp
     * gap between results.
     * </p>
     */
    public void testMultipleCapture() throws Exception {
        double[] avgResultTimes = new double[mCameraIds.length];
        double[] avgDurationMs = new double[mCameraIds.length];

        // A simple CaptureSession StateCallback to handle onCaptureQueueEmpty
        class MultipleCaptureStateCallback extends CameraCaptureSession.StateCallback {
            private ConditionVariable captureQueueEmptyCond = new ConditionVariable();
            private int captureQueueEmptied = 0;

            @Override
            public void onConfigured(CameraCaptureSession session) {
                // Empty implementation
            }

            @Override
            public void onConfigureFailed(CameraCaptureSession session) {
                // Empty implementation
            }

            @Override
            public void onCaptureQueueEmpty(CameraCaptureSession session) {
                captureQueueEmptied++;
                if (VERBOSE) {
                    Log.v(TAG, "onCaptureQueueEmpty received. captureQueueEmptied = "
                        + captureQueueEmptied);
                }

                captureQueueEmptyCond.open();
            }

            /* Wait for onCaptureQueueEmpty, return immediately if an onCaptureQueueEmpty was
             * already received, otherwise, wait for one to arrive. */
            public void waitForCaptureQueueEmpty(long timeout) {
                if (captureQueueEmptied > 0) {
                    captureQueueEmptied--;
                    return;
                }

                if (captureQueueEmptyCond.block(timeout)) {
                    captureQueueEmptyCond.close();
                    captureQueueEmptied = 0;
                } else {
                    throw new TimeoutRuntimeException("Unable to receive onCaptureQueueEmpty after "
                        + timeout + "ms");
                }
            }
        }

        final MultipleCaptureStateCallback sessionListener = new MultipleCaptureStateCallback();

        int counter = 0;
        for (String id : mCameraIds) {
            // Do NOT move these variables to outer scope
            // They will be passed to DeviceReportLog and their references will be stored
            String streamName = "test_multiple_capture";
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
            long[] startTimes = new long[NUM_MAX_IMAGES];
            double[] getResultTimes = new double[NUM_MAX_IMAGES];
            double[] frameDurationMs = new double[NUM_MAX_IMAGES-1];
            try {
                if (!mAllStaticInfo.get(id).isColorOutputSupported()) {
                    Log.i(TAG, "Camera " + id + " does not support color outputs, skipping");
                    continue;
                }

                openDevice(id);
                for (int i = 0; i < NUM_TEST_LOOPS; i++) {

                    // setup builders and listeners
                    CaptureRequest.Builder previewBuilder =
                            mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                    CaptureRequest.Builder captureBuilder =
                            mCamera.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
                    SimpleCaptureCallback previewResultListener =
                            new SimpleCaptureCallback();
                    SimpleTimingResultListener captureResultListener =
                            new SimpleTimingResultListener();
                    SimpleImageReaderListener imageListener =
                            new SimpleImageReaderListener(/*asyncMode*/true, NUM_MAX_IMAGES);

                    Size maxYuvSize = CameraTestUtils.getSortedSizesForFormat(
                        id, mCameraManager, ImageFormat.YUV_420_888, /*bound*/null).get(0);
                    // Find minimum frame duration for YUV_420_888
                    StreamConfigurationMap config = mStaticInfo.getCharacteristics().get(
                            CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

                    final long minStillFrameDuration =
                            config.getOutputMinFrameDuration(ImageFormat.YUV_420_888, maxYuvSize);
                    if (minStillFrameDuration > 0) {
                        Range<Integer> targetRange =
                            CameraTestUtils.getSuitableFpsRangeForDuration(id,
                                    minStillFrameDuration, mStaticInfo);
                        previewBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, targetRange);
                        captureBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, targetRange);
                    }

                    prepareCaptureAndStartPreview(previewBuilder, captureBuilder,
                            mOrderedPreviewSizes.get(0), maxYuvSize,
                            ImageFormat.YUV_420_888, previewResultListener,
                            sessionListener, NUM_MAX_IMAGES, imageListener);

                    // Converge AE
                    CameraTestUtils.waitForAeStable(previewResultListener,
                            NUM_FRAMES_WAITED_FOR_UNKNOWN_LATENCY, mStaticInfo,
                            WAIT_FOR_RESULT_TIMEOUT_MS, NUM_RESULTS_WAIT_TIMEOUT);

                    if (mStaticInfo.isAeLockSupported()) {
                        // Lock AE if possible to improve stability
                        previewBuilder.set(CaptureRequest.CONTROL_AE_LOCK, true);
                        mCameraSession.setRepeatingRequest(previewBuilder.build(),
                                previewResultListener, mHandler);
                        if (mStaticInfo.isHardwareLevelAtLeastLimited()) {
                            // Legacy mode doesn't output AE state
                            CameraTestUtils.waitForResultValue(previewResultListener,
                                    CaptureResult.CONTROL_AE_STATE,
                                    CaptureResult.CONTROL_AE_STATE_LOCKED,
                                    NUM_RESULTS_WAIT_TIMEOUT, WAIT_FOR_RESULT_TIMEOUT_MS);
                        }
                    }

                    // Capture NUM_MAX_IMAGES images based on onCaptureQueueEmpty callback
                    for (int j = 0; j < NUM_MAX_IMAGES; j++) {

                        // Capture an image and get image data
                        startTimes[j] = SystemClock.elapsedRealtime();
                        CaptureRequest request = captureBuilder.build();
                        mCameraSession.capture(request, captureResultListener, mHandler);

                        // Wait for capture queue empty for the current request
                        sessionListener.waitForCaptureQueueEmpty(
                                CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
                    }

                    // Acquire the capture result time and frame duration
                    long prevTimestamp = -1;
                    for (int j = 0; j < NUM_MAX_IMAGES; j++) {
                        Pair<CaptureResult, Long> captureResultNTime =
                                captureResultListener.getCaptureResultNTime(
                                        CameraTestUtils.CAPTURE_RESULT_TIMEOUT_MS);

                        getResultTimes[j] +=
                                (double)(captureResultNTime.second - startTimes[j])/NUM_TEST_LOOPS;

                        // Collect inter-frame timestamp
                        long timestamp = captureResultNTime.first.get(CaptureResult.SENSOR_TIMESTAMP);
                        if (prevTimestamp != -1) {
                            frameDurationMs[j-1] +=
                                    (double)(timestamp - prevTimestamp)/(NUM_TEST_LOOPS * 1000000.0);
                        }
                        prevTimestamp = timestamp;
                    }

                    // simulate real scenario (preview runs a bit)
                    CameraTestUtils.waitForNumResults(previewResultListener, NUM_RESULTS_WAIT,
                            WAIT_FOR_RESULT_TIMEOUT_MS);

                    stopPreview();
                }

                for (int i = 0; i < getResultTimes.length; i++) {
                    Log.v(TAG, "Camera " + id + " result time[" + i + "] is " +
                            getResultTimes[i] + " ms");
                }
                for (int i = 0; i < NUM_MAX_IMAGES-1; i++) {
                    Log.v(TAG, "Camera " + id + " frame duration time[" + i + "] is " +
                            frameDurationMs[i] + " ms");
                }

                mReportLog.addValues("camera_multiple_capture_result_latency", getResultTimes,
                        ResultType.LOWER_BETTER, ResultUnit.MS);
                mReportLog.addValues("camera_multiple_capture_frame_duration", frameDurationMs,
                        ResultType.LOWER_BETTER, ResultUnit.MS);


                avgResultTimes[counter] = Stat.getAverage(getResultTimes);
                avgDurationMs[counter] = Stat.getAverage(frameDurationMs);
            }
            finally {
                closeDefaultImageReader();
                closeDevice(id);
                closePreviewSurface();
            }
            counter++;
            mReportLog.submit(mInstrumentation);
        }

        // Result will not be reported in CTS report if no summary is printed.
        if (mCameraIds.length != 0) {
            String streamName = "test_multiple_capture_average";
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            mReportLog.setSummary("camera_multiple_capture_result_average_latency_for_all_cameras",
                    Stat.getAverage(avgResultTimes), ResultType.LOWER_BETTER, ResultUnit.MS);
            mReportLog.submit(mInstrumentation);
            mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
            mReportLog.setSummary("camera_multiple_capture_frame_duration_average_for_all_cameras",
                    Stat.getAverage(avgDurationMs), ResultType.LOWER_BETTER, ResultUnit.MS);
            mReportLog.submit(mInstrumentation);
        }
    }

    /**
     * Test reprocessing shot-to-shot latency with default NR and edge options, i.e., from the time
     * a reprocess request is issued to the time the reprocess image is returned.
     */
    public void testReprocessingLatency() throws Exception {
        for (String id : mCameraIds) {
            for (int format : REPROCESS_FORMATS) {
                if (!isReprocessSupported(id, format)) {
                    continue;
                }

                try {
                    openDevice(id);
                    String streamName = "test_reprocessing_latency";
                    mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
                    mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
                    mReportLog.addValue("format", format, ResultType.NEUTRAL, ResultUnit.NONE);
                    reprocessingPerformanceTestByCamera(format, /*asyncMode*/false,
                            /*highQuality*/false);
                } finally {
                    closeReaderWriters();
                    closeDevice(id);
                    closePreviewSurface();
                    mReportLog.submit(mInstrumentation);
                }
            }
        }
    }

    /**
     * Test reprocessing throughput with default NR and edge options, i.e., how many frames can be reprocessed
     * during a given amount of time.
     *
     */
    public void testReprocessingThroughput() throws Exception {
        for (String id : mCameraIds) {
            for (int format : REPROCESS_FORMATS) {
                if (!isReprocessSupported(id, format)) {
                    continue;
                }

                try {
                    openDevice(id);
                    String streamName = "test_reprocessing_throughput";
                    mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
                    mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
                    mReportLog.addValue("format", format, ResultType.NEUTRAL, ResultUnit.NONE);
                    reprocessingPerformanceTestByCamera(format, /*asyncMode*/true,
                            /*highQuality*/false);
                } finally {
                    closeReaderWriters();
                    closeDevice(id);
                    closePreviewSurface();
                    mReportLog.submit(mInstrumentation);
                }
            }
        }
    }

    /**
     * Test reprocessing shot-to-shot latency with High Quality NR and edge options, i.e., from the
     * time a reprocess request is issued to the time the reprocess image is returned.
     */
    public void testHighQualityReprocessingLatency() throws Exception {
        for (String id : mCameraIds) {
            for (int format : REPROCESS_FORMATS) {
                if (!isReprocessSupported(id, format)) {
                    continue;
                }

                try {
                    openDevice(id);
                    String streamName = "test_high_quality_reprocessing_latency";
                    mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
                    mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
                    mReportLog.addValue("format", format, ResultType.NEUTRAL, ResultUnit.NONE);
                    reprocessingPerformanceTestByCamera(format, /*asyncMode*/false,
                            /*requireHighQuality*/true);
                } finally {
                    closeReaderWriters();
                    closeDevice(id);
                    closePreviewSurface();
                    mReportLog.submit(mInstrumentation);
                }
            }
        }
    }

    /**
     * Test reprocessing throughput with high quality NR and edge options, i.e., how many frames can
     * be reprocessed during a given amount of time.
     *
     */
    public void testHighQualityReprocessingThroughput() throws Exception {
        for (String id : mCameraIds) {
            for (int format : REPROCESS_FORMATS) {
                if (!isReprocessSupported(id, format)) {
                    continue;
                }

                try {
                    openDevice(id);
                    String streamName = "test_high_quality_reprocessing_throughput";
                    mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
                    mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
                    mReportLog.addValue("format", format, ResultType.NEUTRAL, ResultUnit.NONE);
                    reprocessingPerformanceTestByCamera(format, /*asyncMode*/true,
                            /*requireHighQuality*/true);
                } finally {
                    closeReaderWriters();
                    closeDevice(id);
                    closePreviewSurface();
                    mReportLog.submit(mInstrumentation);
                }
            }
        }
    }

    /**
     * Testing reprocessing caused preview stall (frame drops)
     */
    public void testReprocessingCaptureStall() throws Exception {
        for (String id : mCameraIds) {
            for (int format : REPROCESS_FORMATS) {
                if (!isReprocessSupported(id, format)) {
                    continue;
                }

                try {
                    openDevice(id);
                    String streamName = "test_reprocessing_capture_stall";
                    mReportLog = new DeviceReportLog(REPORT_LOG_NAME, streamName);
                    mReportLog.addValue("camera_id", id, ResultType.NEUTRAL, ResultUnit.NONE);
                    mReportLog.addValue("format", format, ResultType.NEUTRAL, ResultUnit.NONE);
                    reprocessingCaptureStallTestByCamera(format);
                } finally {
                    closeReaderWriters();
                    closeDevice(id);
                    closePreviewSurface();
                    mReportLog.submit(mInstrumentation);
                }
            }
        }
    }

    private void reprocessingCaptureStallTestByCamera(int reprocessInputFormat) throws Exception {
        prepareReprocessCapture(reprocessInputFormat);

        // Let it stream for a while before reprocessing
        startZslStreaming();
        waitForFrames(NUM_RESULTS_WAIT);

        final int NUM_REPROCESS_TESTED = MAX_REPROCESS_IMAGES / 2;
        // Prepare several reprocessing request
        Image[] inputImages = new Image[NUM_REPROCESS_TESTED];
        CaptureRequest.Builder[] reprocessReqs = new CaptureRequest.Builder[MAX_REPROCESS_IMAGES];
        for (int i = 0; i < NUM_REPROCESS_TESTED; i++) {
            inputImages[i] =
                    mCameraZslImageListener.getImage(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
            TotalCaptureResult zslResult =
                    mZslResultListener.getCaptureResult(
                            WAIT_FOR_RESULT_TIMEOUT_MS, inputImages[i].getTimestamp());
            reprocessReqs[i] = mCamera.createReprocessCaptureRequest(zslResult);
            reprocessReqs[i].addTarget(mJpegReader.getSurface());
            reprocessReqs[i].set(CaptureRequest.NOISE_REDUCTION_MODE,
                    CaptureRequest.NOISE_REDUCTION_MODE_HIGH_QUALITY);
            reprocessReqs[i].set(CaptureRequest.EDGE_MODE,
                    CaptureRequest.EDGE_MODE_HIGH_QUALITY);
            mWriter.queueInputImage(inputImages[i]);
        }

        double[] maxCaptureGapsMs = new double[NUM_REPROCESS_TESTED];
        double[] averageFrameDurationMs = new double[NUM_REPROCESS_TESTED];
        Arrays.fill(averageFrameDurationMs, 0.0);
        final int MAX_REPROCESS_RETURN_FRAME_COUNT = 20;
        SimpleCaptureCallback reprocessResultListener = new SimpleCaptureCallback();
        for (int i = 0; i < NUM_REPROCESS_TESTED; i++) {
            mZslResultListener.drain();
            CaptureRequest reprocessRequest = reprocessReqs[i].build();
            mCameraSession.capture(reprocessRequest, reprocessResultListener, mHandler);
            // Wait for reprocess output jpeg and result come back.
            reprocessResultListener.getCaptureResultForRequest(reprocessRequest,
                    CameraTestUtils.CAPTURE_RESULT_TIMEOUT_MS);
            mJpegListener.getImage(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS).close();
            long numFramesMaybeStalled = mZslResultListener.getTotalNumFrames();
            assertTrue("Reprocess capture result should be returned in "
                    + MAX_REPROCESS_RETURN_FRAME_COUNT + " frames",
                    numFramesMaybeStalled <= MAX_REPROCESS_RETURN_FRAME_COUNT);

            // Need look longer time, as the stutter could happen after the reprocessing
            // output frame is received.
            long[] timestampGap = new long[MAX_REPROCESS_RETURN_FRAME_COUNT + 1];
            Arrays.fill(timestampGap, 0);
            CaptureResult[] results = new CaptureResult[timestampGap.length];
            long[] frameDurationsNs = new long[timestampGap.length];
            for (int j = 0; j < results.length; j++) {
                results[j] = mZslResultListener.getCaptureResult(
                        CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
                if (j > 0) {
                    timestampGap[j] = results[j].get(CaptureResult.SENSOR_TIMESTAMP) -
                            results[j - 1].get(CaptureResult.SENSOR_TIMESTAMP);
                    assertTrue("Time stamp should be monotonically increasing",
                            timestampGap[j] > 0);
                }
                frameDurationsNs[j] = results[j].get(CaptureResult.SENSOR_FRAME_DURATION);
            }

            if (VERBOSE) {
                Log.i(TAG, "timestampGap: " + Arrays.toString(timestampGap));
                Log.i(TAG, "frameDurationsNs: " + Arrays.toString(frameDurationsNs));
            }

            // Get the number of candidate results, calculate the average frame duration
            // and max timestamp gap.
            Arrays.sort(timestampGap);
            double maxTimestampGapMs = timestampGap[timestampGap.length - 1] / 1000000.0;
            for (int m = 0; m < frameDurationsNs.length; m++) {
                averageFrameDurationMs[i] += (frameDurationsNs[m] / 1000000.0);
            }
            averageFrameDurationMs[i] /= frameDurationsNs.length;

            maxCaptureGapsMs[i] = maxTimestampGapMs;
        }

        stopZslStreaming();

        String reprocessType = "YUV reprocessing";
        if (reprocessInputFormat == ImageFormat.PRIVATE) {
            reprocessType = "opaque reprocessing";
        }
        mReportLog.addValue("reprocess_type", reprocessType, ResultType.NEUTRAL, ResultUnit.NONE);
        mReportLog.addValues("max_capture_timestamp_gaps", maxCaptureGapsMs,
                ResultType.LOWER_BETTER, ResultUnit.MS);
        mReportLog.addValues("capture_average_frame_duration", averageFrameDurationMs,
                ResultType.LOWER_BETTER, ResultUnit.MS);
        mReportLog.setSummary("camera_reprocessing_average_max_capture_timestamp_gaps",
                Stat.getAverage(maxCaptureGapsMs), ResultType.LOWER_BETTER, ResultUnit.MS);

        // The max timestamp gap should be less than (captureStall + 1) x average frame
        // duration * (1 + error margin).
        int maxCaptureStallFrames = mStaticInfo.getMaxCaptureStallOrDefault();
        for (int i = 0; i < maxCaptureGapsMs.length; i++) {
            double stallDurationBound = averageFrameDurationMs[i] *
                    (maxCaptureStallFrames + 1) * (1 + REPROCESS_STALL_MARGIN);
            assertTrue("max capture stall duration should be no larger than " + stallDurationBound,
                    maxCaptureGapsMs[i] <= stallDurationBound);
        }
    }

    private void reprocessingPerformanceTestByCamera(int reprocessInputFormat, boolean asyncMode,
            boolean requireHighQuality)
            throws Exception {
        // Prepare the reprocessing capture
        prepareReprocessCapture(reprocessInputFormat);

        // Start ZSL streaming
        startZslStreaming();
        waitForFrames(NUM_RESULTS_WAIT);

        CaptureRequest.Builder[] reprocessReqs = new CaptureRequest.Builder[MAX_REPROCESS_IMAGES];
        Image[] inputImages = new Image[MAX_REPROCESS_IMAGES];
        double[] getImageLatenciesMs = new double[MAX_REPROCESS_IMAGES];
        long startTimeMs;
        for (int i = 0; i < MAX_REPROCESS_IMAGES; i++) {
            inputImages[i] =
                    mCameraZslImageListener.getImage(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
            TotalCaptureResult zslResult =
                    mZslResultListener.getCaptureResult(
                            WAIT_FOR_RESULT_TIMEOUT_MS, inputImages[i].getTimestamp());
            reprocessReqs[i] = mCamera.createReprocessCaptureRequest(zslResult);
            if (requireHighQuality) {
                // Reprocessing should support high quality for NR and edge modes.
                reprocessReqs[i].set(CaptureRequest.NOISE_REDUCTION_MODE,
                        CaptureRequest.NOISE_REDUCTION_MODE_HIGH_QUALITY);
                reprocessReqs[i].set(CaptureRequest.EDGE_MODE,
                        CaptureRequest.EDGE_MODE_HIGH_QUALITY);
            }
            reprocessReqs[i].addTarget(mJpegReader.getSurface());
        }

        if (asyncMode) {
            // async capture: issue all the reprocess requests as quick as possible, then
            // check the throughput of the output jpegs.
            for (int i = 0; i < MAX_REPROCESS_IMAGES; i++) {
                // Could be slow for YUV reprocessing, do it in advance.
                mWriter.queueInputImage(inputImages[i]);
            }

            // Submit the requests
            for (int i = 0; i < MAX_REPROCESS_IMAGES; i++) {
                mCameraSession.capture(reprocessReqs[i].build(), null, null);
            }

            // Get images
            startTimeMs = SystemClock.elapsedRealtime();
            Image jpegImages[] = new Image[MAX_REPROCESS_IMAGES];
            for (int i = 0; i < MAX_REPROCESS_IMAGES; i++) {
                jpegImages[i] = mJpegListener.getImage(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
                getImageLatenciesMs[i] = SystemClock.elapsedRealtime() - startTimeMs;
                startTimeMs = SystemClock.elapsedRealtime();
            }
            for (Image i : jpegImages) {
                i.close();
            }
        } else {
            // sync capture: issue reprocess request one by one, only submit next one when
            // the previous capture image is returned. This is to test the back to back capture
            // performance.
            Image jpegImages[] = new Image[MAX_REPROCESS_IMAGES];
            for (int i = 0; i < MAX_REPROCESS_IMAGES; i++) {
                startTimeMs = SystemClock.elapsedRealtime();
                mWriter.queueInputImage(inputImages[i]);
                mCameraSession.capture(reprocessReqs[i].build(), null, null);
                jpegImages[i] = mJpegListener.getImage(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
                getImageLatenciesMs[i] = SystemClock.elapsedRealtime() - startTimeMs;
            }
            for (Image i : jpegImages) {
                i.close();
            }
        }

        stopZslStreaming();

        String reprocessType = "YUV reprocessing";
        if (reprocessInputFormat == ImageFormat.PRIVATE) {
            reprocessType = "opaque reprocessing";
        }

        // Report the performance data
        String captureMsg;
        if (asyncMode) {
            captureMsg = "capture latency";
            if (requireHighQuality) {
                captureMsg += " for High Quality noise reduction and edge modes";
            }
            mReportLog.addValue("reprocess_type", reprocessType, ResultType.NEUTRAL,
                    ResultUnit.NONE);
            mReportLog.addValue("capture_message", captureMsg, ResultType.NEUTRAL,
                    ResultUnit.NONE);
            mReportLog.addValues("latency", getImageLatenciesMs, ResultType.LOWER_BETTER,
                    ResultUnit.MS);
            mReportLog.setSummary("camera_reprocessing_average_latency",
                    Stat.getAverage(getImageLatenciesMs), ResultType.LOWER_BETTER, ResultUnit.MS);
        } else {
            captureMsg = "shot to shot latency";
            if (requireHighQuality) {
                captureMsg += " for High Quality noise reduction and edge modes";
            }
            mReportLog.addValue("reprocess_type", reprocessType, ResultType.NEUTRAL,
                    ResultUnit.NONE);
            mReportLog.addValue("capture_message", captureMsg, ResultType.NEUTRAL,
                    ResultUnit.NONE);
            mReportLog.addValues("latency", getImageLatenciesMs, ResultType.LOWER_BETTER,
                    ResultUnit.MS);
            mReportLog.setSummary("camera_reprocessing_shot_to_shot_average_latency",
                    Stat.getAverage(getImageLatenciesMs), ResultType.LOWER_BETTER, ResultUnit.MS);
        }
    }

    /**
     * Start preview and ZSL streaming
     */
    private void startZslStreaming() throws Exception {
        CaptureRequest.Builder zslBuilder =
                mCamera.createCaptureRequest(CameraDevice.TEMPLATE_ZERO_SHUTTER_LAG);
        zslBuilder.addTarget(mPreviewSurface);
        zslBuilder.addTarget(mCameraZslReader.getSurface());
        mCameraSession.setRepeatingRequest(zslBuilder.build(), mZslResultListener, mHandler);
    }

    private void stopZslStreaming() throws Exception {
        mCameraSession.stopRepeating();
        mCameraSessionListener.getStateWaiter().waitForState(
            BlockingSessionCallback.SESSION_READY, CameraTestUtils.CAMERA_IDLE_TIMEOUT_MS);
    }

    /**
     * Wait for a certain number of frames, the images and results will be drained from the
     * listeners to make sure that next reprocessing can get matched results and images.
     *
     * @param numFrameWait The number of frames to wait before return, 0 means that
     *      this call returns immediately after streaming on.
     */
    private void waitForFrames(int numFrameWait) throws Exception {
        if (numFrameWait < 0) {
            throw new IllegalArgumentException("numFrameWait " + numFrameWait +
                    " should be non-negative");
        }

        for (int i = 0; i < numFrameWait; i++) {
            mCameraZslImageListener.getImage(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS).close();
        }
    }

    private void closeReaderWriters() {
        mCameraZslImageListener.drain();
        CameraTestUtils.closeImageReader(mCameraZslReader);
        mCameraZslReader = null;
        mJpegListener.drain();
        CameraTestUtils.closeImageReader(mJpegReader);
        mJpegReader = null;
        CameraTestUtils.closeImageWriter(mWriter);
        mWriter = null;
    }

    private void prepareReprocessCapture(int inputFormat)
                    throws CameraAccessException {
        // 1. Find the right preview and capture sizes.
        Size maxPreviewSize = mOrderedPreviewSizes.get(0);
        Size[] supportedInputSizes =
                mStaticInfo.getAvailableSizesForFormatChecked(inputFormat,
                StaticMetadata.StreamDirection.Input);
        Size maxInputSize = CameraTestUtils.getMaxSize(supportedInputSizes);
        Size maxJpegSize = mOrderedStillSizes.get(0);
        updatePreviewSurface(maxPreviewSize);
        mZslResultListener = new SimpleCaptureCallback();

        // 2. Create camera output ImageReaders.
        // YUV/Opaque output, camera should support output with input size/format
        mCameraZslImageListener = new SimpleImageReaderListener(
                /*asyncMode*/true, MAX_ZSL_IMAGES - MAX_REPROCESS_IMAGES);
        mCameraZslReader = CameraTestUtils.makeImageReader(
                maxInputSize, inputFormat, MAX_ZSL_IMAGES, mCameraZslImageListener, mHandler);
        // Jpeg reprocess output
        mJpegListener = new SimpleImageReaderListener();
        mJpegReader = CameraTestUtils.makeImageReader(
                maxJpegSize, ImageFormat.JPEG, MAX_JPEG_IMAGES, mJpegListener, mHandler);

        // create camera reprocess session
        List<Surface> outSurfaces = new ArrayList<Surface>();
        outSurfaces.add(mPreviewSurface);
        outSurfaces.add(mCameraZslReader.getSurface());
        outSurfaces.add(mJpegReader.getSurface());
        InputConfiguration inputConfig = new InputConfiguration(maxInputSize.getWidth(),
                maxInputSize.getHeight(), inputFormat);
        mCameraSessionListener = new BlockingSessionCallback();
        mCameraSession = CameraTestUtils.configureReprocessableCameraSession(
                mCamera, inputConfig, outSurfaces, mCameraSessionListener, mHandler);

        // 3. Create ImageWriter for input
        mWriter = CameraTestUtils.makeImageWriter(
                mCameraSession.getInputSurface(), MAX_INPUT_IMAGES, /*listener*/null, /*handler*/null);

    }

    private void blockingStopPreview() throws Exception {
        stopPreview();
        mCameraSessionListener.getStateWaiter().waitForState(SESSION_CLOSED,
                CameraTestUtils.SESSION_CLOSE_TIMEOUT_MS);
    }

    private void blockingStartPreview(CaptureCallback listener, SimpleImageListener imageListener)
            throws Exception {
        if (mPreviewSurface == null || mReaderSurface == null) {
            throw new IllegalStateException("preview and reader surface must be initilized first");
        }

        CaptureRequest.Builder previewBuilder =
                mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
        if (mStaticInfo.isColorOutputSupported()) {
            previewBuilder.addTarget(mPreviewSurface);
        }
        previewBuilder.addTarget(mReaderSurface);
        mCameraSession.setRepeatingRequest(previewBuilder.build(), listener, mHandler);
        imageListener.waitForImageAvailable(CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS);
    }

    /**
     * Setup still capture configuration and start preview.
     *
     * @param previewRequest The capture request to be used for preview
     * @param stillRequest The capture request to be used for still capture
     * @param previewSz Preview size
     * @param captureSizes Still capture sizes
     * @param formats The single capture image formats
     * @param resultListener Capture result listener
     * @param maxNumImages The max number of images set to the image reader
     * @param imageListeners The single capture capture image listeners
     * @param isHeic Capture HEIC image if true, JPEG image if false
     */
    private ImageReader[] prepareStillCaptureAndStartPreview(
            CaptureRequest.Builder previewRequest, CaptureRequest.Builder stillRequest,
            Size previewSz, Size[] captureSizes, int[] formats, CaptureCallback resultListener,
            int maxNumImages, ImageReader.OnImageAvailableListener[] imageListeners,
            boolean isHeic)
            throws Exception {

        if ((captureSizes == null) || (formats == null) || (imageListeners == null) &&
                (captureSizes.length != formats.length) ||
                (formats.length != imageListeners.length)) {
            throw new IllegalArgumentException("Invalid capture sizes/formats or image listeners!");
        }

        if (VERBOSE) {
            Log.v(TAG, String.format("Prepare still capture and preview (%s)",
                    previewSz.toString()));
        }

        // Update preview size.
        updatePreviewSurface(previewSz);

        ImageReader[] readers = new ImageReader[captureSizes.length];
        List<Surface> outputSurfaces = new ArrayList<Surface>();
        outputSurfaces.add(mPreviewSurface);
        for (int i = 0; i < captureSizes.length; i++) {
            readers[i] = CameraTestUtils.makeImageReader(captureSizes[i], formats[i], maxNumImages,
                    imageListeners[i], mHandler);
            outputSurfaces.add(readers[i].getSurface());
        }

        mCameraSessionListener = new BlockingSessionCallback();
        mCameraSession = CameraTestUtils.configureCameraSession(mCamera, outputSurfaces,
                mCameraSessionListener, mHandler);

        // Configure the requests.
        previewRequest.addTarget(mPreviewSurface);
        stillRequest.addTarget(mPreviewSurface);
        for (int i = 0; i < readers.length; i++) {
            stillRequest.addTarget(readers[i].getSurface());
        }

        // Start preview.
        mCameraSession.setRepeatingRequest(previewRequest.build(), resultListener, mHandler);

        return readers;
    }

    /**
     * Setup single capture configuration and start preview.
     *
     * @param previewRequest The capture request to be used for preview
     * @param stillRequest The capture request to be used for still capture
     * @param previewSz Preview size
     * @param captureSz Still capture size
     * @param format The single capture image format
     * @param resultListener Capture result listener
     * @param sessionListener Session listener
     * @param maxNumImages The max number of images set to the image reader
     * @param imageListener The single capture capture image listener
     */
    private void prepareCaptureAndStartPreview(CaptureRequest.Builder previewRequest,
            CaptureRequest.Builder stillRequest, Size previewSz, Size captureSz, int format,
            CaptureCallback resultListener, CameraCaptureSession.StateCallback sessionListener,
            int maxNumImages, ImageReader.OnImageAvailableListener imageListener) throws Exception {
        if ((captureSz == null) || (imageListener == null)) {
            throw new IllegalArgumentException("Invalid capture size or image listener!");
        }

        if (VERBOSE) {
            Log.v(TAG, String.format("Prepare single capture (%s) and preview (%s)",
                    captureSz.toString(), previewSz.toString()));
        }

        // Update preview size.
        updatePreviewSurface(previewSz);

        // Create ImageReader.
        createDefaultImageReader(captureSz, format, maxNumImages, imageListener);

        // Configure output streams with preview and jpeg streams.
        List<Surface> outputSurfaces = new ArrayList<Surface>();
        outputSurfaces.add(mPreviewSurface);
        outputSurfaces.add(mReaderSurface);
        if (sessionListener == null) {
            mCameraSessionListener = new BlockingSessionCallback();
        } else {
            mCameraSessionListener = new BlockingSessionCallback(sessionListener);
        }
        mCameraSession = CameraTestUtils.configureCameraSession(mCamera, outputSurfaces,
                mCameraSessionListener, mHandler);

        // Configure the requests.
        previewRequest.addTarget(mPreviewSurface);
        stillRequest.addTarget(mPreviewSurface);
        stillRequest.addTarget(mReaderSurface);

        // Start preview.
        mCameraSession.setRepeatingRequest(previewRequest.build(), resultListener, mHandler);
    }

    /**
     * Update the preview surface size.
     *
     * @param size The preview size to be updated.
     */
    private void updatePreviewSurface(Size size) {
        if ((mPreviewSurfaceTexture != null ) || (mPreviewSurface != null)) {
            closePreviewSurface();
        }

        mPreviewSurfaceTexture = new SurfaceTexture(/*random int*/ 1);
        mPreviewSurfaceTexture.setDefaultBufferSize(size.getWidth(), size.getHeight());
        mPreviewSurface = new Surface(mPreviewSurfaceTexture);
    }

    /**
     * Release preview surface and corresponding surface texture.
     */
    private void closePreviewSurface() {
        if (mPreviewSurface != null) {
            mPreviewSurface.release();
            mPreviewSurface = null;
        }

        if (mPreviewSurfaceTexture != null) {
            mPreviewSurfaceTexture.release();
            mPreviewSurfaceTexture = null;
        }
    }

    private boolean isReprocessSupported(String cameraId, int format)
            throws CameraAccessException {
        if (format != ImageFormat.YUV_420_888 && format != ImageFormat.PRIVATE) {
            throw new IllegalArgumentException(
                    "format " + format + " is not supported for reprocessing");
        }

        StaticMetadata info = new StaticMetadata(
                mCameraManager.getCameraCharacteristics(cameraId), CheckLevel.ASSERT,
                /*collector*/ null);
        int cap = CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES_YUV_REPROCESSING;
        if (format == ImageFormat.PRIVATE) {
            cap = CameraCharacteristics.REQUEST_AVAILABLE_CAPABILITIES_PRIVATE_REPROCESSING;
        }
        return info.isCapabilitySupported(cap);
    }

    /**
     * Stop preview for current camera device by closing the session.
     * Does _not_ wait for the device to go idle
     */
    private void stopPreview() throws Exception {
        // Stop repeat, wait for captures to complete, and disconnect from surfaces
        if (mCameraSession != null) {
            if (VERBOSE) Log.v(TAG, "Stopping preview");
            mCameraSession.close();
        }
    }

    /**
     * Stop preview for current camera device by closing the session and waiting for it to close,
     * resulting in an idle device.
     */
    private void stopPreviewAndDrain() throws Exception {
        // Stop repeat, wait for captures to complete, and disconnect from surfaces
        if (mCameraSession != null) {
            if (VERBOSE) Log.v(TAG, "Stopping preview and waiting for idle");
            mCameraSession.close();
            mCameraSessionListener.getStateWaiter().waitForState(
                    BlockingSessionCallback.SESSION_CLOSED,
                    /*timeoutMs*/WAIT_FOR_RESULT_TIMEOUT_MS);
        }
    }

    /**
     * Configure reader and preview outputs and wait until done.
     */
    private void configureReaderAndPreviewOutputs() throws Exception {
        if (mPreviewSurface == null || mReaderSurface == null) {
            throw new IllegalStateException("preview and reader surface must be initilized first");
        }
        mCameraSessionListener = new BlockingSessionCallback();
        List<Surface> outputSurfaces = new ArrayList<>();
        if (mStaticInfo.isColorOutputSupported()) {
            outputSurfaces.add(mPreviewSurface);
        }
        outputSurfaces.add(mReaderSurface);
        mCameraSession = CameraTestUtils.configureCameraSession(mCamera, outputSurfaces,
                mCameraSessionListener, mHandler);
    }

    /**
     * Initialize the ImageReader instance and preview surface.
     * @param cameraId The camera to be opened.
     * @param format The format used to create ImageReader instance.
     */
    private void initializeImageReader(String cameraId, int format) throws Exception {
        mOrderedPreviewSizes = CameraTestUtils.getSortedSizesForFormat(
                cameraId, mCameraManager, format,
                CameraTestUtils.getPreviewSizeBound(mWindowManager,
                    CameraTestUtils.PREVIEW_SIZE_BOUND));
        Size maxPreviewSize = mOrderedPreviewSizes.get(0);
        createDefaultImageReader(maxPreviewSize, format, NUM_MAX_IMAGES, /*listener*/null);
        updatePreviewSurface(maxPreviewSize);
    }

    private void simpleOpenCamera(String cameraId) throws Exception {
        mCamera = CameraTestUtils.openCamera(
                mCameraManager, cameraId, mCameraListener, mHandler);
        mCollector.setCameraId(cameraId);
        mStaticInfo = new StaticMetadata(mCameraManager.getCameraCharacteristics(cameraId),
                CheckLevel.ASSERT, /*collector*/null);
    }

    /**
     * Simple image listener that can be used to time the availability of first image.
     *
     */
    private static class SimpleImageListener implements ImageReader.OnImageAvailableListener {
        private ConditionVariable imageAvailable = new ConditionVariable();
        private boolean imageReceived = false;
        private long mTimeReceivedImage = 0;

        @Override
        public void onImageAvailable(ImageReader reader) {
            Image image = null;
            if (!imageReceived) {
                if (VERBOSE) {
                    Log.v(TAG, "First image arrives");
                }
                imageReceived = true;
                mTimeReceivedImage = SystemClock.elapsedRealtime();
                imageAvailable.open();
            }
            image = reader.acquireNextImage();
            if (image != null) {
                image.close();
            }
        }

        /**
         * Wait for image available, return immediately if the image was already
         * received, otherwise wait until an image arrives.
         */
        public void waitForImageAvailable(long timeout) {
            if (imageReceived) {
                imageReceived = false;
                return;
            }

            if (imageAvailable.block(timeout)) {
                imageAvailable.close();
                imageReceived = true;
            } else {
                throw new TimeoutRuntimeException("Unable to get the first image after "
                        + CameraTestUtils.CAPTURE_IMAGE_TIMEOUT_MS + "ms");
            }
        }

        public long getTimeReceivedImage() {
            return mTimeReceivedImage;
        }
    }

    private static class SimpleTimingResultListener
            extends CameraCaptureSession.CaptureCallback {
        private final LinkedBlockingQueue<Pair<CaptureResult, Long> > mPartialResultQueue =
                new LinkedBlockingQueue<Pair<CaptureResult, Long> >();
        private final LinkedBlockingQueue<Pair<CaptureResult, Long> > mResultQueue =
                new LinkedBlockingQueue<Pair<CaptureResult, Long> > ();

        @Override
        public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                TotalCaptureResult result) {
            try {
                Long time = SystemClock.elapsedRealtime();
                mResultQueue.put(new Pair<CaptureResult, Long>(result, time));
            } catch (InterruptedException e) {
                throw new UnsupportedOperationException(
                        "Can't handle InterruptedException in onCaptureCompleted");
            }
        }

        @Override
        public void onCaptureProgressed(CameraCaptureSession session, CaptureRequest request,
                CaptureResult partialResult) {
            try {
                // check if AE and AF state exists
                Long time = -1L;
                if (partialResult.get(CaptureResult.CONTROL_AE_STATE) != null &&
                        partialResult.get(CaptureResult.CONTROL_AF_STATE) != null) {
                    time = SystemClock.elapsedRealtime();
                }
                mPartialResultQueue.put(new Pair<CaptureResult, Long>(partialResult, time));
            } catch (InterruptedException e) {
                throw new UnsupportedOperationException(
                        "Can't handle InterruptedException in onCaptureProgressed");
            }
        }

        public Pair<CaptureResult, Long> getPartialResultNTime(long timeout) {
            try {
                Pair<CaptureResult, Long> result =
                        mPartialResultQueue.poll(timeout, TimeUnit.MILLISECONDS);
                return result;
            } catch (InterruptedException e) {
                throw new UnsupportedOperationException("Unhandled interrupted exception", e);
            }
        }

        public Pair<CaptureResult, Long> getCaptureResultNTime(long timeout) {
            try {
                Pair<CaptureResult, Long> result =
                        mResultQueue.poll(timeout, TimeUnit.MILLISECONDS);
                assertNotNull("Wait for a capture result timed out in " + timeout + "ms", result);
                return result;
            } catch (InterruptedException e) {
                throw new UnsupportedOperationException("Unhandled interrupted exception", e);
            }
        }

        public Pair<CaptureResult, Long> getPartialResultNTimeForRequest(CaptureRequest myRequest,
                int numResultsWait) {
            if (numResultsWait < 0) {
                throw new IllegalArgumentException("numResultsWait must be no less than 0");
            }

            Pair<CaptureResult, Long> result;
            int i = 0;
            do {
                result = getPartialResultNTime(CameraTestUtils.CAPTURE_RESULT_TIMEOUT_MS);
                // The result may be null if no partials are produced on this particular path, so
                // stop trying
                if (result == null) break;
                if (result.first.getRequest().equals(myRequest)) {
                    return result;
                }
            } while (i++ < numResultsWait);

            // No partials produced - this may not be an error, since a given device may not
            // produce any partials on this testing path
            return null;
        }

        public Pair<CaptureResult, Long> getCaptureResultNTimeForRequest(CaptureRequest myRequest,
                int numResultsWait) {
            if (numResultsWait < 0) {
                throw new IllegalArgumentException("numResultsWait must be no less than 0");
            }

            Pair<CaptureResult, Long> result;
            int i = 0;
            do {
                result = getCaptureResultNTime(CameraTestUtils.CAPTURE_RESULT_TIMEOUT_MS);
                if (result.first.getRequest().equals(myRequest)) {
                    return result;
                }
            } while (i++ < numResultsWait);

            throw new TimeoutRuntimeException("Unable to get the expected capture result after "
                    + "waiting for " + numResultsWait + " results");
        }

    }
}
