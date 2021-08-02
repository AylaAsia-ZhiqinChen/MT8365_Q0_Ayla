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

#include <memory>
#include <string>

#include <base/macros.h>

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}  // namespace base


enum BtLogCtrlStatus {
  kBtLogCtrlStatusFail,
  kBtLogCtrlStatusSuccess,
};

enum BtHciLogLevel {
  kBtHciLogOff,
  kBtHciLogOn,
  kBtHciLogUnknown,
};

enum BtHostLogLevel {
  kBtHostLogOff,
  kBtHostLogSqc,
  kBtHostLogDebug,
  kBtHostLogUnknown,
};

enum BtFwLogLevel {
  kBtFwLogOff,
  kBtFwLogLowpower,
  kBtFwLogSqc,
  kBtFwLogDebug,
  kBtFwLogUnknown,
};

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class BluetoothLogSettingImpl;
class BluetoothLogSetting {
 public:

  BtLogCtrlStatus SetAutoStartHciSnoop(bool is_auto_start);
  // Enable Bluetooth HCI log
  BtLogCtrlStatus EnableHciSnoop();
  // Disable Bluetooth HCI log
  BtLogCtrlStatus DisableHciSnoop();
  BtLogCtrlStatus SetHostLogLevel(BtHostLogLevel level);
  BtLogCtrlStatus SetHciLogMaxSize(int max_size);
  BtLogCtrlStatus SetHciLogPath(const char* path);
  BtLogCtrlStatus SetFwLogLevel(BtFwLogLevel level);

  bool IsAutoStartHciSnoop() const;
  bool IsHciSnoopEnabled() const;
  BtHciLogLevel GetHciLogLevel() const;
  int GetHciLogMaxSize() const;
  BtHostLogLevel GetHostLogLevel() const;
  std::string GetHciLogPath() const;
  BtFwLogLevel GetFwLogLevel() const;

  static BluetoothLogSetting* GetInstance();

 private:
  BluetoothLogSetting();
  friend struct base::DefaultSingletonTraits<BluetoothLogSetting>;
  std::unique_ptr<BluetoothLogSettingImpl> setting_impl_;

  DISALLOW_COPY_AND_ASSIGN(BluetoothLogSetting);
};

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
