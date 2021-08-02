//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#include <string>
#include <thread>

#include "socket_server.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

using SocketChangeCallback = std::function<void(android::chre::SocketServer*)>;

class BtLogModeWatcher {
 public:
  BtLogModeWatcher() = default;
  ~BtLogModeWatcher() = default;

  int Start(const std::string& thread_name,
      const std::string& socket_name,
      android::chre::SocketServer::ClientMessageCallback cb,
      SocketChangeCallback socket_change_cb);

  void Stop();
  std::string thread_name() const {
    return thread_name_;
  }

 private:
  BtLogModeWatcher(const BtLogModeWatcher&) = delete;
  BtLogModeWatcher& operator=(const BtLogModeWatcher&) = delete;

  void ThreadRoutine(std::string socket_name,
      android::chre::SocketServer::ClientMessageCallback cb,
      SocketChangeCallback socket_change_cb);

  bool terminate_;
  android::chre::SocketServer* server_;
  std::thread thread_;
  std::string thread_name_;
};

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
