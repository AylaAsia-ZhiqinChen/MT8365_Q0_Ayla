/*
 * Copyright (C) 2018 The Android Open Source Project
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

#define LOG_TAG "sensors_hidl_hal_test"

#include "SensorsHidlEnvironmentV2_0.h"
#include "sensors-vts-utils/SensorsHidlTestBase.h"
#include "sensors-vts-utils/SensorsTestSharedMemory.h"

#include <android/hardware/sensors/2.0/ISensors.h>
#include <android/hardware/sensors/2.0/types.h>
#include <log/log.h>
#include <utils/SystemClock.h>

#include <cinttypes>
#include <condition_variable>
#include <cstring>
#include <map>
#include <vector>

using ::android::sp;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::sensors::V1_0::MetaDataEventType;
using ::android::hardware::sensors::V1_0::OperationMode;
using ::android::hardware::sensors::V1_0::SensorsEventFormatOffset;
using ::android::hardware::sensors::V1_0::SensorStatus;
using ::android::hardware::sensors::V1_0::SharedMemType;
using ::android::hardware::sensors::V1_0::Vec3;

constexpr size_t kEventSize = static_cast<size_t>(SensorsEventFormatOffset::TOTAL_LENGTH);

class EventCallback : public IEventCallback {
   public:
    void reset() {
        mFlushMap.clear();
        mEventMap.clear();
    }

    void onEvent(const ::android::hardware::sensors::V1_0::Event& event) override {
        if (event.sensorType == SensorType::META_DATA &&
            event.u.meta.what == MetaDataEventType::META_DATA_FLUSH_COMPLETE) {
            std::unique_lock<std::recursive_mutex> lock(mFlushMutex);
            mFlushMap[event.sensorHandle]++;
            mFlushCV.notify_all();
        } else if (event.sensorType != SensorType::ADDITIONAL_INFO) {
            std::unique_lock<std::recursive_mutex> lock(mEventMutex);
            mEventMap[event.sensorHandle].push_back(event);
            mEventCV.notify_all();
        }
    }

    int32_t getFlushCount(int32_t sensorHandle) {
        std::unique_lock<std::recursive_mutex> lock(mFlushMutex);
        return mFlushMap[sensorHandle];
    }

    void waitForFlushEvents(const std::vector<SensorInfo>& sensorsToWaitFor,
                            int32_t numCallsToFlush, int64_t timeoutMs) {
        std::unique_lock<std::recursive_mutex> lock(mFlushMutex);
        mFlushCV.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                          [&] { return flushesReceived(sensorsToWaitFor, numCallsToFlush); });
    }

    const std::vector<Event> getEvents(int32_t sensorHandle) {
        std::unique_lock<std::recursive_mutex> lock(mEventMutex);
        return mEventMap[sensorHandle];
    }

    void waitForEvents(const std::vector<SensorInfo>& sensorsToWaitFor, int32_t timeoutMs) {
        std::unique_lock<std::recursive_mutex> lock(mEventMutex);
        mEventCV.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                          [&] { return eventsReceived(sensorsToWaitFor); });
    }

   protected:
    bool flushesReceived(const std::vector<SensorInfo>& sensorsToWaitFor, int32_t numCallsToFlush) {
        for (const SensorInfo& sensor : sensorsToWaitFor) {
            if (getFlushCount(sensor.sensorHandle) < numCallsToFlush) {
                return false;
            }
        }
        return true;
    }

    bool eventsReceived(const std::vector<SensorInfo>& sensorsToWaitFor) {
        for (const SensorInfo& sensor : sensorsToWaitFor) {
            if (getEvents(sensor.sensorHandle).size() == 0) {
                return false;
            }
        }
        return true;
    }

    std::map<int32_t, int32_t> mFlushMap;
    std::recursive_mutex mFlushMutex;
    std::condition_variable_any mFlushCV;

    std::map<int32_t, std::vector<Event>> mEventMap;
    std::recursive_mutex mEventMutex;
    std::condition_variable_any mEventCV;
};

// The main test class for SENSORS HIDL HAL.

class SensorsHidlTest : public SensorsHidlTestBase {
   protected:
    SensorInfo defaultSensorByType(SensorType type) override;
    std::vector<SensorInfo> getSensorsList();
    // implementation wrapper
    Return<void> getSensorsList(ISensors::getSensorsList_cb _hidl_cb) override {
        return getSensors()->getSensorsList(_hidl_cb);
    }

    Return<Result> activate(int32_t sensorHandle, bool enabled) override;

    Return<Result> batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                         int64_t maxReportLatencyNs) override {
        return getSensors()->batch(sensorHandle, samplingPeriodNs, maxReportLatencyNs);
    }

    Return<Result> flush(int32_t sensorHandle) override {
        return getSensors()->flush(sensorHandle);
    }

    Return<Result> injectSensorData(const Event& event) override {
        return getSensors()->injectSensorData(event);
    }

    Return<void> registerDirectChannel(const SharedMemInfo& mem,
                                       ISensors::registerDirectChannel_cb _hidl_cb) override;

    Return<Result> unregisterDirectChannel(int32_t channelHandle) override {
        return getSensors()->unregisterDirectChannel(channelHandle);
    }

    Return<void> configDirectReport(int32_t sensorHandle, int32_t channelHandle, RateLevel rate,
                                    ISensors::configDirectReport_cb _hidl_cb) override {
        return getSensors()->configDirectReport(sensorHandle, channelHandle, rate, _hidl_cb);
    }

    inline sp<::android::hardware::sensors::V2_0::ISensors>& getSensors() {
        return SensorsHidlEnvironmentV2_0::Instance()->mSensors;
    }

    SensorsHidlEnvironmentBase* getEnvironment() override {
        return SensorsHidlEnvironmentV2_0::Instance();
    }

    // Test helpers
    void runSingleFlushTest(const std::vector<SensorInfo>& sensors, bool activateSensor,
                            int32_t expectedFlushCount, Result expectedResponse);
    void runFlushTest(const std::vector<SensorInfo>& sensors, bool activateSensor,
                      int32_t flushCalls, int32_t expectedFlushCount, Result expectedResponse);

    // Helper functions
    void activateAllSensors(bool enable);
    std::vector<SensorInfo> getNonOneShotSensors();
    std::vector<SensorInfo> getOneShotSensors();
    std::vector<SensorInfo> getInjectEventSensors();
    int32_t getInvalidSensorHandle();
    bool getDirectChannelSensor(SensorInfo* sensor, SharedMemType* memType, RateLevel* rate);
    void verifyDirectChannel(SharedMemType memType);
    void verifyRegisterDirectChannel(const SensorInfo& sensor, SharedMemType memType,
                                     std::shared_ptr<SensorsTestSharedMemory> mem,
                                     int32_t* directChannelHandle);
    void verifyConfigure(const SensorInfo& sensor, SharedMemType memType,
                         int32_t directChannelHandle);
    void verifyUnregisterDirectChannel(const SensorInfo& sensor, SharedMemType memType,
                                       int32_t directChannelHandle);
    void checkRateLevel(const SensorInfo& sensor, int32_t directChannelHandle, RateLevel rateLevel);
};

Return<Result> SensorsHidlTest::activate(int32_t sensorHandle, bool enabled) {
    // If activating a sensor, add the handle in a set so that when test fails it can be turned off.
    // The handle is not removed when it is deactivating on purpose so that it is not necessary to
    // check the return value of deactivation. Deactivating a sensor more than once does not have
    // negative effect.
    if (enabled) {
        mSensorHandles.insert(sensorHandle);
    }
    return getSensors()->activate(sensorHandle, enabled);
}

Return<void> SensorsHidlTest::registerDirectChannel(const SharedMemInfo& mem,
                                                    ISensors::registerDirectChannel_cb cb) {
    // If registeration of a channel succeeds, add the handle of channel to a set so that it can be
    // unregistered when test fails. Unregister a channel does not remove the handle on purpose.
    // Unregistering a channel more than once should not have negative effect.
    getSensors()->registerDirectChannel(mem, [&](auto result, auto channelHandle) {
        if (result == Result::OK) {
            mDirectChannelHandles.insert(channelHandle);
        }
        cb(result, channelHandle);
    });
    return Void();
}

SensorInfo SensorsHidlTest::defaultSensorByType(SensorType type) {
    SensorInfo ret;

    ret.type = (SensorType)-1;
    getSensors()->getSensorsList([&](const auto& list) {
        const size_t count = list.size();
        for (size_t i = 0; i < count; ++i) {
            if (list[i].type == type) {
                ret = list[i];
                return;
            }
        }
    });

    return ret;
}

std::vector<SensorInfo> SensorsHidlTest::getSensorsList() {
    std::vector<SensorInfo> ret;

    getSensors()->getSensorsList([&](const auto& list) {
        const size_t count = list.size();
        ret.reserve(list.size());
        for (size_t i = 0; i < count; ++i) {
            ret.push_back(list[i]);
        }
    });

    return ret;
}

std::vector<SensorInfo> SensorsHidlTest::getNonOneShotSensors() {
    std::vector<SensorInfo> sensors;
    for (const SensorInfo& info : getSensorsList()) {
        if (extractReportMode(info.flags) != SensorFlagBits::ONE_SHOT_MODE) {
            sensors.push_back(info);
        }
    }
    return sensors;
}

std::vector<SensorInfo> SensorsHidlTest::getOneShotSensors() {
    std::vector<SensorInfo> sensors;
    for (const SensorInfo& info : getSensorsList()) {
        if (extractReportMode(info.flags) == SensorFlagBits::ONE_SHOT_MODE) {
            sensors.push_back(info);
        }
    }
    return sensors;
}

std::vector<SensorInfo> SensorsHidlTest::getInjectEventSensors() {
    std::vector<SensorInfo> sensors;
    for (const SensorInfo& info : getSensorsList()) {
        if (info.flags & static_cast<uint32_t>(SensorFlagBits::DATA_INJECTION)) {
            sensors.push_back(info);
        }
    }
    return sensors;
}

int32_t SensorsHidlTest::getInvalidSensorHandle() {
    // Find a sensor handle that does not exist in the sensor list
    int32_t maxHandle = 0;
    for (const SensorInfo& sensor : getSensorsList()) {
        maxHandle = max(maxHandle, sensor.sensorHandle);
    }
    return maxHandle + 1;
}

// Test if sensor list returned is valid
TEST_F(SensorsHidlTest, SensorListValid) {
    getSensors()->getSensorsList([&](const auto& list) {
        const size_t count = list.size();
        for (size_t i = 0; i < count; ++i) {
            const auto& s = list[i];
            SCOPED_TRACE(::testing::Message()
                         << i << "/" << count << ": "
                         << " handle=0x" << std::hex << std::setw(8) << std::setfill('0')
                         << s.sensorHandle << std::dec << " type=" << static_cast<int>(s.type)
                         << " name=" << s.name);

            // Test non-empty type string
            EXPECT_FALSE(s.typeAsString.empty());

            // Test defined type matches defined string type
            EXPECT_NO_FATAL_FAILURE(assertTypeMatchStringType(s.type, s.typeAsString));

            // Test if all sensor has name and vendor
            EXPECT_FALSE(s.name.empty());
            EXPECT_FALSE(s.vendor.empty());

            // Test power > 0, maxRange > 0
            EXPECT_LE(0, s.power);
            EXPECT_LT(0, s.maxRange);

            // Info type, should have no sensor
            EXPECT_FALSE(s.type == SensorType::ADDITIONAL_INFO || s.type == SensorType::META_DATA);

            // Test fifoMax >= fifoReserved
            EXPECT_GE(s.fifoMaxEventCount, s.fifoReservedEventCount)
                << "max=" << s.fifoMaxEventCount << " reserved=" << s.fifoReservedEventCount;

            // Test Reporting mode valid
            EXPECT_NO_FATAL_FAILURE(assertTypeMatchReportMode(s.type, extractReportMode(s.flags)));

            // Test min max are in the right order
            EXPECT_LE(s.minDelay, s.maxDelay);
            // Test min/max delay matches reporting mode
            EXPECT_NO_FATAL_FAILURE(
                assertDelayMatchReportMode(s.minDelay, s.maxDelay, extractReportMode(s.flags)));
        }
    });
}

// Test that SetOperationMode returns the expected value
TEST_F(SensorsHidlTest, SetOperationMode) {
    std::vector<SensorInfo> sensors = getInjectEventSensors();
    if (getInjectEventSensors().size() > 0) {
        ASSERT_EQ(Result::OK, getSensors()->setOperationMode(OperationMode::NORMAL));
        ASSERT_EQ(Result::OK, getSensors()->setOperationMode(OperationMode::DATA_INJECTION));
        ASSERT_EQ(Result::OK, getSensors()->setOperationMode(OperationMode::NORMAL));
    } else {
        ASSERT_EQ(Result::BAD_VALUE, getSensors()->setOperationMode(OperationMode::DATA_INJECTION));
    }
}

// Test that an injected event is written back to the Event FMQ
TEST_F(SensorsHidlTest, InjectSensorEventData) {
    std::vector<SensorInfo> sensors = getInjectEventSensors();
    if (sensors.size() == 0) {
        return;
    }

    ASSERT_EQ(Result::OK, getSensors()->setOperationMode(OperationMode::DATA_INJECTION));

    EventCallback callback;
    getEnvironment()->registerCallback(&callback);

    // AdditionalInfo event should not be sent to Event FMQ
    Event additionalInfoEvent;
    additionalInfoEvent.sensorType = SensorType::ADDITIONAL_INFO;
    additionalInfoEvent.timestamp = android::elapsedRealtimeNano();

    Event injectedEvent;
    injectedEvent.timestamp = android::elapsedRealtimeNano();
    Vec3 data = {1, 2, 3, SensorStatus::ACCURACY_HIGH};
    injectedEvent.u.vec3 = data;

    for (const auto& s : sensors) {
        additionalInfoEvent.sensorHandle = s.sensorHandle;
        EXPECT_EQ(Result::OK, getSensors()->injectSensorData(additionalInfoEvent));

        injectedEvent.sensorType = s.type;
        injectedEvent.sensorHandle = s.sensorHandle;
        EXPECT_EQ(Result::OK, getSensors()->injectSensorData(injectedEvent));
    }

    // Wait for events to be written back to the Event FMQ
    callback.waitForEvents(sensors, 1000 /* timeoutMs */);

    for (const auto& s : sensors) {
        auto events = callback.getEvents(s.sensorHandle);
        auto lastEvent = events.back();

        // Verify that only a single event has been received
        ASSERT_EQ(events.size(), 1);

        // Verify that the event received matches the event injected and is not the additional
        // info event
        ASSERT_EQ(lastEvent.sensorType, s.type);
        ASSERT_EQ(lastEvent.sensorType, s.type);
        ASSERT_EQ(lastEvent.timestamp, injectedEvent.timestamp);
        ASSERT_EQ(lastEvent.u.vec3.x, injectedEvent.u.vec3.x);
        ASSERT_EQ(lastEvent.u.vec3.y, injectedEvent.u.vec3.y);
        ASSERT_EQ(lastEvent.u.vec3.z, injectedEvent.u.vec3.z);
        ASSERT_EQ(lastEvent.u.vec3.status, injectedEvent.u.vec3.status);
    }

    getEnvironment()->unregisterCallback();
    ASSERT_EQ(Result::OK, getSensors()->setOperationMode(OperationMode::NORMAL));
}

