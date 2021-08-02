/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2012. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef __SITUATION_SENSOR_H__
#define __SITUATION_SENSOR_H__

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "SensorContext.h"
#include "SensorBase.h"
#include "SensorEventReader.h"
#include <linux/hwmsensor.h>

class SituationSensor : public SensorBase {
private:
    enum {
        inpocket,
        stationary,
        wake_gesture,
        pickup_gesture,
        glance_gesture,
        answer_call,
        motion_detect,
        device_orientation,
        tilt_detector,
#ifdef CUSTOM_KERNEL_FLAT_SENSOR
        flat,
#endif
        sar,
        max_situation,
    };
    SensorEventCircularReader mSensorReader;
    bool mEnabled[max_situation];
    sensors_event_t mPendingEvent;
    int64_t mEnabledTime[max_situation];
    char input_sysfs_path[PATH_MAX];
    int input_sysfs_path_len;

    void processEvent(struct sensor_event const *event);
    int HandleToIndex(int handle);

public:
    SituationSensor();
    virtual ~SituationSensor();
    virtual int readEvents(sensors_event_t* data, int count);
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual int enable(int32_t handle, int enabled);
    virtual int batch(int handle, int flags, int64_t samplingPeriodNs, int64_t maxBatchReportLatencyNs);
    virtual int flush(int handle);
    virtual int getFd() {
        return mSensorReader.getReadFd();
    };
    virtual bool pendingEvent(void);
};

#endif
