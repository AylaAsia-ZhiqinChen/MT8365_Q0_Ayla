/*
 * Copyright 2018 The Android Open Source Project
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
#include "device.h"

#include <base/message_loop/message_loop.h>

#include "connection_handler.h"
#include "packet/avrcp/avrcp_reject_packet.h"
#include "packet/avrcp/general_reject_packet.h"
#include "packet/avrcp/get_play_status_packet.h"
#include "packet/avrcp/pass_through_packet.h"
#include "packet/avrcp/set_absolute_volume.h"
#include "packet/avrcp/set_addressed_player.h"
#include "stack_config.h"
#include "mediatek/include/mtk_bta_av_act.h"
#include "btif/avrcp/avrcp_service.h"
#if defined(MTK_A2DP_SRC_SINK_BOTH) && (MTK_A2DP_SRC_SINK_BOTH == TRUE)
extern bool btif_av_peer_is_connected_sink(const RawAddress& peer_address);
extern bool btif_av_both_enable(void);
#endif
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
#include "mediatek/include/interop_mtk.h"
#endif

/** M: Ignore play while SCO exist. @{ */
#include "btif/include/btif_hf.h"
/** @} */

namespace bluetooth {
namespace avrcp {

#define DEVICE_LOG(LEVEL) LOG(LEVEL) << address_.ToString() << " : "
#define DEVICE_VLOG(LEVEL) VLOG(LEVEL) << address_.ToString() << " : "

#define VOL_NOT_SUPPORTED -1
#define VOL_REGISTRATION_FAILED -2

Device::Device(
    const RawAddress& bdaddr, bool avrcp13_compatibility,
    base::Callback<void(uint8_t label, bool browse,
                        std::unique_ptr<::bluetooth::PacketBuilder> message)>
        send_msg_cb,
    uint16_t ctrl_mtu, uint16_t browse_mtu)
    : weak_ptr_factory_(this),
      address_(bdaddr),
      avrcp13_compatibility_(avrcp13_compatibility),
      send_message_cb_(send_msg_cb),
      ctrl_mtu_(ctrl_mtu),
      browse_mtu_(browse_mtu) {
  avrcp13_compatibility_ = MtkRcIsAvrcp13Compatibility(bdaddr);
}

void Device::RegisterInterfaces(MediaInterface* media_interface,
                                A2dpInterface* a2dp_interface,
                                VolumeInterface* volume_interface) {
  CHECK(media_interface);
  CHECK(a2dp_interface);
  a2dp_interface_ = a2dp_interface;
  media_interface_ = media_interface;
  volume_interface_ = volume_interface;
}

base::WeakPtr<Device> Device::Get() {
  return weak_ptr_factory_.GetWeakPtr();
}

void Device::SetBrowseMtu(uint16_t browse_mtu) {
  DEVICE_LOG(INFO) << __PRETTY_FUNCTION__ << ": browse_mtu = " << browse_mtu;
  browse_mtu_ = browse_mtu;
}

bool Device::IsActive() const {
  return address_ == a2dp_interface_->active_peer();
}

bool Device::IsInSilenceMode() const {
  return a2dp_interface_->is_peer_in_silence_mode(address_);
}

void Device::VendorPacketHandler(uint8_t label,
                                 std::shared_ptr<VendorPacket> pkt) {
  CHECK(media_interface_);
  DEVICE_VLOG(3) << __func__ << ": pdu=" << pkt->GetCommandPdu();

  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = RejectBuilder::MakeBuilder(static_cast<CommandPdu>(0), Status::INVALID_COMMAND);
    send_message(label, false, std::move(response));
    return;
  }

  // All CTypes at and above NOT_IMPLEMENTED are all response types.
  if (pkt->GetCType() == CType::NOT_IMPLEMENTED) {
    return;
  }

  if (pkt->GetCType() >= CType::ACCEPTED) {
    switch (pkt->GetCommandPdu()) {
      // VOLUME_CHANGED is the only notification we register for while target.
      case CommandPdu::REGISTER_NOTIFICATION: {
        auto register_notification =
            Packet::Specialize<RegisterNotificationResponse>(pkt);
        if (register_notification->GetEvent() != Event::VOLUME_CHANGED) {
          DEVICE_LOG(WARNING)
              << __func__ << ": Unhandled register notification received: "
              << register_notification->GetEvent();
          return;
        }
        HandleVolumeChanged(label, register_notification);
        break;
      }
      case CommandPdu::SET_ABSOLUTE_VOLUME:
        // TODO (apanicke): Add a retry mechanism if the response has a
        // different volume than the one we set. For now, we don't care
        // about the response to this message.
        /** M: Fix avrcp absolute volume change fail @{ */
        ct_active_labels_.erase(label);
        /** @} */
        break;
      default:
        DEVICE_LOG(WARNING)
            << __func__ << ": Unhandled Response: pdu=" << pkt->GetCommandPdu();
        break;
    }
    return;
  }