// Test if sensor hal can do UI speed accelerometer streaming properly
TEST_F(SensorsHidlTest, AccelerometerStreamingOperationSlow) {
    testStreamingOperation(SensorType::ACCELEROMETER, std::chrono::milliseconds(200),
                           std::chrono::seconds(5), sAccelNormChecker);
}

// Test if sensor hal can do normal speed accelerometer streaming properly
TEST_F(SensorsHidlTest, AccelerometerStreamingOperationNormal) {
    testStreamingOperation(SensorType::ACCELEROMETER, std::chrono::milliseconds(20),
                           std::chrono::seconds(5), sAccelNormChecker);
}

// Test if sensor hal can do game speed accelerometer streaming properly
TEST_F(SensorsHidlTest, AccelerometerStreamingOperationFast) {
    testStreamingOperation(SensorType::ACCELEROMETER, std::chrono::milliseconds(5),
                           std::chrono::seconds(5), sAccelNormChecker);
}

// Test if sensor hal can do UI speed gyroscope streaming properly
TEST_F(SensorsHidlTest, GyroscopeStreamingOperationSlow) {
    testStreamingOperation(SensorType::GYROSCOPE, std::chrono::milliseconds(200),
                           std::chrono::seconds(5), sGyroNormChecker);
}

// Test if sensor hal can do normal speed gyroscope streaming properly
TEST_F(SensorsHidlTest, GyroscopeStreamingOperationNormal) {
    testStreamingOperation(SensorType::GYROSCOPE, std::chrono::milliseconds(20),
                           std::chrono::seconds(5), sGyroNormChecker);
}

