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
#include "chre_interface.h"

#include <android-base/logging.h>

#include <chrono>

#include "chre_host/host_protocol_host.h"

using android::chre::getStringFromByteVector;
using android::chre::HostProtocolHost;
using flatbuffers::FlatBufferBuilder;

namespace chre_constants = android::hardware::wifi::offload::V1_0::implementation::chre_constants;

namespace fbs = ::chre::fbs;

namespace android {
namespace hardware {
namespace wifi {
namespace offload {
namespace V1_0 {
namespace implementation {

SocketCallbacks::SocketCallbacks(ChreInterface* parent) : mParent(parent) {
}

void SocketCallbacks::onMessageReceived(const void* data, size_t length) {
    LOG(VERBOSE) << "Message received from CHRE socket";
    if (!HostProtocolHost::decodeMessageFromChre(data, length, *this)) {
        LOG(WARNING) << "Failed to decode message";
    }
}

void SocketCallbacks::onConnected() {
    LOG(INFO) << "Connected to CHRE socket";
    mParent->reportConnectionEvent(ChreInterfaceCallbacks::CONNECTED);
}

void SocketCallbacks::onConnectionAborted() {
    LOG(WARNING) << "Connection to CHRE socket Aborted";
    mParent->reportConnectionEvent(ChreInterfaceCallbacks::CONNECTION_ABORT);
}

void SocketCallbacks::onDisconnected() {
    LOG(WARNING) << "Disconnected from CHRE socket";
    mParent->reportConnectionEvent(ChreInterfaceCallbacks::DISCONNECTED);
}

void SocketCallbacks::handleNanoappMessage(const fbs::NanoappMessageT& message) {
    LOG(VERBOSE) << "handleNanoappMessage from appId: " << message.app_id;
    LOG(VERBOSE) << "HostEndPoint: " << message.host_endpoint;
    if (message.app_id == chre_constants::kWifiOffloadNanoAppId) {
        mParent->handleMessage(message.message_type, message.message.data(), message.message.size());
    }
}

void SocketCallbacks::handleHubInfoResponse(const fbs::HubInfoResponseT& response) {
    LOG(VERBOSE) << "Hub Info response";
    LOG(VERBOSE) << "Hub Info name: " << getStringFromByteVector(response.name);
    LOG(VERBOSE) << "Version : " << response.chre_platform_version;
    LOG(VERBOSE) << "Legacy Platform Version: " << response.platform_version;
    LOG(VERBOSE) << "Legacy Toolchain Version: " << response.toolchain_version;
    LOG(VERBOSE) << "Peak Mips: " << response.peak_mips;
    LOG(VERBOSE) << "Stopped Power: " << response.stopped_power;
    LOG(VERBOSE) << "Sleep Power: " << response.sleep_power;
    LOG(VERBOSE) << "Peak Power: " << response.peak_power;
    LOG(VERBOSE) << "Platform ID: " << response.platform_id;
    LOG(VERBOSE) << "Vendor : " << getStringFromByteVector(response.vendor);
    LOG(VERBOSE) << "Toolchain : " << getStringFromByteVector(response.toolchain);
    LOG(VERBOSE) << "maxMessageLen : " << response.max_msg_len;
    if (response.max_msg_len < chre_constants::kMaxMessageLen) {
        LOG(WARNING) << "Incorrect max message length";
    }
}

void SocketCallbacks::handleNanoappListResponse(const fbs::NanoappListResponseT& response) {
    LOG(VERBOSE) << "handleNanoAppListResponse";
    for (const std::unique_ptr<fbs::NanoappListEntryT>& nanoapp : response.nanoapps) {
        if (nanoapp == nullptr) {
            continue;
        }
        if (nanoapp->app_id == chre_constants::kWifiOffloadNanoAppId && nanoapp->enabled) {
            LOG(INFO) << "Wifi Offload Nano app found";
            LOG(INFO) << "Version: " << nanoapp->version;
            break;
        }
    }
}

void SocketCallbacks::handleLoadNanoappResponse(const fbs::LoadNanoappResponseT& response) {
    LOG(VERBOSE) << "Load Nano app response";
    LOG(VERBOSE) << "Transaction ID: " << response.transaction_id;
    LOG(VERBOSE) << "Status: " << response.success;
}

void SocketCallbacks::handleUnloadNanoappResponse(const fbs::UnloadNanoappResponseT& response) {
    LOG(VERBOSE) << "Unload Nano app response";
    LOG(VERBOSE) << "Transaction ID: " << response.transaction_id;
    LOG(VERBOSE) << "Status: " << response.success;
}

ChreInterface::ChreInterface(ChreInterfaceCallbacks* callback)
    : mSocketCallbacks(new SocketCallbacks(this)), mServerCallbacks(callback),
      mSocketConnected(false) {
    if (!mClient.connectInBackground(chre_constants::kSocketName, mSocketCallbacks)) {
        LOG(ERROR) << "Offload HAL is not connected to Chre";
    }
}

ChreInterface::~ChreInterface() {
    mClient.disconnect();
}

bool ChreInterface::isConnected() {
    std::lock_guard<std::mutex> lock(mChreInterfaceLock);
    return mSocketConnected;
}

void ChreInterface::reportConnectionEvent(ChreInterfaceCallbacks::ConnectionEvent event) {
    bool connectionStatus = false;
    switch (event) {
        case ChreInterfaceCallbacks::ConnectionEvent::CONNECTED:
            connectionStatus = true;
            if (!getHubInfo() || !getNanoAppList()) {
                LOG(WARNING) << "Unable to get platform and nano app info";
            }
            break;
        case ChreInterfaceCallbacks::ConnectionEvent::DISCONNECTED:
        case ChreInterfaceCallbacks::ConnectionEvent::CONNECTION_ABORT:
            break;
        default:
            LOG(WARNING) << "Invalid connection event recieved";
            return;
    }
    {
        std::lock_guard<std::mutex> lock(mChreInterfaceLock);
        mSocketConnected = connectionStatus;
    }
    mServerCallbacks->handleConnectionEvents(event);
}

bool ChreInterface::sendCommandToApp(uint32_t messageType, const std::vector<uint8_t>& message) {
    FlatBufferBuilder builder(chre_constants::kMaxMessageLen);
    void* messageData = nullptr;
    size_t messageDataLen = message.size();
    if (messageDataLen > 0) {
        messageData = (void*)message.data();
    }
    HostProtocolHost::encodeNanoappMessage(builder, chre_constants::kWifiOffloadNanoAppId,
                                           messageType, 0, messageData, messageDataLen);
    if (!mClient.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
        LOG(WARNING) << "Failed to send message to Nano app";
        return false;
    }
    LOG(VERBOSE) << "Command sent " << messageType;
    return true;
}

void ChreInterface::handleMessage(uint32_t messageType, const void* messageData,
                                  size_t messageDataLen) {
    const uint8_t* messageBuf = reinterpret_cast<const uint8_t*>(messageData);
    std::vector<uint8_t> message(messageBuf, messageBuf + messageDataLen);
    mServerCallbacks->handleMessage(messageType, message);
}

bool ChreInterface::getHubInfo() {
    LOG(VERBOSE) << "getHubInfo";

    FlatBufferBuilder builder(chre_constants::kHubInfoRequestBufLen);
    HostProtocolHost::encodeHubInfoRequest(builder);
    if (!mClient.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
        LOG(WARNING) << "Failed to send Hub Info request";
        return false;
    }
    return true;
}

bool ChreInterface::getNanoAppList() {
    LOG(VERBOSE) << "getNanoAppList";
    FlatBufferBuilder builder(chre_constants::kNanoAppListRequestBufLen);
    HostProtocolHost::encodeNanoappListRequest(builder);

    if (!mClient.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
        LOG(WARNING) << "Unable to send Nano app List request";
        return false;
    }
    return true;
}
}  // namespace implementation
}  // namespace V1_0
}  // namespace offload
}  // namespace wifi
}  // namespace hardware
}  // namespace android
