/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "bt_log_snoop_config"

#include "snoop_log_config.h"

#include <inttypes.h>
#include <stdint.h>

#include <base/logging.h>
#include <base/memory/singleton.h>

#include "bt_log_state_observer.h"
#include "bt_log_controller.h"
#include "bt_log_tool.h"
#include "osi/include/log.h"
#include "osi/include/properties.h"
#include "stack_config.h"

// To disable by default
// #define DBG_LOG_ENABLE TRUE

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

const std::string kBTSnoopMaxSize("persist.vendor.bluetooth.btsnoopmaxsize");

constexpr int64_t kKByteUnit(1024);
constexpr int64_t kMegaUnit(kKByteUnit*kKByteUnit);
constexpr int64_t kDefaultMaxLogSizePerFileUnit(200);
constexpr int64_t kDefaultMaxLogSizePerFile(
    kMegaUnit*kDefaultMaxLogSizePerFileUnit);
constexpr int64_t kDefaultMaxTotalLogsSizeUnit(2048);
constexpr int64_t kDefaultMaxTotalLogsSize(
    kMegaUnit*kDefaultMaxTotalLogsSizeUnit);
constexpr int kDefaultPacketsCacheUpperLimit(10);
constexpr int kDefaultLogsRecycleRate(2);

#if !defined(DBG_LOG_ENABLE) || (DBG_LOG_ENABLE != TRUE)
constexpr int64_t kValidMaxLogSizePerFileUnit(1);
constexpr int64_t kValidMaxLogSizePerFile(
    kMegaUnit*kValidMaxLogSizePerFileUnit);
#endif
constexpr int64_t kValidMaxTotalLogsSizeUnit(16);
#if !defined(DBG_LOG_ENABLE) || (DBG_LOG_ENABLE != TRUE)
constexpr int64_t kValidMaxTotalLogsSize(
    kValidMaxLogSizePerFile*kValidMaxTotalLogsSizeUnit);
#endif
constexpr int kValidPacketsCacheUpperLimit(0);
constexpr int kValidLogsRecycleRate(1);

class SnoopLogConfigImpl : public BtLogStateObserver {
  public:
   SnoopLogConfigImpl()
     : max_total_logs_size_(kDefaultMaxTotalLogsSize),
       max_log_size_per_file_(kDefaultMaxLogSizePerFile),
       packets_cache_upper_limit_(kDefaultPacketsCacheUpperLimit),
       logs_recycle_rate_(kDefaultLogsRecycleRate) {
    // Update();
   }

   void Setup() {
     btlogtool::AddObserver(this);
   }

   void TearDown() {
     btlogtool::RemoveObserver(this);
   }

   void Refresh() {
     Update();
   }

   void OnBtLogStateChanged(const BluetoothLogSetting* setting) override {
     int64_t current_size(max_total_logs_size_/kMegaUnit);
     int64_t new_size(setting->GetHciLogMaxSize());
     LOG_INFO(LOG_TAG, "%s : new size (%" PRId64 ") vs current size (%" PRId64 ").",
         __func__, new_size, current_size);
#if defined(DBG_LOG_ENABLE) && (DBG_LOG_ENABLE == TRUE)
     max_total_logs_size_ = kValidMaxTotalLogsSizeUnit * kKByteUnit;
#else
     if ((new_size >= kValidMaxTotalLogsSize) &&
         (new_size !=current_size)) {
       max_total_logs_size_ = new_size * kMegaUnit;
     }
#endif
   }

   int64_t max_total_logs_size() const {
     return max_total_logs_size_;
   }

   int64_t max_log_size_per_file() const {
     return max_log_size_per_file_;
   }

   int packets_cache_upper_limit() const {
     return packets_cache_upper_limit_;
   }

   int64_t GetRecycleLogsSize() const {
     return  max_total_logs_size_*logs_recycle_rate_/100;
   }

  private:
   void Update() {
     UpdateMaxTotalLogsSize();
     const config_t* config = stack_config_get_interface()->get_all();
     if (config) {
       UpdateMaxLogSizePerFile(config);
       UpdatePacketsCacheUpperLimit(config);
       UpdateLogsRecycleRate(config);
     }
     Dump();
   }

