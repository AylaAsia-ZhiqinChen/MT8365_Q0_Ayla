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

#define LOG_TAG "bt_hci_inbound_data_monitor"

#include "hci_inbound_data_monitor.h"

#include <string.h>
#include <atomic>
#include <mutex>
#include <vector>

#include <base/logging.h>

#include "fw_logger_filter.h"
#include "osi/include/alarm.h"
#include "osi/include/log.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

/*****************************************************************************
 * Notice module begin
 *****************************************************************************/

class InboundData {
 public:
  InboundData(uint32_t times_per_sec, float data_per_sec, const char* msg,
              bool is_alert = false)
      : inbound_times_per_sec_(times_per_sec),
        inbound_data_per_sec_(data_per_sec),
        data_filter_msg_(msg),
        is_alert_if_needed_(is_alert) {}
  uint32_t inbound_times_per_sec() const { return inbound_times_per_sec_; }

  float inbound_data_per_sec() const { return inbound_data_per_sec_; }

  const char* data_filter_msg() const { return data_filter_msg_; }

  bool is_alert_if_needed() const { return is_alert_if_needed_; }

 private:
  uint32_t inbound_times_per_sec_;
  float inbound_data_per_sec_;
  const char* data_filter_msg_;
  bool is_alert_if_needed_;
};

using InboundDataLevelCallback = std::function<void(const InboundData&)>;

// If chain of responsibility with performance concern, to replace with BST
class DataLevel {
 public:
  explicit DataLevel(std::unique_ptr<DataLevel> successor)
      : successor_(std::move(successor)) {}
  virtual ~DataLevel() {}

  virtual void Handle(const InboundData& inbound_data) = 0;

 protected:
  std::unique_ptr<DataLevel> successor_;

 private:
  DISALLOW_COPY_AND_ASSIGN(DataLevel);
};

class UltraHighDataLevel : public DataLevel {
 public:
  UltraHighDataLevel() : DataLevel(nullptr) {}
  virtual ~UltraHighDataLevel() {}

  void Handle(const InboundData& inbound_data) override {
    if (inbound_data.is_alert_if_needed()) {
//        LOG_ALWAYS_FATAL(
//            "%s: %s: oh boy, to the moon: times %u/s,  amount %.2f KB/s.",
//            __func__, inbound_data.data_filter_msg(),
//            inbound_data.inbound_times_per_sec(),
//            inbound_data.inbound_data_per_sec());
        LOG_ERROR(LOG_TAG,
                  "%s: %s: oh boy, to the moon: times %u/s,  amount %.2f KB/s.",
                  __func__, inbound_data.data_filter_msg(),
                  inbound_data.inbound_times_per_sec(),
                  inbound_data.inbound_data_per_sec());
      } else {
        LOG_INFO(LOG_TAG,
                 "%s: %s: oh boy, to the moon: times %u/s,  amount %.2f KB/s.",
                 __func__, inbound_data.data_filter_msg(),
                 inbound_data.inbound_times_per_sec(),
                 inbound_data.inbound_data_per_sec());
      }
    }
};

class HighDataLevel : public DataLevel {
 public:
  HighDataLevel()
      : DataLevel(std::unique_ptr<DataLevel>(new UltraHighDataLevel())) {}
  virtual ~HighDataLevel() {}

  void Handle(const InboundData& inbound_data) override {
    if ((inbound_data.inbound_times_per_sec() <= HIGH_TIMES) &&
        (inbound_data.inbound_data_per_sec() <= HIGH_DATA_PER_SEC)) {
      if (inbound_data.is_alert_if_needed()) {
        LOG_WARN(LOG_TAG, "%s: %s: loading smells: times %u/s, amount %.2f KB/s.",
                 __func__, inbound_data.data_filter_msg(),
                 inbound_data.inbound_times_per_sec(),
                 inbound_data.inbound_data_per_sec());
      } else {
        LOG_INFO(LOG_TAG, "%s: %s: loading smells: times %u/s, amount %.2f KB/s.",
                 __func__, inbound_data.data_filter_msg(),
                 inbound_data.inbound_times_per_sec(),
                 inbound_data.inbound_data_per_sec());
      }
    } else {
      successor_->Handle(inbound_data);
    }
  }
};

class MediumDataLevel : public DataLevel {
 public:
  MediumDataLevel()
      : DataLevel(std::unique_ptr<DataLevel>(new HighDataLevel())) {}
  virtual ~MediumDataLevel() {}