  switch (pkt->GetCommandPdu()) {
    case CommandPdu::GET_CAPABILITIES: {
      HandleGetCapabilities(label,
                            Packet::Specialize<GetCapabilitiesRequest>(pkt));
    } break;

    case CommandPdu::REGISTER_NOTIFICATION: {
      HandleNotification(label,
                         Packet::Specialize<RegisterNotificationRequest>(pkt));
    } break;

    case CommandPdu::GET_ELEMENT_ATTRIBUTES: {
      auto get_element_attributes_request_pkt = Packet::Specialize<GetElementAttributesRequest>(pkt);

      if (!get_element_attributes_request_pkt->IsValid()) {
        DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
        auto response = RejectBuilder::MakeBuilder(pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
      }
      media_interface_->GetSongInfo(base::Bind(&Device::GetElementAttributesResponse, weak_ptr_factory_.GetWeakPtr(),
                                               label, get_element_attributes_request_pkt));
    } break;

    case CommandPdu::GET_PLAY_STATUS: {
      media_interface_->GetPlayStatus(base::Bind(&Device::GetPlayStatusResponse,
                                                 weak_ptr_factory_.GetWeakPtr(),
                                                 label));
    } break;

    case CommandPdu::PLAY_ITEM: {
      HandlePlayItem(label, Packet::Specialize<PlayItemRequest>(pkt));
    } break;

    case CommandPdu::SET_ADDRESSED_PLAYER: {
      // TODO (apanicke): Implement set addressed player. We don't need
      // this currently since the current implementation only has one
      // player and the player will never change, but we need it for a
      // more complete implementation.
      auto set_addressed_player_request = Packet::Specialize<SetAddressedPlayerRequest>(pkt);

      if (!set_addressed_player_request->IsValid()) {
        DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
        auto response = RejectBuilder::MakeBuilder(pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
        send_message(label, false, std::move(response));
        return;
      }

      media_interface_->GetMediaPlayerList(base::Bind(&Device::HandleSetAddressedPlayer, weak_ptr_factory_.GetWeakPtr(),
                                                      label, set_addressed_player_request));
    } break;

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
    case CommandPdu::LIST_APPLICATION_SETTING_ATTRIBUTES:
    case CommandPdu::LIST_APPLICATION_SETTING_VALUES:
    case CommandPdu::GET_CUR_PLAYER_APPLICATION_VALUE:
    case CommandPdu::SET_PLAYER_APPLICATION_SETTING_VALUE:
    case CommandPdu::GET_PLAYER__APPLICATION_ATTRIBUTES_TEXT:
    case CommandPdu::GET_PLAYER_APPLICATION_VALUE_TEXT: {
      SettingPacketHandler(label, pkt);
    } break;
#endif
    default: {
#if defined(MTK_A2DP_SRC_SINK_BOTH) && (MTK_A2DP_SRC_SINK_BOTH == TRUE)
      if (btif_av_both_enable()) {
        DEVICE_LOG(ERROR) << "Unhandled Vendor Packet: Pdu" << pkt->GetCommandPdu();
        break;
      }
#endif
      DEVICE_LOG(ERROR) << "Unhandled Vendor Packet: " << pkt->ToString();
      auto response = RejectBuilder::MakeBuilder(
          (CommandPdu)pkt->GetCommandPdu(), Status::INVALID_COMMAND);
      send_message(label, false, std::move(response));
    } break;
  }
}

void Device::HandleGetCapabilities(
    uint8_t label, const std::shared_ptr<GetCapabilitiesRequest>& pkt) {
  DEVICE_VLOG(4) << __func__
                 << ": capability=" << pkt->GetCapabilityRequested();

#if defined(MTK_A2DP_SRC_SINK_BOTH) && (MTK_A2DP_SRC_SINK_BOTH == TRUE)
  if (btif_av_both_enable() && !btif_av_peer_is_connected_sink(this->address_)) {
    DEVICE_VLOG(4) << __func__ << ": peer is not sink";
    return;
  }
#endif

  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = RejectBuilder::MakeBuilder(pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
    send_message(label, false, std::move(response));
    return;
  }

  switch (pkt->GetCapabilityRequested()) {
    case Capability::COMPANY_ID: {
      auto response =
          GetCapabilitiesResponseBuilder::MakeCompanyIdBuilder(0x001958);
      response->AddCompanyId(0x002345);
      send_message_cb_.Run(label, false, std::move(response));
    } break;

    case Capability::EVENTS_SUPPORTED: {
      auto response =
          GetCapabilitiesResponseBuilder::MakeEventsSupportedBuilder(
              Event::PLAYBACK_STATUS_CHANGED);
      response->AddEvent(Event::TRACK_CHANGED);
      response->AddEvent(Event::PLAYBACK_POS_CHANGED);

      if (!avrcp13_compatibility_) {
        response->AddEvent(Event::AVAILABLE_PLAYERS_CHANGED);
        response->AddEvent(Event::ADDRESSED_PLAYER_CHANGED);
        response->AddEvent(Event::UIDS_CHANGED);
        response->AddEvent(Event::NOW_PLAYING_CONTENT_CHANGED);
      }
#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
      response->AddEvent(Event::PLAYER_APPLICATION_SETTING_CHANGED);
#endif

      send_message(label, false, std::move(response));
    } break;

    default: {
      DEVICE_LOG(WARNING) << "Unhandled Capability: "
                          << pkt->GetCapabilityRequested();
#if defined(MTK_A2DP_SRC_SINK_BOTH) && (MTK_A2DP_SRC_SINK_BOTH == TRUE)
      if (btif_av_both_enable()) break;
#endif
      auto response = RejectBuilder::MakeBuilder(CommandPdu::GET_CAPABILITIES,
                                                 Status::INVALID_PARAMETER);
      send_message(label, false, std::move(response));
    } break;
  }
}

void Device::HandleNotification(
    uint8_t label, const std::shared_ptr<RegisterNotificationRequest>& pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = RejectBuilder::MakeBuilder(pkt->GetCommandPdu(),
                                               Status::INVALID_PARAMETER);
    send_message(label, false, std::move(response));
    return;
  }

  DEVICE_VLOG(4) << __func__ << ": event=" << pkt->GetEventRegistered();

  switch (pkt->GetEventRegistered()) {
    case Event::TRACK_CHANGED: {
      media_interface_->GetNowPlayingList(
          base::Bind(&Device::TrackChangedNotificationResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, true));
      /** M: IOT soution for carkit HZ audio 9825 @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
    if (interop_mtk_match_addr(
            INTEROP_MTK_AVRCP_SEND_EXTRA_TRACK_CHANGE, &address_)) {
        media_interface_->GetPlayStatus(base::Bind(
            [](base::WeakPtr<Device> d, uint8_t label, PlayStatus state) {
              if (!d) return;
              if (state.position == 0xffffffff || state.position < 10000) {
                 LOG(INFO) << __func__ << "send extra track, positon = " <<state.position;
                 d->HandleTrackUpdate();
               }
             }, weak_ptr_factory_.GetWeakPtr(), label));
    }
#endif
      /** @} */
    } break;

    case Event::PLAYBACK_STATUS_CHANGED: {
      media_interface_->GetPlayStatus(
          base::Bind(&Device::PlaybackStatusNotificationResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, true));
    } break;

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
    case Event::PLAYER_APPLICATION_SETTING_CHANGED: {
      player_app_setting_changed_ = Notification(true, label);
      media_interface_->GetAppSettingChange(
          base::Bind(&Device::AppSettingChangeNotificationResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, true));
    } break;
#endif
    case Event::PLAYBACK_POS_CHANGED: {
      play_pos_interval_ = pkt->GetInterval();
/** M: Carkit 5s interval cause pos issue @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
      if (play_pos_interval_ == 5 && interop_mtk_match_addr(
              INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL, &address_)) {
        DEVICE_VLOG(2) << __func__ << "Change position interval from 5s to 1s";
        play_pos_interval_ = 1;
      }
#endif
/** @} */
      //interval 0 is not allowed by spec
      if (play_pos_interval_ == 0 || play_pos_interval_ == 0xFFFFFFFF)
        play_pos_interval_ = 1;
      media_interface_->GetPlayStatus(
          base::Bind(&Device::PlaybackPosNotificationResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, true));
    } break;

    case Event::NOW_PLAYING_CONTENT_CHANGED: {
      media_interface_->GetNowPlayingList(
          base::Bind(&Device::HandleNowPlayingNotificationResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, true));
    } break;

    case Event::AVAILABLE_PLAYERS_CHANGED: {
      // TODO (apanicke): If we make a separate handler function for this, make
      // sure to register the notification in the interim response.

      // Respond immediately since this notification doesn't require any info
      avail_players_changed_ = Notification(true, label);
      auto response =
          RegisterNotificationResponseBuilder::MakeAvailablePlayersBuilder(
              true);
      send_message(label, false, std::move(response));
    } break;

    case Event::ADDRESSED_PLAYER_CHANGED: {
      media_interface_->GetMediaPlayerList(
          base::Bind(&Device::AddressedPlayerNotificationResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, true));
    } break;

    case Event::UIDS_CHANGED: {
      // TODO (apanicke): If we make a separate handler function for this, make
      // sure to register the notification in the interim response.

      // Respond immediately since this notification doesn't require any info
      uids_changed_ = Notification(true, label);
      auto response =
          RegisterNotificationResponseBuilder::MakeUidsChangedBuilder(true, 0);
      send_message(label, false, std::move(response));
    } break;

    default: {
      DEVICE_LOG(ERROR) << __func__ << " : Unknown event registered. Event ID="
                        << pkt->GetEventRegistered();
#if defined(MTK_A2DP_SRC_SINK_BOTH) && (MTK_A2DP_SRC_SINK_BOTH == TRUE)
      if (btif_av_both_enable()) break;
#endif
      auto response = RejectBuilder::MakeBuilder(
          (CommandPdu)pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
      send_message(label, false, std::move(response));
    } break;
  }
}

void Device::RegisterVolumeChanged() {
  DEVICE_VLOG(2) << __func__;
  if (volume_interface_ == nullptr) return;

  auto request =
      RegisterNotificationRequestBuilder::MakeBuilder(Event::VOLUME_CHANGED, 0);

  // Find an open transaction label to prevent conflicts with other commands
  // that are in flight. We can not use the reserved label while the
  // notification hasn't been completed.
  uint8_t label = MAX_TRANSACTION_LABEL;
  for (uint8_t i = 0; i < MAX_TRANSACTION_LABEL; i++) {
    /** M: Fix avrcp absolute volume change fail @{ */
    if (ct_active_labels_.find(i) == ct_active_labels_.end()) {
      ct_active_labels_.insert(i);
    /** @} */
      label = i;
      break;
    }
  }

  if (label == MAX_TRANSACTION_LABEL) {
    DEVICE_LOG(FATAL)
        << __func__
        << ": Abandon all hope, something went catastrophically wrong";
  }

  send_message_cb_.Run(label, false, std::move(request));
}

void Device::HandleVolumeChanged(
    uint8_t label, const std::shared_ptr<RegisterNotificationResponse>& pkt) {
  DEVICE_VLOG(1) << __func__ << ": interim=" << pkt->IsInterim();

  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = RejectBuilder::MakeBuilder(pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
    send_message(label, false, std::move(response));
    ct_active_labels_.erase(label);
    volume_interface_ = nullptr;
    volume_ = VOL_REGISTRATION_FAILED;
    return;
  }

  if (volume_interface_ == nullptr) return;

  if (pkt->GetCType() == CType::REJECTED) {
    // Disable Absolute Volume
    active_labels_.erase(label);
    volume_interface_ = nullptr;
    volume_ = VOL_REGISTRATION_FAILED;
    return;
  }

  // We only update on interim and just re-register on changes.
  if (!pkt->IsInterim()) {
    /** M: Fix IOT box send volume change event even we don't register @{ */
    if (ct_active_labels_.find(label) == ct_active_labels_.end())
      return;
    /** @} */
    ct_active_labels_.erase(label);
    RegisterVolumeChanged();
    return;
  }

  // Handle the first volume update.
  if (volume_ == VOL_NOT_SUPPORTED) {
    volume_ = pkt->GetVolume();
    volume_interface_->DeviceConnected(
        GetAddress(),
        base::Bind(&Device::SetVolume, weak_ptr_factory_.GetWeakPtr()));

    // Ignore the returned volume in favor of the volume returned
    // by the volume interface.
    return;
  }

  if (!IsActive()) {
    DEVICE_VLOG(3) << __func__
                   << ": Ignoring volume changes from non active device";
    return;
  }

  /** M: remote send same volume value @{ */
  int8_t temp_volume = pkt->GetVolume();
  if (temp_volume == volume_) {
    DEVICE_VLOG(3) <<"Volume not change, return ";
    return;
  }
  /** @} */

  volume_ = pkt->GetVolume();
  DEVICE_VLOG(1) << __func__ << ": Volume has changed to " << (uint32_t)volume_;
  volume_interface_->SetVolume(volume_);
}

void Device::SetVolume(int8_t volume) {
  // TODO (apanicke): Implement logic for Multi-AVRCP
  DEVICE_VLOG(1) << __func__ << ": volume=" << (int)volume;
  /** M: Some device don't response the same volume. @{ */
  if (volume_ == volume) {
    DEVICE_VLOG(3) << " return since volume is the same";
    return;
  }
  /** @} */
  auto request = SetAbsoluteVolumeRequestBuilder::MakeBuilder(volume);

  uint8_t label = MAX_TRANSACTION_LABEL;
  for (uint8_t i = 0; i < MAX_TRANSACTION_LABEL; i++) {
    if (ct_active_labels_.find(i) == ct_active_labels_.end()) {
      ct_active_labels_.insert(i);
      label = i;
      break;
    }
  }

  volume_ = volume;
  send_message_cb_.Run(label, false, std::move(request));
}

void Device::TrackChangedNotificationResponse(uint8_t label, bool interim,
                                              std::string curr_song_id,
                                              std::vector<SongInfo> song_list) {
  DEVICE_VLOG(1) << __func__ <<": curr_song_id= "<< curr_song_id;
  uint64_t uid = 0;

  if (interim) {
    track_changed_ = Notification(true, label);
  } else if (!track_changed_.first) {
    DEVICE_VLOG(0) << __func__ << ": Device not registered for update";
    return;
  }

  // Anytime we use the now playing list, update our map so that its always
  // current
  now_playing_ids_.clear();
  for (const SongInfo& song : song_list) {
    now_playing_ids_.insert(song.media_id);
    DEVICE_VLOG(2) << __func__ << ":song.media_id= " << song.media_id;
    if (curr_song_id == song.media_id) {
      DEVICE_VLOG(1) << __func__ << ": Found media ID match for "
                     << song.media_id;
      uid = now_playing_ids_.get_uid(curr_song_id);
    }
  }

  /* Handle for PTS AVRCP/TG/NFY/BV-05-C and AVRCP/TG/NFY/BV-08-C */
  if (interim && stack_config_get_interface()->get_pts_avrcp_test()) {
    uid = 0;
  }

  if (curr_song_id == "") {
    DEVICE_LOG(WARNING) << "Empty media ID";
    uid = 0;
    if (stack_config_get_interface()->get_pts_avrcp_test()) {
      DEVICE_LOG(WARNING) << __func__ << ": pts test mode";
      uid = 0xffffffffffffffff;
    }
    /** M: Changed the track id for some IOT device. @{ */
    // Some devices don't get the song info if the track id no change.
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
    else if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_USE_FAKE_ID,
                                                                  &address_)) {
      uid = now_playing_ids_.get_fake_track_id(interim);
      DEVICE_VLOG(1) << __func__ << ": fake track id =  " << uid;
    }
#endif
    /** @} */
  }

