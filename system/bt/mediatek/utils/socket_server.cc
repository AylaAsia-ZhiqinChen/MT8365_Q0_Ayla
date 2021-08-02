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

#include "socket_server.h"

#include <poll.h>

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <map>
#include <mutex>
#include <thread>

#include <cutils/fs.h>
#include <cutils/sockets.h>

#include "chre_log.h"

namespace android {
namespace chre {

SocketServer::SocketServer()
    : mSockFd(INVALID_SOCKET),
      mNextClientId(1),
      mPollFds{},
      mClientMessageCallback(nullptr),
      signal_received_{false} {
  // Initialize the socket fds field for all inactive client slots to -1, so
  // poll skips over it, and we don't attempt to send on it
  for (size_t i = 0; i <= kMaxActiveClients; i++) {
    mPollFds[i].fd = -1;
    mPollFds[i].events = POLLIN;
  }
}

void SocketServer::run(const char *socketName, bool allowSocketCreation,
                       ClientMessageCallback clientMessageCallback) {
  mClientMessageCallback = clientMessageCallback;

  mSockFd = android_get_control_socket(socketName);
  if (mSockFd == INVALID_SOCKET && allowSocketCreation) {
    LOGI("Didn't inherit socket, creating...");
    mSockFd = socket_local_server(socketName,
        ANDROID_SOCKET_NAMESPACE_ABSTRACT,
        SOCK_STREAM);
  }

  if (mSockFd == INVALID_SOCKET) {
    LOGE("Couldn't get/create socket");
  } else {
    int ret = listen(mSockFd, kMaxPendingConnectionRequests);
    if (ret < 0) {
      LOG_ERROR("Couldn't listen on socket", errno);
    } else {
      serviceSocket();
    }

    {
      std::lock_guard<std::mutex> lock(mClientsMutex);
      for (const auto& pair : mClients) {
        int clientSocket = pair.first;
        if (close(clientSocket) != 0) {
          LOGI("Couldn't close client %" PRIu16 "'s socket: %s",
               pair.second.clientId, strerror(errno));
        }
      }
      mClients.clear();
      LOGI("All clients are cleared");
    }
    socket_close(mSockFd);
    mSockFd = INVALID_SOCKET;
    std::atomic_exchange(&signal_received_, false);
  }
}

void SocketServer::sendToAllClients(const void *data, size_t length) {
  std::lock_guard<std::mutex> lock(mClientsMutex);

  int deliveredCount = 0;
  for (const auto& pair : mClients) {
    int clientSocket = pair.first;
    uint16_t clientId = pair.second.clientId;
    if (sendToClientSocket(data, length, clientSocket, clientId)) {
      deliveredCount++;
    } else {
      LOG_ERROR("send fail caused by: ", errno);
      break;
    }
  }

  if (deliveredCount == 0) {
    LOGW("Got message but didn't deliver to any clients");
  }
}

bool SocketServer::sendToClientById(const void *data, size_t length,
                                    uint16_t clientId) {
  std::lock_guard<std::mutex> lock(mClientsMutex);

  bool sent = false;
  for (const auto& pair : mClients) {
    uint16_t thisClientId = pair.second.clientId;
    if (thisClientId == clientId) {
      int clientSocket = pair.first;
      sent = sendToClientSocket(data, length, clientSocket, thisClientId);
      break;
    }
  }

  return sent;
}

void SocketServer::acceptClientConnection() {
  struct sockaddr addr;
  socklen_t addr_len = sizeof(addr);
  int clientSocket = accept(mSockFd, &addr, &addr_len);
  if (clientSocket < 0) {
    LOG_ERROR("Couldn't accept client connection", errno);
  } else if (mClients.size() >= kMaxActiveClients) {
    LOGW("Rejecting client request - maximum number of clients reached");
    close(clientSocket);
  } else {
    ClientData clientData;
    clientData.clientId = mNextClientId++;

    // We currently don't handle wraparound - if we're getting this many
    // connects/disconnects, then something is wrong.
    // TODO: can handle this properly by iterating over the existing clients to
    // avoid a conflict.
    if (clientData.clientId == 0) {
      LOGE("Couldn't allocate client ID");
      std::exit(-1);
    }

    bool slotFound = false;
    for (size_t i = kClientStartIndex; i <= kMaxActiveClients; i++) {
      if (mPollFds[i].fd < 0) {
        mPollFds[i].fd = clientSocket;
        slotFound = true;
        break;
      }
    }

    if (!slotFound) {
      LOGE("Couldn't find slot for client!");
      assert(slotFound);
      close(clientSocket);
    } else {
      {
        std::lock_guard<std::mutex> lock(mClientsMutex);
        mClients[clientSocket] = clientData;
      }
      LOGI("Accepted new client connection (count %zu), assigned client ID %"
           PRIu16, mClients.size(), clientData.clientId);
    }
  }
}

void SocketServer::handleClientData(int clientSocket) {
  const ClientData& clientData = mClients[clientSocket];
  uint16_t clientId = clientData.clientId;

  uint8_t buffer[kMaxPacketSize];
  ssize_t packetSize = TEMP_FAILURE_RETRY(
      recv(clientSocket, buffer, sizeof(buffer), MSG_DONTWAIT));
  if (packetSize < 0) {
    LOGE("Couldn't get packet from client %" PRIu16 ": %s", clientId,
         strerror(errno));
  } else if (packetSize == 0) {
    LOGI("Client %" PRIu16 " disconnected", clientId);
    disconnectClient(clientSocket);
  } else {
    LOGV("Got %zd byte packet from client %" PRIu16, packetSize, clientId);
    mClientMessageCallback(clientId, buffer, packetSize);
  }
}

void SocketServer::disconnectClient(int clientSocket) {
  {
    std::lock_guard<std::mutex> lock(mClientsMutex);
    mClients.erase(clientSocket);
  }
  close(clientSocket);

  bool removed = false;
  for (size_t i = kClientStartIndex; i <= kMaxActiveClients; i++) {
    if (mPollFds[i].fd == clientSocket) {
      mPollFds[i].fd = -1;
      removed = true;
      break;
    }
  }

  if (!removed) {
    LOGE("Out of sync");
    assert(removed);
  }
}

bool SocketServer::sendToClientSocket(const void *data, size_t length,
                                      int clientSocket, uint16_t clientId) {
  errno = 0;
  ssize_t bytesSent = TEMP_FAILURE_RETRY(send(clientSocket, data, length, 0));
  if (bytesSent < 0) {
    LOGE("Error sending packet of size %zu to client %" PRIu16 ": %s",
         length, clientId, strerror(errno));
  } else if (bytesSent == 0) {
    LOGW("Client %" PRIu16 " disconnected before message could be delivered",
         clientId);
  } else {
    LOGV("Delivered message of size %zu bytes to client %" PRIu16, length,
         clientId);
  }

  return (bytesSent > 0);
}

void SocketServer::serviceSocket() {
  static_assert(kListenIndex == 0, "Code assumes that the first index is "
                "always the listen socket");

  mPollFds[kListenIndex].fd = mSockFd;
  mPollFds[kListenIndex].events = POLLIN;

  LOGI("Ready to accept connections");
  while (!signal_received_) {
    int ret = ppoll(mPollFds, 1 + kMaxActiveClients, nullptr, nullptr);
    if (ret == -1) {
      LOGI("Exiting poll loop: %s", strerror(errno));
      break;
    }

    if (mPollFds[kListenIndex].revents & POLLIN) {
      acceptClientConnection();
    }

    for (size_t i = kClientStartIndex; i <= kMaxActiveClients; i++) {
      if (mPollFds[i].fd < 0) {
        continue;
      }

      if (mPollFds[i].revents & POLLIN) {
        handleClientData(mPollFds[i].fd);
      }
    }
  }
}

void SocketServer::StopPoll() {
  std::atomic_exchange(&signal_received_, true);
  {
    std::lock_guard<std::mutex> lock(mClientsMutex);
    shutdown(mPollFds[kListenIndex].fd, SHUT_RDWR);
  }
}

}  // namespace chre
}  // namespace android