   inline void UpdateMaxTotalLogsSize() {
#if defined(DBG_LOG_ENABLE) && (DBG_LOG_ENABLE == TRUE)
     max_total_logs_size_ = kValidMaxTotalLogsSizeUnit * kKByteUnit;
#else
     char log_max_size[PROPERTY_VALUE_MAX] = {0};
     osi_property_get(kBTSnoopMaxSize.c_str(),
         log_max_size,
         std::to_string(kDefaultMaxTotalLogsSizeUnit).c_str());
     int64_t size = std::stoi(std::string(log_max_size)) * kMegaUnit;
     if (size >= kValidMaxTotalLogsSize) {
       max_total_logs_size_ = size;
     } else {
       LOG_ERROR(LOG_TAG, "%s: invalid input for (%" PRId64 ")"
           ", at least equal or larger than %" PRId64 "",
           __func__, size, kValidMaxTotalLogsSizeUnit);
     }
#endif
   }

   inline void UpdateMaxLogSizePerFile(const config_t* config) {
#if defined(DBG_LOG_ENABLE) && (DBG_LOG_ENABLE == TRUE)
     max_log_size_per_file_ = kKByteUnit;
#else
     int64_t size = config_get_int(*config,
         CONFIG_DEFAULT_SECTION,
         MAX_LOG_SIZE_PER_FILE_KEY, kDefaultMaxLogSizePerFileUnit);
     size *= kMegaUnit;
     if (size >= kValidMaxLogSizePerFile) {
       max_log_size_per_file_ = size;
     } else {
       LOG_ERROR(LOG_TAG, "%s: invalid input for (%" PRId64 ")"
           ", at least equal or larger than %" PRId64 "",
           __func__, size, kValidMaxLogSizePerFileUnit);
     }
#endif
   }

   inline void UpdatePacketsCacheUpperLimit(const config_t* config) {
     int size = config_get_int(*config,
         CONFIG_DEFAULT_SECTION,
         PACKETS_CACHE_UPPER_LIMIT_KEY, kValidPacketsCacheUpperLimit);
     if (size >= kValidPacketsCacheUpperLimit) {
       packets_cache_upper_limit_ = size;
     } else {
       LOG_ERROR(LOG_TAG, "%s: invalid input for (%d)"
           ", at least equal or larger than %d",
           __func__, size, kValidPacketsCacheUpperLimit);
     }
   }

   inline void UpdateLogsRecycleRate(const config_t* config) {
     int rate = config_get_int(*config,
         CONFIG_DEFAULT_SECTION,
         RECYCLE_RATE_KEY, kDefaultLogsRecycleRate);
     if (rate >= kValidLogsRecycleRate) {
       logs_recycle_rate_ = rate;
     } else {
       LOG_ERROR(LOG_TAG, "%s: invalid input for (%d)"
           ", at least equal or larger than %d",
           __func__, rate, kValidLogsRecycleRate);
     }
   }

   void Dump() const {
     LOG_INFO(LOG_TAG, "%s: max_total_logs_size_(%" PRId64 ") M",
         __func__, max_total_logs_size_/kMegaUnit);
     LOG_INFO(LOG_TAG, "%s: max_log_size_per_file_(%" PRId64 ") M",
         __func__, max_log_size_per_file_/kMegaUnit);
     LOG_INFO(LOG_TAG, "%s: packets_cache_upper_limit_(%d)",
         __func__, packets_cache_upper_limit_);
     LOG_INFO(LOG_TAG, "%s: logs_recycle_rate_(%d)",
         __func__, logs_recycle_rate_);
   }

   int64_t max_total_logs_size_;
   int64_t max_log_size_per_file_;
   int packets_cache_upper_limit_;
   int logs_recycle_rate_;
};

SnoopLogConfig::SnoopLogConfig() : config_impl_(new SnoopLogConfigImpl()) {}

SnoopLogConfig* SnoopLogConfig::GetInstance() {
  return base::Singleton<SnoopLogConfig>::get();
}

void SnoopLogConfig::Setup() {
  config_impl_->Setup();
}

void SnoopLogConfig::TearDown() {
  config_impl_->TearDown();
}

void SnoopLogConfig::Refresh() {
  config_impl_->Refresh();
}

int64_t SnoopLogConfig::GetMaxTotalLogsSize() const {
  return config_impl_->max_total_logs_size();
}

int64_t SnoopLogConfig::GetMaxLogSizePerFile() const {
  return config_impl_->max_log_size_per_file();
}

int SnoopLogConfig::GetPacketCacheUpperLimit() const {
  return config_impl_->packets_cache_upper_limit();
}

int64_t SnoopLogConfig::GetRecycleLogsSize() const {
  return config_impl_->GetRecycleLogsSize();
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
