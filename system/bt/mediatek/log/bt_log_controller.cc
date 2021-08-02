
/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2017. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "mtk.bt.logtool-controller"

#include "bt_log_controller.h"

#include <memory>
#include <string>

#include <base/logging.h>
#include <base/memory/singleton.h>
#include <cutils/properties.h>
#include <log/log.h>

#include "log_mode.h"
#include "mtk_stack_config.h"
#include "twrite.h"

namespace {

inline std::string BtFwLogEnumToString(BtFwLogLevel level) {
  switch (level) {
  case kBtFwLogOff: return std::string("off");
  // Intentionally same as SQC level
  case kBtFwLogLowpower: return std::string("sqc");
  case kBtFwLogSqc: return std::string("sqc");
  case kBtFwLogDebug: return std::string("debug");
  case kBtFwLogUnknown: return std::string("unknown");
  default: {
    LOG_ALWAYS_FATAL("%s: invalid level: %d.", __func__, level);
    return nullptr;
  }
  }
}

inline std::string BtHostLogEnumToString(BtHostLogLevel level) {
  switch (level) {
  case kBtHostLogOff: return std::string("off");
  case kBtHostLogSqc: return std::string("sqc");
  case kBtHostLogDebug: return std::string("debug");
  case kBtHostLogUnknown: return std::string("unknown");
  default: {
    LOG_ALWAYS_FATAL("%s: invalid level: %d.", __func__, level);
    return nullptr;
  }
  }
}

inline const char* GetBtHciSnoopAutoStartKey(bool is_auto_start) {
  return is_auto_start ? "true" : "false";
}

}  // anonymous namespace

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class BluetoothLogSettingImpl {
 public:
  BluetoothLogSettingImpl()
      : is_hci_log_enabled_(false),
        // default as 2G
        max_hci_log_size_(2048),
        hci_log_root_path_("/sdcard/"),
        hci_log_level_(kBtHciLogUnknown),
        host_log_level_(kBtHostLogUnknown),
        fw_log_level_(kBtFwLogUnknown) {}
  ~BluetoothLogSettingImpl() = default;

  bool is_hci_log_auto_started() const {
    // AOSP request
    if (StackConfig::GetInstance()->IsBTSnoopEnabled()) {
      return true;
    }

    if (IsPropertySet(kBluetoothSnoopAutoStart)) {
      return GetPropertyValue(kBluetoothSnoopAutoStart) ==
          std::string(GetBtHciSnoopAutoStartKey(true));
    } else {
      return kEngBuild == GetBuildType();
    }
  }

  bool is_hci_log_enabled() const {
    return is_hci_log_enabled_;
  }

  BtHciLogLevel GetHciLogLevel() const {
    return hci_log_level_;
  }

  void set_is_hci_log_enabled(bool is_hci_log_enabled) {
    ALOGI("%s: update to %s!",
        __func__, is_hci_log_enabled ? "enabled" : "disabled");
    is_hci_log_enabled_ = is_hci_log_enabled;
    hci_log_level_ = is_hci_log_enabled_ ? kBtHciLogOn : kBtHciLogOff;
  }

  int max_hci_log_size() const {
    return max_hci_log_size_;
  }

  void set_max_hci_log_size(int max_hci_log_size) {
    max_hci_log_size_ = max_hci_log_size;
  }

  std::string hci_log_root_path() const {
    return hci_log_root_path_;
  }

  void set_hci_log_root_path(const std::string& hci_log_root_path) {
    hci_log_root_path_ = hci_log_root_path;
  }

  BtHostLogLevel host_log_level() const {
    return host_log_level_;
  }

  void set_host_log_level(BtHostLogLevel level) {
    host_log_level_ = level;
  }

  BtFwLogLevel fw_log_level() const {
    return fw_log_level_;
  }

  void set_fw_log_level(BtFwLogLevel level) {
    fw_log_level_ = level;
  }

 private:
  bool is_hci_log_enabled_;
  int max_hci_log_size_;
  std::string hci_log_root_path_;
  BtHciLogLevel hci_log_level_;
  BtHostLogLevel host_log_level_;
  BtFwLogLevel fw_log_level_;
};

