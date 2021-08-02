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

// The code is from AOSP /system/chre/host/common

#pragma once

#include <poll.h>

#include <atomic>
#include <functional>
#include <map>
#include <mutex>

#include <android-base/macros.h>
#include <cutils/sockets.h>

namespace android {
namespace chre {

class SocketServer {
 public:
  SocketServer();

  /**
   * Defines the function signature of the callback given to run() which
   * receives message data sent in by a client.
   *
   * @param clientId A unique identifier for the client that sent this request
   *        (assigned locally)
   * @param data Pointer to buffer containing the raw message data
   * @param len Number of bytes of data received
   */
  typedef std::function<void(uint16_t clientId, void *data, size_t len)>
      ClientMessageCallback;

  /**
   * Opens the socket, and runs the receive loop until an error is encountered,
   * or SIGINT/SIGTERM is received. Masks off all other signals.
   *
   * @param socketName Android socket name to use when listening
   * @param allowSocketCreation If true, allow creation of the socket rather
   *        than strictly inheriting it from init (used primarily for
   *        development purposes)
   * @param clientMessageCallback Callback to be invoked when a message is
   *        received from a client
   */
  void run(const char *socketName, bool allowSocketCreation,
           ClientMessageCallback clientMessageCallback);

  /**
   * Delivers data to all connected clients. This method is thread-safe.
   *
   * @param data Pointer to buffer containing message data
   * @param length Number of bytes of data to send
   */
  void sendToAllClients(const void *data, size_t length);

  /**
   * Sends a message to one client, specified via its unique client ID. This
   * method is thread-safe.
   *
   * @param data
   * @param length
   * @param clientId
   *
   * @return true if the message was successfully sent to the specified client
   */
  bool sendToClientById(const void *data, size_t length, uint16_t clientId);

  void StopPoll();

 private:
  static constexpr int kMaxPendingConnectionRequests = 4;
  static constexpr size_t kMaxActiveClients = 4;
  static constexpr size_t kMaxPacketSize = 512;
  static constexpr size_t kListenIndex = 0;
  static constexpr size_t kClientStartIndex = 1;

  int mSockFd;
  uint16_t mNextClientId;
  // TODO: std::vector-ify this
  struct pollfd mPollFds[1 + kMaxActiveClients];

  struct ClientData {
    uint16_t clientId;
  };

  // Maps from socket FD to ClientData
  std::map<int, ClientData> mClients;

  // Ensures that mClients can be safely iterated over from other threads
  // without worrying about potential modification from the RX thread
  std::mutex mClientsMutex;

  ClientMessageCallback mClientMessageCallback;

  void acceptClientConnection();
  void disconnectClient(int clientSocket);
  void handleClientData(int clientSocket);
  bool sendToClientSocket(const void *data, size_t length, int clientSocket,
                          uint16_t clientId);
  void serviceSocket();

  std::atomic<bool> signal_received_;

  DISALLOW_COPY_AND_ASSIGN(SocketServer);
};

}  // namespace chre
}  // namespace android

