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

#pragma once

#include <set>
#include <string>

#include <base/callback_forward.h>

#include "avrcp_common.h"
#include "raw_address.h"

namespace bluetooth {
namespace avrcp {

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
constexpr uint32_t BTRC_MAX_ATTR_STR_LEN = (1u << 16);
constexpr uint8_t BTRC_MAX_APP_SETTINGS = 8;
constexpr uint8_t BTRC_MAX_APP_ATTR_SIZE = 16;

enum BtrcStatus : uint8_t {
  BTRC_STS_BAD_CMD = 0x00,       /* Invalid command */
  BTRC_STS_BAD_PARAM = 0x01,     /* Invalid parameter */
  BTRC_STS_NOT_FOUND = 0x02,     /* Specified parameter is wrong or not found */
  BTRC_STS_INTERNAL_ERR = 0x03,  /* Internal Error */
  BTRC_STS_NO_ERROR = 0x04,      /* Operation Success */
  BTRC_STS_UID_CHANGED = 0x05,   /* UIDs changed */
  BTRC_STS_RESERVED = 0x06,      /* Reserved */
  BTRC_STS_INV_DIRN = 0x07,      /* Invalid direction */
  BTRC_STS_INV_DIRECTORY = 0x08, /* Invalid directory */
  BTRC_STS_INV_ITEM = 0x09,      /* Invalid Item */
  BTRC_STS_INV_SCOPE = 0x0a,     /* Invalid scope */
  BTRC_STS_INV_RANGE = 0x0b,     /* Invalid range */
  BTRC_STS_DIRECTORY = 0x0c,     /* UID is a directory */
  BTRC_STS_MEDIA_IN_USE = 0x0d,  /* Media in use */
  BTRC_STS_PLAY_LIST_FULL = 0x0e, /* Playing list full */
  BTRC_STS_SRCH_NOT_SPRTD = 0x0f, /* Search not supported */
  BTRC_STS_SRCH_IN_PROG = 0x10,   /* Search in progress */
  BTRC_STS_INV_PLAYER = 0x11,     /* Invalid player */
  BTRC_STS_PLAY_NOT_BROW = 0x12,  /* Player not browsable */
  BTRC_STS_PLAY_NOT_ADDR = 0x13,  /* Player not addressed */
  BTRC_STS_INV_RESULTS = 0x14,    /* Invalid results */
  BTRC_STS_NO_AVBL_PLAY = 0x15,   /* No available players */
  BTRC_STS_ADDR_PLAY_CHGD = 0x16, /* Addressed player changed */
};

enum BtrcPlayerAttr : uint8_t {
  BTRC_PLAYER_ATTR_EQUALIZER = 0x01,
  BTRC_PLAYER_ATTR_REPEAT = 0x02,
  BTRC_PLAYER_ATTR_SHUFFLE = 0x03,
  BTRC_PLAYER_ATTR_SCAN = 0x04,
};

struct BtrcPlayerSettings {
  uint8_t num_attr;
  uint8_t attr_ids[BTRC_MAX_APP_SETTINGS];
  uint8_t attr_values[BTRC_MAX_APP_SETTINGS];
};

struct BtrcPlayerSettingText {
  uint8_t id; /* can be attr_id or value_id */
  uint8_t text[BTRC_MAX_ATTR_STR_LEN];
};
#endif

struct SongInfo {
  std::string media_id;  // This gets converted to a UID in the native service
  std::set<AttributeEntry> attributes;
};

enum PlayState : uint8_t {
  STOPPED = 0x00,
  PLAYING,
  PAUSED,
  FWD_SEEK,
  REV_SEEK,
  ERROR = 0xFF,
};

struct PlayStatus {
  uint32_t position;
  uint32_t duration;
  PlayState state;
};

struct MediaPlayerInfo {
  uint16_t id;
  std::string name;
  bool browsing_supported;
};

struct FolderInfo {
  std::string media_id;
  bool is_playable;
  std::string name;
};

// TODO (apanicke): Convert this to a union
struct ListItem {
  enum : uint8_t {
    FOLDER,
    SONG,
  } type;

  FolderInfo folder;
  SongInfo song;
};

class MediaCallbacks {
 public:
  virtual void SendMediaUpdate(bool track_changed, bool play_state,
                               bool queue) = 0;
  virtual void SendFolderUpdate(bool available_players, bool addressed_players,
                                bool uids_changed);
  virtual void SendActiveDeviceChanged(const RawAddress& address);
#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
  virtual void SendAppSettingUpdate(bool setting_changed);
#endif
  virtual ~MediaCallbacks() = default;
};

// The classes below are used by the JNI and are loaded dynamically with the
// Bluetooth library. All classes must be pure virtual otherwise a compiler
// error occurs when trying to link the function implementation.

// MediaInterface defines the class that the AVRCP Service uses in order
// communicate with the media layer. The media layer will define its own
// implementation of this object and register it with the service using
// Avrcp::ServiceInterface::Init(). At this point the AVRCP Service will
// call RegisterUpdateCallbacks() to provide an handle to use to send
// notifications about changes in the Media Interface.
//
// NOTES: The current implementation has the native service handle all the
// thread switching. It will call the interface functions on the btif/jni
// thread and the callback will post its results to the bta thread.
// In the future the interface the JNI registered with the
// service should post all its tasks to the JNI thread itself so that the native
// service isn't aware of the thread the interface functions need to be called
// on. It can then supply callbacks that post results to the correct thread
// allowing the threading model to be totally encapsulated and allow correct
// behavior in case the threading model changes on either side.
class MediaInterface {
 public:
  virtual void SendKeyEvent(uint8_t key, KeyState state) = 0;