  auto response = RegisterNotificationResponseBuilder::MakeTrackChangedBuilder(
      interim, uid);
  send_message_cb_.Run(label, false, std::move(response));
  if (!interim) {
    active_labels_.erase(label);
    track_changed_ = Notification(false, 0);
  }
}

void Device::PlaybackStatusNotificationResponse(uint8_t label, bool interim,
                                                PlayStatus status) {
  DEVICE_VLOG(1) << __func__;
//IOT device use music status only
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (!interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_USE_MUSIC_ONLY,
                                                         &address_))
#endif
  {
    /** M:third party apk not rsp play status, check from the audio status @{ */
    if (status.state != PlayState::PLAYING && nullptr != AvrcpService::Get()) {
      PlayState audio_state = AvrcpService::Get()->GetAudioState();
      if (audio_state == PlayState::PLAYING) status.state = audio_state;
    }
    /** @} */
  }
//IOT device use a2dp status only
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_USE_A2DP_ONLY,
                                                         &address_)) {
    /** M:IOT carkit need a2dp status to avoid send play key @{ */
    if (nullptr != AvrcpService::Get()) {
      status.state = AvrcpService::Get()->GetA2dpState();
    }
    /** @} */
  }
#endif

  //M: bug fix for QQ music no song info when online pre/next
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_FOR_QQ_PLAYER, &address_)) {
    if (status.state == PlayState::STOPPED) {
      status.state = PlayState::PAUSED;
    }
  }
#endif

  if (status.state == PlayState::PAUSED) play_pos_update_cb_.Cancel();
/** M: Fix IOT when status change to play @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if ((last_play_status_.state != PlayState::PLAYING)
              && (status.state == PlayState::PLAYING)) {
    //M: bug fix for some carkit need track info after playing to show pos
    if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_SEND_TRACK_WHEN_PLAY, &address_)) {
        HandleTrackUpdate();
    }
  }
#endif
/** @} */

  if (interim) {
    play_status_changed_ = Notification(true, label);
  } else if (!play_status_changed_.first) {
    DEVICE_VLOG(0) << __func__ << ": Device not registered for update";
    return;
  }

  auto state_to_send = status.state;
  /** M: Send Play status to IOT carkit due to position not sync @{ */
  if (!IsActive()
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
      && !interop_mtk_match_addr_name(
          INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE, &address_)
#endif
     ) {
     state_to_send = PlayState::PAUSED;
  }
  if (!interim && state_to_send == last_play_status_.state) {
    DEVICE_VLOG(0) << __func__
                   << ": Not sending notification due to no state update "
                   << address_.ToString();
    return;
  }

  last_play_status_.state = state_to_send;

/** M: Carkit can't stop when fast_forward/rewind @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr(INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS, &address_)) {
    if (fast_forward_rewind_status_ != PlayState::ERROR)
      state_to_send = fast_forward_rewind_status_;
  }
#endif
/** @} */

  auto response =
      RegisterNotificationResponseBuilder::MakePlaybackStatusBuilder(
          interim, state_to_send);
  send_message_cb_.Run(label, false, std::move(response));

  if (!interim) {
    active_labels_.erase(label);
    play_status_changed_ = Notification(false, 0);
  }
}

void Device::PlaybackPosNotificationResponse(uint8_t label, bool interim,
                                             PlayStatus status) {
  DEVICE_VLOG(4) << __func__;

  if (status.position == 0xFFFFFFFF) {
    status.position = 0;
  }

  if (interim) {
    play_pos_changed_ = Notification(true, label);
  } else if (!play_pos_changed_.first) {
    DEVICE_VLOG(3) << __func__ << ": Device not registered for update";
    return;
  }

   /** M: when music give the same pos we still get position interval @{ */
  // We still try to send updates while music is playing to the non active
  // device even though the device thinks the music is paused. This makes
  // the status bar on the remote device move.
  if (status.state == PlayState::PLAYING && !IsInSilenceMode()) {
    DEVICE_VLOG(2) << __func__ << ": Queue next play position update";
    play_pos_update_cb_.Reset(base::Bind(&Device::HandlePlayPosUpdate,
                                         weak_ptr_factory_.GetWeakPtr()));
    base::MessageLoop::current()->task_runner()->PostDelayedTask(
        FROM_HERE, play_pos_update_cb_.callback(),
        base::TimeDelta::FromSeconds(play_pos_interval_));
  }

  if (!interim && last_play_status_.position == status.position) {
    DEVICE_LOG(WARNING) << address_.ToString()
                        << ": No update to play position";
    return;
  }

  auto response =
      RegisterNotificationResponseBuilder::MakePlaybackPositionBuilder(
          interim, status.position);
  send_message_cb_.Run(label, false, std::move(response));

  last_play_status_.position = status.position;

  if (!interim) {
    active_labels_.erase(label);
    play_pos_changed_ = Notification(false, 0);
  }

  /** M: move this part to front @{
  // We still try to send updates while music is playing to the non active
  // device even though the device thinks the music is paused. This makes
  // the status bar on the remote device move.
  if (status.state == PlayState::PLAYING && !IsInSilenceMode()) {
    DEVICE_VLOG(0) << __func__ << ": Queue next play position update";
    play_pos_update_cb_.Reset(base::Bind(&Device::HandlePlayPosUpdate,
                                         weak_ptr_factory_.GetWeakPtr()));
    base::MessageLoop::current()->task_runner()->PostDelayedTask(
        FROM_HERE, play_pos_update_cb_.callback(),
        base::TimeDelta::FromSeconds(play_pos_interval_));
  }
  */
}

