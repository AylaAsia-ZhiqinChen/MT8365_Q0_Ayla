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

class GetAppSettingValuesRequest : public VendorPacket {
 public:
  virtual ~GetAppSettingValuesRequest() = default;

  /**
   *  Get Application Setting Values Request Packet Layout
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
  virtual std::vector<BtrcPlayerAttr> GetSettingAttrs();
  virtual bool IsValid() const override;
  virtual std::string ToString() const override;

 protected:
  using VendorPacket::VendorPacket;
  std::vector<BtrcPlayerAttr> attrs_;//AVRC_MAX_APP_ATTR_SIZE
};

class GetAppSettingValuesResponseBuilder : public VendorPacketBuilder {
 public:
  virtual ~GetAppSettingValuesResponseBuilder() = default;

  static std::unique_ptr<GetAppSettingValuesResponseBuilder> MakeBuilder(
      BtrcPlayerSettings get_values);

  virtual size_t size() const override;
  virtual bool Serialize(
      const std::shared_ptr<::bluetooth::Packet>& pkt) override;

 protected:
  BtrcPlayerSettings get_values_;

  GetAppSettingValuesResponseBuilder(BtrcPlayerSettings get_values)
      : VendorPacketBuilder(CType::STABLE, CommandPdu::GET_CUR_PLAYER_APPLICATION_VALUE,
                            PacketType::SINGLE),
        get_values_(get_values){};
};

class SetAppSettingValuesRequest : public VendorPacket {
 public:
  virtual ~SetAppSettingValuesRequest() = default;

  /**
   *  Set Application Setting Values Request Packet Layout
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
  virtual BtrcPlayerSettings SetSettingValues();
  virtual bool IsValid() const override;
  virtual std::string ToString() const override;

 protected:
  using VendorPacket::VendorPacket;
  BtrcPlayerSettings set_values_;
};

class SetAppSettingValuesResponseBuilder : public VendorPacketBuilder {
 public:
  virtual ~SetAppSettingValuesResponseBuilder() = default;

  static std::unique_ptr<SetAppSettingValuesResponseBuilder> MakeBuilder(
      BtrcStatus rsp_status);

  virtual size_t size() const override;
  virtual bool Serialize(
      const std::shared_ptr<::bluetooth::Packet>& pkt) override;

 protected:
  BtrcStatus status_;

  SetAppSettingValuesResponseBuilder(BtrcStatus rsp_status)
      : VendorPacketBuilder(CType::ACCEPTED, CommandPdu::SET_PLAYER_APPLICATION_SETTING_VALUE,
                            PacketType::SINGLE),
        status_(rsp_status){};
};

}  // namespace avrcp
}  // namespace bluetooth
#endif