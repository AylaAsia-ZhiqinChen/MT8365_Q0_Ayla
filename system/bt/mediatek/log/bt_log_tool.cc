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

/*******************************************************************************
 *
 *  Filename:      bt_log_tool.cc
 *
 *  Description:   Bluetooth log control
 *
 ******************************************************************************/
#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "mtk.bt.logtool"

#include "bt_log_tool.h"

#include "bt_log_command_handler.h"
#include "bt_logmode_watcher.h"
#include "chre_log.h"
#include "osi/include/osi.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {
namespace btlogtool {

static BtLogCmdHandler* log_cmd_handler(nullptr);
static BtLogModeWatcher* log_mode_watcher(nullptr);

bool SetUp(const char* sockect_server_name) {
  LOGI("%s", __func__);

  log_cmd_handler = new BtLogCmdHandler();
  log_mode_watcher = new BtLogModeWatcher();

  if (log_mode_watcher) {
    constexpr char kDefaultSocketServerName[] = "bthostlogd";
    constexpr char kSocketServerThreadName[] = "btlogwatcher";
    log_mode_watcher->Start(
        kSocketServerThreadName,
        sockect_server_name ? sockect_server_name : kDefaultSocketServerName,
        [](UNUSED_ATTR uint16_t clientId,
            void *data, size_t len) {
          log_cmd_handler->Handle(data, len);
        },
        OnSocketChange);
  }

  if (log_cmd_handler &&
      log_mode_watcher) {
    return true;
  } else {
    return false;
  }
}

void TearDown(void) {
  LOGI("%s", __func__);

  if (log_mode_watcher) {
    log_mode_watcher->Stop();
    delete log_mode_watcher;
    log_mode_watcher = nullptr;
  }

  if (log_cmd_handler) {
    delete log_cmd_handler;
    log_cmd_handler = nullptr;
  }
}

void OnSocketChange(android::chre::SocketServer* server) {
  log_cmd_handler->set_server(server);
}

void AddObserver(BtLogStateObserver *ob) {
  if (ob && log_cmd_handler) {
    log_cmd_handler->AddObserver(ob);
  }
}

void RemoveObserver(BtLogStateObserver *ob) {
  if (ob && log_cmd_handler) {
    log_cmd_handler->RemoveObserver(ob);
  }
}

}  // namespace btlogtool
}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
