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

#ifndef ANDROID_BIOMETRIC_SENSOR_H
#define ANDROID_BIOMETRIC_SENSOR_H

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>


#include "nusensors.h"
#include "SensorBase.h"
#include "SensorEventReader.h"
#include <linux/hwmsensor.h>

/*****************************************************************************/

#define MAX_BIOMETRIC_SUPPORT 3

class BiometricSensor : public SensorBase {
    enum biometric_handle {
        ekg,
        ppg1,
        ppg2,
    };
    int mEnabled[MAX_BIOMETRIC_SUPPORT];
    SensorEventCircularReader mSensorReader;
    int64_t mEnabledTime[MAX_BIOMETRIC_SUPPORT];
    uint32_t mFlushCnt[MAX_BIOMETRIC_SUPPORT];
    char input_sysfs_path[PATH_MAX];
    int input_sysfs_path_len;
    int mDataDiv[MAX_BIOMETRIC_SUPPORT];
    float sn[MAX_BIOMETRIC_SUPPORT];

public:
    int mdata_fd;
    sensors_event_t mPendingEvent;

            BiometricSensor();
    virtual ~BiometricSensor();

    virtual int readEvents(sensors_event_t* data, int count);
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual int enable(int32_t handle, int enabled);
    virtual int batch(int handle, int flags, int64_t samplingPeriodNs, int64_t maxBatchReportLatencyNs);
    virtual int flush(int handle);
    void processEvent(struct sensor_event const *event);
    int FindDataFd();
    int HandleToIndex(int handle);
};

/*****************************************************************************/

#endif  // ANDROID_BIOMETRIC_SENSOR_H
