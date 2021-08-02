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

#define LOG_TAG "bt_mtk_stack_config"

#include "mtk_stack_config.h"

#include <stdint.h>
#include <unistd.h>  // access()
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <base/logging.h>
#include <base/memory/singleton.h>

#include "hci_internals.h"
#include "log_mode.h"
#include "mtk_util.h"
#include "osi/include/list.h"
#include "osi/include/log.h"
#include "osi/include/properties.h"
#include "snoop_log_config.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

static const std::string kPath("/etc/bluetooth/");
static const std::string kStackConf("mtk_bt_stack.conf");
static const std::string kControllerConf("mtk_bt_fw.conf");

class StackConfigParser {
 public:
  StackConfigParser() : kMtkBtStackConf_(kPath + kStackConf) {
    stack_trace_["TRC_BTM"] = 0;
    stack_trace_["TRC_HCI"] = 0;
    stack_trace_["TRC_L2CAP"] = 0;
    stack_trace_["TRC_RFCOMM"] = 0;
    stack_trace_["TRC_OBEX"] = 0;
    stack_trace_["TRC_AVCT"] = 0;
    stack_trace_["TRC_AVDT"] = 0;
    stack_trace_["TRC_AVRC"] = 0;
    stack_trace_["TRC_AVDT_SCB"] = 0;
    stack_trace_["TRC_AVDT_CCB"] = 0;
    stack_trace_["TRC_A2D"] = 0;
    stack_trace_["TRC_SDP"] = 0;
    stack_trace_["TRC_GATT"] = 0;
    stack_trace_["TRC_SMP"] = 0;
    stack_trace_["TRC_BTAPP"] = 0;
    stack_trace_["TRC_BTIF"] = 0;
    stack_trace_["TRC_GAP"] = 0;
    stack_trace_["TRC_BNEP"] = 0;
    stack_trace_["TRC_PAN"] = 0;
    stack_trace_["TRC_HID_HOST"] = 0;
    stack_trace_["TRC_HID_DEV"] = 0;
    conf_str_ext_["LoggingV"] = "--v=0";
    conf_str_ext_["LoggingVModule"] =
        "--vmodule=*/btm/*=1,btm_ble_multi*=2,btif_*=1";
    conf_int_ext_[RECYCLE_RATE_KEY] = 10;
    conf_int_ext_[PACKETS_CACHE_UPPER_LIMIT_KEY] = 10;
    conf_int_ext_[MAX_LOG_SIZE_PER_FILE_KEY] = 200;
  }

  std::unique_ptr<config_t> OverwriteConfig(std::unique_ptr<config_t> config) {
    CHECK(config != nullptr);
    std::unique_ptr<config_t> mtk_config = config_new(kMtkBtStackConf_.c_str());
    if (!mtk_config) {
      LOG_ERROR(LOG_TAG, "%s file %s not found!", __func__,
                kMtkBtStackConf_.c_str());
    } else {
      for (auto& it : stack_trace_) {
        if (config_has_key(*config, CONFIG_DEFAULT_SECTION, it.first) &&
            config_has_key(*mtk_config, CONFIG_DEFAULT_SECTION, it.first)) {
          it.second = config_get_int(*mtk_config, CONFIG_DEFAULT_SECTION,
                                     it.first, it.second);
          config_set_int(config.get(), CONFIG_DEFAULT_SECTION, it.first,
                         it.second);
          LOG_INFO(LOG_TAG, "%s Overwrite: %s = %d", __func__, it.first.c_str(),
                   it.second);
        }
      }
      AppendStrConfig(config.get(), mtk_config.get());
      AppendIntConfig(config.get(), mtk_config.get());
    }
    return config;
  }

 private:
  void AppendStrConfig(config_t* config, config_t* mtk_config) {
    for (auto& it : conf_str_ext_) {
      if (config_has_key(*mtk_config, CONFIG_DEFAULT_SECTION,
                         it.first)) {
        it.second = *config_get_string(*mtk_config, CONFIG_DEFAULT_SECTION,
                                      it.first, &(it.second));
        config_set_string(config, CONFIG_DEFAULT_SECTION, it.first,
                          it.second);
        LOG_INFO(LOG_TAG, "%s Append: %s = %s", __func__, it.first.c_str(),
                 it.second.c_str());
      }
    }
  }

  void AppendIntConfig(config_t* config, config_t* mtk_config) {
    for (auto& it : conf_int_ext_) {
      if (config_has_key(*mtk_config, CONFIG_DEFAULT_SECTION,
                         it.first.c_str())) {
        it.second = config_get_int(*mtk_config, CONFIG_DEFAULT_SECTION,
                                   it.first, it.second);
        config_set_int(config, CONFIG_DEFAULT_SECTION, it.first,
                       it.second);
        LOG_INFO(LOG_TAG, "%s Append: %s = %d", __func__, it.first.c_str(),
                 it.second);
      }
    }
  }

