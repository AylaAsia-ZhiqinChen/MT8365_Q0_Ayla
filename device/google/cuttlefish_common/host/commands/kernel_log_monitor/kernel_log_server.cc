/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "host/commands/kernel_log_monitor/kernel_log_server.h"

#include <map>
#include <utility>

#include <glog/logging.h>
#include <netinet/in.h>
#include "common/libs/fs/shared_select.h"

using cvd::SharedFD;

namespace {
static const std::map<std::string, std::string> kInformationalPatterns = {
    {"] Linux version ", "GUEST_KERNEL_VERSION: "},
    {"GUEST_BUILD_FINGERPRINT: ", "GUEST_BUILD_FINGERPRINT: "},
};

static const std::map<std::string, monitor::BootEvent> kStageToEventMap = {
    {"VIRTUAL_DEVICE_BOOT_STARTED", monitor::BootEvent::BootStarted},
    {"VIRTUAL_DEVICE_BOOT_COMPLETED", monitor::BootEvent::BootCompleted},
    {"VIRTUAL_DEVICE_BOOT_FAILED", monitor::BootEvent::BootFailed},
    {"VIRTUAL_DEVICE_NETWORK_MOBILE_CONNECTED",
     monitor::BootEvent::MobileNetworkConnected},
    {"VIRTUAL_DEVICE_NETWORK_WIFI_CONNECTED",
     monitor::BootEvent::WifiNetworkConnected},
    // TODO(b/131864854): Replace this with a string less likely to change
    {"init: starting service 'adbd'", monitor::BootEvent::AdbdStarted},
};

void ProcessSubscriptions(
    monitor::BootEvent evt,
    std::vector<monitor::BootEventCallback>* subscribers) {
  auto active_subscription_count = subscribers->size();
  std::size_t idx = 0;
  while (idx < active_subscription_count) {
    // Call the callback
    auto action = (*subscribers)[idx](evt);
    if (action == monitor::SubscriptionAction::ContinueSubscription) {
      ++idx;
    } else {
      // Cancel the subscription by swaping it with the last active subscription
      // and decreasing the active subscription count
      --active_subscription_count;
      std::swap((*subscribers)[idx], (*subscribers)[active_subscription_count]);
    }
  }
  // Keep only the active subscriptions
  subscribers->resize(active_subscription_count);
}
}  // namespace

namespace monitor {
KernelLogServer::KernelLogServer(cvd::SharedFD server_socket,
                                 const std::string& log_name,
                                 bool deprecated_boot_completed)
    : server_fd_(server_socket),
      log_fd_(cvd::SharedFD::Open(log_name.c_str(), O_CREAT | O_RDWR, 0666)),
      deprecated_boot_completed_(deprecated_boot_completed) {}

void KernelLogServer::BeforeSelect(cvd::SharedFDSet* fd_read) const {
  if (!client_fd_->IsOpen()) fd_read->Set(server_fd_);
  if (client_fd_->IsOpen()) fd_read->Set(client_fd_);
}

void KernelLogServer::AfterSelect(const cvd::SharedFDSet& fd_read) {
  if (fd_read.IsSet(server_fd_)) HandleIncomingConnection();

  if (client_fd_->IsOpen() && fd_read.IsSet(client_fd_)) {
    if (!HandleIncomingMessage()) {
      client_fd_->Close();
    }
  }
}

void KernelLogServer::SubscribeToBootEvents(
    monitor::BootEventCallback callback) {
  subscribers_.push_back(callback);
}

// Accept new kernel log connection.
void KernelLogServer::HandleIncomingConnection() {
  if (client_fd_->IsOpen()) {
    LOG(ERROR) << "Client already connected. No longer accepting connection.";
    return;
  }

  client_fd_ = SharedFD::Accept(*server_fd_, nullptr, nullptr);
  if (!client_fd_->IsOpen()) {
    LOG(ERROR) << "Client connection failed: " << client_fd_->StrError();
    return;
  }
  if (client_fd_->Fcntl(F_SETFL, O_NONBLOCK) == -1) {
    LOG(ERROR) << "Client connection refused O_NONBLOCK: " << client_fd_->StrError();
  }
}

bool KernelLogServer::HandleIncomingMessage() {
  const size_t buf_len = 256;
  char buf[buf_len];
  ssize_t ret = client_fd_->Read(buf, buf_len);
  if (ret < 0) {
    LOG(ERROR) << "Could not read from QEmu serial port: " << client_fd_->StrError();
    return false;
  }
  if (ret == 0) return false;
  // Write the log to a file
  if (log_fd_->Write(buf, ret) < 0) {
    LOG(ERROR) << "Could not write kernel log to file: " << log_fd_->StrError();
    return false;
  }

  // Detect VIRTUAL_DEVICE_BOOT_*
  for (ssize_t i=0; i<ret; i++) {
    if ('\n' == buf[i]) {
      for (auto& info_kv : kInformationalPatterns) {
        auto& match = info_kv.first;
        auto& prefix = info_kv.second;
        auto pos = line_.find(match);
        if (std::string::npos != pos) {
          LOG(INFO) << prefix << line_.substr(pos + match.size());
        }
      }
      for (auto& stage_kv : kStageToEventMap) {
        auto& stage = stage_kv.first;
        auto event = stage_kv.second;
        if (std::string::npos != line_.find(stage)) {
          // Log the stage
          LOG(INFO) << stage;
          ProcessSubscriptions(event, &subscribers_);
          //TODO(b/69417553) Remove this when our clients have transitioned to the
          // new boot completed
          if (deprecated_boot_completed_) {
            // Write to host kernel log
            FILE* log = popen("/usr/bin/sudo /usr/bin/tee /dev/kmsg", "w");
            fprintf(log, "%s\n", stage.c_str());
            fclose(log);
          }
        }
      }
      line_.clear();
    }
    line_.append(1, buf[i]);
  }

  return true;
}

}  // namespace monitor
