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

#include "list_application_setting_packet.h"

namespace bluetooth {
namespace avrcp {

uint8_t ListAppSettingValuesRequest::GetSettingAttributeID() const {
  auto it = begin() + VendorPacket::kMinSize();
  uint8_t attr_id = it.extract<uint8_t>();
  return attr_id;
}

bool ListAppSettingValuesRequest::IsValid() const {
  auto it = begin() + VendorPacket::kMinSize();
  uint8_t attr_id = it.extract<uint8_t>();
  // DUT don't support EqualizerON/OFFstatus 1 and ScanON/OFFstatus 4
  // Only support RepeatModestatus 2 and ShuffleON/OFFstatus 3
  return (attr_id == 2 || attr_id == 3);
}

std::string ListAppSettingValuesRequest::ToString() const {
  std::stringstream ss;
  ss << "ListAppSettingValuesRequest: " << std::endl;
  ss << "  └ cType = " << GetCType() << std::endl;
  ss << "  └ Subunit Type = " << loghex(GetSubunitType()) << std::endl;
  ss << "  └ Subunit ID = " << loghex(GetSubunitId()) << std::endl;
  ss << "  └ OpCode = " << GetOpcode() << std::endl;
  ss << "  └ Company ID = " << loghex(GetCompanyId()) << std::endl;
  ss << "  └ Command PDU = " << GetCommandPdu() << std::endl;
  ss << "  └ PacketType = " << GetPacketType() << std::endl;
  ss << "  └ Parameter Length = " << GetParameterLength() << std::endl;
  ss << "  └ AttributeID = " << GetSettingAttributeID() << std::endl;

  return ss.str();
}

std::unique_ptr<ListAppSettingValuesResponseBuilder>
ListAppSettingValuesResponseBuilder::MakeBuilder(std::vector<uint8_t> values) {
  std::unique_ptr<ListAppSettingValuesResponseBuilder> builder(
      new ListAppSettingValuesResponseBuilder(values));

  return builder;
}

size_t ListAppSettingValuesResponseBuilder::size() const {
  return VendorPacket::kMinSize() + 1 + values_.size();
}

bool ListAppSettingValuesResponseBuilder::Serialize(
    const std::shared_ptr<::bluetooth::Packet>& pkt) {
  ReserveSpace(pkt, size());

  // Push the standard avrcp headers
  PacketBuilder::PushHeader(pkt);

  // Push the avrcp vendor command headers
  uint16_t parameter_count = size() - VendorPacket::kMinSize();
  VendorPacketBuilder::PushHeader(pkt, parameter_count);

  AddPayloadOctets1(pkt, values_.size());

  for (auto it = values_.begin(); it != values_.end(); it++) {
    AddPayloadOctets1(pkt, *it);
  }
  return true;
}

std::unique_ptr<ListAppSettingAttrsResponseBuilder>
ListAppSettingAttrsResponseBuilder::MakeBuilder(
                                           std::vector<BtrcPlayerAttr> attrs) {
  std::unique_ptr<ListAppSettingAttrsResponseBuilder> builder(
      new ListAppSettingAttrsResponseBuilder(attrs));

  return builder;
}

size_t ListAppSettingAttrsResponseBuilder::size() const {
  return VendorPacket::kMinSize() + 1 + attrs_.size();
}

bool ListAppSettingAttrsResponseBuilder::Serialize(
    const std::shared_ptr<::bluetooth::Packet>& pkt) {
  ReserveSpace(pkt, size());

  // Push the standard avrcp headers
  PacketBuilder::PushHeader(pkt);

  // Push the avrcp vendor command headers
  uint16_t parameter_count = size() - VendorPacket::kMinSize();
  VendorPacketBuilder::PushHeader(pkt, parameter_count);

  AddPayloadOctets1(pkt, attrs_.size());

  for (auto it = attrs_.begin(); it != attrs_.end(); it++) {
    AddPayloadOctets1(pkt, *it);
  }
  return true;
}

}  // namespace avrcp
}  // namespace bluetooth
#endif