// Test if sensor hal can do game speed gyroscope streaming properly
TEST_F(SensorsHidlTest, GyroscopeStreamingOperationFast) {
    testStreamingOperation(SensorType::GYROSCOPE, std::chrono::milliseconds(5),
                           std::chrono::seconds(5), sGyroNormChecker);
}

// Test if sensor hal can do UI speed magnetometer streaming properly
TEST_F(SensorsHidlTest, MagnetometerStreamingOperationSlow) {
    testStreamingOperation(SensorType::MAGNETIC_FIELD, std::chrono::milliseconds(200),
                           std::chrono::seconds(5), NullChecker());
}

// Test if sensor hal can do normal speed magnetometer streaming properly
TEST_F(SensorsHidlTest, MagnetometerStreamingOperationNormal) {
    testStreamingOperation(SensorType::MAGNETIC_FIELD, std::chrono::milliseconds(20),
                           std::chrono::seconds(5), NullChecker());
}

// Test if sensor hal can do game speed magnetometer streaming properly
TEST_F(SensorsHidlTest, MagnetometerStreamingOperationFast) {
    testStreamingOperation(SensorType::MAGNETIC_FIELD, std::chrono::milliseconds(5),
                           std::chrono::seconds(5), NullChecker());
}

// Test if sensor hal can do accelerometer sampling rate switch properly when sensor is active
TEST_F(SensorsHidlTest, AccelerometerSamplingPeriodHotSwitchOperation) {
    testSamplingRateHotSwitchOperation(SensorType::ACCELEROMETER);
    testSamplingRateHotSwitchOperation(SensorType::ACCELEROMETER, false /*fastToSlow*/);
}

