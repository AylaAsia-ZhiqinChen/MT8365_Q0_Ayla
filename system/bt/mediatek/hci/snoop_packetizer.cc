/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "bt_log_snoop_packetizer"

#include "snoop_packetizer.h"

#include <inttypes.h>
#include <netinet/in.h>
#include <string.h>

#include <base/logging.h>
#include <base/synchronization/waitable_event.h>

#include "bt_hci_bdroid.h"
#include "bt_types.h"
#include "mtk_util.h"
#include "osi/include/log.h"
#include "snoop_log_config.h"

// To disable by default
#define DBG_LOG_ENABLE FALSE

#if DBG_LOG_ENABLE == TRUE
#define DBG_LOG(tag, fmt, args...) LOG_DEBUG(LOG_TAG, fmt, ##args)
#else
#define DBG_LOG(tag, fmt, args...) ((void)0)
#endif

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

BTSnoopPacketWrapper& BTSnoopPacketWrapper::operator=(
    const BTSnoopPacketWrapper& wrapper) {
  if (this != &wrapper) {
    is_received_ = wrapper.is_received();
    timestamp_us_ = wrapper.timestamp_us();
    delete [] buffer_;
    buffer_ = Clone(wrapper.buffer());
  }
  return *this;
}

BTSnoopPacketWrapper::~BTSnoopPacketWrapper() {
  if (buffer_) {
    delete [] buffer_;
    buffer_ = nullptr;
  }
}

BT_HDR* BTSnoopPacketWrapper::Clone(const BT_HDR* buffer) {
  BT_HDR* packet = nullptr;
  if (buffer) {
    // Total buffer size = len + offset
    size_t packet_size = buffer->len + sizeof(BT_HDR) + buffer->offset;
    packet = reinterpret_cast<BT_HDR*>(new uint8_t[packet_size]);
    if (packet) {
      packet->offset = buffer->offset;
      packet->len = buffer->len;
      packet->layer_specific = buffer->layer_specific;
      packet->event = buffer->event;
      memcpy(packet->data, buffer->data, buffer->len+buffer->offset);
    }
  }
  return packet;
}

void BtSnoopPacketizer::OnDataReady(const BT_HDR* buffer,
    bool is_received, uint64_t timestamp_us) {
  parcels_.push_back(MakeParcel(buffer, is_received, timestamp_us));
  if (IsParcelReady()) {
    parcels_ready_cb_(parcels_, NULL);
    parcels_.clear();
  }
}

BTSnoopParcel BtSnoopPacketizer::MakeParcel(const BT_HDR* buffer,
    bool is_received, uint64_t timestamp_us) {
  uint8_t* p = const_cast<uint8_t*>(buffer->data + buffer->offset);
  switch (buffer->event & MSG_EVT_MASK) {
    case MSG_HC_TO_STACK_HCI_EVT:
      return ComposePackets(kEventPacket, p, false, timestamp_us);
    case MSG_HC_TO_STACK_HCI_ACL:
    case MSG_STACK_TO_HC_HCI_ACL:
      return ComposePackets(kAclPacket, p, is_received, timestamp_us);
    case MSG_HC_TO_STACK_HCI_SCO:
    case MSG_STACK_TO_HC_HCI_SCO:
      return ComposePackets(kScoPacket, p, is_received, timestamp_us);
    case MSG_STACK_TO_HC_HCI_CMD:
      return ComposePackets(kCommandPacket, p, true, timestamp_us);
    default: {
      // Should never goes into here
      LOG_ALWAYS_FATAL(
          "%s: invalid: is_received: %d, buffer: event: %u, len: %u, data: %s",
          __func__, is_received, buffer->event, buffer->len,
          DataArrayToString<uint8_t>(buffer->data, buffer->len).c_str());
      // Dummy call here
      return ComposePackets(kEventPacket, p, false, timestamp_us);
    }
  }
}