// TODO (apanicke): Finish implementing when we add support for more than one
// player
void Device::AddressedPlayerNotificationResponse(
    uint8_t label, bool interim, uint16_t curr_player,
    std::vector<MediaPlayerInfo> /* unused */) {
  DEVICE_VLOG(1) << __func__
                 << ": curr_player_id=" << (unsigned int)curr_player;

  if (interim) {
    addr_player_changed_ = Notification(true, label);
  } else if (!addr_player_changed_.first) {
    DEVICE_VLOG(3) << __func__ << ": Device not registered for update";
    return;
  }

  // If there is no set browsed player, use the current addressed player as the
  // default NOTE: Using any browsing commands before the browsed player is set
  // is a violation of the AVRCP Spec but there are some carkits that try too
  // anyways
  curr_browsed_player_id_ = curr_player;

  auto response =
      RegisterNotificationResponseBuilder::MakeAddressedPlayerBuilder(
          interim, curr_player, 0x0000);
  send_message_cb_.Run(label, false, std::move(response));

  if (!interim) {
    active_labels_.erase(label);
    addr_player_changed_ = Notification(false, 0);
  }
}

void Device::RejectNotification() {
  DEVICE_VLOG(1) << __func__;
  Notification* rejectNotification[] = {&play_status_changed_, &track_changed_,
                                        &play_pos_changed_,
                                        &now_playing_changed_};
  for (int i = 0; i < 4; i++) {
    uint8_t label = rejectNotification[i]->second;
    auto response = RejectBuilder::MakeBuilder(
        CommandPdu::REGISTER_NOTIFICATION, Status::ADDRESSED_PLAYER_CHANGED);
    send_message_cb_.Run(label, false, std::move(response));
    active_labels_.erase(label);
    rejectNotification[i] = new Notification(false, 0);
  }
}

void Device::GetPlayStatusResponse(uint8_t label, PlayStatus status) {
  DEVICE_VLOG(2) << __func__ << ": position=" << status.position
                 << " duration=" << status.duration
                 << " state=" << status.state;

  if (status.position == 0xFFFFFFFF) {
    status.position = 0;
  }

  //IOT device use music status only
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (!interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_USE_MUSIC_ONLY,
                                                         &address_))
#endif
  {
    /** M:third party apk not rsp play status, check from the audio status @{ */
    if (status.state != PlayState::PLAYING && nullptr != AvrcpService::Get()) {
      PlayState audio_state = AvrcpService::Get()->GetAudioState();
      if (audio_state == PlayState::PLAYING) status.state = audio_state;
    }
    /** @} */
  }

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_USE_A2DP_ONLY,
                                                         &address_)) {
    /** M:IOT carkit need a2dp status to avoid send play key @{ */
    if (nullptr != AvrcpService::Get()) {
      status.state = AvrcpService::Get()->GetA2dpState();
    }
    /** @} */
  }
#endif

  //M: bug fix for QQ music no song info when online pre/next
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_FOR_QQ_PLAYER, &address_)) {
    if (status.state == PlayState::STOPPED) {
      status.state = PlayState::PAUSED;
    }
  }

/** M: Carkit can't stop when fast_forward/rewind @{ */
  if (interop_mtk_match_addr(INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS, &address_)) {
    if (fast_forward_rewind_status_ != PlayState::ERROR)
      status.state = fast_forward_rewind_status_;
  }
/** @} */
#endif
  /** M: Send Play status to IOT carkit due to position not sync @{ */
  if (!IsActive()
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
      && !interop_mtk_match_addr_name(
          INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE, &address_)
#endif
     ) {
    status.state = PlayState::PAUSED;
  }
  /** @} */
  auto response = GetPlayStatusResponseBuilder::MakeBuilder(
      status.duration, status.position, status.state);
  send_message(label, false, std::move(response));
//M: bug fix for QQ music no song info when online pre/next
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr_name(INTEROP_MTK_AVRCP_FOR_QQ_PLAYER, &address_)) {
    if (status.position == 0 && status.state == PlayState::PLAYING) {
      SendMediaUpdate(true,true,true);
    }
  }
#endif
}

void Device::GetElementAttributesResponse(
    uint8_t label, std::shared_ptr<GetElementAttributesRequest> pkt,
    SongInfo info) {
  DEVICE_VLOG(2) << __func__;

  auto get_element_attributes_pkt = pkt;
  auto attributes_requested =
      get_element_attributes_pkt->GetAttributesRequested();

  auto response = GetElementAttributesResponseBuilder::MakeBuilder(ctrl_mtu_);

  last_song_info_ = info;

  if (attributes_requested.size() != 0) {
    for (const auto& attribute : attributes_requested) {
      if (info.attributes.find(attribute) != info.attributes.end()) {
        response->AddAttributeEntry(*info.attributes.find(attribute));
      }
    }
  } else {  // zero attributes requested which means all attributes requested
    for (const auto& attribute : info.attributes) {
      response->AddAttributeEntry(attribute);
    }
  }

  send_message(label, false, std::move(response));
}

#if defined(CALL_SCO_KEY_FEATURE) && (CALL_SCO_KEY_FEATURE == TRUE)
bool Device::isDropKey() {
    DEVICE_VLOG(1) << __func__ << ": key_drop_ = " << key_drop_
             << "; callIdle= " << bluetooth::headset::IsCallIdle()
             << "; ATDReceived= " << bluetooth::headset::isATDReceived();

  return (key_drop_ || !bluetooth::headset::IsCallIdle()
          || bluetooth::headset::isATDReceived());
}
#endif