// Test if sensor hal can do gyroscope sampling rate switch properly when sensor is active
TEST_F(SensorsHidlTest, GyroscopeSamplingPeriodHotSwitchOperation) {
    testSamplingRateHotSwitchOperation(SensorType::GYROSCOPE);
    testSamplingRateHotSwitchOperation(SensorType::GYROSCOPE, false /*fastToSlow*/);
}

// Test if sensor hal can do magnetometer sampling rate switch properly when sensor is active
TEST_F(SensorsHidlTest, MagnetometerSamplingPeriodHotSwitchOperation) {
    testSamplingRateHotSwitchOperation(SensorType::MAGNETIC_FIELD);
    testSamplingRateHotSwitchOperation(SensorType::MAGNETIC_FIELD, false /*fastToSlow*/);
}

// Test if sensor hal can do accelerometer batching properly
TEST_F(SensorsHidlTest, AccelerometerBatchingOperation) {
    testBatchingOperation(SensorType::ACCELEROMETER);
}

// Test if sensor hal can do gyroscope batching properly
TEST_F(SensorsHidlTest, GyroscopeBatchingOperation) {
    testBatchingOperation(SensorType::GYROSCOPE);
}

// Test if sensor hal can do magnetometer batching properly
TEST_F(SensorsHidlTest, MagnetometerBatchingOperation) {
    testBatchingOperation(SensorType::MAGNETIC_FIELD);
}

// Test sensor event direct report with ashmem for accel sensor at normal rate
TEST_F(SensorsHidlTest, AccelerometerAshmemDirectReportOperationNormal) {
    testDirectReportOperation(SensorType::ACCELEROMETER, SharedMemType::ASHMEM, RateLevel::NORMAL,
                              sAccelNormChecker);
}

// Test sensor event direct report with ashmem for accel sensor at fast rate
TEST_F(SensorsHidlTest, AccelerometerAshmemDirectReportOperationFast) {
    testDirectReportOperation(SensorType::ACCELEROMETER, SharedMemType::ASHMEM, RateLevel::FAST,
                              sAccelNormChecker);
}

// Test sensor event direct report with ashmem for accel sensor at very fast rate
TEST_F(SensorsHidlTest, AccelerometerAshmemDirectReportOperationVeryFast) {
    testDirectReportOperation(SensorType::ACCELEROMETER, SharedMemType::ASHMEM,
                              RateLevel::VERY_FAST, sAccelNormChecker);
}

// Test sensor event direct report with ashmem for gyro sensor at normal rate
TEST_F(SensorsHidlTest, GyroscopeAshmemDirectReportOperationNormal) {
    testDirectReportOperation(SensorType::GYROSCOPE, SharedMemType::ASHMEM, RateLevel::NORMAL,
                              sGyroNormChecker);
}

// Test sensor event direct report with ashmem for gyro sensor at fast rate
TEST_F(SensorsHidlTest, GyroscopeAshmemDirectReportOperationFast) {
    testDirectReportOperation(SensorType::GYROSCOPE, SharedMemType::ASHMEM, RateLevel::FAST,
                              sGyroNormChecker);
}

// Test sensor event direct report with ashmem for gyro sensor at very fast rate
TEST_F(SensorsHidlTest, GyroscopeAshmemDirectReportOperationVeryFast) {
    testDirectReportOperation(SensorType::GYROSCOPE, SharedMemType::ASHMEM, RateLevel::VERY_FAST,
                              sGyroNormChecker);
}

// Test sensor event direct report with ashmem for mag sensor at normal rate
TEST_F(SensorsHidlTest, MagnetometerAshmemDirectReportOperationNormal) {
    testDirectReportOperation(SensorType::MAGNETIC_FIELD, SharedMemType::ASHMEM, RateLevel::NORMAL,
                              NullChecker());
}

// Test sensor event direct report with ashmem for mag sensor at fast rate
TEST_F(SensorsHidlTest, MagnetometerAshmemDirectReportOperationFast) {
    testDirectReportOperation(SensorType::MAGNETIC_FIELD, SharedMemType::ASHMEM, RateLevel::FAST,
                              NullChecker());
}

