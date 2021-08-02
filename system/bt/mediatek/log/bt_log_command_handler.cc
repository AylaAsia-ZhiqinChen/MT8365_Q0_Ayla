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

#define LOG_TAG "mtk.bt.logtool-cmd-handler"

#include "bt_log_command_handler.h"

#include "chre_log.h"
#include "socket_server.h"
#include "osi/include/osi.h"

namespace {

inline std::string GenerateAckString(const std::string &cmd,
    BtLogCtrlStatus ret) {
  std::string result(cmd);
  result += (kBtLogCtrlStatusSuccess == ret) ?
      std::string(",1") : std::string(",0");
  return result;
}

}  // anonymous namespace

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

void BtLogCmdHandler::InitCommandTable() {
  command_table_[kAutoStartCommand] = [this](const std::string &cmd_key,
      const std::string &cmd_buffer) {
    std::size_t found = cmd_buffer.find_first_of("=");
    if (found != std::string::npos) {
      std::string log_size_buf = cmd_buffer.substr(found+1);
      int enabled = std::stoi(log_size_buf);
      BtLogCtrlStatus ret =
          BluetoothLogSetting::GetInstance()->SetAutoStartHciSnoop(
              enabled ? true : false);
      Notify(ret, BluetoothLogSetting::GetInstance());
      std::string result(GenerateAckString(cmd_key, ret));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    } else {
      std::string result(GenerateAckString(cmd_key, kBtLogCtrlStatusFail));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    }
  };
  command_table_[kStartCommand] = [this](const std::string &cmd_key,
      UNUSED_ATTR const std::string &cmd_buffer) {
    BtLogCtrlStatus ret = BluetoothLogSetting::GetInstance()->EnableHciSnoop();
    Notify(ret, BluetoothLogSetting::GetInstance());
    std::string result(GenerateAckString(cmd_key, ret));
    socket_server_->sendToAllClients(result.c_str(), result.length());
  };
  command_table_[kStopCommand] = [this](const std::string &cmd_key,
      UNUSED_ATTR const std::string &cmd_buffer) {
    BtLogCtrlStatus ret = BluetoothLogSetting::GetInstance()->DisableHciSnoop();
    Notify(ret, BluetoothLogSetting::GetInstance());
    std::string result(GenerateAckString(cmd_key, ret));
    socket_server_->sendToAllClients(result.c_str(), result.length());
  };
  command_table_[kSetLogMaxSizeCommand] = [this](const std::string &cmd_key,
      const std::string &cmd_buffer) {
    std::size_t found = cmd_buffer.find_first_of("=");
    if (found != std::string::npos) {
      std::string log_size_buf = cmd_buffer.substr(found+1);
      int log_size = std::stoi(log_size_buf);
      BtLogCtrlStatus ret =
          BluetoothLogSetting::GetInstance()->SetHciLogMaxSize(log_size);
      std::string result(GenerateAckString(cmd_key, ret));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    } else {
      std::string result(GenerateAckString(cmd_key, kBtLogCtrlStatusFail));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    }
  };
  command_table_[kSetStoragePathCommand] = [this](const std::string &cmd_key,
      const std::string &cmd_buffer) {
    std::size_t found = cmd_buffer.find_first_of(",");
    if (found != std::string::npos) {
      std::string log_path = cmd_buffer.substr(found+1);
      BtLogCtrlStatus ret =
          BluetoothLogSetting::GetInstance()->SetHciLogPath(log_path.c_str());
      std::string result(GenerateAckString(cmd_key, ret));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    } else {
      std::string result(GenerateAckString(cmd_key, kBtLogCtrlStatusFail));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    }
  };
  command_table_[kSetBtHostLogCommand] = [this](const std::string &cmd_key,
      const std::string &cmd_buffer) {
    std::size_t found = cmd_buffer.find_first_of(",");
    if (found != std::string::npos) {
      std::string log_size_buf = cmd_buffer.substr(found+1);
      int level = std::stoi(log_size_buf);
      BtLogCtrlStatus ret =
          BluetoothLogSetting::GetInstance()->SetHostLogLevel(
              static_cast<BtHostLogLevel>(level));
      Notify(ret, BluetoothLogSetting::GetInstance());
      std::string result(GenerateAckString(cmd_key, ret));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    } else {
      std::string result(GenerateAckString(cmd_key, kBtLogCtrlStatusFail));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    }
  };
  command_table_[kSetBtFWLogLevelCommmand] = [this](const std::string &cmd_key,
      const std::string &cmd_buffer) {
    std::size_t found = cmd_buffer.find_first_of(",");
    if (found != std::string::npos) {
      std::string log_size_buf = cmd_buffer.substr(found+1);
      int level = std::stoi(log_size_buf);
      BtLogCtrlStatus ret =
          BluetoothLogSetting::GetInstance()->SetFwLogLevel(
              static_cast<BtFwLogLevel>(level));
      Notify(ret, BluetoothLogSetting::GetInstance());
      std::string result(GenerateAckString(cmd_key, ret));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    } else {
      std::string result(GenerateAckString(cmd_key, kBtLogCtrlStatusFail));
      socket_server_->sendToAllClients(result.c_str(), result.length());
    }
  };
}

void BtLogCmdHandler::set_server(android::chre::SocketServer* server) {
  socket_server_ = server;
}

void BtLogCmdHandler::Handle(void *data, size_t len) const {
  std::string command_buf(static_cast<char*>(data), len);
  LOGI("command received: %s", command_buf.c_str());
  for (const auto& it : command_table_) {
    if (command_buf.find(it.first) != std::string::npos) {
      it.second(it.first, command_buf);
      break;
    }
  }
}

void BtLogCmdHandler::AddObserver(BtLogStateObserver *ob)  {
  std::lock_guard<std::mutex> lock(observers_mutex_);
  log_state_observers_.insert(ob);
}

void BtLogCmdHandler::RemoveObserver(BtLogStateObserver *ob) {
  std::lock_guard<std::mutex> lock(observers_mutex_);
  log_state_observers_.erase(ob);
}

void BtLogCmdHandler::NotifyStateChanged(const BluetoothLogSetting* setting) {
  // Add error handling for the observers being freed but notification coming
  std::lock_guard<std::mutex> lock(observers_mutex_);
  if (!log_state_observers_.empty()) {
    for (auto& it : log_state_observers_) {
      if (it) {
        it->OnBtLogStateChanged(setting);
      } else {
        LOGW("skip to notify since the observer is freed anymore");
      }
    }
  } else {
    LOGW("skip to notify since no observer anymore");
  }
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