void Device::MessageReceived(uint8_t label, std::shared_ptr<Packet> pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = RejectBuilder::MakeBuilder(static_cast<CommandPdu>(0), Status::INVALID_COMMAND);
    send_message(label, false, std::move(response));
    return;
  }

  DEVICE_VLOG(4) << __func__ << ": opcode=" << pkt->GetOpcode();
  active_labels_.insert(label);
  switch (pkt->GetOpcode()) {
    // TODO (apanicke): Remove handling of UNIT_INFO and SUBUNIT_INFO from
    // the AVRC_API and instead handle it here to reduce fragmentation.
    case Opcode::UNIT_INFO: {
    } break;
    case Opcode::SUBUNIT_INFO: {
    } break;
    case Opcode::PASS_THROUGH: {
      auto pass_through_packet = Packet::Specialize<PassThroughPacket>(pkt);

      if (!pass_through_packet->IsValid()) {
        DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
        auto response = RejectBuilder::MakeBuilder(static_cast<CommandPdu>(0), Status::INVALID_COMMAND);
        send_message(label, false, std::move(response));
        return;
      }

      auto response = PassThroughPacketBuilder::MakeBuilder(
          true, pass_through_packet->GetKeyState() == KeyState::PUSHED,
          pass_through_packet->GetOperationId());
      send_message(label, false, std::move(response));

#if defined(CALL_SCO_KEY_FEATURE) && (CALL_SCO_KEY_FEATURE == TRUE)
      long disc_time = bluetooth::headset::getScoDiscTime();
      long cur_time = time(NULL);
      key_drop_ = ((cur_time - disc_time) < SCO_DISCONNECT_TIMER);
      if (isDropKey())
        return;
#endif

      // TODO (apanicke): Use an enum for media key ID's
      /** M: put PLAY push/release in the same logic block @{ */
      if (pass_through_packet->GetOperationId() == 0x44) {
        // We need to get the play status since we need to know
        // what the actual playstate is without being modified
        // by whether the device is active.
        media_interface_->GetPlayStatus(base::Bind(
            [](base::WeakPtr<Device> d, KeyState ks, PlayStatus s) {
              if (!d) return;

              if (ks == KeyState::RELEASED && !d->IsActive()) {
                LOG(INFO) << "Setting " << d->address_.ToString()
                          << " to be the active device";
                d->media_interface_->SetActiveDevice(d->address_);
              }
              if (s.state == PlayState::PLAYING) {
                LOG(INFO)
                    << "Drop play key since music playing";
                return;
              }

              d->media_interface_->SendKeyEvent(0x44, ks);
            },
            weak_ptr_factory_.GetWeakPtr(), pass_through_packet->GetKeyState()));
        return;
      }

/** M: Carkit can't stop when fast_forward/rewind @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
      if (interop_mtk_match_addr(INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS, &address_)) {
        if (pass_through_packet->GetKeyState() == KeyState::PUSHED) {
          if (pass_through_packet->GetOperationId() == 0x49) {
            fast_forward_rewind_status_ = PlayState::FWD_SEEK;
            SendFastForwardRewindStatus(false, PlayState::FWD_SEEK);
          } else if(pass_through_packet->GetOperationId() == 0x48) {
            fast_forward_rewind_status_ = PlayState::REV_SEEK;
            SendFastForwardRewindStatus(false, PlayState::REV_SEEK);
          }
        } else if (pass_through_packet->GetOperationId() == 0x49 ||
                   pass_through_packet->GetOperationId() == 0x48) {
          fast_forward_rewind_status_ = PlayState::ERROR;
          SendFastForwardRewindStatus(false, last_play_status_.state);
        }
      }
#endif
/** @} */

      if (IsActive()) {
        media_interface_->SendKeyEvent(pass_through_packet->GetOperationId(),
                                       pass_through_packet->GetKeyState());
      }
    } break;
    case Opcode::VENDOR: {
      auto vendor_pkt = Packet::Specialize<VendorPacket>(pkt);
      VendorPacketHandler(label, vendor_pkt);
    } break;
  }
}

void Device::HandlePlayItem(uint8_t label,
                            std::shared_ptr<PlayItemRequest> pkt) {
  DEVICE_VLOG(2) << __func__ << ": scope=" << pkt->GetScope()
                 << " uid=" << pkt->GetUid();

  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = RejectBuilder::MakeBuilder(pkt->GetCommandPdu(), Status::INVALID_PARAMETER);
    send_message(label, false, std::move(response));
    return;
  }

  std::string media_id = "";
  switch (pkt->GetScope()) {
    case Scope::NOW_PLAYING:
      media_id = now_playing_ids_.get_media_id(pkt->GetUid());
      break;
    case Scope::VFS:
      media_id = vfs_ids_.get_media_id(pkt->GetUid());
      break;
    default:
      DEVICE_LOG(WARNING) << __func__ << ": Unknown scope for play item";
  }

  if (media_id == "") {
    DEVICE_VLOG(2) << "Could not find item";
    auto response = RejectBuilder::MakeBuilder(CommandPdu::PLAY_ITEM,
                                               Status::DOES_NOT_EXIST);
    send_message(label, false, std::move(response));
    return;
  }

  media_interface_->PlayItem(curr_browsed_player_id_,
                             pkt->GetScope() == Scope::NOW_PLAYING, media_id);

  auto response = PlayItemResponseBuilder::MakeBuilder(Status::NO_ERROR);
  send_message(label, false, std::move(response));
}

void Device::HandleSetAddressedPlayer(
    uint8_t label, std::shared_ptr<SetAddressedPlayerRequest> pkt,
    uint16_t curr_player, std::vector<MediaPlayerInfo> players) {
  DEVICE_VLOG(2) << __func__ << ": PlayerId=" << pkt->GetPlayerId();

  if (curr_player != pkt->GetPlayerId()) {
    DEVICE_VLOG(2) << "Reject invalid addressed player ID";
    auto response = RejectBuilder::MakeBuilder(CommandPdu::SET_ADDRESSED_PLAYER,
                                               Status::INVALID_PLAYER_ID);
    send_message(label, false, std::move(response));
    return;
  }

  auto response =
      SetAddressedPlayerResponseBuilder::MakeBuilder(Status::NO_ERROR);
  send_message(label, false, std::move(response));
}

void Device::BrowseMessageReceived(uint8_t label,
                                   std::shared_ptr<BrowsePacket> pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = GeneralRejectBuilder::MakeBuilder(Status::INVALID_COMMAND);
    send_message(label, false, std::move(response));
    return;
  }

  DEVICE_VLOG(1) << __func__ << ": pdu=" << pkt->GetPdu();

  switch (pkt->GetPdu()) {
    case BrowsePdu::SET_BROWSED_PLAYER:
      HandleSetBrowsedPlayer(label,
                             Packet::Specialize<SetBrowsedPlayerRequest>(pkt));
      break;
    case BrowsePdu::GET_FOLDER_ITEMS:
      HandleGetFolderItems(label,
                           Packet::Specialize<GetFolderItemsRequest>(pkt));
      break;
    case BrowsePdu::CHANGE_PATH:
      HandleChangePath(label, Packet::Specialize<ChangePathRequest>(pkt));
      break;
    case BrowsePdu::GET_ITEM_ATTRIBUTES:
      HandleGetItemAttributes(
          label, Packet::Specialize<GetItemAttributesRequest>(pkt));
      break;
    case BrowsePdu::GET_TOTAL_NUMBER_OF_ITEMS:
      HandleGetTotalNumberOfItems(
          label, Packet::Specialize<GetTotalNumberOfItemsRequest>(pkt));
      break;
    default:
      DEVICE_LOG(WARNING) << __func__ << ": " << pkt->GetPdu();
      auto response = GeneralRejectBuilder::MakeBuilder(Status::INVALID_COMMAND);
      send_message(label, true, std::move(response));

      break;
  }
}

void Device::HandleGetFolderItems(uint8_t label,
                                  std::shared_ptr<GetFolderItemsRequest> pkt) {
  if (!pkt->IsValid()) {
    // The specific get folder items builder is unimportant on failure.
    DEVICE_LOG(WARNING) << __func__ << ": Get folder items request packet is not valid";
    auto response =
        GetFolderItemsResponseBuilder::MakePlayerListBuilder(Status::INVALID_PARAMETER, 0x0000, browse_mtu_);
    send_message(label, true, std::move(response));
    return;
  }

  DEVICE_VLOG(2) << __func__ << ": scope=" << pkt->GetScope();

  switch (pkt->GetScope()) {
    case Scope::MEDIA_PLAYER_LIST:
      media_interface_->GetMediaPlayerList(
          base::Bind(&Device::GetMediaPlayerListResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, pkt));
      break;
    case Scope::VFS:
      media_interface_->GetFolderItems(
          curr_browsed_player_id_, CurrentFolder(),
          base::Bind(&Device::GetVFSListResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, pkt));
      break;
    case Scope::NOW_PLAYING:
      media_interface_->GetNowPlayingList(
          base::Bind(&Device::GetNowPlayingListResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, pkt));
      break;
    default:
      DEVICE_LOG(ERROR) << __func__ << ": " << pkt->GetScope();
      auto response = GetFolderItemsResponseBuilder::MakePlayerListBuilder(Status::INVALID_PARAMETER, 0, browse_mtu_);
      send_message(label, true, std::move(response));
      break;
  }
}

void Device::HandleGetTotalNumberOfItems(
    uint8_t label, std::shared_ptr<GetTotalNumberOfItemsRequest> pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = GetTotalNumberOfItemsResponseBuilder::MakeBuilder(Status::INVALID_PARAMETER, 0x0000, 0);
    send_message(label, true, std::move(response));
    return;
  }

  DEVICE_VLOG(2) << __func__ << ": scope=" << pkt->GetScope();

  switch (pkt->GetScope()) {
    case Scope::MEDIA_PLAYER_LIST: {
      media_interface_->GetMediaPlayerList(
          base::Bind(&Device::GetTotalNumberOfItemsMediaPlayersResponse,
                     weak_ptr_factory_.GetWeakPtr(), label));
      break;
    }
    case Scope::VFS:
      media_interface_->GetFolderItems(
          curr_browsed_player_id_, CurrentFolder(),
          base::Bind(&Device::GetTotalNumberOfItemsVFSResponse,
                     weak_ptr_factory_.GetWeakPtr(), label));
      break;
    case Scope::NOW_PLAYING:
      media_interface_->GetNowPlayingList(
          base::Bind(&Device::GetTotalNumberOfItemsNowPlayingResponse,
                     weak_ptr_factory_.GetWeakPtr(), label));
      break;
    default:
      DEVICE_LOG(ERROR) << __func__ << ": " << pkt->GetScope();
      break;
  }
}

