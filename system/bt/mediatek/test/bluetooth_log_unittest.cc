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

#define LOG_TAG "mtk.bt.logtool-unittest"

#include <chrono>
#include <memory>
#include <string>

#include <cutils/properties.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <log/log.h>
#include <utils/StrongPointer.h>

#include "bt_log_controller.h"
#include "bt_log_tool.h"
#include "socket_client.h"

using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::NotNull;
using ::testing::Return;
using android::chre::SocketClient;
using android::sp;

namespace {

class MockSocketCallbacks : public SocketClient::ICallbacks {
 public:
  MOCK_METHOD2(onMessageReceived, void(const void *data, size_t length));
  MOCK_METHOD0(onConnected, void());
  MOCK_METHOD0(onConnectionAborted, void());
  MOCK_METHOD0(onDisconnected, void());
};

MATCHER_P(ResponseMatcher, expected, "") {
  return (std::string(static_cast<const char*>(arg)) == expected);
}

class MockSocketCallbacksHelper {
  public:
    explicit MockSocketCallbacksHelper(const std::string& expected)
        : expected_(expected) {}

    void CheckMessageResponse(const void *data, size_t length) {
      std::string res_data(static_cast<const char*>(data), length);
      EXPECT_STREQ(expected_.c_str(), res_data.c_str());
      EXPECT_EQ(expected_.size(), length);
    }

  private:
    std::string expected_;
};

using vendor::mediatek::bluetooth::stack::BtLogStateObserver;
using vendor::mediatek::bluetooth::stack::BluetoothLogSetting;
class MockLogStateObserver : public BtLogStateObserver {
 public:
  MOCK_METHOD1(OnBtLogStateChanged, void(const BluetoothLogSetting* setting));
};

MATCHER_P(HciEnableMatcher, expected, "") {
  return (expected && expected->IsHciSnoopEnabled());
}

MATCHER_P(HciDisableMatcher, expected, "") {
  return (expected && !expected->IsHciSnoopEnabled());
}

}  // anonymous namespace

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

constexpr char BT_HOST_LOG_LEVEL_CTL[] = "persist.vendor.bluetooth.hostloglevel";
constexpr char SQC[] = "sqc";
// constexpr char DEBUG[] = "debug";
constexpr char OFF[] = "off";
constexpr char SOCKET_SERVER_NAME[] = "btlogtooltest";
const std::string SET_LOG_PATH_CMD("set_storage_path,/sdcard");
const std::string AUTOSTART_CMD("autostart=");
const std::string START_CMD("deep_start");
const std::string STOP_CMD("deep_stop");
const std::string SET_LOG_SIZE_CMD("logsize=600");
const std::string SET_BT_FW_LOG_LEVEL_CMD("set_btfw_log_level,");
const std::string SET_BT_STACK_LOG_LEVEL_CMD("set_bthost_debuglog_enable,");

const std::string AUTOSTART_CMD_RESPONSE("autostart,1");
const std::string SET_LOG_PATH_CMD_RESPONSE("set_storage_path,1");
const std::string START_CMD_RESPONSE("deep_start,1");
const std::string STOP_CMD_RESPONSE("deep_stop,1");
const std::string SET_LOG_SIZE_CMD_RESPONSE("logsize,1");
const std::string SET_BT_FW_LOG_LEVEL_CMD_RESPONSE("set_btfw_log_level,1");
const std::string SET_BT_STACK_LOG_LEVEL_RESPONSE("set_bthost_debuglog_enable,1");