// Test sensor event direct report with ashmem for mag sensor at very fast rate
TEST_F(SensorsHidlTest, MagnetometerAshmemDirectReportOperationVeryFast) {
    testDirectReportOperation(SensorType::MAGNETIC_FIELD, SharedMemType::ASHMEM,
                              RateLevel::VERY_FAST, NullChecker());
}

// Test sensor event direct report with gralloc for accel sensor at normal rate
TEST_F(SensorsHidlTest, AccelerometerGrallocDirectReportOperationNormal) {
    testDirectReportOperation(SensorType::ACCELEROMETER, SharedMemType::GRALLOC, RateLevel::NORMAL,
                              sAccelNormChecker);
}

// Test sensor event direct report with gralloc for accel sensor at fast rate
TEST_F(SensorsHidlTest, AccelerometerGrallocDirectReportOperationFast) {
    testDirectReportOperation(SensorType::ACCELEROMETER, SharedMemType::GRALLOC, RateLevel::FAST,
                              sAccelNormChecker);
}

// Test sensor event direct report with gralloc for accel sensor at very fast rate
TEST_F(SensorsHidlTest, AccelerometerGrallocDirectReportOperationVeryFast) {
    testDirectReportOperation(SensorType::ACCELEROMETER, SharedMemType::GRALLOC,
                              RateLevel::VERY_FAST, sAccelNormChecker);
}

// Test sensor event direct report with gralloc for gyro sensor at normal rate
TEST_F(SensorsHidlTest, GyroscopeGrallocDirectReportOperationNormal) {
    testDirectReportOperation(SensorType::GYROSCOPE, SharedMemType::GRALLOC, RateLevel::NORMAL,
                              sGyroNormChecker);
}

// Test sensor event direct report with gralloc for gyro sensor at fast rate
TEST_F(SensorsHidlTest, GyroscopeGrallocDirectReportOperationFast) {
    testDirectReportOperation(SensorType::GYROSCOPE, SharedMemType::GRALLOC, RateLevel::FAST,
                              sGyroNormChecker);
}

// Test sensor event direct report with gralloc for gyro sensor at very fast rate
TEST_F(SensorsHidlTest, GyroscopeGrallocDirectReportOperationVeryFast) {
    testDirectReportOperation(SensorType::GYROSCOPE, SharedMemType::GRALLOC, RateLevel::VERY_FAST,
                              sGyroNormChecker);
}

// Test sensor event direct report with gralloc for mag sensor at normal rate
TEST_F(SensorsHidlTest, MagnetometerGrallocDirectReportOperationNormal) {
    testDirectReportOperation(SensorType::MAGNETIC_FIELD, SharedMemType::GRALLOC, RateLevel::NORMAL,
                              NullChecker());
}

// Test sensor event direct report with gralloc for mag sensor at fast rate
TEST_F(SensorsHidlTest, MagnetometerGrallocDirectReportOperationFast) {
    testDirectReportOperation(SensorType::MAGNETIC_FIELD, SharedMemType::GRALLOC, RateLevel::FAST,
                              NullChecker());
}

// Test sensor event direct report with gralloc for mag sensor at very fast rate
TEST_F(SensorsHidlTest, MagnetometerGrallocDirectReportOperationVeryFast) {
    testDirectReportOperation(SensorType::MAGNETIC_FIELD, SharedMemType::GRALLOC,
                              RateLevel::VERY_FAST, NullChecker());
}

void SensorsHidlTest::activateAllSensors(bool enable) {
    for (const SensorInfo& sensorInfo : getSensorsList()) {
        if (isValidType(sensorInfo.type)) {
            batch(sensorInfo.sensorHandle, sensorInfo.minDelay, 0 /* maxReportLatencyNs */);
            activate(sensorInfo.sensorHandle, enable);
        }
    }
}

// Test that if initialize is called twice, then the HAL writes events to the FMQs from the second
// call to the function.
TEST_F(SensorsHidlTest, CallInitializeTwice) {
    // Create a helper class so that a second environment is able to be instantiated
    class SensorsHidlEnvironmentTest : public SensorsHidlEnvironmentV2_0 {};

    if (getSensorsList().size() == 0) {
        // No sensors
        return;
    }

    constexpr useconds_t kCollectionTimeoutUs = 1000 * 1000;  // 1s
    constexpr int32_t kNumEvents = 1;

    // Create a new environment that calls initialize()
    std::unique_ptr<SensorsHidlEnvironmentTest> newEnv =
        std::make_unique<SensorsHidlEnvironmentTest>();
    newEnv->HidlSetUp();

    activateAllSensors(true);
    // Verify that the old environment does not receive any events
    ASSERT_EQ(collectEvents(kCollectionTimeoutUs, kNumEvents, getEnvironment()).size(), 0);
    // Verify that the new event queue receives sensor events
    ASSERT_GE(collectEvents(kCollectionTimeoutUs, kNumEvents, newEnv.get()).size(), kNumEvents);
    activateAllSensors(false);

    // Cleanup the test environment
    newEnv->HidlTearDown();

    // Restore the test environment for future tests
    SensorsHidlEnvironmentV2_0::Instance()->HidlTearDown();
    SensorsHidlEnvironmentV2_0::Instance()->HidlSetUp();

    // Ensure that the original environment is receiving events
    activateAllSensors(true);
    ASSERT_GE(collectEvents(kCollectionTimeoutUs, kNumEvents).size(), kNumEvents);
    activateAllSensors(false);
}