  using SongInfoCallback = base::Callback<void(SongInfo)>;
  virtual void GetSongInfo(SongInfoCallback info_cb) = 0;

  using PlayStatusCallback = base::Callback<void(PlayStatus)>;
  virtual void GetPlayStatus(PlayStatusCallback status_cb) = 0;

  // Contains the current queue and the media ID of the currently playing item
  // in the queue
  using NowPlayingCallback =
      base::Callback<void(std::string, std::vector<SongInfo>)>;
  virtual void GetNowPlayingList(NowPlayingCallback now_playing_cb) = 0;

  // TODO (apanicke): Use a map with the ID as the key instead of vector
  // in follow up cleanup patches. This allows simplification of the
  // MediaPlayerInfo object
  using MediaListCallback =
      base::Callback<void(uint16_t curr_player, std::vector<MediaPlayerInfo>)>;
  virtual void GetMediaPlayerList(MediaListCallback list_cb) = 0;

  using FolderItemsCallback = base::Callback<void(std::vector<ListItem>)>;
  virtual void GetFolderItems(uint16_t player_id, std::string media_id,
                              FolderItemsCallback folder_cb) = 0;

  using SetBrowsedPlayerCallback = base::Callback<void(
      bool success, std::string root_id, uint32_t num_items)>;
  virtual void SetBrowsedPlayer(uint16_t player_id,
                                SetBrowsedPlayerCallback browse_cb) = 0;

  virtual void PlayItem(uint16_t player_id, bool now_playing,
                        std::string media_id) = 0;

  virtual void SetActiveDevice(const RawAddress& address) = 0;

  virtual void RegisterUpdateCallback(MediaCallbacks* callback) = 0;

  virtual void UnregisterUpdateCallback(MediaCallbacks* callback) = 0;

#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
  using GetSettingChangeCallback =
      base::Callback<void(BtrcPlayerSettings vals)>;
  virtual void GetAppSettingChange(GetSettingChangeCallback get_values_cb) = 0;

  using ListAttributesCallback =
      base::Callback<void(std::vector<BtrcPlayerAttr>)>;
  virtual void ListAppSettingAttrs(
                              ListAttributesCallback list_attributes_cb) = 0;

  using ListValuesCallback =
      base::Callback<void(std::vector<uint8_t>)>;
  virtual void ListAppSettingValues(uint8_t attr_id,
                              ListValuesCallback list_values_cb) = 0;

  using GetValuesCallback =
      base::Callback<void(BtrcPlayerSettings vals)>;
  virtual void GetAppSettingValues(std::vector<BtrcPlayerAttr>,
                              GetValuesCallback get_values_cb) = 0;

  using SetValuesCallback =
      base::Callback<void(BtrcStatus rsp_status)>;
  virtual void SetAppSettingValues(BtrcPlayerSettings p_vals,
                              SetValuesCallback set_values_cb) = 0;

  using GetAttrsTxtCallback =
      base::Callback<void(std::vector<BtrcPlayerSettingText>)>;
  virtual void GetAppSettingAttrsText(std::vector<BtrcPlayerAttr>,
                              GetAttrsTxtCallback get_attrs_txt_cb) = 0;

  using GetValuesTxtCallback =
      base::Callback<void(std::vector<BtrcPlayerSettingText>)>;
  virtual void GetAppSettingValuesText(uint8_t attr_id,
                              std::vector<uint8_t>, GetValuesTxtCallback get_vals_txt_cb) = 0;
#endif

  MediaInterface() = default;
  virtual ~MediaInterface() = default;
};

class VolumeInterface {
 public:
  // TODO (apanicke): Investigate the best value type for volume. Right now it
  // is a value from 0-127 because thats what AVRCP uses.
  using VolumeChangedCb = base::Callback<void(int8_t volume)>;

  // Indicate that a device has been connected that does not support absolute
  // volume.
  virtual void DeviceConnected(const RawAddress& bdaddr) = 0;

  // Indicate that a device has been connected that does support absolute
  // volume. The callback will be immediately called with the current volume
  // which will be sent to the device.
  virtual void DeviceConnected(const RawAddress& bdaddr,
                               VolumeChangedCb cb) = 0;

  // Indicate that a device has been disconnected from AVRCP. Will unregister
  // any callbacks if absolute volume is supported.
  virtual void DeviceDisconnected(const RawAddress& bdaddr) = 0;

  virtual void SetVolume(int8_t volume) = 0;

  virtual ~VolumeInterface() = default;
};

class ServiceInterface {
 public:
  // mediaInterface can not be null. If volumeInterface is null then Absolute
  // Volume is disabled.
  virtual void Init(MediaInterface* mediaInterface,
                    VolumeInterface* volumeInterface) = 0;
  virtual bool ConnectDevice(const RawAddress& bdaddr) = 0;
  virtual bool DisconnectDevice(const RawAddress& bdaddr) = 0;
  virtual bool Cleanup() = 0;

 protected:
  virtual ~ServiceInterface() = default;
};

}  // namespace avrcp
}  // namespace bluetooth