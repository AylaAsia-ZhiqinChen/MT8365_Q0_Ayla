/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
* *
* * MediaTek Inc. (C) 2016. All rights reserved.
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

#define LOG_TAG "mtk_bta_av_act"
#include "osi/include/alarm.h"
#include "osi/include/log.h"
#include "osi/include/osi.h"
#include "sdp_api.h"
#include "../include/mtk_bta_av_act.h"
#include "../../osi/include/properties.h"
#include "../bta/ar/bta_ar_int.h"
#include "../bta/av/bta_av_int.h"

#ifndef AVRCP_VERSION_PROPERTY
#define AVRCP_VERSION_PROPERTY "persist.bluetooth.avrcpversion"
#endif

#ifndef BTA_AV_RC_SUPF_TG
#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
#define BTA_AV_RC_SUPF_TG              \
  (AVRC_SUPF_TG_CAT1 | AVRC_SUPF_TG_MULTI_PLAYER | \
   AVRC_SUPF_TG_BROWSE | AVRC_SUPF_TG_APP_SETTINGS)
#else
#define BTA_AV_RC_SUPF_TG              \
  (AVRC_SUPF_TG_CAT1 | AVRC_SUPF_TG_MULTI_PLAYER | \
   AVRC_SUPF_TG_BROWSE) /* TODO: | AVRC_SUPF_TG_APP_SETTINGS) */
#endif
#endif

#if defined(MTK_BT_PROPRIETARY_HANDLING) && (MTK_BT_PROPRIETARY_HANDLING == TRUE)
extern tBTA_AR_CB bta_ar_cb;
namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace bta {

BtaAvAct* g_BtaAvAct_inst = NULL;
BtaAvAct::BtaAvAct() {
  for (int i = 0; i < MAX_DYNA_TB_SIZE; i++) {
    av_dyna_tb[i].categories = 0;
    av_dyna_tb[i].local_ver = 0;
    av_dyna_tb[i].peer_addr = "";
    av_dyna_tb[i].peer_ver = 0;
  }
  last = -1;
  default_local_ver = AVRC_REV_1_5;
  last_update = "";
  //get the default local TG version
  char avrcp_version[PROPERTY_VALUE_MAX] = { 0 };
  osi_property_get(AVRCP_VERSION_PROPERTY, avrcp_version, AVRCP_DEFAULT_VERSION);
  if (!strncmp(AVRCP_1_3_STRING, avrcp_version, sizeof(AVRCP_1_3_STRING))) {
    default_local_ver = AVRC_REV_1_3;
  }
  else if (!strncmp(AVRCP_1_6_STRING, avrcp_version, sizeof(AVRCP_1_6_STRING))) {
    default_local_ver = AVRC_REV_1_6;
  }
  else if (!strncmp(AVRCP_1_5_STRING, avrcp_version, sizeof(AVRCP_1_5_STRING))) {
    default_local_ver = AVRC_REV_1_5;
  }
  else {
    default_local_ver = AVRC_REV_1_4;
  }
  in_sdp = false;
}

struct av_dyna_st* BtaAvAct::Find(std::string addr) {
  int pos = 0;
  if (last < 0) return NULL;
  for (int i = 0; i < MAX_DYNA_TB_SIZE; i++) {
    pos = (last - i + MAX_DYNA_TB_SIZE - 1) % (MAX_DYNA_TB_SIZE-1);
    if (av_dyna_tb[pos].peer_addr.compare(addr) == 0) {
      return &av_dyna_tb[pos];
    }
  }
  return NULL;
}

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
bool BtaAvAct::IsInBlacklistOf(const mtk_interop_feature_t black_list_entry, std::string addr) {
  RawAddress raw_addr;
  RawAddress::FromString(addr, raw_addr);
  return interop_mtk_match_addr(black_list_entry, &raw_addr)
    || interop_mtk_match_name(black_list_entry, &raw_addr);
}
#endif

BtaAvAct* BtaAvAct::GetInstance() {
  if (!g_BtaAvAct_inst) {
    g_BtaAvAct_inst = new BtaAvAct();
  }
  return g_BtaAvAct_inst;
}

void BtaAvAct::SetPeerVersion(std::string addr, uint16_t ver, uint16_t feature) {
  struct av_dyna_st* ad = Find(addr);
  if (ad) {
    APPL_TRACE_WARNING("[BtaAvAct]SetPeerVersion: the addr(%s) had saved already", addr.c_str());
    return;
  }
  if (!ver) {
    //must be error, peer version can not be zero
    APPL_TRACE_WARNING("[BtaAvAct]SetPeerVersion: the peer verion is 0.");
    return;
  }
  last += 1;
  last = (last + MAX_DYNA_TB_SIZE - 1) % (MAX_DYNA_TB_SIZE - 1);
  ad = &av_dyna_tb[last];
  ad->peer_addr = addr;
  ad->peer_ver = ver;
  ad->categories = BTA_AV_RC_SUPF_TG;
  ad->local_ver = default_local_ver;
  if ((ad->local_ver >AVRC_REV_1_3) && (ad->local_ver > ad->peer_ver)) {
    //if our avrcp version big than peer, we adjust it as follows:
    //local > 1.4 && peer < local => set local as 1.4
    //if peer < 1.3 => set local to 1.3
    //other => local = peer
    ad->local_ver = ad->peer_ver < AVRC_REV_1_4?AVRC_REV_1_4 : ad->peer_ver;
    if (ad->peer_ver < AVRC_REV_1_3)
      ad->local_ver = AVRC_REV_1_3;
  }

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
  if (IsInBlacklistOf(INTEROP_MTK_AVRCP13_USE, addr)) {
    ad->local_ver = AVRC_REV_1_3;
  }
  else if (IsInBlacklistOf(INTEROP_MTK_AVRCP14_USE, addr)) {
    ad->local_ver = AVRC_REV_1_4;
  }
#endif
  if (ad->local_ver == AVRC_REV_1_3) {
    ad->categories = AVRC_SUPF_TG_CAT1
#if defined(MTK_AVRCP_APP_SETTINGS) && (MTK_AVRCP_APP_SETTINGS == TRUE)
      | AVRC_SUPF_TG_APP_SETTINGS
#endif
      ;
  }
  APPL_TRACE_DEBUG("[BtaAvAct]SetPeerVersion: [%d](addr:%s,peer_ver:%04x,local_ver:%04x,"
      "categories:%04x)",last,addr.c_str(),ad->peer_ver,ad->local_ver,ad->categories);
}

uint16_t BtaAvAct::GetLocalVersion(std::string addr) {
  struct av_dyna_st* ad = Find(addr);
  uint16_t local_ver = AVRC_REV_1_4;
  if (!ad) {
    APPL_TRACE_WARNING("[BtaAvAct]GetLocalVersion: not find addr %s.", addr.c_str());
  }
  else {
    local_ver = ad->local_ver;
  }
  APPL_TRACE_DEBUG("[BtaAvAct]GetLocalVersion: return local version %04x", local_ver);
  return local_ver;
}

bool BtaAvAct::UpdateSdpAvrcVersion(std::string addr, uint32_t sdp_handle) {
  struct av_dyna_st* ad = Find(addr);
  if (sdp_handle == 0) {
    //avrcp tg sdp handle not create yeat, return
    APPL_TRACE_WARNING("[BtaAvAct]UpdateSdpAvrcVersion: sdp not create %s.",addr.c_str());
    return false;
  }
  if (!ad) {
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
    if (IsInBlacklistOf(INTEROP_MTK_AVRCP13_USE, addr) ||
      IsInBlacklistOf(INTEROP_MTK_AVRCP14_USE, addr)) {
      //this will be cover the case, when carkit need DUT used specify version
      //but do not do sdp after pair, so we add this device in pool for the first
      //sdp process
      APPL_TRACE_WARNING("[BtaAvAct]UpdateSdpAvrcVersion: not find addr %s; but in blacklist",
                 addr.c_str());
      SetPeerVersion(addr, default_local_ver, BTA_AV_RC_SUPF_TG);
      ad = Find(addr);
    } else
#endif
    {
      APPL_TRACE_WARNING("[BtaAvAct]UpdateSdpAvrcVersion: not find addr %s.", addr.c_str());
      return false;
    }
  }
  if (last_update.compare(addr) == 0) {
    //do not need update
    return true;
  }
  last_update = addr;
  /* update profile descriptor list*/
  bool result = SDP_AddProfileDescriptorList(
        sdp_handle, UUID_SERVCLASS_AV_REMOTE_CONTROL, ad->local_ver);
  /* add supported categories */
  if (ad->categories) {
    uint8_t temp[8] = { 0 };
    uint8_t *p = temp;
    UINT16_TO_BE_STREAM(p, ad->categories);
    result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE,
                               (uint32_t)2, (uint8_t*)temp);
  }
  APPL_TRACE_DEBUG("[BtaAvAct]UpdateSdpAvrcVersion result : %x", result);
  return result;
}

