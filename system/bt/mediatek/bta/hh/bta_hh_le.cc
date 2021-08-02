/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */


#include "mediatek/bta/hh/bta_hh_int.h"

#include "btm_ble_api_types.h"
#include "stack/btm/btm_ble_int_types.h"


/*******************************************************************************
*
* Function         bta_hh_le_check_conn_params
*
* Description    This  function used to check the whether connection
*                  parameters is vaild
*
* Parameter     bta_hh_dev_cb
*
* Returns          true or false
*
******************************************************************************/
bool bta_hh_le_check_conn_params(tBTA_HH_DEV_CB* p_cb) {

    if (p_cb == NULL)
      return false;

    if (BTM_BLE_ISVALID_PARAM(p_cb->min_conn_int, BTM_BLE_CONN_INT_MIN,
                              BTM_BLE_CONN_INT_MAX) &&
        BTM_BLE_ISVALID_PARAM(p_cb->max_conn_int, BTM_BLE_CONN_INT_MIN,
                              BTM_BLE_CONN_INT_MAX) &&
        BTM_BLE_ISVALID_PARAM(p_cb->supervision_tout, BTM_BLE_CONN_SUP_TOUT_MIN,
                              BTM_BLE_CONN_SUP_TOUT_MAX) &&
        (p_cb->slave_latency <= BTM_BLE_CONN_LATENCY_MAX) &&
        (p_cb->min_conn_int != BTM_BLE_CONN_PARAM_UNDEF) &&
        (p_cb->max_conn_int != BTM_BLE_CONN_PARAM_UNDEF))

      return true;
    else
      return false;
}

/*******************************************************************************
*
* Function         bta_hh_le_save_conn_params
*
* Description    This  function used to save the connection parameters
*
*
* Parameter     bta_hh_dev_cb and connection parameters
*
* Returns          true or false
*
******************************************************************************/
void bta_hh_le_save_conn_params(tBTA_HH_DEV_CB* p_dev_cb,
                                uint16_t min_int, uint16_t max_int,
                                uint16_t latency, uint16_t tout) {

    if (p_dev_cb == NULL)
      return;

    p_dev_cb->min_conn_int = min_int;
    p_dev_cb->max_conn_int = max_int;
    p_dev_cb->slave_latency = latency;
    p_dev_cb->supervision_tout = tout;

    if (min_int == BTM_BLE_CONN_PARAM_UNDEF || max_int == BTM_BLE_CONN_PARAM_UNDEF) {
      if (min_int != BTM_BLE_CONN_PARAM_UNDEF) p_dev_cb->max_conn_int = min_int;

      if (max_int != BTM_BLE_CONN_PARAM_UNDEF) p_dev_cb->min_conn_int = max_int;
    }

    if (latency == BTM_BLE_CONN_PARAM_UNDEF)
      p_dev_cb->slave_latency = BTM_BLE_CONN_SLAVE_LATENCY_DEF;
}