  const std::string kMtkBtStackConf_;
  std::map<std::string, int> stack_trace_;
  std::map<std::string, std::string> conf_str_ext_;
  std::map<std::string, int> conf_int_ext_;
};

class ControllerLogModeParser {
 public:
  explicit ControllerLogModeParser(TraceLevel level) : trace_level_(level) {}
  virtual ~ControllerLogModeParser() {}

  virtual std::vector<uint8_t> GetEnableCommandPacket() const = 0;
  virtual std::vector<uint8_t> GetFilterommandPacket() const = 0;

 protected:
  TraceLevel GetTraceLevel() const { return trace_level_; }

 private:
  TraceLevel trace_level_;
};

class ControllerLogModeDefaultParser : public ControllerLogModeParser {
 public:
  explicit ControllerLogModeDefaultParser(TraceLevel level)
      : ControllerLogModeParser(level) {
    InitEnableCmdPackets();
    InitFilterCmdPackets();
  }
  virtual ~ControllerLogModeDefaultParser() {}

  std::vector<uint8_t> GetEnableCommandPacket() const override {
    return enable_cmd_packets_[GetTraceLevel()];
  }

  std::vector<uint8_t> GetFilterommandPacket() const override {
    return filter_cmd_packets_[GetTraceLevel()];
  }

 private:
  void InitEnableCmdPackets() {
    // kDefault
    enable_cmd_packets_.push_back(std::vector<uint8_t>{0xBE, 0xFC, 0x01, 0x00});
    // kSqc
    enable_cmd_packets_.push_back(std::vector<uint8_t>{0xBE, 0xFC, 0x01, 0x05});
    // kDebug
    enable_cmd_packets_.push_back(std::vector<uint8_t>{0xBE, 0xFC, 0x01, 0x05});
  }

  void InitFilterCmdPackets() {
    // kDefault
    filter_cmd_packets_.push_back(std::vector<uint8_t>{
        // C2 FC5F filter
        0x5F, 0xFC, 0x2A, 0x50, 0x01, 0x09, 0x00, 0x00, 0x00,
        // C201 SYS
        0x00, 0x00, 0x00, 0x00,
        // C202 TIM
        0x00, 0x00, 0x00, 0x00,
        // C203 PKV
        0x00, 0x00, 0x00, 0x00,
        // C204 LC
        0x00, 0x00, 0x00, 0x00,
        // C205 LL
        0x00, 0x00, 0x00, 0x00,
        // C206 ANT
        0x00, 0x00, 0x00, 0x00,
        // C207 LMP
        0x00, 0x00, 0x00, 0x00,
        // C208 CO
        0x00, 0x00, 0x00, 0x00,
        // C209 VOICE
        0x00, 0x00, 0x00, 0x00});
    // kSqc
    filter_cmd_packets_.push_back(std::vector<uint8_t>{
        // C2 FC5F filter
        0x5F, 0xFC, 0x2A, 0x50, 0x01, 0x09, 0x00, 0x00, 0x00,
        // C201 SYS
        0x00, 0x00, 0x00, 0x88,
        // C202 TIM
        0x00, 0x00, 0x00, 0x00,
        // C203 PKV
        0x00, 0x00, 0x00, 0x00,
        // C204 LC
        0x00, 0x00, 0x00, 0x00,
        // C205 LL
        0x00, 0x00, 0x00, 0x00,
        // C206 ANT
        0x00, 0x00, 0x00, 0x00,
        // C207 LMP
        0x00, 0x00, 0x00, 0x00,
        // C208 CO
        0x00, 0x00, 0x00, 0x00,
        // C209 VOICE
        0x00, 0x00, 0x00, 0x00});

    // kDebug
    filter_cmd_packets_.push_back(std::vector<uint8_t>{
        // C2 FC5F filter
        0x5F, 0xFC, 0x2A, 0x50, 0x01, 0x09, 0x00, 0x00, 0x00,
        // C201 SYS
        0x00, 0x00, 0x00, 0xCC,
        // C202 TIM
        0x00, 0x00, 0x00, 0x00,
        // C203 PKV
        0x00, 0x00, 0x00, 0x00,
        // C204 LC
        0x1F, 0xF0, 0x00, 0x00,
        // C205 LL
        0x07, 0x27, 0x06, 0x00,
        // C206 ANT
        0x00, 0x00, 0x00, 0x00,
        // C207 LMP
        0x03, 0x00, 0x03, 0x00,
        // C208 CO
        0x03, 0x08, 0x00, 0x00,
        // C209 VOICE
        0x00, 0x00, 0x00, 0x00});
  }

