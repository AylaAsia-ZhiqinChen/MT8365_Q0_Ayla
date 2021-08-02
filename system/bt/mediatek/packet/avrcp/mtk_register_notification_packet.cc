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
#include <base/logging.h>

#include "register_notification_packet.h"

namespace bluetooth {
namespace avrcp {

constexpr uint8_t AVRC_PLAYER_VAL_GROUP_REPEAT = 0x04;
constexpr uint8_t  AVRC_PLAYER_VAL_GROUP_SHUFFLE = 0x03;
constexpr uint8_t AVRC_PLAYER_SETTING_LOW_MENU_EXT = 0x80;
constexpr uint8_t AVRC_PLAYER_VAL_ON = 0x02;

/*******************************************************************************
 *
 * Function         avrc_is_valid_player_attrib_value
 *
 * Description      Check if the given attrib value is valid for its attribute
 *
 * Returns          returns true if it is valid
 *
 ******************************************************************************/
bool avrc_is_valid_player_attrib_value(uint8_t attrib, uint8_t value) {
  bool result = false;

  switch (attrib) {
    case BTRC_PLAYER_ATTR_EQUALIZER:
      if ((value > 0) && (value <= AVRC_PLAYER_VAL_ON)) result = true;
      break;

    case BTRC_PLAYER_ATTR_REPEAT:
      if ((value > 0) && (value <= AVRC_PLAYER_VAL_GROUP_REPEAT)) result = true;
      break;

    case BTRC_PLAYER_ATTR_SHUFFLE:
    case BTRC_PLAYER_ATTR_SCAN:
      if ((value > 0) && (value <= AVRC_PLAYER_VAL_GROUP_SHUFFLE))
        result = true;
      break;
    /**M: Unable to match value @{*/
    default:
      LOG(INFO)<<__func__<<" found not matching attrib " <<std::to_string(attrib)<<" value "
        <<std::to_string(value);
      break;
    /**@} */
  }

  if (attrib >= AVRC_PLAYER_SETTING_LOW_MENU_EXT) result = true;
  return result;
}

// avrcp setting
std::unique_ptr<RegisterNotificationResponseBuilder>
RegisterNotificationResponseBuilder::MakeAppSettingChangedBuilder(
    bool interim, BtrcPlayerSettings player_setting) {
  std::unique_ptr<RegisterNotificationResponseBuilder> builder(
      new RegisterNotificationResponseBuilder(interim, Event::PLAYER_APPLICATION_SETTING_CHANGED));

  if (player_setting.num_attr > BTRC_MAX_APP_SETTINGS)
    player_setting.num_attr = BTRC_MAX_APP_SETTINGS;

  builder->player_setting_.num_attr = player_setting.num_attr;
  if (player_setting.num_attr > 0) {
    builder->player_setting_.num_attr = player_setting.num_attr;

    for (int xx = 0; xx < player_setting.num_attr; xx++) {
      if (avrc_is_valid_player_attrib_value(
            player_setting.attr_ids[xx],
            player_setting.attr_values[xx])) {
        builder->player_setting_.attr_ids[xx] = player_setting.attr_ids[xx];
        builder->player_setting_.attr_values[xx] = player_setting.attr_values[xx];
      } else {
        /**M: Unable to match attr_values @{*/
        LOG(INFO) << "bad player app seeting attribute or value";
        break;
        /**@} */
      }
    }
  }

  return builder;
}

}  // namespace avrcp
}  // namespace bluetooth
#endif