BTSnoopParcel BtSnoopPacketizer::ComposePackets(PacketType type,
    uint8_t* packet, bool is_received, uint64_t timestamp_us) {
  uint32_t length_he = 0;
  uint32_t flags = 0;

  switch (type) {
    case kCommandPacket:
      length_he = packet[2] + 4;
      flags = 2;
      break;
    case kAclPacket:
      length_he = (packet[3] << 8) + packet[2] + 5;
      flags = is_received;
      break;
    case kScoPacket:
      length_he = packet[2] + 4;
      flags = is_received;
      break;
    case kEventPacket:
      length_he = packet[1] + 3;
      flags = 3;
      break;
  }

  btsnoop_header_t header;
  header.length_original = htonl(length_he);
  header.length_captured = header.length_original;
  header.flags = htonl(flags);
  header.dropped_packets = 0;
  header.timestamp = timestamp_us;
  header.type = type;

  DBG_LOG(LOG_TAG, "%s len_ori: %x, len_captured: %x, flags: %x, type: %x",
          __func__, header.length_original, header.length_captured,
          header.flags, header.type);
  DBG_LOG(LOG_TAG, "%s data: %s", __func__,
          DataArrayToString<uint8_t>(packet, (length_he - 1)).c_str());
  std::vector<uint8_t> packets(packet, (packet + (length_he - 1)));
  BTSnoopParcel parcel(std::move(header), std::move(packets));
  packet_ready_cb_(parcel);
  return parcel;
}

void BtSnoopPacketizer::OnShutdown(base::WaitableEvent* event) {
  if (parcels_.size()) {
    parcels_ready_cb_(parcels_, event);
    parcels_.clear();
  } else {
    // No left data, to notify event directly
    if (event) {
      event->Signal();
    }
  }
}

BTSnoopParcel BtSnoopPacketizer::ConvertToParcel(
    const BTSnoopPacketWrapper& packet) {
  // TODO: change this chatty LOG_INFO into DBG_LOG when it is stable
  LOG_INFO(LOG_TAG,
      "%s: wrapper: is_received: %d, "
      "buffer: event: 0x%x, len: %u, offset: %u, "
      "layer_specific: %u, data: %s",
       __func__,
       packet.is_received(),
       packet.buffer()->event, packet.buffer()->len,
       packet.buffer()->offset, packet.buffer()->layer_specific,
       DataArrayToString<uint8_t>(
           packet.buffer()->data,
           packet.buffer()->len).c_str());
  return MakeParcel(packet.buffer(),
      packet.is_received(),
      packet.timestamp_us());
}

bool BtSnoopPacketizer::IsParcelReady() {
  return (parcels_.size() >=
      static_cast<size_t>(
          SnoopLogConfig::GetInstance()->GetPacketCacheUpperLimit()));
}

bool BtSnoopPacketizer::Validate(const BT_HDR* buffer, bool is_received) {
  bool is_valid(false);
  if (buffer) {
    uint8_t* p = const_cast<uint8_t*>(buffer->data + buffer->offset);
    switch (buffer->event & MSG_EVT_MASK) {
      case MSG_HC_TO_STACK_HCI_EVT:
        // 1 byte (event type) + 1 byte event length (p1]) + event data
        is_valid = ((p[1] + 2) == buffer->len);
        break;
      case MSG_HC_TO_STACK_HCI_ACL:
      case MSG_STACK_TO_HC_HCI_ACL:
        // 2 byte (handle) + 2 byte acl length (p[3]<<8 + p[2]) + acl data
        is_valid = (((p[3] << 8) + p[2] + 4) == buffer->len);
        break;
      case MSG_HC_TO_STACK_HCI_SCO:
      case MSG_STACK_TO_HC_HCI_SCO:
        is_valid = ((p[2] + 3) == buffer->len);
        break;
      case MSG_STACK_TO_HC_HCI_CMD:
        // 2 byte (OP code) + 1 byte cmd length (p[2] + cmd data length
        is_valid = ((p[2] + 3) == buffer->len);
        break;
      default: {
        // Should never goes into here
        LOG_ALWAYS_FATAL(
            "%s: invalid: is_received: %d, buffer: event: %u, len: %u, "
            "data: %s",
            __func__, is_received, buffer->event, buffer->len,
            DataArrayToString<uint8_t>(buffer->data, buffer->len).c_str());
        break;
      }
    }
  }

  if (!is_valid) {
    LOG_INFO(LOG_TAG,
        "%s: invalid data, to be dropped: is_received: %d, "
        "buffer: %p, event: 0x%x, len: %u, offset: %u, "
        "layer_specific: %u, data: %s",
         __func__,
         is_received, buffer,
         buffer->event, buffer->len,
         buffer->offset, buffer->layer_specific,
         DataArrayToString<uint8_t>(buffer->data,
             buffer->len).c_str());
  }

  return is_valid;
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
