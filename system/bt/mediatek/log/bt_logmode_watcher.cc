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
#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "mtk.bt.logtool-watcher"

#include "bt_logmode_watcher.h"
#include <errno.h>
#include <sys/prctl.h>
#include <csignal>
#include "chre_log.h"
#include "osi/include/compat.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

int BtLogModeWatcher::Start(const std::string &thread_name,
    const std::string& socket_name,
    android::chre::SocketServer::ClientMessageCallback cb,
    SocketChangeCallback socket_change_cb) {
  thread_name_ = thread_name;
  server_ = nullptr;
  terminate_ = false;
  // Bug fix: need a copy for socket_name here and in ThreadRoutine()
  // To avoid socket_name is freed after the invoking function out of scope
  thread_ = std::thread([this, socket_name, cb, socket_change_cb]() {
    ThreadRoutine(std::move(socket_name), cb, socket_change_cb);
  });
  if (!thread_.joinable()) {
    return -1;
  } else {
    return 0;
  }
}

void BtLogModeWatcher::Stop() {
  terminate_ = true;
  if(server_) {
    LOGI("Stop server");
    server_->StopPoll();
  } else {
    LOGI("Stop server, but thread may already terminated");
  }
  if (std::this_thread::get_id() != thread_.get_id()) {
    thread_.join();
  }
}

void BtLogModeWatcher::ThreadRoutine(std::string socket_name,
    android::chre::SocketServer::ClientMessageCallback cb,
    SocketChangeCallback socket_change_cb) {
  const int kThreadNameLenMax(16);
  char name_buffer[kThreadNameLenMax+1] = {'0'};
  size_t copy_len = thread_name_.length() < kThreadNameLenMax ?
      thread_name_.length() : kThreadNameLenMax;
  thread_name_.copy(name_buffer, copy_len);
  pid_t tid = gettid();
  if (prctl(PR_SET_NAME, (unsigned long)name_buffer) == -1) {
    LOGE("unable to set thread name: %s for %s",
        name_buffer, strerror(errno));
  } else {
    LOGI("thread id %d, thread name %s started", tid, name_buffer);
  }

  /** M: Bug fix for ALPS04301211: restart socket if broken @{ */
  int i = 0;
  while(!terminate_ && i<=5) {
    server_ = new android::chre::SocketServer();
    if (server_) {
      socket_change_cb(server_);
      LOGI("new socket created [#%d]", i);
      server_->run(socket_name.c_str(), true, cb);

      // run done, run next loop if terminate_ is false
      LOGI("delete socket [#%d]", i);
      delete server_;
      server_ = nullptr;
    }
    i++;
  }
  /** @} */

  LOGI("thread id %d, thread name %s exited", tid, name_buffer);
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