void Device::GetTotalNumberOfItemsMediaPlayersResponse(
    uint8_t label, uint16_t curr_player, std::vector<MediaPlayerInfo> list) {
  DEVICE_VLOG(2) << __func__ << ": num_items=" << list.size();

  auto builder = GetTotalNumberOfItemsResponseBuilder::MakeBuilder(
      Status::NO_ERROR, 0x0000, list.size());
  send_message(label, true, std::move(builder));
}

void Device::GetTotalNumberOfItemsVFSResponse(uint8_t label,
                                              std::vector<ListItem> list) {
  DEVICE_VLOG(2) << __func__ << ": num_items=" << list.size();

  auto builder = GetTotalNumberOfItemsResponseBuilder::MakeBuilder(
      Status::NO_ERROR, 0x0000, list.size());
  send_message(label, true, std::move(builder));
}

void Device::GetTotalNumberOfItemsNowPlayingResponse(
    uint8_t label, std::string curr_song_id, std::vector<SongInfo> list) {
  DEVICE_VLOG(2) << __func__ << ": num_items=" << list.size();

  auto builder = GetTotalNumberOfItemsResponseBuilder::MakeBuilder(
      Status::NO_ERROR, 0x0000, list.size());
  send_message(label, true, std::move(builder));
}

void Device::HandleChangePath(uint8_t label,
                              std::shared_ptr<ChangePathRequest> pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = ChangePathResponseBuilder::MakeBuilder(Status::INVALID_PARAMETER, 0);
    send_message(label, true, std::move(response));
    return;
  }

  DEVICE_VLOG(2) << __func__ << ": direction=" << pkt->GetDirection()
                 << " uid=" << loghex(pkt->GetUid());

  if (pkt->GetDirection() == Direction::DOWN &&
      vfs_ids_.get_media_id(pkt->GetUid()) == "") {
    DEVICE_LOG(ERROR) << __func__
                      << ": No item found for UID=" << pkt->GetUid();
    auto builder =
        ChangePathResponseBuilder::MakeBuilder(Status::DOES_NOT_EXIST, 0);
    send_message(label, true, std::move(builder));
    return;
  }

  if (pkt->GetDirection() == Direction::DOWN) {
    current_path_.push(vfs_ids_.get_media_id(pkt->GetUid()));
    DEVICE_VLOG(2) << "Pushing Path to stack: \"" << CurrentFolder() << "\"";
  } else {
    // Don't pop the root id off the stack
    if (current_path_.size() > 0) {
      current_path_.pop();
    } else {
      DEVICE_LOG(ERROR) << "Trying to change directory up past root.";
      auto builder =
          ChangePathResponseBuilder::MakeBuilder(Status::DOES_NOT_EXIST, 0);
      send_message(label, true, std::move(builder));
      return;
    }

    DEVICE_VLOG(2) << "Popping Path from stack: new path=\"" << CurrentFolder()
                   << "\"";
  }

  media_interface_->GetFolderItems(
      curr_browsed_player_id_, CurrentFolder(),
      base::Bind(&Device::ChangePathResponse, weak_ptr_factory_.GetWeakPtr(),
                 label, pkt));
}

void Device::ChangePathResponse(uint8_t label,
                                std::shared_ptr<ChangePathRequest> pkt,
                                std::vector<ListItem> list) {
  // TODO (apanicke): Reconstruct the VFS ID's here. Right now it gets
  // reconstructed in GetFolderItemsVFS
  // save number of items for set browser rsp
  num_items_in_br_folder_ = list.size();
  auto builder =
      ChangePathResponseBuilder::MakeBuilder(Status::NO_ERROR, list.size());
  send_message(label, true, std::move(builder));
}

void Device::HandleGetItemAttributes(
    uint8_t label, std::shared_ptr<GetItemAttributesRequest> pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto builder = GetItemAttributesResponseBuilder::MakeBuilder(Status::INVALID_PARAMETER, browse_mtu_);
    send_message(label, true, std::move(builder));
    return;
  }

  DEVICE_VLOG(2) << __func__ << ": scope=" << pkt->GetScope()
                 << " uid=" << loghex(pkt->GetUid())
                 << " uid counter=" << loghex(pkt->GetUidCounter());
  if (pkt->GetUidCounter() != 0x0000) {  // For database unaware player, use 0
    DEVICE_LOG(WARNING) << "UidCounter is invalid";
    auto builder = GetItemAttributesResponseBuilder::MakeBuilder(
        Status::UIDS_CHANGED, browse_mtu_);
    send_message(label, true, std::move(builder));
    return;
  }

  switch (pkt->GetScope()) {
    case Scope::NOW_PLAYING: {
      media_interface_->GetNowPlayingList(
          base::Bind(&Device::GetItemAttributesNowPlayingResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, pkt));
    } break;
    case Scope::VFS:
      // TODO (apanicke): Check the vfs_ids_ here. If the item doesn't exist
      // then we can auto send the error without calling up. We do this check
      // later right now though in order to prevent race conditions with updates
      // on the media layer.
      media_interface_->GetFolderItems(
          curr_browsed_player_id_, CurrentFolder(),
          base::Bind(&Device::GetItemAttributesVFSResponse,
                     weak_ptr_factory_.GetWeakPtr(), label, pkt));
      break;
    default:
      DEVICE_LOG(ERROR) << "UNKNOWN SCOPE FOR HANDLE GET ITEM ATTRIBUTES";
      break;
  }
}

void Device::GetItemAttributesNowPlayingResponse(
    uint8_t label, std::shared_ptr<GetItemAttributesRequest> pkt,
    std::string curr_media_id, std::vector<SongInfo> song_list) {
  DEVICE_VLOG(2) << __func__ << ": uid=" << loghex(pkt->GetUid());
  auto builder = GetItemAttributesResponseBuilder::MakeBuilder(Status::NO_ERROR,
                                                               browse_mtu_);

  auto media_id = now_playing_ids_.get_media_id(pkt->GetUid());
  if (media_id == "") {
    media_id = curr_media_id;
  }

  DEVICE_VLOG(2) << __func__ << ": media_id=\"" << media_id << "\"";

  SongInfo info;
  for (const auto& temp : song_list) {
    if (temp.media_id == media_id) {
      info = temp;
    }
  }

  /** M: Set track number and total tack number to 1 @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (interop_mtk_match_addr(
        INTEROP_MTK_AVRCP_FORCE_TRACK_NUMBER_TO_ONE, &address_)) {
    info.attributes.erase(Attribute::TRACK_NUMBER);
    info.attributes.insert(AttributeEntry(Attribute::TRACK_NUMBER, std::string("1")));
    info.attributes.erase(Attribute::TOTAL_NUMBER_OF_TRACKS);
    info.attributes.insert(AttributeEntry(Attribute::TOTAL_NUMBER_OF_TRACKS, std::string("1")));
  }
#endif
  /** @} */

  auto attributes_requested = pkt->GetAttributesRequested();
  if (attributes_requested.size() != 0) {
    for (const auto& attribute : attributes_requested) {
      if (info.attributes.find(attribute) != info.attributes.end()) {
        builder->AddAttributeEntry(*info.attributes.find(attribute));
      }
    }
  } else {
    // If zero attributes were requested, that means all attributes were
    // requested
    for (const auto& attribute : info.attributes) {
      builder->AddAttributeEntry(attribute);
    }
  }

  send_message(label, true, std::move(builder));
}