  void Handle(const InboundData& inbound_data) override {
    if ((inbound_data.inbound_times_per_sec() <= MEDIUM_TIMES) &&
        (inbound_data.inbound_data_per_sec() <= MEDIUM_DATA_PER_SEC)) {
      if (inbound_data.is_alert_if_needed()) {
        LOG_INFO(LOG_TAG, "%s: %s: times %u/s, amount %.2f KB/s.", __func__,
                 inbound_data.data_filter_msg(),
                 inbound_data.inbound_times_per_sec(),
                 inbound_data.inbound_data_per_sec());
      }
    } else {
      successor_->Handle(inbound_data);
    }
  }
};

class LowDataLevel : public DataLevel {
 public:
  LowDataLevel()
      : DataLevel(std::unique_ptr<DataLevel>(new MediumDataLevel())) {}
  virtual ~LowDataLevel() {}

  void Handle(const InboundData& inbound_data) override {
    if ((inbound_data.inbound_times_per_sec() <= LOW_TIMES) &&
        (inbound_data.inbound_data_per_sec() <= LOW_DATA_PER_SEC)) {
      {}  // low level do nothing
    } else {
      successor_->Handle(inbound_data);
    }
  }
};

class Observer {
 public:
  Observer(InboundDataLevel data_level, const std::string& name,
           bool is_registered = false)
      : data_level_(data_level),
        name_(name),
        is_registered_(is_registered),
        inbound_times_per_sec_(0),
        inbound_data_per_sec_(0.0f) {}
  virtual ~Observer() {}

  virtual void Update(const BT_HDR* buffer) = 0;
  virtual void Notify(InboundDataLevelCallback cb) = 0;

  InboundDataLevel data_level() const { return data_level_; }

  std::string name() const { return name_; }

  bool is_registered() const { return is_registered_; }

  void set_is_registered(bool is_registered) { is_registered_ = is_registered; }

  uint32_t inbound_times_per_sec() const { return inbound_times_per_sec_; }

  float inbound_data_per_sec() const { return inbound_data_per_sec_; }

 protected:
  void Notify(InboundDataLevelCallback cb, bool is_alert_if_needed) {
    InboundData notice_data = {inbound_times_per_sec_,
                               (float)inbound_data_per_sec_ / 1024,
                               name_.c_str(), is_alert_if_needed};
    cb(notice_data);
    Reset();
  }

  void UpdateData(const uint8_t* data, uint16_t inbound_data_len) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    inbound_times_per_sec_++;
    inbound_data_per_sec_ += (uint32_t)inbound_data_len;
  }

 private:
  void Reset() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    inbound_times_per_sec_ = 0;
    inbound_data_per_sec_ = 0.0f;
  }

  InboundDataLevel data_level_;
  const std::string name_;
  bool is_registered_;
  uint32_t inbound_times_per_sec_;
  float inbound_data_per_sec_;
  std::mutex data_mutex_;
};

class ObserverWhole : public Observer {
 public:
  ObserverWhole()
      : Observer(kInboundDataLevelUnknown, std::string("all inbound data"),
                 true) {}
  virtual ~ObserverWhole() {}

  void Update(const BT_HDR* buffer) override {
    CHECK(buffer != NULL);
    Observer::UpdateData(buffer->data, buffer->len);
  }

  void Notify(InboundDataLevelCallback cb) override {
    return Observer::Notify(cb, false);
  }
};

class ObserverFWLog : public Observer {
 public:
  ObserverFWLog()
      : Observer(kInboundDataLevelUnknown, std::string("FW log data")) {}
  virtual ~ObserverFWLog() {}

  void Update(const BT_HDR* buffer) override {
    CHECK(buffer != NULL);
    if (FWLogFilter::GetInstance()->IsFWLogEvent(buffer)) {
      Observer::UpdateData(buffer->data, buffer->len);
    }
  }

  void Notify(InboundDataLevelCallback cb) override {
    return Observer::Notify(cb, true);
  }
};

class ObserverUTTest : public Observer {
 public:
  ObserverUTTest()
      : Observer(kInboundDataLevelUnknown, std::string("GTest data")) {}
  virtual ~ObserverUTTest() {}

