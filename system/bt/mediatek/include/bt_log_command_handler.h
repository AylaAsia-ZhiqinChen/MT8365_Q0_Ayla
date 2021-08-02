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

#pragma once

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <string>

#include "bt_log_controller.h"
#include "bt_log_state_observer.h"

// BT host HCI log related
constexpr char kAutoStartCommand[] = "autostart";
constexpr char kStartCommand[] = "deep_start";
constexpr char kStopCommand[] = "deep_stop";
constexpr char kSetLogMaxSizeCommand[] = "logsize";
constexpr char kSetStoragePathCommand[] = "set_storage_path";
constexpr char kSetBtHostLogCommand[] = "set_bthost_debuglog_enable";

// Legacy: BT FW log related
constexpr char kSetBtFWLogLevelCommmand[] = "set_btfw_log_level";

namespace android {
namespace chre {
class SocketServer;
}  // namespace chre
}  // namespace android

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

using OnCommandReady =
    std::function<void(const std::string&, const std::string&)>;

class BtLogCmdHandler : public BtLogStateSubject {
 public:
  explicit BtLogCmdHandler() {
    InitCommandTable();
  }
  ~BtLogCmdHandler() = default;

 void Handle(void *data, size_t len) const;

 void AddObserver(BtLogStateObserver *ob) override;

 void RemoveObserver(BtLogStateObserver *ob) override;

 void NotifyStateChanged(const BluetoothLogSetting* setting) override;
 void set_server(android::chre::SocketServer* server);

 private:
  void InitCommandTable();
  void Notify(BtLogCtrlStatus ret, const BluetoothLogSetting* setting) {
    if (kBtLogCtrlStatusSuccess == ret) {
      NotifyStateChanged(setting);
    }
  }

  android::chre::SocketServer* socket_server_;
  std::map<std::string, OnCommandReady> command_table_;
  std::set<BtLogStateObserver*> log_state_observers_;

  // Sorry to apply mutex lock for avoiding race condition between
  // BT service call and btlogwatcher
  std::mutex observers_mutex_;
};

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