void Device::GetItemAttributesVFSResponse(
    uint8_t label, std::shared_ptr<GetItemAttributesRequest> pkt,
    std::vector<ListItem> item_list) {
  DEVICE_VLOG(2) << __func__ << ": uid=" << loghex(pkt->GetUid());

  auto media_id = vfs_ids_.get_media_id(pkt->GetUid());
  if (media_id == "") {
    LOG(WARNING) << __func__ << ": Item not found";
    auto builder = GetItemAttributesResponseBuilder::MakeBuilder(
        Status::DOES_NOT_EXIST, browse_mtu_);
    send_message(label, true, std::move(builder));
    return;
  }

  auto builder = GetItemAttributesResponseBuilder::MakeBuilder(Status::NO_ERROR,
                                                               browse_mtu_);

  ListItem item_requested;
  item_requested.type = ListItem::SONG;
  for (const auto& temp : item_list) {
    if ((temp.type == ListItem::FOLDER && temp.folder.media_id == media_id) ||
        (temp.type == ListItem::SONG && temp.song.media_id == media_id)) {
      item_requested = temp;
    }
  }

  // TODO (apanicke): Add a helper function or allow adding a map
  // of attributes to GetItemAttributesResponseBuilder
  auto attributes_requested = pkt->GetAttributesRequested();
  if (item_requested.type == ListItem::FOLDER) {
    if (attributes_requested.size() == 0) {
      builder->AddAttributeEntry(Attribute::TITLE, item_requested.folder.name);
    } else {
      for (auto& attr : attributes_requested) {
        if (attr == Attribute::TITLE) {
          builder->AddAttributeEntry(Attribute::TITLE,
                                     item_requested.folder.name);
        }
      }
    }
  } else {
    if (attributes_requested.size() != 0) {
      for (const auto& attribute : attributes_requested) {
        if (item_requested.song.attributes.find(attribute) !=
            item_requested.song.attributes.end()) {
          builder->AddAttributeEntry(
              *item_requested.song.attributes.find(attribute));
        }
      }
    } else {
      // If zero attributes were requested, that means all attributes were
      // requested
      for (const auto& attribute : item_requested.song.attributes) {
        builder->AddAttributeEntry(attribute);
      }
    }
  }

  send_message(label, true, std::move(builder));
}

void Device::GetMediaPlayerListResponse(
    uint8_t label, std::shared_ptr<GetFolderItemsRequest> pkt,
    uint16_t curr_player, std::vector<MediaPlayerInfo> players) {
  DEVICE_VLOG(2) << __func__;

  if (players.size() == 0) {
    auto no_items_rsp = GetFolderItemsResponseBuilder::MakePlayerListBuilder(
        Status::RANGE_OUT_OF_BOUNDS, 0x0000, browse_mtu_);
    send_message(label, true, std::move(no_items_rsp));
  }

  auto builder = GetFolderItemsResponseBuilder::MakePlayerListBuilder(
      Status::NO_ERROR, 0x0000, browse_mtu_);

  // Move the current player to the first slot due to some carkits always
  // connecting to the first listed player rather than using the ID
  // returned by Addressed Player Changed
  for (auto it = players.begin(); it != players.end(); it++) {
    if (it->id == curr_player) {
      DEVICE_VLOG(1) << " Adding player to first spot: " << it->name;
      auto temp_player = *it;
      players.erase(it);
      players.insert(players.begin(), temp_player);
      break;
    }
  }

  for (size_t i = pkt->GetStartItem();
       i <= pkt->GetEndItem() && i < players.size(); i++) {
    MediaPlayerItem item(players[i].id, players[i].name,
                         players[i].browsing_supported);
    builder->AddMediaPlayer(item);
  }

  send_message(label, true, std::move(builder));
}

std::set<AttributeEntry> filter_attributes_requested(
    const SongInfo& song, const std::vector<Attribute>& attrs) {
  std::set<AttributeEntry> result;
  for (const auto& attr : attrs) {
    if (song.attributes.find(attr) != song.attributes.end()) {
      result.insert(*song.attributes.find(attr));
    }
  }

  return result;
}

void Device::GetVFSListResponse(uint8_t label,
                                std::shared_ptr<GetFolderItemsRequest> pkt,
                                std::vector<ListItem> items) {
  DEVICE_VLOG(2) << __func__ << ": start_item=" << pkt->GetStartItem()
                 << " end_item=" << pkt->GetEndItem();

  // The builder will automatically correct the status if there are zero items
  auto builder = GetFolderItemsResponseBuilder::MakeVFSBuilder(
      Status::NO_ERROR, 0x0000, browse_mtu_);

  // TODO (apanicke): Add test that checks if vfs_ids_ is the correct size after
  // an operation.
  for (const auto& item : items) {
    if (item.type == ListItem::FOLDER) {
      vfs_ids_.insert(item.folder.media_id);
    } else if (item.type == ListItem::SONG) {
      vfs_ids_.insert(item.song.media_id);
    }
  }

  // Add the elements retrieved in the last get folder items request and map
  // them to UIDs The maps will be cleared every time a directory change
  // happens. These items do not need to correspond with the now playing list as
  // the UID's only need to be unique in the context of the current scope and
  // the current folder
  for (auto i = pkt->GetStartItem(); i <= pkt->GetEndItem() && i < items.size();
       i++) {
    if (items[i].type == ListItem::FOLDER) {
      auto folder = items[i].folder;
      // right now we always use folders of mixed type
      FolderItem folder_item(vfs_ids_.get_uid(folder.media_id), 0x00,
                             folder.is_playable, folder.name);
      auto can_break = !builder->AddFolder(folder_item);
      /** M: Save current folder name for set browse player rsp @{ */
      if (i == pkt->GetStartItem()) {
        mFolderName_[current_path_.size()] = folder.name;
        DEVICE_VLOG(2) << __func__ << "current_path_.size() = " << current_path_.size()
                 << "current folder.name = " << folder.name;
      }
      /** @} */
      if (can_break) break;
    } else if (items[i].type == ListItem::SONG) {
      auto song = items[i].song;
      auto title =
          song.attributes.find(Attribute::TITLE) != song.attributes.end()
              ? song.attributes.find(Attribute::TITLE)->value()
              : "No Song Info";
      MediaElementItem song_item(vfs_ids_.get_uid(song.media_id), title,
                                 std::set<AttributeEntry>());

      if (pkt->GetNumAttributes() == 0x00) {  // All attributes requested
        song_item.attributes_ = std::move(song.attributes);
      } else {
        song_item.attributes_ =
            filter_attributes_requested(song, pkt->GetAttributesRequested());
      }

      // If we fail to add a song, don't accidentally add one later that might
      // fit.
      if (!builder->AddSong(song_item)) break;
    }
  }

  send_message(label, true, std::move(builder));
}

void Device::GetNowPlayingListResponse(
    uint8_t label, std::shared_ptr<GetFolderItemsRequest> pkt,
    std::string /* unused curr_song_id */, std::vector<SongInfo> song_list) {
  DEVICE_VLOG(2) << __func__;
  auto builder = GetFolderItemsResponseBuilder::MakeNowPlayingBuilder(
      Status::NO_ERROR, 0x0000, browse_mtu_);

  now_playing_ids_.clear();
  for (const SongInfo& song : song_list) {
    now_playing_ids_.insert(song.media_id);
  }

  for (size_t i = pkt->GetStartItem();
       i <= pkt->GetEndItem() && i < song_list.size(); i++) {
    auto song = song_list[i];
    auto title = song.attributes.find(Attribute::TITLE) != song.attributes.end()
                     ? song.attributes.find(Attribute::TITLE)->value()
                     : "No Song Info";

    MediaElementItem item(i + 1, title, std::set<AttributeEntry>());
    if (pkt->GetNumAttributes() == 0x00) {
      item.attributes_ = std::move(song.attributes);
    } else {
      item.attributes_ =
          filter_attributes_requested(song, pkt->GetAttributesRequested());
    }

    // If we fail to add a song, don't accidentally add one later that might
    // fit.
    if (!builder->AddSong(item)) break;
  }

  send_message(label, true, std::move(builder));
}

void Device::HandleSetBrowsedPlayer(
    uint8_t label, std::shared_ptr<SetBrowsedPlayerRequest> pkt) {
  if (!pkt->IsValid()) {
    DEVICE_LOG(WARNING) << __func__ << ": Request packet is not valid";
    auto response = SetBrowsedPlayerResponseBuilder::MakeBuilder(Status::INVALID_PARAMETER, 0x0000, 0, 0, "");
    send_message(label, true, std::move(response));
    return;
  }
  DEVICE_VLOG(2) << __func__ << ": player_id=" << pkt->GetPlayerId();

  media_interface_->SetBrowsedPlayer(
      pkt->GetPlayerId(),
      base::Bind(&Device::SetBrowsedPlayerResponse,
                 weak_ptr_factory_.GetWeakPtr(), label, pkt));
}