  std::vector<std::vector<uint8_t> > enable_cmd_packets_;
  std::vector<std::vector<uint8_t> > filter_cmd_packets_;
};

class ControllerLogModeConfParser : public ControllerLogModeParser {
 public:
  explicit ControllerLogModeConfParser(TraceLevel level)
      : ControllerLogModeParser(level), kSerialTypeLen_(1) {
    InitSections();
    InitConfig();
  }
  virtual ~ControllerLogModeConfParser() {}

  std::vector<uint8_t> GetEnableCommandPacket() const override {
    return LoadEnableCommandData(GetTraceLevel());
  }

  std::vector<uint8_t> GetFilterommandPacket() const override {
    return LoadFilterCommandData(GetTraceLevel());
  }

 private:
  void InitSections() {
    sections_[kDefault] = std::string("MtkBtFwLogOff");
    sections_[kSqc] = std::string("MtkBtFwLogSqc");
    sections_[kDebug] = std::string("MtkBtFwLogDebug");
  }

  void InitConfig() {
    config_ = config_new(std::string(kPath + kControllerConf).c_str());
    CHECK(config_ != nullptr);
  }

  std::vector<uint8_t> LoadEnableCommandData(TraceLevel level) const {
    const std::string kEnableCmdKey("C1");
    std::string cmd_temp(config_get_string(*config_, sections_.at(level),
                                           kEnableCmdKey, NULL)->c_str());
    std::vector<uint8_t> cmd_data = ConvertCmdStringToData(cmd_temp);
    std::vector<uint8_t> enable_cmd;
    // Skip to add serial type bit
    std::copy(cmd_data.begin() + kSerialTypeLen_, cmd_data.end(),
              std::back_inserter(enable_cmd));
    return enable_cmd;
  }

  std::vector<uint8_t> LoadFilterCommandData(TraceLevel level) const {
    const std::string kFilterCmdKey("C2");
    constexpr int kC2NumOfRows(10);
    std::string cmd_temp(config_get_string(*config_, sections_.at(level),
                                           kFilterCmdKey, NULL)->c_str());
    for (int i(1); i < kC2NumOfRows; i++) {
      std::stringstream ss;
      ss << kFilterCmdKey << std::setfill('0') << std::setw(2) << i;
      std::string cmdx(config_get_string(*config_, sections_.at(level),
                                         ss.str(), NULL)->c_str());
      cmd_temp += std::string(" ") + cmdx;
    }
    std::vector<uint8_t> cmd_data = ConvertCmdStringToData(cmd_temp);
    std::vector<uint8_t> filter_cmd;
    // Skip to add serial type bit
    std::copy(cmd_data.begin() + kSerialTypeLen_, cmd_data.end(),
              std::back_inserter(filter_cmd));
    return filter_cmd;
  }

  static uint8_t StringToUInt8(const std::string& str) {
    return static_cast<uint8_t>(std::stoi(str, nullptr, 16));
  }

  std::vector<uint8_t> ConvertCmdStringToData(const std::string& cmd) const {
    std::istringstream iss(cmd);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{}};
    std::vector<uint8_t> data(tokens.size(), 0);
    std::transform(tokens.begin(), tokens.end(), data.begin(), StringToUInt8);
    return data;
  }

  const int kSerialTypeLen_;

  std::unique_ptr<config_t> config_;
  std::map<TraceLevel, std::string> sections_;
};

class ControllerLogModeFactory : public Singleton<ControllerLogModeFactory> {
 public:
  std::shared_ptr<ControllerLogModeParser> Create(TraceLevel level);

 private:
  friend class Singleton<ControllerLogModeFactory>;
};

std::shared_ptr<ControllerLogModeParser> ControllerLogModeFactory::Create(
    TraceLevel level) {
  if (access(std::string(kPath + kControllerConf).c_str(), F_OK)) {
    return std::shared_ptr<ControllerLogModeParser>(
        new ControllerLogModeDefaultParser(level));
  } else {
    return std::shared_ptr<ControllerLogModeParser>(
        new ControllerLogModeConfParser(level));
  }
}

class BTStackLogConfig : public Singleton<BTStackLogConfig> {
 public:
  bool IsNeedOverWriteConfig() const {
    TraceLevel level = LogLevel::GetInstance()->QueryTraceLevel(kStack);
    return (kDebug == level) || (kSqc == level);
  }

  bool IsBTSnoopEnabled() const {
    // AOSP default
    return isPropertySet(kBtSnoopEnableProperty) &&
        IsBTSnoopFeatureEnabled(kBtSnoopEnableProperty);
  }