TEST_F(SensorsHidlTest, CleanupConnectionsOnInitialize) {
    activateAllSensors(true);

    // Verify that events are received
    constexpr useconds_t kCollectionTimeoutUs = 1000 * 1000;  // 1s
    constexpr int32_t kNumEvents = 1;
    ASSERT_GE(collectEvents(kCollectionTimeoutUs, kNumEvents, getEnvironment()).size(), kNumEvents);

    // Clear the active sensor handles so they are not disabled during TearDown
    auto handles = mSensorHandles;
    mSensorHandles.clear();
    getEnvironment()->TearDown();
    getEnvironment()->SetUp();

    // Verify no events are received until sensors are re-activated
    ASSERT_EQ(collectEvents(kCollectionTimeoutUs, kNumEvents, getEnvironment()).size(), 0);
    activateAllSensors(true);
    ASSERT_GE(collectEvents(kCollectionTimeoutUs, kNumEvents, getEnvironment()).size(), kNumEvents);

    // Disable sensors
    activateAllSensors(false);

    // Restore active sensors prior to clearing the environment
    mSensorHandles = handles;
}

void SensorsHidlTest::runSingleFlushTest(const std::vector<SensorInfo>& sensors,
                                         bool activateSensor, int32_t expectedFlushCount,
                                         Result expectedResponse) {
    runFlushTest(sensors, activateSensor, 1 /* flushCalls */, expectedFlushCount, expectedResponse);
}

void SensorsHidlTest::runFlushTest(const std::vector<SensorInfo>& sensors, bool activateSensor,
                                   int32_t flushCalls, int32_t expectedFlushCount,
                                   Result expectedResponse) {
    EventCallback callback;
    getEnvironment()->registerCallback(&callback);

    for (const SensorInfo& sensor : sensors) {
        // Configure and activate the sensor
        batch(sensor.sensorHandle, sensor.maxDelay, 0 /* maxReportLatencyNs */);
        activate(sensor.sensorHandle, activateSensor);

        // Flush the sensor
        for (int32_t i = 0; i < flushCalls; i++) {
            Result flushResult = flush(sensor.sensorHandle);
            ASSERT_EQ(flushResult, expectedResponse);
        }
    }

    // Wait up to one second for the flush events
    callback.waitForFlushEvents(sensors, flushCalls, 1000 /* timeoutMs */);

    // Deactivate all sensors after waiting for flush events so pending flush events are not
    // abandoned by the HAL.
    for (const SensorInfo& sensor : sensors) {
        activate(sensor.sensorHandle, false);
    }
    getEnvironment()->unregisterCallback();

    // Check that the correct number of flushes are present for each sensor
    for (const SensorInfo& sensor : sensors) {
        ASSERT_EQ(callback.getFlushCount(sensor.sensorHandle), expectedFlushCount);
    }
}

TEST_F(SensorsHidlTest, FlushSensor) {
    // Find a sensor that is not a one-shot sensor
    std::vector<SensorInfo> sensors = getNonOneShotSensors();
    if (sensors.size() == 0) {
        return;
    }

    constexpr int32_t kFlushes = 5;
    runSingleFlushTest(sensors, true /* activateSensor */, 1 /* expectedFlushCount */, Result::OK);
    runFlushTest(sensors, true /* activateSensor */, kFlushes, kFlushes, Result::OK);
}

TEST_F(SensorsHidlTest, FlushOneShotSensor) {
    // Find a sensor that is a one-shot sensor
    std::vector<SensorInfo> sensors = getOneShotSensors();
    if (sensors.size() == 0) {
        return;
    }

    runSingleFlushTest(sensors, true /* activateSensor */, 0 /* expectedFlushCount */,
                       Result::BAD_VALUE);
}

TEST_F(SensorsHidlTest, FlushInactiveSensor) {
    // Attempt to find a non-one shot sensor, then a one-shot sensor if necessary
    std::vector<SensorInfo> sensors = getNonOneShotSensors();
    if (sensors.size() == 0) {
        sensors = getOneShotSensors();
        if (sensors.size() == 0) {
            return;
        }
    }

    runSingleFlushTest(sensors, false /* activateSensor */, 0 /* expectedFlushCount */,
                       Result::BAD_VALUE);
}

TEST_F(SensorsHidlTest, FlushNonexistentSensor) {
    SensorInfo sensor;
    std::vector<SensorInfo> sensors = getNonOneShotSensors();
    if (sensors.size() == 0) {
        sensors = getOneShotSensors();
        if (sensors.size() == 0) {
            return;
        }
    }
    sensor = sensors.front();
    sensor.sensorHandle = getInvalidSensorHandle();
    runSingleFlushTest(std::vector<SensorInfo>{sensor}, false /* activateSensor */,
                       0 /* expectedFlushCount */, Result::BAD_VALUE);
}

TEST_F(SensorsHidlTest, Batch) {
    if (getSensorsList().size() == 0) {
        return;
    }

    activateAllSensors(false /* enable */);
    for (const SensorInfo& sensor : getSensorsList()) {
        // Call batch on inactive sensor
        ASSERT_EQ(batch(sensor.sensorHandle, sensor.minDelay, 0 /* maxReportLatencyNs */),
                  Result::OK);

        // Activate the sensor
        activate(sensor.sensorHandle, true /* enabled */);

        // Call batch on an active sensor
        ASSERT_EQ(batch(sensor.sensorHandle, sensor.maxDelay, 0 /* maxReportLatencyNs */),
                  Result::OK);
    }
    activateAllSensors(false /* enable */);

    // Call batch on an invalid sensor
    SensorInfo sensor = getSensorsList().front();
    sensor.sensorHandle = getInvalidSensorHandle();
    ASSERT_EQ(batch(sensor.sensorHandle, sensor.minDelay, 0 /* maxReportLatencyNs */),
              Result::BAD_VALUE);
}