void BtaAvAct::CheckNeedDoSdp(std::string addr, uint8_t disc) {
  if (!Find(addr)) {
    APPL_TRACE_WARNING("[BtaAvAct]CheckNeedDoSdp start sdp %s,%d",
      addr.c_str(), disc);
    in_sdp = true;
    bta_av_rc_disc(disc);
  }
  else {
    APPL_TRACE_WARNING("[BtaAvAct]CheckNeedDoSdp no need %s,%d",
      addr.c_str(), disc);
  }
}

bool BtaAvAct::IsInSdp() {
  if (in_sdp) {
    in_sdp = false;
    APPL_TRACE_WARNING("[BtaAvAct]IsInSdp in");
    return true;
  }
  APPL_TRACE_WARNING("[BtaAvAct]IsInSdp not in");
  return false;
}

}  // namespace bta
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

using namespace vendor::mediatek::bluetooth::bta;
void MtkRcSetPeerVersion(const RawAddress& addr, uint16_t ver, uint16_t feature) {
  BtaAvAct *baa = BtaAvAct::GetInstance();
  if (baa) {
    baa->SetPeerVersion(addr.ToString(), ver, feature);
  }
}

bool MtkRcIsAvrcp13Compatibility(const RawAddress& addr) {
  BtaAvAct *baa = BtaAvAct::GetInstance();
  if (baa) {
    uint16_t ver = baa->GetLocalVersion(addr.ToString());
    return ver <= AVRC_REV_1_3;
  }
  return false;
}

bool MtkRcUpdateSdpAvrcVersion(const RawAddress& addr) {
  BtaAvAct *baa = BtaAvAct::GetInstance();
  if (baa) {
    if (!bta_ar_cb.sdp_tg_handle) {
      APPL_TRACE_WARNING("[BtaAvAct]MtkRcUpdateSdpAvrcVersion: sdp_tg_handle not set.");
      return false;
    }
    return baa->UpdateSdpAvrcVersion(addr.ToString(), bta_ar_cb.sdp_tg_handle);
  }
  return false;
}

void MtkRcCheckNeedDoSdp(const RawAddress& addr, uint8_t disc) {
  BtaAvAct *baa = BtaAvAct::GetInstance();
  if (baa) {
    baa->CheckNeedDoSdp(addr.ToString(), disc);
  }
}

bool MtkRcIsInSdp() {
  BtaAvAct *baa = BtaAvAct::GetInstance();
  if (baa) {
    return baa->IsInSdp();
  }
  return false;
}
#endif