  bool IsInboundDataMonitorEnabled() const {
    if (isPropertySet(kBTForceTurnOffInboundDataMonitorProperty_.c_str())) {
      return !IsBTSnoopFeatureEnabled(
          kBTForceTurnOffInboundDataMonitorProperty_.c_str());
    } else {
      return IsBTSnoopEnabled();
    }
  }

  bool IsConnSYSDedicatedLogFeatureEnabled() const {
    constexpr char kFeatureKey[] = "ro.vendor.connsys.dedicated.log";
    char value[PROPERTY_VALUE_MAX] = {0};
    osi_property_get(kFeatureKey, value, "-1");
    return std::string(value) == std::string("1");
  }

 private:
  BTStackLogConfig()
      : kBTForceTurnOffInboundDataMonitorProperty_("persist.vendor.bluetooth.offmonitor") {}

  bool isPropertySet(const std::string& key) const {
    char value[PROPERTY_VALUE_MAX] = {0};
    if (osi_property_get(key.c_str(), value, "")) {
      return true;
    } else {
      return false;
    }
  }

  bool IsBTSnoopFeatureEnabled(const char* key) const {
    char btsnoop_enabled[PROPERTY_VALUE_MAX] = {0};
    osi_property_get(key, btsnoop_enabled, "false");
    return std::string(btsnoop_enabled) == std::string("true");
  }

  friend class Singleton<BTStackLogConfig>;

  const std::string kBTForceTurnOffInboundDataMonitorProperty_;
};

class StackConfigImpl {
 public:
  StackConfigImpl() = default;
  ~StackConfigImpl() = default;

  bool IsNeedOverWriteConfig() const {
    return BTStackLogConfig::GetInstance()->IsNeedOverWriteConfig();
  }

  std::unique_ptr<config_t> OverwriteConfig(std::unique_ptr<config_t> config) {
    std::unique_ptr<StackConfigParser> config_parse(new StackConfigParser);
    return config_parse->OverwriteConfig(std::move(config));
  }

  const std::shared_ptr<ControllerLogModeParser> LoadFWLogModeParser(
      TraceLevel level) {
    return ControllerLogModeFactory::GetInstance()->Create(level);
  }

  std::vector<uint8_t> GetFWLogEnableCommandPacket(
      const std::shared_ptr<ControllerLogModeParser>& parser) {
    return parser->GetEnableCommandPacket();
  }

  std::vector<uint8_t> GetFWLogFilterommandPacket(
      const std::shared_ptr<ControllerLogModeParser>& parser) {
    return parser->GetFilterommandPacket();
  }

  bool IsBTSnoopEnabled() const {
    return BTStackLogConfig::GetInstance()->IsBTSnoopEnabled();
  }

  bool IsInboundDataMonitorEnabled() const {
    return BTStackLogConfig::GetInstance()->IsInboundDataMonitorEnabled();
  }

  bool IsConnSYSDedicatedLogFeatureEnabled() const {
    return BTStackLogConfig::GetInstance()->IsConnSYSDedicatedLogFeatureEnabled();
  }
};

StackConfig::StackConfig() : config_impl_(new StackConfigImpl()) {}

StackConfig* StackConfig::GetInstance() {
  return base::Singleton<StackConfig>::get();
}

bool StackConfig::IsNeedOverWriteConfig() const {
  return config_impl_->IsNeedOverWriteConfig();
}

std::unique_ptr<config_t> StackConfig::OverwriteConfig(
    std::unique_ptr<config_t> config) {
  return config_impl_->OverwriteConfig(std::move(config));
}

const std::shared_ptr<ControllerLogModeParser> StackConfig::LoadFWLogModeParser(
    TraceLevel level) {
  return config_impl_->LoadFWLogModeParser(level);
}

std::vector<uint8_t> StackConfig::GetFWLogEnableCommandPacket(
    const std::shared_ptr<ControllerLogModeParser>& parser) {
  return config_impl_->GetFWLogEnableCommandPacket(parser);
}

std::vector<uint8_t> StackConfig::GetFWLogFilterommandPacket(
    const std::shared_ptr<ControllerLogModeParser>& parser) {
  return config_impl_->GetFWLogFilterommandPacket(parser);
}

bool StackConfig::IsBTSnoopEnabled() const {
  return config_impl_->IsBTSnoopEnabled();
}

bool StackConfig::IsInboundDataMonitorEnabled() const {
  return config_impl_->IsInboundDataMonitorEnabled();
}

bool StackConfig::IsConnSYSDedicatedLogFeatureEnabled() const {
  return config_impl_->IsConnSYSDedicatedLogFeatureEnabled();
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