TEST_F(SensorsHidlTest, Activate) {
    if (getSensorsList().size() == 0) {
        return;
    }

    // Verify that sensor events are generated when activate is called
    for (const SensorInfo& sensor : getSensorsList()) {
        batch(sensor.sensorHandle, sensor.minDelay, 0 /* maxReportLatencyNs */);
        ASSERT_EQ(activate(sensor.sensorHandle, true), Result::OK);

        // Call activate on a sensor that is already activated
        ASSERT_EQ(activate(sensor.sensorHandle, true), Result::OK);

        // Deactivate the sensor
        ASSERT_EQ(activate(sensor.sensorHandle, false), Result::OK);

        // Call deactivate on a sensor that is already deactivated
        ASSERT_EQ(activate(sensor.sensorHandle, false), Result::OK);
    }

    // Attempt to activate an invalid sensor
    int32_t invalidHandle = getInvalidSensorHandle();
    ASSERT_EQ(activate(invalidHandle, true), Result::BAD_VALUE);
    ASSERT_EQ(activate(invalidHandle, false), Result::BAD_VALUE);
}

TEST_F(SensorsHidlTest, NoStaleEvents) {
    constexpr int64_t kFiveHundredMilliseconds = 500 * 1000;
    constexpr int64_t kOneSecond = 1000 * 1000;

    // Register the callback to receive sensor events
    EventCallback callback;
    getEnvironment()->registerCallback(&callback);

    const std::vector<SensorInfo> sensors = getSensorsList();
    int32_t maxMinDelay = 0;
    for (const SensorInfo& sensor : getSensorsList()) {
        maxMinDelay = std::max(maxMinDelay, sensor.minDelay);
    }

    // Activate the sensors so that they start generating events
    activateAllSensors(true);

    // According to the CDD, the first sample must be generated within 400ms + 2 * sample_time
    // and the maximum reporting latency is 100ms + 2 * sample_time. Wait a sufficient amount
    // of time to guarantee that a sample has arrived.
    callback.waitForEvents(sensors, kFiveHundredMilliseconds + (5 * maxMinDelay));
    activateAllSensors(false);

    // Save the last received event for each sensor
    std::map<int32_t, int64_t> lastEventTimestampMap;
    for (const SensorInfo& sensor : sensors) {
        ASSERT_GE(callback.getEvents(sensor.sensorHandle).size(), 1);
        lastEventTimestampMap[sensor.sensorHandle] =
            callback.getEvents(sensor.sensorHandle).back().timestamp;
    }

    // Allow some time to pass, reset the callback, then reactivate the sensors
    usleep(kOneSecond + (5 * maxMinDelay));
    callback.reset();
    activateAllSensors(true);
    callback.waitForEvents(sensors, kFiveHundredMilliseconds + (5 * maxMinDelay));
    activateAllSensors(false);

    for (const SensorInfo& sensor : sensors) {
        // Ensure that the first event received is not stale by ensuring that its timestamp is
        // sufficiently different from the previous event
        const Event newEvent = callback.getEvents(sensor.sensorHandle).front();
        int64_t delta = newEvent.timestamp - lastEventTimestampMap[sensor.sensorHandle];
        ASSERT_GE(delta, kFiveHundredMilliseconds + (3 * sensor.minDelay));
    }

    getEnvironment()->unregisterCallback();
}

void SensorsHidlTest::checkRateLevel(const SensorInfo& sensor, int32_t directChannelHandle,
                                     RateLevel rateLevel) {
    configDirectReport(sensor.sensorHandle, directChannelHandle, rateLevel,
                       [&](Result result, int32_t reportToken) {
                           if (isDirectReportRateSupported(sensor, rateLevel)) {
                               ASSERT_EQ(result, Result::OK);
                               ASSERT_GT(reportToken, 0);
                           } else {
                               ASSERT_EQ(result, Result::BAD_VALUE);
                           }
                       });
}

void SensorsHidlTest::verifyRegisterDirectChannel(const SensorInfo& sensor, SharedMemType memType,
                                                  std::shared_ptr<SensorsTestSharedMemory> mem,
                                                  int32_t* directChannelHandle) {
    char* buffer = mem->getBuffer();
    memset(buffer, 0xff, mem->getSize());

    registerDirectChannel(mem->getSharedMemInfo(), [&](Result result, int32_t channelHandle) {
        if (isDirectChannelTypeSupported(sensor, memType)) {
            ASSERT_EQ(result, Result::OK);
            ASSERT_GT(channelHandle, 0);

            // Verify that the memory has been zeroed
            for (size_t i = 0; i < mem->getSize(); i++) {
                ASSERT_EQ(buffer[i], 0x00);
            }
        } else {
            ASSERT_EQ(result, Result::INVALID_OPERATION);
            ASSERT_EQ(channelHandle, -1);
        }
        *directChannelHandle = channelHandle;
    });
}

void SensorsHidlTest::verifyConfigure(const SensorInfo& sensor, SharedMemType memType,
                                      int32_t directChannelHandle) {
    if (isDirectChannelTypeSupported(sensor, memType)) {
        // Verify that each rate level is properly supported
        checkRateLevel(sensor, directChannelHandle, RateLevel::NORMAL);
        checkRateLevel(sensor, directChannelHandle, RateLevel::FAST);
        checkRateLevel(sensor, directChannelHandle, RateLevel::VERY_FAST);
        checkRateLevel(sensor, directChannelHandle, RateLevel::STOP);

        // Verify that a sensor handle of -1 is only acceptable when using RateLevel::STOP
        configDirectReport(
            -1 /* sensorHandle */, directChannelHandle, RateLevel::NORMAL,
            [](Result result, int32_t /* reportToken */) { ASSERT_EQ(result, Result::BAD_VALUE); });
        configDirectReport(
            -1 /* sensorHandle */, directChannelHandle, RateLevel::STOP,
            [](Result result, int32_t /* reportToken */) { ASSERT_EQ(result, Result::OK); });
    } else {
        // Direct channel is not supported for this SharedMemType
        configDirectReport(sensor.sensorHandle, directChannelHandle, RateLevel::NORMAL,
                           [](Result result, int32_t /* reportToken */) {
                               ASSERT_EQ(result, Result::INVALID_OPERATION);
                           });
    }
}