class BtLogToolTest : public ::testing::Test {
 protected:
  void SetUp() override {
//    ALOGI("%s: SetUp: start", __func__);
    btlogtool::SetUp(SOCKET_SERVER_NAME);
    socket_client_.reset(new SocketClient());
    socket_cb_ = new MockSocketCallbacks();
    log_state_ob_.reset(new MockLogStateObserver);
    btlogtool::AddObserver(log_state_ob_.get());

    EXPECT_CALL(*(socket_cb_.get()), onConnected()).Times(AtLeast(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(socket_client_->connect(SOCKET_SERVER_NAME, socket_cb_));
//    ALOGI("%s: SetUp: end", __func__);
  }

  void TearDown() override {
//    ALOGI("%s: TearDown: start", __func__);
//    EXPECT_CALL(*(socket_cb_.get()), onDisconnected()).WillOnce(Return());
    socket_client_->disconnect();
    socket_cb_.clear();
    socket_client_.reset();
    log_state_ob_.reset();
    btlogtool::TearDown();
//    ALOGI("%s: TearDown: end", __func__);
  }

  std::unique_ptr<SocketClient> socket_client_;
  sp<MockSocketCallbacks> socket_cb_;
  std::unique_ptr<MockLogStateObserver> log_state_ob_;
};

TEST_F(BtLogToolTest, EnableDisableBtHciLog) {
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      ResponseMatcher(START_CMD_RESPONSE), START_CMD_RESPONSE.size()));
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      HciEnableMatcher(BluetoothLogSetting::GetInstance())));
  EXPECT_EQ(kBtHciLogUnknown, BluetoothLogSetting::GetInstance()->GetHciLogLevel());
  socket_client_->sendMessage(START_CMD.c_str(), START_CMD.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_TRUE(BluetoothLogSetting::GetInstance()->IsHciSnoopEnabled());
  EXPECT_EQ(kBtHciLogOn, BluetoothLogSetting::GetInstance()->GetHciLogLevel());

  MockSocketCallbacksHelper cmd_helper(STOP_CMD_RESPONSE);
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      NotNull(), STOP_CMD_RESPONSE.size()))
          .WillOnce(Invoke(&cmd_helper,
              &MockSocketCallbacksHelper::CheckMessageResponse));
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      HciDisableMatcher(BluetoothLogSetting::GetInstance())));
  socket_client_->sendMessage(STOP_CMD.c_str(), STOP_CMD.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_FALSE(BluetoothLogSetting::GetInstance()->IsHciSnoopEnabled());
  EXPECT_EQ(kBtHciLogOff, BluetoothLogSetting::GetInstance()->GetHciLogLevel());
}

TEST_F(BtLogToolTest, EnableDisableBtHciLogRepeatly) {
  const int kRepeatTimes(5);
  MockSocketCallbacksHelper start_cmd_helper(START_CMD_RESPONSE);
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      NotNull(), START_CMD_RESPONSE.size()))
          .Times(kRepeatTimes)
          .WillRepeatedly(Invoke(&start_cmd_helper,
              &MockSocketCallbacksHelper::CheckMessageResponse));

  MockSocketCallbacksHelper stop_cmd_helper(STOP_CMD_RESPONSE);
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      NotNull(), STOP_CMD_RESPONSE.size()))
          .Times(kRepeatTimes)
          .WillRepeatedly(Invoke(&stop_cmd_helper,
              &MockSocketCallbacksHelper::CheckMessageResponse));
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      HciEnableMatcher(BluetoothLogSetting::GetInstance())))
          .Times(kRepeatTimes);
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      HciDisableMatcher(BluetoothLogSetting::GetInstance())))
          .Times(kRepeatTimes);
  for (int i(0); i < kRepeatTimes; i++) {
    socket_client_->sendMessage(START_CMD.c_str(), START_CMD.size());
    // wait for the command being sent
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(BluetoothLogSetting::GetInstance()->IsHciSnoopEnabled());

    socket_client_->sendMessage(STOP_CMD.c_str(), STOP_CMD.size());
    // wait for the command being sent
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(BluetoothLogSetting::GetInstance()->IsHciSnoopEnabled());
  }
}

TEST_F(BtLogToolTest, SetHostLogSize) {
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      ResponseMatcher(SET_LOG_SIZE_CMD_RESPONSE),
        SET_LOG_SIZE_CMD_RESPONSE.size()));

  socket_client_->sendMessage(SET_LOG_SIZE_CMD.c_str(), SET_LOG_SIZE_CMD.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  const int LOG_SIZE(600);
  EXPECT_EQ(LOG_SIZE, BluetoothLogSetting::GetInstance()->GetHciLogMaxSize());
}

TEST_F(BtLogToolTest, SetHostLogPath) {
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      ResponseMatcher(SET_LOG_PATH_CMD_RESPONSE),
        SET_LOG_PATH_CMD_RESPONSE.size()));

  socket_client_->sendMessage(SET_LOG_PATH_CMD.c_str(), SET_LOG_PATH_CMD.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  const std::string LOG_PATH("/sdcard");
  EXPECT_EQ(BluetoothLogSetting::GetInstance()->GetHciLogPath(), LOG_PATH);
}

