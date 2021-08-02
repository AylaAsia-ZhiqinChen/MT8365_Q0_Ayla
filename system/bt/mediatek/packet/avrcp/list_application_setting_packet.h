/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly
 * * prohibited.
 * *
 * * MediaTek Inc. (C) 2018. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * * NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO
 * * SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER
 * * EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN
 * * FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES
 * * MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR
 * * OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED
 * * HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK
 * * SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE
 * * PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
#pragma once

#include "avrcp.h"
#include "vendor_packet.h"

namespace bluetooth {
namespace avrcp {

// TODO (apanicke): This packet doesn't really need to exist as it provides
// zero extra information other than it is a strong type and provides a
// validator
class ListAppSettingValuesRequest : public VendorPacket {
 public:
  virtual ~ListAppSettingValuesRequest() = default;

  /**
   *  List application Setting Values Request Packet Layout
   *   AvrcpPacket:
   *     CType c_type_;
   *     uint8_t subunit_type_ : 5;
   *     uint8_t subunit_id_ : 3;
   *     Opcode opcode_;
   *   VendorPacket:
   *     uint8_t company_id[3];
   *     uint8_t command_pdu;
   *     uint8_t packet_type;
   *     uint16_t param_length = 0;
   */
  static constexpr size_t kMinSize() { return Packet::kMinSize() + 1; }

  // Overloaded Functions
  virtual uint8_t GetSettingAttributeID() const;
  virtual bool IsValid() const override;
  virtual std::string ToString() const override;

 protected:
  using VendorPacket::VendorPacket;
};

class ListAppSettingValuesResponseBuilder : public VendorPacketBuilder {
 public:
  virtual ~ListAppSettingValuesResponseBuilder() = default;

  static std::unique_ptr<ListAppSettingValuesResponseBuilder> MakeBuilder(
      std::vector<uint8_t> values);

  virtual size_t size() const override;
  virtual bool Serialize(
      const std::shared_ptr<::bluetooth::Packet>& pkt) override;

 protected:
  std::vector<uint8_t> values_; //[AVRC_MAX_APP_ATTR_SIZE]

  ListAppSettingValuesResponseBuilder(std::vector<uint8_t> values)
      : VendorPacketBuilder(CType::STABLE, CommandPdu::LIST_APPLICATION_SETTING_VALUES,
                            PacketType::SINGLE),
        values_(values){};
};

class ListAppSettingAttrsResponseBuilder : public VendorPacketBuilder {
 public:
  virtual ~ListAppSettingAttrsResponseBuilder() = default;

  static std::unique_ptr<ListAppSettingAttrsResponseBuilder> MakeBuilder(
      std::vector<BtrcPlayerAttr> attrs);

  virtual size_t size() const override;
  virtual bool Serialize(
      const std::shared_ptr<::bluetooth::Packet>& pkt) override;

 protected:
  std::vector<BtrcPlayerAttr> attrs_; //[AVRC_MAX_APP_ATTR_SIZE]

  ListAppSettingAttrsResponseBuilder(std::vector<BtrcPlayerAttr> attrs)
      : VendorPacketBuilder(CType::STABLE, CommandPdu::LIST_APPLICATION_SETTING_ATTRIBUTES,
                            PacketType::SINGLE),
        attrs_(attrs){};
};

}  // namespace avrcp
}  // namespace bluetooth
#endif