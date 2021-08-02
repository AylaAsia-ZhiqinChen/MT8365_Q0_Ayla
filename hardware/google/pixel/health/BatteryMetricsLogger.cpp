
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

#include <android/frameworks/stats/1.0/IStats.h>
#include <pixelhealth/BatteryMetricsLogger.h>

namespace hardware {
namespace google {
namespace pixel {
namespace health {

using android::sp;
using android::frameworks::stats::V1_0::BatteryHealthSnapshotArgs;
using android::frameworks::stats::V1_0::IStats;

BatteryMetricsLogger::BatteryMetricsLogger(const char *const batt_res, const char *const batt_ocv,
                                           int sample_period, int upload_period)
    : kBatteryResistance(batt_res),
      kBatteryOCV(batt_ocv),
      kSamplePeriod(sample_period),
      kUploadPeriod(upload_period),
      kMaxSamples(upload_period / sample_period) {
    last_sample_ = 0;
    last_upload_ = 0;
    num_res_samples_ = 0;
    num_samples_ = 0;
    memset(min_, 0, sizeof(min_));
    memset(max_, 0, sizeof(max_));
    accum_resistance_ = 0;
}

int64_t BatteryMetricsLogger::getTime(void) {
    return nanoseconds_to_seconds(systemTime(SYSTEM_TIME_BOOTTIME));
}

bool BatteryMetricsLogger::uploadOutlierMetric(sp<IStats> stats_client, sampleType type) {
    BatteryHealthSnapshotArgs min_stats_ss = {
            .type = static_cast<BatteryHealthSnapshotArgs::BatterySnapshotType>(0),
            .temperatureDeciC = min_[type][TEMP],
            .voltageMicroV = min_[type][VOLT],
            .currentMicroA = min_[type][CURR],
            .openCircuitVoltageMicroV = min_[type][OCV],
            .resistanceMicroOhm = min_[type][RES],
            .levelPercent = min_[type][SOC]};
    BatteryHealthSnapshotArgs max_stats_ss = {
            .type = static_cast<BatteryHealthSnapshotArgs::BatterySnapshotType>(0),
            .temperatureDeciC = max_[type][TEMP],
            .voltageMicroV = max_[type][VOLT],
            .currentMicroA = max_[type][CURR],
            .openCircuitVoltageMicroV = max_[type][OCV],
            .resistanceMicroOhm = max_[type][RES],
            .levelPercent = max_[type][SOC]};
    if (kStatsSnapshotType[type] < 0)
        return false;

    min_stats_ss.type = (BatteryHealthSnapshotArgs::BatterySnapshotType)kStatsSnapshotType[type];
    max_stats_ss.type =
            (BatteryHealthSnapshotArgs::BatterySnapshotType)(kStatsSnapshotType[type] + 1);

    stats_client->reportBatteryHealthSnapshot(min_stats_ss);
    stats_client->reportBatteryHealthSnapshot(max_stats_ss);

    return true;
}

bool BatteryMetricsLogger::uploadMetrics(void) {
    int64_t time = getTime();
    int32_t avg_resistance = 0;

    if (last_sample_ == 0)
        return false;

    LOG(INFO) << "Uploading metrics at time " << std::to_string(time) << " w/ "
              << std::to_string(num_samples_) << " samples";

    if (num_res_samples_)
        avg_resistance = accum_resistance_ / num_res_samples_;

    LOG(INFO) << "Logging metrics";

    sp<IStats> stats_client = IStats::tryGetService();
    if (!stats_client) {
        LOG(ERROR) << "Unable to connect to Stats service";
        return false;
    }

    // Only log and upload the min and max for metric types we want to upload
    for (int metric = 0; metric < NUM_FIELDS; metric++) {
        if ((metric == RES && num_res_samples_ == 0) || kStatsSnapshotType[metric] < 0)
            continue;
        std::string log_min = "min-" + std::to_string(metric) + " ";
        std::string log_max = "max-" + std::to_string(metric) + " ";
        for (int j = 0; j < NUM_FIELDS; j++) {
            log_min += std::to_string(min_[metric][j]) + " ";
            log_max += std::to_string(max_[metric][j]) + " ";
        }
        LOG(INFO) << log_min;
        LOG(INFO) << log_max;
        // Upload min/max metrics
        uploadOutlierMetric(stats_client, static_cast<sampleType>(metric));
    }

    // Upload average metric
    BatteryHealthSnapshotArgs avg_res_ss_stats = {
            .type = BatteryHealthSnapshotArgs::BatterySnapshotType::AVG_RESISTANCE,
            .temperatureDeciC = 0,
            .voltageMicroV = 0,
            .currentMicroA = 0,
            .openCircuitVoltageMicroV = 0,
            .resistanceMicroOhm = avg_resistance,
            .levelPercent = 0};
    if (num_res_samples_) {
        stats_client->reportBatteryHealthSnapshot(avg_res_ss_stats);
    }

    // Clear existing data
    memset(min_, 0, sizeof(min_));
    memset(max_, 0, sizeof(max_));
    num_res_samples_ = 0;
    num_samples_ = 0;
    last_upload_ = time;
    accum_resistance_ = 0;
    LOG(INFO) << "Finished uploading to tron";
    return true;
}

bool BatteryMetricsLogger::recordSample(struct android::BatteryProperties *props) {
    std::string resistance_str, ocv_str;
    int32_t resistance, ocv;
    int32_t time = getTime();

    LOG(INFO) << "Recording a sample at time " << std::to_string(time);

    if (!android::base::ReadFileToString(kBatteryResistance, &resistance_str)) {
        LOG(ERROR) << "Can't read the battery resistance";
        resistance = 0;
    } else if (!(resistance = stoi(resistance_str))) {
        LOG(ERROR) << "Can't parse battery resistance value " << resistance_str;
    }

    if (!android::base::ReadFileToString(kBatteryOCV, &ocv_str)) {
        LOG(ERROR) << "Can't read the open-circuit voltage (ocv) value";
        ocv = 0;
    } else if (!(ocv = stoi(ocv_str))) {
        LOG(ERROR) << "Can't parse open-circuit voltage (ocv) value " << ocv_str;
    }

    int32_t sample[NUM_FIELDS] = {[TIME] = time,
                                  [RES] = resistance,
                                  [CURR] = props->batteryCurrent,
                                  [VOLT] = props->batteryVoltage,
                                  [TEMP] = props->batteryTemperature,
                                  [SOC] = props->batteryLevel,
                                  [OCV] = ocv};
    if (props->batteryStatus != android::BATTERY_STATUS_CHARGING) {
        accum_resistance_ += resistance;
        num_res_samples_++;
    }

    // Only calculate the min and max for metric types we want to upload
    for (int metric = 0; metric < NUM_FIELDS; metric++) {
        // Discard resistance min/max when charging
        if ((metric == RES && props->batteryStatus == android::BATTERY_STATUS_CHARGING) ||
            kStatsSnapshotType[metric] < 0)
            continue;
        if (num_samples_ == 0 || (metric == RES && num_res_samples_ == 0) ||
            sample[metric] < min_[metric][metric]) {
            for (int i = 0; i < NUM_FIELDS; i++) {  // update new min with current sample
                min_[metric][i] = sample[i];
            }
        }
        if (num_samples_ == 0 || (metric == RES && num_res_samples_ == 0) ||
            sample[metric] > max_[metric][metric]) {
            for (int i = 0; i < NUM_FIELDS; i++) {  // update new max with current sample
                max_[metric][i] = sample[i];
            }
        }
    }

    num_samples_++;
    last_sample_ = time;
    return true;
}

void BatteryMetricsLogger::logBatteryProperties(struct android::BatteryProperties *props) {
    int32_t time = getTime();
    if (last_sample_ == 0 || time - last_sample_ >= kSamplePeriod)
        recordSample(props);
    if (last_sample_ - last_upload_ > kUploadPeriod || num_samples_ >= kMaxSamples)
        uploadMetrics();

    return;
}

}  // namespace health
}  // namespace pixel
}  // namespace google
}  // namespace hardware