BtLogCtrlStatus BluetoothLogSetting::SetAutoStartHciSnoop(
    bool is_auto_start) {
  int ret = property_set(kBluetoothSnoopAutoStart,
      GetBtHciSnoopAutoStartKey(is_auto_start));
  if (0 == ret) {
    ALOGI("%s: BT HCI auto start update %s to %s!",
        __func__,
        kBluetoothSnoopAutoStart,
        GetBtHciSnoopAutoStartKey(is_auto_start));
    return kBtLogCtrlStatusSuccess;
  } else {
    ALOGE("%s: BT HCI auto start failed to update %s to %s!", __func__,
        kBluetoothSnoopAutoStart,
        GetBtHciSnoopAutoStartKey(is_auto_start));
    return kBtLogCtrlStatusFail;
  }
}

BtLogCtrlStatus BluetoothLogSetting::EnableHciSnoop() {
  setting_impl_->set_is_hci_log_enabled(true);
  return kBtLogCtrlStatusSuccess;
}

BtLogCtrlStatus BluetoothLogSetting::DisableHciSnoop() {
  setting_impl_->set_is_hci_log_enabled(false);
  return kBtLogCtrlStatusSuccess;
}

BtLogCtrlStatus BluetoothLogSetting::SetHostLogLevel(BtHostLogLevel level) {
  int ret = property_set(kBluetoothHostLogLevelSysProp,
      BtHostLogEnumToString(level).c_str());
  if (0 == ret) {
    setting_impl_->set_host_log_level(level);
    ALOGI("%s: update %s to %s!", __func__,
        kBluetoothHostLogLevelSysProp, BtHostLogEnumToString(level).c_str());
    return kBtLogCtrlStatusSuccess;
  } else {
    ALOGE("%s: failed to update %s to %s!", __func__,
        kBluetoothHostLogLevelSysProp, BtHostLogEnumToString(level).c_str());
    return kBtLogCtrlStatusFail;
  }
}

BtLogCtrlStatus BluetoothLogSetting::SetHciLogMaxSize(int max_size) {
  ALOGI("%s: set to %d M", __func__, max_size);
  setting_impl_->set_max_hci_log_size(max_size);
  return kBtLogCtrlStatusSuccess;
}

BtLogCtrlStatus BluetoothLogSetting::SetHciLogPath(const char* path) {
  CHECK(path);
  ALOGI("%s: set to log path %s", __func__, path);
  setting_impl_->set_hci_log_root_path(std::string(path));
  return kBtLogCtrlStatusSuccess;
}

BtLogCtrlStatus BluetoothLogSetting::SetFwLogLevel(BtFwLogLevel level) {
  setting_impl_->set_fw_log_level(level);
  ALOGI("%s: BT FW update to %s!", __func__,
      BtFwLogEnumToString(level).c_str());
  return kBtLogCtrlStatusSuccess;
}

bool BluetoothLogSetting::IsAutoStartHciSnoop() const {
  return setting_impl_->is_hci_log_auto_started();
}

bool BluetoothLogSetting::IsHciSnoopEnabled() const {
  return setting_impl_->is_hci_log_enabled();
}

BtHciLogLevel BluetoothLogSetting::GetHciLogLevel() const {
  return setting_impl_->GetHciLogLevel();
}

int BluetoothLogSetting::GetHciLogMaxSize() const {
  return setting_impl_->max_hci_log_size();
}

BtHostLogLevel BluetoothLogSetting::GetHostLogLevel() const {
  return setting_impl_->host_log_level();
}

std::string BluetoothLogSetting::GetHciLogPath() const {
  return setting_impl_->hci_log_root_path();
}

BtFwLogLevel BluetoothLogSetting::GetFwLogLevel() const {
  return setting_impl_->fw_log_level();
}

BluetoothLogSetting::BluetoothLogSetting()
: setting_impl_(new BluetoothLogSettingImpl) {}

BluetoothLogSetting* BluetoothLogSetting::GetInstance() {
  return base::Singleton<BluetoothLogSetting>::get();
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