void SensorsHidlTest::verifyUnregisterDirectChannel(const SensorInfo& sensor, SharedMemType memType,
                                                    int32_t directChannelHandle) {
    Result result = unregisterDirectChannel(directChannelHandle);
    if (isDirectChannelTypeSupported(sensor, memType)) {
        ASSERT_EQ(result, Result::OK);
    } else {
        ASSERT_EQ(result, Result::INVALID_OPERATION);
    }
}

void SensorsHidlTest::verifyDirectChannel(SharedMemType memType) {
    constexpr size_t kNumEvents = 1;
    constexpr size_t kMemSize = kNumEvents * kEventSize;

    std::shared_ptr<SensorsTestSharedMemory> mem(
        SensorsTestSharedMemory::create(memType, kMemSize));
    ASSERT_NE(mem, nullptr);

    for (const SensorInfo& sensor : getSensorsList()) {
        int32_t directChannelHandle = 0;
        verifyRegisterDirectChannel(sensor, memType, mem, &directChannelHandle);
        verifyConfigure(sensor, memType, directChannelHandle);
        verifyUnregisterDirectChannel(sensor, memType, directChannelHandle);
    }
}

TEST_F(SensorsHidlTest, DirectChannelAshmem) {
    verifyDirectChannel(SharedMemType::ASHMEM);
}

TEST_F(SensorsHidlTest, DirectChannelGralloc) {
    verifyDirectChannel(SharedMemType::GRALLOC);
}

bool SensorsHidlTest::getDirectChannelSensor(SensorInfo* sensor, SharedMemType* memType,
                                             RateLevel* rate) {
    bool found = false;
    for (const SensorInfo& curSensor : getSensorsList()) {
        if (isDirectChannelTypeSupported(curSensor, SharedMemType::ASHMEM)) {
            *memType = SharedMemType::ASHMEM;
            *sensor = curSensor;
            found = true;
            break;
        } else if (isDirectChannelTypeSupported(curSensor, SharedMemType::GRALLOC)) {
            *memType = SharedMemType::GRALLOC;
            *sensor = curSensor;
            found = true;
            break;
        }
    }

    if (found) {
        // Find a supported rate level
        constexpr int kNumRateLevels = 3;
        RateLevel rates[kNumRateLevels] = {RateLevel::NORMAL, RateLevel::FAST,
                                           RateLevel::VERY_FAST};
        *rate = RateLevel::STOP;
        for (int i = 0; i < kNumRateLevels; i++) {
            if (isDirectReportRateSupported(*sensor, rates[i])) {
                *rate = rates[i];
            }
        }

        // At least one rate level must be supported
        EXPECT_NE(*rate, RateLevel::STOP);
    }
    return found;
}

TEST_F(SensorsHidlTest, ConfigureDirectChannelWithInvalidHandle) {
    SensorInfo sensor;
    SharedMemType memType;
    RateLevel rate;
    if (!getDirectChannelSensor(&sensor, &memType, &rate)) {
        return;
    }

    // Verify that an invalid channel handle produces a BAD_VALUE result
    configDirectReport(sensor.sensorHandle, -1, rate, [](Result result, int32_t /* reportToken */) {
        ASSERT_EQ(result, Result::BAD_VALUE);
    });
}

TEST_F(SensorsHidlTest, CleanupDirectConnectionOnInitialize) {
    constexpr size_t kNumEvents = 1;
    constexpr size_t kMemSize = kNumEvents * kEventSize;

    SensorInfo sensor;
    SharedMemType memType;
    RateLevel rate;

    if (!getDirectChannelSensor(&sensor, &memType, &rate)) {
        return;
    }

    std::shared_ptr<SensorsTestSharedMemory> mem(
        SensorsTestSharedMemory::create(memType, kMemSize));
    ASSERT_NE(mem, nullptr);

    int32_t directChannelHandle = 0;
    registerDirectChannel(mem->getSharedMemInfo(), [&](Result result, int32_t channelHandle) {
        ASSERT_EQ(result, Result::OK);
        directChannelHandle = channelHandle;
    });

    // Configure the channel and expect success
    configDirectReport(
        sensor.sensorHandle, directChannelHandle, rate,
        [](Result result, int32_t /* reportToken */) { ASSERT_EQ(result, Result::OK); });

    // Call initialize() via the environment setup to cause the HAL to re-initialize
    // Clear the active direct connections so they are not stopped during TearDown
    auto handles = mDirectChannelHandles;
    mDirectChannelHandles.clear();
    getEnvironment()->TearDown();
    getEnvironment()->SetUp();

    // Attempt to configure the direct channel and expect it to fail
    configDirectReport(
        sensor.sensorHandle, directChannelHandle, rate,
        [](Result result, int32_t /* reportToken */) { ASSERT_EQ(result, Result::BAD_VALUE); });

    // Restore original handles, though they should already be deactivated
    mDirectChannelHandles = handles;
}

int main(int argc, char** argv) {
    ::testing::AddGlobalTestEnvironment(SensorsHidlEnvironmentV2_0::Instance());
    ::testing::InitGoogleTest(&argc, argv);
    SensorsHidlEnvironmentV2_0::Instance()->init(&argc, argv);
    int status = RUN_ALL_TESTS();
    ALOGI("Test result = %d", status);
    return status;
}
// vim: set ts=2 sw=2