TEST_F(BtLogToolTest, SetHostLogLevel) {
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      ResponseMatcher(SET_BT_STACK_LOG_LEVEL_RESPONSE),
      SET_BT_STACK_LOG_LEVEL_RESPONSE.size()))
          .Times(2);

  const BtHostLogLevel kHostLogLevel[] = {
      kBtHostLogOff,
      kBtHostLogSqc
  };
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      BluetoothLogSetting::GetInstance()))
          .Times(2)
          .WillOnce(Invoke([&kHostLogLevel](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kHostLogLevel[1], setting->GetHostLogLevel());
             }))
          .WillOnce(Invoke([&kHostLogLevel](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kHostLogLevel[0], setting->GetHostLogLevel());
             }));

  std::string cmd = SET_BT_STACK_LOG_LEVEL_CMD + std::to_string(kBtHostLogSqc);
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  char log_level[128] = {'\0'};
  property_get(BT_HOST_LOG_LEVEL_CTL, log_level, NULL);
  EXPECT_STREQ(log_level, SQC);
  EXPECT_EQ(BluetoothLogSetting::GetInstance()->GetHostLogLevel(), kBtHostLogSqc);

  cmd = SET_BT_STACK_LOG_LEVEL_CMD + std::to_string(kBtHostLogOff);
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  memset(log_level, 0, sizeof(log_level)/sizeof(log_level[0]));
  property_get(BT_HOST_LOG_LEVEL_CTL, log_level, NULL);
  EXPECT_STREQ(log_level, OFF);
  EXPECT_EQ(BluetoothLogSetting::GetInstance()->GetHostLogLevel(), kBtHostLogOff);
}

TEST_F(BtLogToolTest, SetFWLogLevel) {
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      ResponseMatcher(SET_BT_FW_LOG_LEVEL_CMD_RESPONSE),
        SET_BT_FW_LOG_LEVEL_CMD_RESPONSE.size()))
          .Times(3);
  const BtFwLogLevel kFwLogLevel[] = {
      kBtFwLogOff,
      kBtFwLogSqc,
      kBtFwLogDebug
  };
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      BluetoothLogSetting::GetInstance()))
          .Times(3)
          .WillOnce(Invoke([&kFwLogLevel](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kFwLogLevel[1], setting->GetFwLogLevel());
             }))
          .WillOnce(Invoke([&kFwLogLevel](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kFwLogLevel[2], setting->GetFwLogLevel());
             }))
          .WillOnce(Invoke([&kFwLogLevel](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kFwLogLevel[0], setting->GetFwLogLevel());
             }));
  std::string cmd = SET_BT_FW_LOG_LEVEL_CMD + std::to_string(kBtFwLogSqc);
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_EQ(BluetoothLogSetting::GetInstance()->GetFwLogLevel(), kBtFwLogSqc);

  cmd = SET_BT_FW_LOG_LEVEL_CMD + std::to_string(kBtFwLogDebug);
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_EQ(BluetoothLogSetting::GetInstance()->GetFwLogLevel(), kBtFwLogDebug);

  cmd = SET_BT_FW_LOG_LEVEL_CMD + std::to_string(kBtFwLogOff);
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_EQ(BluetoothLogSetting::GetInstance()->GetFwLogLevel(), kBtFwLogOff);
}

TEST_F(BtLogToolTest, SetBtHciAutoStart) {
  EXPECT_CALL(*(socket_cb_.get()), onMessageReceived(
      ResponseMatcher(AUTOSTART_CMD_RESPONSE),
        AUTOSTART_CMD_RESPONSE.size()))
          .Times(2);
  const bool kAutoStart[] = {
      true,
      false,
  };
  EXPECT_CALL(*(log_state_ob_.get()), OnBtLogStateChanged(
      BluetoothLogSetting::GetInstance()))
          .Times(2)
          .WillOnce(Invoke([&kAutoStart](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kAutoStart[0], setting->IsAutoStartHciSnoop());
             }))
          .WillOnce(Invoke([&kAutoStart](const BluetoothLogSetting* setting) {
               EXPECT_EQ(kAutoStart[1], setting->IsAutoStartHciSnoop());
             }));
  std::string cmd = AUTOSTART_CMD + std::string("1");
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_TRUE(BluetoothLogSetting::GetInstance()->IsAutoStartHciSnoop());

  cmd = AUTOSTART_CMD + std::string("0");
  socket_client_->sendMessage(cmd.c_str(), cmd.size());
  // wait for the command being sent
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_FALSE(BluetoothLogSetting::GetInstance()->IsAutoStartHciSnoop());
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor
