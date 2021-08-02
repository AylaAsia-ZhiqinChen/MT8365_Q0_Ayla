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
#include <base/message_loop/message_loop.h>

#include "mtk_packet.h"
#include "connection_handler.h"
#include "device.h"
#include "stack_config.h"
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
#include "mediatek/include/interop_mtk.h"
#endif

namespace bluetooth {
namespace avrcp {

#define DEVICE_LOG(LEVEL) LOG(LEVEL) << address_.ToString() << " : "
#define DEVICE_VLOG(LEVEL) VLOG(LEVEL) << address_.ToString() << " : "

void Device::SettingPacketHandler(uint8_t label,
                                 std::shared_ptr<VendorPacket> pkt) {
  switch (pkt->GetCommandPdu()) {

    case CommandPdu::LIST_APPLICATION_SETTING_ATTRIBUTES:{
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
      if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_NO_APP_SETTINGS,
                                      &address_)) {
        auto response = RejectBuilder::MakeBuilder(
             (CommandPdu)pkt->GetCommandPdu(), Status::INVALID_COMMAND);
        send_message(label, false, std::move(response));
      } else
#endif
      media_interface_->ListAppSettingAttrs(base::Bind(&Device::ListAppSettingAttrsResponse,
              weak_ptr_factory_.GetWeakPtr(), label));
    } break;
    case CommandPdu::LIST_APPLICATION_SETTING_VALUES:{
      auto setting_pkt =
              Packet::Specialize<ListAppSettingValuesRequest>(pkt);
      if (!setting_pkt->IsValid()) {
        auto response = RejectBuilder::MakeBuilder(
            (CommandPdu)setting_pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
        return;
      }
      media_interface_->ListAppSettingValues(setting_pkt->GetSettingAttributeID(),
              base::Bind(&Device::ListAppSettingValuesResponse,
              weak_ptr_factory_.GetWeakPtr(), label));
    } break;

    case CommandPdu::GET_CUR_PLAYER_APPLICATION_VALUE:{
      auto setting_pkt =
              Packet::Specialize<GetAppSettingValuesRequest>(pkt);
      if (!setting_pkt->IsValid()) {
        auto response = RejectBuilder::MakeBuilder(
            (CommandPdu)setting_pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
        return;
      }
      media_interface_->GetAppSettingValues(setting_pkt->GetSettingAttrs(),
              base::Bind(&Device::GetAppSettingValuesResponse,
              weak_ptr_factory_.GetWeakPtr(), label));
    } break;
    case CommandPdu::SET_PLAYER_APPLICATION_SETTING_VALUE:{
      auto setting_pkt =
              Packet::Specialize<SetAppSettingValuesRequest>(pkt);
      if (!setting_pkt->IsValid()) {
        auto response = RejectBuilder::MakeBuilder(
            (CommandPdu)setting_pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
        return;
      }
      media_interface_->SetAppSettingValues(setting_pkt->SetSettingValues(),
              base::Bind(&Device::SetAppSettingValuesResponse,
              weak_ptr_factory_.GetWeakPtr(), label));
    } break;

    case CommandPdu::GET_PLAYER__APPLICATION_ATTRIBUTES_TEXT:{
      auto setting_pkt =
              Packet::Specialize<AppSettingAttrTxtRequest>(pkt);
      if (!setting_pkt->IsValid()) {
        auto response = RejectBuilder::MakeBuilder(
            (CommandPdu)setting_pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
        return;
      }
      media_interface_->GetAppSettingAttrsText(setting_pkt->GetSettingTxtAttrs(),
              base::Bind(&Device::GetAppSettingAttTxtResponse,
              weak_ptr_factory_.GetWeakPtr(), label));
    } break;
    case CommandPdu::GET_PLAYER_APPLICATION_VALUE_TEXT: {
      auto setting_pkt =
              Packet::Specialize<AppSettingValuesTxtRequest>(pkt);
      if (!setting_pkt->IsValid()) {
        auto response = RejectBuilder::MakeBuilder(
            (CommandPdu)setting_pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
        return;
      }
      media_interface_->GetAppSettingValuesText(setting_pkt->GetSettingAttrID(),
              setting_pkt->GetSettingValueTxt(),
              base::Bind(&Device::GetAppSettingValueTxtResponse,
              weak_ptr_factory_.GetWeakPtr(), label));
    } break;

    default: {
      DEVICE_LOG(ERROR) << "Unhandled Vendor Packet: " << pkt->ToString();
    } break;
  }
}


void Device::SendSettingChange(bool setting_changed) {
  CHECK(media_interface_);
  DEVICE_VLOG(4) << __func__ << ": setting_Changed=" << setting_changed;

  if (!player_app_setting_changed_.first) {
    LOG(WARNING) << "Device is not registered for track changed updates";
    return;
  }

  if (setting_changed) {
    media_interface_->GetAppSettingChange(
        base::Bind(&Device::AppSettingChangeNotificationResponse,
                   weak_ptr_factory_.GetWeakPtr(), player_app_setting_changed_.second, false));
  }
}

// avrcp setting
void Device::AppSettingChangeNotificationResponse(uint8_t label,
      bool interim, BtrcPlayerSettings player_setting) {
  DEVICE_VLOG(1) << __func__;

  if (!player_app_setting_changed_.first) {
    DEVICE_VLOG(0) << __func__ << ": Device not registered for update";
    return;
  }

  auto response = RegisterNotificationResponseBuilder::MakeAppSettingChangedBuilder(
      interim, player_setting);
  send_message_cb_.Run(label, false, std::move(response));
  if (!interim) {
    active_labels_.erase(label);
    player_app_setting_changed_ = Notification(false, 0);
  }
}

void Device::ListAppSettingAttrsResponse(
    uint8_t label, std::vector<BtrcPlayerAttr> attrs) {
  DEVICE_VLOG(2) << __func__ << ": label=" << label;

  auto response = ListAppSettingAttrsResponseBuilder::MakeBuilder(attrs);
  send_message(label, false, std::move(response));
}

void Device::ListAppSettingValuesResponse(
    uint8_t label, std::vector<uint8_t> values) {
  DEVICE_VLOG(2) << __func__ << ": label=" << label;

  auto response = ListAppSettingValuesResponseBuilder::MakeBuilder(values);
  send_message(label, false, std::move(response));
}

void Device::GetAppSettingValuesResponse(
    uint8_t label, BtrcPlayerSettings values) {
  DEVICE_VLOG(2) << __func__ << ": label=" << label;

  auto response = GetAppSettingValuesResponseBuilder::MakeBuilder(values);
  send_message(label, false, std::move(response));
}

void Device::SetAppSettingValuesResponse(
    uint8_t label, BtrcStatus rsp_status) {
  DEVICE_VLOG(2) << __func__ << ": label=" << label;

  auto response = SetAppSettingValuesResponseBuilder::MakeBuilder(rsp_status);
  send_message(label, false, std::move(response));
}

void Device::GetAppSettingAttTxtResponse(
    uint8_t label, std::vector<BtrcPlayerSettingText> attrs_txt) {
  DEVICE_VLOG(2) << __func__ << ": label=" << label;

  auto response = AppSettingAttrTxtResponseBuilder::MakeBuilder();
  response->SetAttrsTxt(attrs_txt);
  send_message(label, false, std::move(response));
}

void Device::GetAppSettingValueTxtResponse(
    uint8_t label, std::vector<BtrcPlayerSettingText> attrs_values_txt) {
  DEVICE_VLOG(2) << __func__ << ": label=" << label;

  auto response = AppSettingValuesTxtResponseBuilder::MakeBuilder();
  response->SetValuesTxt(attrs_values_txt);
  send_message(label, false, std::move(response));
}

}
}
#endif