  void Update(const BT_HDR* buffer) override {
    CHECK(buffer != NULL);
    if (IsGTestData(buffer->data)) {
      Observer::UpdateData(buffer->data, buffer->len);
    }
  }

  void Notify(InboundDataLevelCallback cb) override {
    return Observer::Notify(cb, false);
  }

 private:
  bool IsGTestData(const uint8_t* data) {
    // "Life is like a box of chocolate"
    return (data[0] == 0x4C && data[1] == 0x69 && data[2] == 0x66);
  }
};

class InboundDataMonitorImpl {
 public:
  explicit InboundDataMonitorImpl(const std::string& name)
      : is_monitor_timer_started_{false},
        data_monitor_timer_(
          alarm_new((name + std::string("_timer")).c_str())),
        data_level_checker_(new LowDataLevel()) {
    ConstructObservers();
  }

  ~InboundDataMonitorImpl() {
    if (data_monitor_timer_) {
      alarm_free(data_monitor_timer_);
      data_monitor_timer_ = NULL;
    }
  }

  void InboundDataFilterUpdate(InboundDataFilter filter_type, bool is_on) {
    observer_list_.at(filter_type)->set_is_registered(is_on);
  }

  void InboundDataUpdate(const BT_HDR* buffer) {
    // alarm timer with wakeup lock, with low power suspend concern
    // re-factor to change to event driven from periodic timer
    RefreshMonitorTimer();
    for (auto& observer : observer_list_) {
      observer->Update(buffer);
    }
  }

  void InboundDataNotify() {
    for (auto& observer : observer_list_) {
      observer->Notify([this](const InboundData& inbound_data) {
        this->data_level_checker_->Handle(inbound_data);
      });
    }
  }

  InboundDataLevel GetDataLevel(InboundDataFilter filter_type) const {
    return observer_list_.at(filter_type)->data_level();
  }

 private:
  void ConstructObservers() {
    // Need to align with declaration sequence as InboundDataFilter
    observer_list_.emplace_back(new ObserverWhole());
    observer_list_.emplace_back(new ObserverUTTest());
    observer_list_.emplace_back(new ObserverFWLog());
  }

  void RefreshMonitorTimer() {
    if (!is_monitor_timer_started_) {
      alarm_set(data_monitor_timer_, DATA_MONITOR_TIMEOUT_MS,
                [](void* context) {
                  CHECK(NULL != context);
                  // LOG_INFO(LOG_TAG, "%s", __func__);
                  InboundDataMonitorImpl* monitor =
                      static_cast<InboundDataMonitorImpl*>(context);
                  monitor->InboundDataNotify();
                  std::atomic_exchange(
                    &monitor->is_monitor_timer_started_, false);
                },
                this);
      std::atomic_exchange(&is_monitor_timer_started_, true);
    }
  }

  std::atomic_bool is_monitor_timer_started_;
  alarm_t* data_monitor_timer_;
  std::vector<std::unique_ptr<Observer> > observer_list_;
  std::unique_ptr<DataLevel> data_level_checker_;

  DISALLOW_COPY_AND_ASSIGN(InboundDataMonitorImpl);
};

/*****************************************************************************
 * Monitor module begin
 *****************************************************************************/

/*****************************************************************************
 * Monitor API begin
 *****************************************************************************/
InboundDataMonitor::InboundDataMonitor(const std::string& name)
    : inbound_data_mointor_impl_(new InboundDataMonitorImpl(name)) {}

InboundDataMonitor::~InboundDataMonitor() {
  inbound_data_mointor_impl_.reset(nullptr);
}

void InboundDataMonitor::InboundDataFilterUpdate(InboundDataFilter filter_type,
                                                 bool is_on) {
  CHECK(inbound_data_mointor_impl_ != nullptr);
  inbound_data_mointor_impl_->InboundDataFilterUpdate(filter_type, is_on);
}
void InboundDataMonitor::InboundDataUpdate(const BT_HDR* buffer) {
  CHECK(inbound_data_mointor_impl_ != nullptr);
  inbound_data_mointor_impl_->InboundDataUpdate(buffer);
}

InboundDataLevel InboundDataMonitor::GetDataLevel(
    InboundDataFilter filter_type) const {
  CHECK(inbound_data_mointor_impl_ != nullptr);
  return inbound_data_mointor_impl_->GetDataLevel(filter_type);
}

/*****************************************************************************
 * Monitor API end
 *****************************************************************************/
}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
