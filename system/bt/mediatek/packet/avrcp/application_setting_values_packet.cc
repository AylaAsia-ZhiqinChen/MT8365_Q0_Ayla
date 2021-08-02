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
#include <algorithm>

#include "application_setting_values_packet.h"

namespace bluetooth {
namespace avrcp {

std::vector<BtrcPlayerAttr> GetAppSettingValuesRequest::GetSettingAttrs() {
  auto it = begin() + VendorPacket::kMinSize();

  uint8_t num_attr = it.extract<uint8_t>();

  for (uint8_t i = 0; i < num_attr && i < BTRC_MAX_APP_ATTR_SIZE; i++) {
    attrs_.push_back((BtrcPlayerAttr)it.extract<uint8_t>());
  }

  return attrs_;
}

bool GetAppSettingValuesRequest::IsValid() const {
  auto it = begin() + VendorPacket::kMinSize();
  auto attri_id = 0;
  uint8_t num_attr = it.extract<uint8_t>();
  // Only support RepeatModestatus 2 and ShuffleON/OFFstatus 3
  if (num_attr == 0 || num_attr > 2)
    return false;
  for (uint8_t i = 0; i < num_attr; i++) {
    attri_id = it.extract<uint8_t>();
    if (attri_id == 1 || attri_id == 4)
      return false;
  }
  return true;
}

std::string GetAppSettingValuesRequest::ToString() const {
  std::stringstream ss;
  ss << "GetAppSettingValuesRequest: " << std::endl;
  ss << "  └ cType = " << GetCType() << std::endl;
  ss << "  └ Subunit Type = " << loghex(GetSubunitType()) << std::endl;
  ss << "  └ Subunit ID = " << loghex(GetSubunitId()) << std::endl;
  ss << "  └ OpCode = " << GetOpcode() << std::endl;
  ss << "  └ Company ID = " << loghex(GetCompanyId()) << std::endl;
  ss << "  └ Command PDU = " << GetCommandPdu() << std::endl;
  ss << "  └ PacketType = " << GetPacketType() << std::endl;
  ss << "  └ Parameter Length = " << GetParameterLength() << std::endl;
  ss << "  └ attrs_.size = " << loghex(attrs_.size()) << std::endl;

  return ss.str();
}

std::unique_ptr<GetAppSettingValuesResponseBuilder>
GetAppSettingValuesResponseBuilder::MakeBuilder(
                                           BtrcPlayerSettings get_values) {
  std::unique_ptr<GetAppSettingValuesResponseBuilder> builder(
      new GetAppSettingValuesResponseBuilder(get_values));

  return builder;
}

size_t GetAppSettingValuesResponseBuilder::size() const {
  return VendorPacket::kMinSize() + 1 ;
}

bool GetAppSettingValuesResponseBuilder::Serialize(
    const std::shared_ptr<::bluetooth::Packet>& pkt) {
  ReserveSpace(pkt, size() + 2 * get_values_.num_attr);

  // Push the standard avrcp headers
  PacketBuilder::PushHeader(pkt);

  // Push the avrcp vendor command headers
  uint16_t parameter_count = (size() + 2 * get_values_.num_attr) - VendorPacket::kMinSize();
  VendorPacketBuilder::PushHeader(pkt, parameter_count);
  AddPayloadOctets1(pkt, get_values_.num_attr);

  for (int xx = 0; xx < get_values_.num_attr; xx++) {
    AddPayloadOctets1(pkt, get_values_.attr_ids[xx]);
    AddPayloadOctets1(pkt, get_values_.attr_values[xx]);
  }
  return true;
}

BtrcPlayerSettings SetAppSettingValuesRequest::SetSettingValues() {
  auto it = begin() + VendorPacket::kMinSize();

  size_t number_attributes = it.extract<uint8_t>();
  set_values_.num_attr = number_attributes;
  for (uint8_t i = 0; i < number_attributes; i++) {
    set_values_.attr_ids[i] = it.extract<uint8_t>();
    set_values_.attr_values[i] = it.extract<uint8_t>();
  }

  return set_values_;
}

bool SetAppSettingValuesRequest::IsValid() const {
  auto it = begin() + VendorPacket::kMinSize();
  auto attri_id = 0;
  uint8_t num_values = it.extract<uint8_t>();
  // Only support RepeatModestatus 2 and ShuffleON/OFFstatus 3
  if (num_values == 0 || num_values > 2)
    return false;
  for (uint8_t i = 0; i < num_values; i++) {
    attri_id = it.extract<uint8_t>();
    if (attri_id == 1 || attri_id == 4)
      return false;
    it.extract<uint8_t>();
  }
  return true;
}
std::string SetAppSettingValuesRequest::ToString() const {
  std::stringstream ss;
  ss << "SetAppSettingValuesRequest: " << std::endl;
  ss << "  └ cType = " << GetCType() << std::endl;
  ss << "  └ Subunit Type = " << loghex(GetSubunitType()) << std::endl;
  ss << "  └ Subunit ID = " << loghex(GetSubunitId()) << std::endl;
  ss << "  └ OpCode = " << GetOpcode() << std::endl;
  ss << "  └ Company ID = " << loghex(GetCompanyId()) << std::endl;
  ss << "  └ Command PDU = " << GetCommandPdu() << std::endl;
  ss << "  └ PacketType = " << GetPacketType() << std::endl;
  ss << "  └ Parameter Length = " << GetParameterLength() << std::endl;
  ss << "  └ set_values_.num_attr = " << loghex(set_values_.num_attr) << std::endl;

  return ss.str();
}

std::unique_ptr<SetAppSettingValuesResponseBuilder>
SetAppSettingValuesResponseBuilder::MakeBuilder(BtrcStatus rsp_status) {
  std::unique_ptr<SetAppSettingValuesResponseBuilder> builder(
      new SetAppSettingValuesResponseBuilder(rsp_status));

  return builder;
}

size_t SetAppSettingValuesResponseBuilder::size() const {
  return VendorPacket::kMinSize();
}

bool SetAppSettingValuesResponseBuilder::Serialize(
    const std::shared_ptr<::bluetooth::Packet>& pkt) {
  ReserveSpace(pkt, size());

  // Push the standard avrcp headers
  PacketBuilder::PushHeader(pkt);

  // Push the avrcp vendor command headers
  uint16_t parameter_count = size() - VendorPacket::kMinSize();
  VendorPacketBuilder::PushHeader(pkt, parameter_count);

  return true;
}

}  // namespace avrcp
}  // namespace bluetooth
#endif