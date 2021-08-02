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

#include "application_setting_text_values_packet.h"

namespace bluetooth {
namespace avrcp {

/* the frequently used character set ids */
constexpr uint16_t AVRC_CHARSET_ID_UTF8 = 0x6a00; /* UTF-8 */
// constexpr uint8_t AVRC_PLAYER_VAL_GROUP_REPEAT = 0x04;

std::vector<BtrcPlayerAttr> AppSettingAttrTxtRequest::GetSettingTxtAttrs() {
  auto it = begin() + VendorPacket::kMinSize();
  uint8_t num_attr = it.extract<uint8_t>();

  for (uint8_t i = 0; i < num_attr; i++) {
    attrs_txt_.push_back((BtrcPlayerAttr)it.extract<uint8_t>());
  }

  return attrs_txt_;
}

bool AppSettingAttrTxtRequest::IsValid() const {
  auto it = begin() + VendorPacket::kMinSize();
  uint8_t num_attr = it.extract<uint8_t>();
  auto attri_id = 0;
  // DUT don't support EqualizerON/OFFstatus 1 and ScanON/OFFstatus 4
  if (num_attr == 0 || num_attr > 2)
    return false;
  for (uint8_t i = 0; i < num_attr; i++) {
    attri_id = it.extract<uint8_t>();
    if (attri_id == 1 || attri_id == 4)
      return false;
  }
  return true;
}

std::string AppSettingAttrTxtRequest::ToString() const {
  std::stringstream ss;
  ss << "AppSettingAttrTxtRequest: " << std::endl;
  ss << "  └ cType = " << GetCType() << std::endl;
  ss << "  └ Subunit Type = " << loghex(GetSubunitType()) << std::endl;
  ss << "  └ Subunit ID = " << loghex(GetSubunitId()) << std::endl;
  ss << "  └ OpCode = " << GetOpcode() << std::endl;
  ss << "  └ Company ID = " << loghex(GetCompanyId()) << std::endl;
  ss << "  └ Command PDU = " << GetCommandPdu() << std::endl;
  ss << "  └ PacketType = " << GetPacketType() << std::endl;
  ss << "  └ Parameter Length = " << GetParameterLength() << std::endl;
  ss << "  └ attrs_txt_ = " << loghex(attrs_txt_.size()) << std::endl;

  return ss.str();
}

std::unique_ptr<AppSettingAttrTxtResponseBuilder>
AppSettingAttrTxtResponseBuilder::MakeBuilder() {
  std::unique_ptr<AppSettingAttrTxtResponseBuilder> builder(
      new AppSettingAttrTxtResponseBuilder());
  return builder;
}

void AppSettingAttrTxtResponseBuilder::SetAttrsTxt(
    std::vector<BtrcPlayerSettingText> attrs_txt) {
  for (auto it = attrs_txt.begin(); it != attrs_txt.end(); it++)
  {
    AvrcAppSettingTxt attr_txt_temp;
    attr_txt_temp.charset_id = AVRC_CHARSET_ID_UTF8;
    attr_txt_temp.attr_id = it->id ;
    attr_txt_temp.str_len = (uint8_t)strnlen((char *)it->text, BTRC_MAX_ATTR_STR_LEN);
    attr_txt_temp.p_str = it->text ;
    attr_txt_values_.push_back(std::move(attr_txt_temp));
  }
}

size_t AppSettingAttrTxtResponseBuilder::size() const {
  return VendorPacket::kMinSize() + 1;
}

bool AppSettingAttrTxtResponseBuilder::Serialize(
    const std::shared_ptr<::bluetooth::Packet>& pkt) {
  size_t data_length = 0;
  data_length += attr_txt_values_.size() * 4;

  for (auto it = attr_txt_values_.begin(); it != attr_txt_values_.end(); it++)
  {
      data_length += it->str_len;
  }
  ReserveSpace(pkt, size() + data_length);

  // Push the standard avrcp headers
  PacketBuilder::PushHeader(pkt);

  // Push the avrcp vendor command headers
  uint16_t parameter_count = size() + data_length - VendorPacket::kMinSize();
  VendorPacketBuilder::PushHeader(pkt, parameter_count);

  AddPayloadOctets1(pkt, attr_txt_values_.size());

  for (auto it = attr_txt_values_.begin(); it != attr_txt_values_.end(); it++) {
    AddPayloadOctets1(pkt, it->attr_id);
    AddPayloadOctets2(pkt, it->charset_id);
    AddPayloadOctets1(pkt, it->str_len);
    for (uint8_t i = 0; i < it->str_len; i++) {
      AddPayloadOctets1(pkt, it->p_str[i]);
    }
  }
  return true;
}

uint8_t AppSettingValuesTxtRequest::GetSettingAttrID() {
  auto it = begin() + VendorPacket::kMinSize();
  attr_id_ = it.extract<uint8_t>();
  return attr_id_;
}

std::vector<uint8_t> AppSettingValuesTxtRequest::GetSettingValueTxt() {
  auto it = begin() + VendorPacket::kMinSize() + static_cast<size_t>(1);
  uint8_t num_val_ = it.extract<uint8_t>();

  for (uint8_t i = 0; i < num_val_; i++) {
    vals_txt_.push_back(it.extract<uint8_t>());
  }
  return vals_txt_;
}

bool AppSettingValuesTxtRequest::IsValid() const {
  auto it = begin() + VendorPacket::kMinSize();
  uint8_t attr_id = it.extract<uint8_t>();
  // DUT don't support EqualizerON/OFFstatus 1 and ScanON/OFFstatus 4
  return (attr_id == 2 || attr_id == 3);
}

std::string AppSettingValuesTxtRequest::ToString() const {
  std::stringstream ss;
  ss << "AppSettingValuesTxtRequest: " << std::endl;
  ss << "  └ cType = " << GetCType() << std::endl;
  ss << "  └ Subunit Type = " << loghex(GetSubunitType()) << std::endl;
  ss << "  └ Subunit ID = " << loghex(GetSubunitId()) << std::endl;
  ss << "  └ OpCode = " << GetOpcode() << std::endl;
  ss << "  └ Company ID = " << loghex(GetCompanyId()) << std::endl;
  ss << "  └ Command PDU = " << GetCommandPdu() << std::endl;
  ss << "  └ PacketType = " << GetPacketType() << std::endl;
  ss << "  └ Parameter Length = " << GetParameterLength() << std::endl;
  ss << "  └ attr_id_ = " << loghex(attr_id_) << std::endl;
  ss << "  └ vals_txt size = " << loghex(vals_txt_.size()) << std::endl;
  return ss.str();
}

std::unique_ptr<AppSettingValuesTxtResponseBuilder>
AppSettingValuesTxtResponseBuilder::MakeBuilder() {
  std::unique_ptr<AppSettingValuesTxtResponseBuilder> builder(
      new AppSettingValuesTxtResponseBuilder());

  return builder;
}

void AppSettingValuesTxtResponseBuilder::SetValuesTxt(
    std::vector<BtrcPlayerSettingText> txt_values) {
  for (auto it = txt_values.begin(); it != txt_values.end(); it++) {
    AvrcAppSettingTxt value_txt_temp;
    value_txt_temp.charset_id = AVRC_CHARSET_ID_UTF8;
    value_txt_temp.attr_id = it->id ;
    value_txt_temp.str_len = (uint8_t)strnlen((char *)it->text, BTRC_MAX_ATTR_STR_LEN);
    value_txt_temp.p_str = it->text ;
    values_txt_.push_back(std::move(value_txt_temp));
  }
}

size_t AppSettingValuesTxtResponseBuilder::size() const {
  return VendorPacket::kMinSize() + 1;
}

bool AppSettingValuesTxtResponseBuilder::Serialize(
    const std::shared_ptr<::bluetooth::Packet>& pkt) {

  size_t data_length = 0;
  data_length += values_txt_.size() * 4;

  for (auto it = values_txt_.begin(); it != values_txt_.end(); it++)
  {
      data_length += it->str_len;
  }
  ReserveSpace(pkt, size() + data_length);

  // Push the standard avrcp headers
  PacketBuilder::PushHeader(pkt);

  // Push the avrcp vendor command headers
  uint16_t parameter_count = size() + data_length - VendorPacket::kMinSize();
  VendorPacketBuilder::PushHeader(pkt, parameter_count);
  AddPayloadOctets1(pkt, values_txt_.size());

  for (auto it = values_txt_.begin(); it != values_txt_.end(); it++) {
    AddPayloadOctets1(pkt, it->attr_id);
    AddPayloadOctets2(pkt, it->charset_id);
    AddPayloadOctets1(pkt, it->str_len);
    for (uint8_t i = 0; i < it->str_len; i++) {
      AddPayloadOctets1(pkt, it->p_str[i]);
    }
  }

  return true;
}

}  // namespace avrcp
}  // namespace bluetooth
#endif