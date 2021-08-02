/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO
 * SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER
 * EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN
 * FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES
 * MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR
 * OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED
 * HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK
 * SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE
 * PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef MTK_BTA_AV_API_H
#define MTK_BTA_AV_API_H
#include "../types/raw_address.h"
#include "interop_mtk.h"
#include "avrc_defs.h"

#ifndef AVRCP_1_6_STRING
#define AVRCP_1_6_STRING "avrcp16"
#endif

#ifndef AVRCP_1_5_STRING
#define AVRCP_1_5_STRING "avrcp15"
#endif

#ifndef AVRCP_1_4_STRING
#define AVRCP_1_4_STRING "avrcp14"
#endif

#ifndef AVRCP_1_3_STRING
#define AVRCP_1_3_STRING "avrcp13"
#endif

#ifndef AVRCP_DEFAULT_VERSION
#define AVRCP_DEFAULT_VERSION AVRCP_1_5_STRING
#endif

#if defined(MTK_BT_PROPRIETARY_HANDLING) && (MTK_BT_PROPRIETARY_HANDLING == TRUE)
namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace bta {

struct av_dyna_st {
  std::string peer_addr;
  uint16_t  peer_ver;
  uint16_t  local_ver;
  uint16_t  categories;
};

constexpr int MAX_DYNA_TB_SIZE = 8;

class BtaAvAct {
public:
  BtaAvAct();
  //get instance
  static BtaAvAct* GetInstance();
  /**
  * call to save the peer device's avrcp version
  * @param addr is the peer's address
  * @param ver the peer avrcp version
  */
  void SetPeerVersion(std::string addr, uint16_t ver, uint16_t feature);
  /**
  * call to get local avrcp version
  * @param addr is the peer's address
  * @return the local avrcp for this device
  */
  uint16_t GetLocalVersion(std::string addr);
  /**
  * call to update local avrcp db
  * @param addr is the peer's address
  * @param sdp_handle the avrcp TG handle
  */
  bool UpdateSdpAvrcVersion(std::string addr,uint32_t sdp_handle);
  /**
  * call to find the address has saved in pool
  * @param addr is the peer's address
  */
  struct av_dyna_st* Find(std::string addr);
  /**
  * call to check if we need do avrcp sdp to get this device's version
  * if version had exist, do nothing, otherwise we start the sdp
  * @param addr is the peer's address
  */
  void CheckNeedDoSdp(std::string addr,uint8_t disc);
  /**
  * call to check if this device is in sdp for avrcp version
  * if is in sdp, we will clear this device befor we return
  */
  bool IsInSdp();
private:
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  bool IsInBlacklistOf(const mtk_interop_feature_t black_list_entry, std::string addr);
#endif
  uint16_t default_local_ver;
  struct av_dyna_st av_dyna_tb[MAX_DYNA_TB_SIZE];
  bool in_sdp;
  std::string last_update;
  int last; //last saved device's positoin in g_av_dyna_tb
};
}  // namespace bta
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor
void MtkRcSetPeerVersion(const RawAddress& addr, uint16_t ver, uint16_t feature);
bool MtkRcIsAvrcp13Compatibility(const RawAddress& addr);
bool MtkRcUpdateSdpAvrcVersion(const RawAddress& addr);
void MtkRcCheckNeedDoSdp(const RawAddress& addr, uint8_t disc);
//if in sdp, we return true, and then clear this device from the vector
bool MtkRcIsInSdp();
#else

#define MtkRcSetPeerVersion(addr, ver, feature)
#define MtkRcIsAvrcp13Compatibility(addr) false
#define MtkRcUpdateSdpAvrcVersion(addr ) true
#define MtkRcCheckNeedDoSdp(addr)
#define MtkRcIsInSdp() false

#endif

#endif
