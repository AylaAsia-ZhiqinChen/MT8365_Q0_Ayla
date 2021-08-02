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

#if defined(MTK_BT_PROPRIETARY_HANDLING) && (MTK_BT_PROPRIETARY_HANDLING == TRUE)
#define LOG_TAG "mtk_btif_ble_scanner"
#include <base/bind.h>
#include <base/threading/thread.h>
#include <errno.h>
#include <hardware/bluetooth.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_set>
#include "device/include/controller.h"

#include "btif_common.h"
#include "btif_util.h"

#include <hardware/bt_gatt.h>

#include "advertise_data_parser.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "btif_config.h"
#include "btif_dm.h"
#include "btif_gatt.h"
#include "btif_gatt_util.h"
#include "btif_storage.h"
#include "osi/include/log.h"

#include "../include/mtk_btif_ble_scanner.h"
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
/*******************************************************************************
**
** Function         btif_av_change_pkt_len_callback
**
** Description     Callback function for vendor specific event that to change packet length of a2dp
**
** Returns          void
**
*******************************************************************************/
void mtk_bta_batch_scan_reports_cb(int client_id, tBTA_STATUS status,
                               uint8_t report_format, uint8_t num_records,
                               std::vector<uint8_t> data){

  // read all, check all recorsd inside data
  uint8_t* pdata = data.data();
  int32_t remain = 0;
  RawAddress remote_addr;
  uint8_t remote_addr_type;
  int device_type;
  int addr_type;
  if ( 0x01 == report_format ){
    // Truncated Mode: Address[0]: 6 octets, Address_Type[0]: 1 octet, Tx_Pwr[0]: 1 octet, RSSI[0] : 1 octet, Timestamp[0]: 2 octets
    for(pdata = data.data(); pdata < (data.data()+data.size()); ){
      remain = (int32_t)(data.size() - ( pdata - data.data() ));
      LOG_DEBUG(LOG_TAG, "batch remain:%d size:%lu diff:%ld", remain, (unsigned long)data.size(), (signed long)(pdata - data.data())  );
      if( remain >= 10 && remain > 0 ){
        STREAM_TO_BDADDR(remote_addr, pdata);
        LOG_DEBUG(LOG_TAG, "batch addr: remote_addr %s", remote_addr.ToString().c_str());
        STREAM_TO_UINT8(remote_addr_type, pdata);
        pdata += 1; // tx_pwr
        pdata += 1; // rssi
        pdata += 2; // timestamp
        LOG_DEBUG(LOG_TAG, "%s: truncated_report add peer %s remote_addr_type=0x%x ", __func__,
            remote_addr.ToString().c_str(), remote_addr_type);
        if (!btif_get_device_type(remote_addr, &device_type) &&
          !btif_get_address_type(remote_addr, &addr_type) ) {
          // add device_type and address_type if not exist
          device_type = BT_DEVICE_TYPE_BLE;
          addr_type = remote_addr_type;
          BTA_DmAddBleDevice(remote_addr, addr_type, device_type);
          LOG_INFO(LOG_TAG, "%s: truncated_report add peer %s remote_addr_type=0x%x ", __func__,
              remote_addr.ToString().c_str(), remote_addr_type);
        }else{
        LOG_INFO(LOG_TAG, "%s: device_type %d addr_type %d", __func__, device_type, addr_type);
        }
      }else{
        LOG_INFO(LOG_TAG, "%s: truncated_report no more size remain %d", __func__, remain);
        break;
      }
    }
  }else if( 0x02 == report_format){
    // Full Mode: Address[0]: 6 octets, Address_Type[0]: 1 octet, Tx_Pwr[0]: 1 octet, RSSI[0]: 1 octet
    // Timestamp[0]: 2 octets, Adv packet_len[0]: 1 octet, Adv_packet[0]: Adv_packet_len octets,
    // Scan_data_resp_len[0]: 1 octet, Scan_data_resp[0]: Scan_data_resp octets
    uint8_t packet_len = 0;
    uint8_t data_resp_len = 0;
    for(pdata = data.data(); pdata < (data.data()+data.size()); ){
      remain = (int32_t)(data.size() - ( pdata - data.data() ));
      LOG_DEBUG(LOG_TAG, "batch remain:%d size:%lu diff:%ld", remain, (unsigned long)data.size(), (signed long)(pdata - data.data())  );
      if( remain >= 12 && remain > 0 ){
        STREAM_TO_BDADDR(remote_addr, pdata);
        LOG_INFO(LOG_TAG, "batch addr: remote_addr %s", remote_addr.ToString().c_str());
        STREAM_TO_UINT8(remote_addr_type, pdata);
        pdata += 1; // tx_pwr
        pdata += 1; // rssi
        pdata += 2; // timestamp
        STREAM_TO_UINT8(packet_len, pdata);
        pdata += packet_len;
        STREAM_TO_UINT8(data_resp_len, pdata);
        pdata += data_resp_len;
        LOG_DEBUG(LOG_TAG, "%s: full_report peer %s remote_addr_type=0x%x  ", __func__,
            remote_addr.ToString().c_str(), remote_addr_type);
        if (!btif_get_device_type(remote_addr, &device_type) &&
          !btif_get_address_type(remote_addr, &addr_type) ) {
          // unknown device_type but want to connect LE transport. Add it as a BLE device first
          device_type = BT_DEVICE_TYPE_BLE;
          addr_type = remote_addr_type;
          BTA_DmAddBleDevice(remote_addr, addr_type, device_type);
          LOG_DEBUG(LOG_TAG, "%s: full_report add peer %s remote_addr_type=0x%x packet_len %d data_resp_len %d", __func__,
              remote_addr.ToString().c_str(), remote_addr_type, packet_len, data_resp_len);
        }else{
          LOG_DEBUG(LOG_TAG, "%s: device_type %d addr_type %d", __func__, device_type, addr_type);
        }
      }else{
        LOG_INFO(LOG_TAG, "%s: full_report no more size remain %d", __func__, remain);
        break;
      }
    }
  }else{
    LOG_INFO(LOG_TAG, "%s: unknown report_format", __func__);
  }
  // report to upper layer
  LOG_INFO(LOG_TAG, "%s: num_records %d data.size() %lu", __func__, num_records, (unsigned long)data.size());

}
#endif



#endif