void Device::SetBrowsedPlayerResponse(
    uint8_t label, std::shared_ptr<SetBrowsedPlayerRequest> pkt, bool success,
    std::string root_id, uint32_t num_items) {
  DEVICE_VLOG(2) << __func__ << ": success=" << success << " root_id=\""
                 << root_id << "\" num_items=" << num_items;

  if (!success) {
    auto response = SetBrowsedPlayerResponseBuilder::MakeBuilder(
        Status::INVALID_PLAYER_ID, 0x0000, num_items, 0, "");
    send_message(label, true, std::move(response));
    return;
  }

  curr_browsed_player_id_ = pkt->GetPlayerId();
  DEVICE_VLOG(2) << " curr_browsed_player_id_ = " << curr_browsed_player_id_;

  /** M: Send setBrowsedPlayer rsp with folder depth and items. @{ */
  if (num_items_in_br_folder_ > 0)
    num_items = num_items_in_br_folder_;

  std::size_t folder_depth = current_path_.size();

  std::string folder_name;
  for (std::size_t i = 0; i < folder_depth; i ++ ) {
    folder_name.append(mFolderName_[i]);
    folder_name.append("/");
  }

  DEVICE_VLOG(2) << " num_items = " << num_items << " folder depth = " << folder_depth
                 << " browse rsp folder_name = " << folder_name;
  /** @} */

  auto response = SetBrowsedPlayerResponseBuilder::MakeBuilder(
      Status::NO_ERROR, 0x0000, num_items, folder_depth, folder_name);
  send_message(label, true, std::move(response));
}

void Device::SendMediaUpdate(bool metadata, bool play_status, bool queue) {
  bool is_silence = IsInSilenceMode();

  CHECK(media_interface_);
  DEVICE_VLOG(4) << __func__ << ": Metadata=" << metadata
                 << " : play_status= " << play_status << " : queue=" << queue
                 << " ; is_silence=" << is_silence;

  if (queue) {
    HandleNowPlayingUpdate();
  }

  if (play_status) {
    HandlePlayStatusUpdate();
    if (!is_silence) {
      HandlePlayPosUpdate();
    }
  }

  if (metadata) HandleTrackUpdate();
}

void Device::SendFolderUpdate(bool available_players, bool addressed_player,
                              bool uids) {
  CHECK(media_interface_);
  DEVICE_VLOG(4) << __func__;

  if (available_players) {
    HandleAvailablePlayerUpdate();
  }

  if (addressed_player) {
    HandleAddressedPlayerUpdate();
  }
}

void Device::HandleTrackUpdate() {
  DEVICE_VLOG(2) << __func__;
  if (!track_changed_.first) {
    LOG(WARNING) << "Device is not registered for track changed updates";
    return;
  }

  media_interface_->GetNowPlayingList(
      base::Bind(&Device::TrackChangedNotificationResponse,
                 weak_ptr_factory_.GetWeakPtr(), track_changed_.second, false));
}

void Device::HandlePlayStatusUpdate() {
  DEVICE_VLOG(2) << __func__;
  if (!play_status_changed_.first) {
    LOG(WARNING) << "Device is not registered for play status updates";
    return;
  }

  media_interface_->GetPlayStatus(base::Bind(
      &Device::PlaybackStatusNotificationResponse,
      weak_ptr_factory_.GetWeakPtr(), play_status_changed_.second, false));
}

void Device::HandleNowPlayingUpdate() {
  DEVICE_VLOG(2) << __func__;

  if (!now_playing_changed_.first) {
    LOG(WARNING) << "Device is not registered for now playing updates";
    return;
  }

  media_interface_->GetNowPlayingList(base::Bind(
      &Device::HandleNowPlayingNotificationResponse,
      weak_ptr_factory_.GetWeakPtr(), now_playing_changed_.second, false));
}

void Device::HandleNowPlayingNotificationResponse(
    uint8_t label, bool interim, std::string curr_song_id,
    std::vector<SongInfo> song_list) {
  if (interim) {
    now_playing_changed_ = Notification(true, label);
  } else if (!now_playing_changed_.first) {
    LOG(WARNING) << "Device is not registered for now playing updates";
    return;
  }

  now_playing_ids_.clear();
  for (const SongInfo& song : song_list) {
    now_playing_ids_.insert(song.media_id);
  }

  auto response =
      RegisterNotificationResponseBuilder::MakeNowPlayingBuilder(interim);
  send_message(now_playing_changed_.second, false, std::move(response));

  if (!interim) {
    active_labels_.erase(label);
    now_playing_changed_ = Notification(false, 0);
  }
}

void Device::HandlePlayPosUpdate() {
  DEVICE_VLOG(0) << __func__;
  if (!play_pos_changed_.first) {
    LOG(WARNING) << "Device is not registered for play position updates";
    return;
  }

  media_interface_->GetPlayStatus(base::Bind(
      &Device::PlaybackPosNotificationResponse, weak_ptr_factory_.GetWeakPtr(),
      play_pos_changed_.second, false));
}

void Device::HandleAvailablePlayerUpdate() {
  DEVICE_VLOG(1) << __func__;

  if (!avail_players_changed_.first) {
    LOG(WARNING) << "Device is not registered for available player updates";
    return;
  }

  auto response =
      RegisterNotificationResponseBuilder::MakeAvailablePlayersBuilder(false);
  send_message_cb_.Run(avail_players_changed_.second, false,
                       std::move(response));

  if (!avail_players_changed_.first) {
    active_labels_.erase(avail_players_changed_.second);
    avail_players_changed_ = Notification(false, 0);
  }
}

void Device::HandleAddressedPlayerUpdate() {
  DEVICE_VLOG(1) << __func__;
  if (!addr_player_changed_.first) {
    DEVICE_LOG(WARNING)
        << "Device is not registered for addressed player updates";
    return;
  }
  media_interface_->GetMediaPlayerList(base::Bind(
      &Device::AddressedPlayerNotificationResponse,
      weak_ptr_factory_.GetWeakPtr(), addr_player_changed_.second, false));
}

void Device::DeviceDisconnected() {
  DEVICE_LOG(INFO) << "Device was disconnected";
  play_pos_update_cb_.Cancel();

  // TODO (apanicke): Once the interfaces are set in the Device construction,
  // remove these conditionals.
  if (volume_interface_ != nullptr)
    volume_interface_->DeviceDisconnected(GetAddress());
}

static std::string volumeToStr(int8_t volume) {
  if (volume == VOL_NOT_SUPPORTED) return "Absolute Volume not supported";
  if (volume == VOL_REGISTRATION_FAILED)
    return "Volume changed notification was rejected";
  return std::to_string(volume);
}

std::ostream& operator<<(std::ostream& out, const Device& d) {
  out << d.address_.ToString();
  if (d.IsActive()) out << " <Active>";
  out << std::endl;

  ScopedIndent indent(out);
  out << "Current Volume: " << volumeToStr(d.volume_) << std::endl;
  out << "Current Browsed Player ID: " << d.curr_browsed_player_id_
      << std::endl;
  out << "Registered Notifications:\n";
  {
    ScopedIndent indent(out);
    if (d.track_changed_.first) out << "Track Changed\n";
    if (d.play_status_changed_.first) out << "Play Status\n";
    if (d.play_pos_changed_.first) out << "Play Position\n";
    if (d.now_playing_changed_.first) out << "Now Playing\n";
    if (d.addr_player_changed_.first) out << "Addressed Player\n";
    if (d.avail_players_changed_.first) out << "Available Players\n";
    if (d.uids_changed_.first) out << "UIDs Changed\n";
  }
  out << "Last Play State: " << d.last_play_status_.state << std::endl;
  out << "Last Song Sent ID: \"" << d.last_song_info_.media_id << "\"\n";
  out << "Current Folder: \"" << d.CurrentFolder() << "\"\n";
  out << "MTU Sizes: CTRL=" << d.ctrl_mtu_ << " BROWSE=" << d.browse_mtu_
      << std::endl;
  // TODO (apanicke): Add supported features as well as media keys
  return out;
}

void Device::SendFastForwardRewindStatus(bool interim, PlayState state) {
  if (!play_status_changed_.first) {
    DEVICE_VLOG(0) << __func__ << ": Device not registered for update";
    return;
  }
  uint8_t label = play_status_changed_.second;

  //last_play_status_.state = state;

  auto response =
      RegisterNotificationResponseBuilder::MakePlaybackStatusBuilder(
          interim, state);
  send_message_cb_.Run(label, false, std::move(response));

  active_labels_.erase(label);
  play_status_changed_ = Notification(false, 0);
}

}  // namespace avrcp
}  // namespace bluetooth
