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

#pragma once

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#include <stdint.h>
#include <functional>

#include <queue>
#include <vector>

#include "bt_types.h"
#include "osi/include/osi.h"

namespace base {
class WaitableEvent;
}

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

struct __attribute__((__packed__)) btsnoop_header_t {
  uint32_t length_original;
  uint32_t length_captured;
  uint32_t flags;
  uint32_t dropped_packets;
  uint64_t timestamp;
  uint8_t type;
};

enum PacketType {
  kCommandPacket = 1,
  kAclPacket = 2,
  kScoPacket = 3,
  kEventPacket = 4
};

class BTSnoopParcel {
  public:
   explicit BTSnoopParcel(const btsnoop_header_t &header,
       const std::vector<uint8_t> &packet)
       : header_(header),
         packet_(std::move(packet)) { }

   const btsnoop_header_t& header() const {
     return header_;
   }

   const std::vector<uint8_t>& packet() const {
     return packet_;
   }

  private:
   btsnoop_header_t header_;
   std::vector<uint8_t> packet_;
};

class BTSnoopPacketWrapper {
  public:
   explicit BTSnoopPacketWrapper(const BT_HDR* buffer,
       bool is_received, uint64_t timestamp_us)
       : buffer_(nullptr),
         is_received_(is_received),
         timestamp_us_(timestamp_us) {
     if (buffer) {
       buffer_ = Clone(buffer);
     }
   }

   BTSnoopPacketWrapper(const BTSnoopPacketWrapper& wrapper)
       : buffer_(nullptr),
         is_received_(wrapper.is_received()),
         timestamp_us_(wrapper.timestamp_us()) {
     if (wrapper.buffer()) {
       buffer_ = Clone(wrapper.buffer());
     }
   }

   BTSnoopPacketWrapper(BTSnoopPacketWrapper&& wrapper)
       : buffer_(wrapper.buffer_),
         is_received_(wrapper.is_received_),
         timestamp_us_(wrapper.timestamp_us_) {
     wrapper.buffer_ = nullptr;
     wrapper.is_received_ = false;
     wrapper.timestamp_us_ = 0;
   }

   BTSnoopPacketWrapper& operator=(const BTSnoopPacketWrapper& wrapper);

   ~BTSnoopPacketWrapper();

   const BT_HDR* buffer() const {
     return buffer_;
   }

   bool is_received() const {
     return is_received_;
   }

   uint64_t timestamp_us() const {
     return timestamp_us_;
   }

   bool IsDataValid() const {
     return nullptr != buffer_;
   }

  private:
   BT_HDR* Clone(const BT_HDR* buffer);

   BT_HDR* buffer_;
   bool is_received_;
   uint64_t timestamp_us_;
};

using BtSnoopParcelsReadyCallback =
    std::function<void(
        const std::vector<BTSnoopParcel>&,
        base::WaitableEvent* event)>;
using BtSnoopPacketReadyCallback = std::function<void(const BTSnoopParcel&)>;

class SnoopPacketizer {
  public:
    SnoopPacketizer() = default;
    virtual ~SnoopPacketizer() = default;

    virtual void OnDataReady(const BT_HDR* buffer,
        bool is_received, uint64_t timestamp_us) = 0;
    // Need to flash kept data on storage before shutdown
    virtual void OnShutdown(base::WaitableEvent* event) = 0;
    virtual BTSnoopParcel ConvertToParcel(
        const BTSnoopPacketWrapper& packet) = 0;
};

class BtSnoopPacketizer : public SnoopPacketizer {
  public:
    explicit BtSnoopPacketizer(BtSnoopParcelsReadyCallback parcels_cb,
        BtSnoopPacketReadyCallback packet_cb)
       : parcels_ready_cb_(parcels_cb),
         packet_ready_cb_(packet_cb) {}
    ~BtSnoopPacketizer() override = default;

   void OnDataReady(const BT_HDR* buffer,
       bool is_received, uint64_t timestamp_us) override;
   void OnShutdown(base::WaitableEvent* event) override;
   virtual BTSnoopParcel ConvertToParcel(
       const BTSnoopPacketWrapper& packet) override;

   static bool Validate(const BT_HDR* buffer, bool is_received);

  private:
   BTSnoopParcel MakeParcel(const BT_HDR* buffer,
       bool is_received, uint64_t timestamp_us);
   BTSnoopParcel ComposePackets(PacketType type, uint8_t* packet,
       bool is_received, uint64_t timestamp_us);

   bool IsParcelReady();

   BtSnoopParcelsReadyCallback parcels_ready_cb_;
   BtSnoopPacketReadyCallback packet_ready_cb_;
   std::vector<BTSnoopParcel> parcels_;
};


}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
