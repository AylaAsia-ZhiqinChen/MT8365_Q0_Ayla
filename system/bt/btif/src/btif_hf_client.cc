/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *  Filename:      btif_hf_client.c
 *
 *  Description:   Handsfree Profile (HF role) Bluetooth Interface
 *
 *  Notes:
 *  a) Lifecycle of a control block
 *  Control block handles the lifecycle for a particular remote device's
 *  connection. The connection can go via the classic phases but more
 *  importantly there's only two messages from BTA that affect this.
 *  BTA_HF_CLIENT_OPEN_EVT and BTA_HF_CLIENT_CLOSE_EVT. Since the API between
 *  BTIF and BTA is controlled entirely by handles it's important to know where
 *  the handles are created and destroyed. Handles can be created at two
 *  locations:
 *  -- While connect() is called from BTIF. This is an outgoing connection
 *  -- While accepting an incoming connection (see BTA_HF_CLIENT_OPEN_EVT
 *  handling).
 *
 *  The destruction or rather reuse of handles can be done when
 *  BTA_HF_CLIENT_CLOSE_EVT is called. Refer to the event handling for details
 *  of this.
 *
 ******************************************************************************/

#define LOG_TAG "bt_btif_hfc"

#include <stdlib.h>
#include <string.h>

#include <hardware/bluetooth.h>
#include <hardware/bt_hf_client.h>

#include "bt_utils.h"
#include "bta_hf_client_api.h"
#include "btif_common.h"
#include "btif_profile_queue.h"
#include "btif_util.h"
#include "osi/include/osi.h"
#include "osi/include/properties.h"

/*******************************************************************************
 *  Constants & Macros
 ******************************************************************************/

#ifndef BTIF_HF_CLIENT_SERVICE_NAME
#define BTIF_HF_CLIENT_SERVICE_NAME ("Handsfree")
#endif

#ifndef BTIF_HF_CLIENT_SECURITY
#define BTIF_HF_CLIENT_SECURITY (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTIF_HF_CLIENT_FEATURES
#define BTIF_HF_CLIENT_FEATURES                                                \
  (BTA_HF_CLIENT_FEAT_ECNR | BTA_HF_CLIENT_FEAT_3WAY |                         \
   BTA_HF_CLIENT_FEAT_CLI | BTA_HF_CLIENT_FEAT_VREC | BTA_HF_CLIENT_FEAT_VOL | \
   BTA_HF_CLIENT_FEAT_ECS | BTA_HF_CLIENT_FEAT_ECC | BTA_HF_CLIENT_FEAT_CODEC)
#endif

/*******************************************************************************
 *  Local type definitions
 ******************************************************************************/
/* BTIF-HF control block to map bdaddr to BTA handle */
typedef struct {
  uint16_t handle;                       // Handle obtained frm the BTA
  RawAddress peer_bda;                   // Device corresponding to handle
  bthf_client_connection_state_t state;  // State of current connection
  tBTA_HF_CLIENT_PEER_FEAT peer_feat;    // HF features
  tBTA_HF_CLIENT_CHLD_FEAT chld_feat;    // AT+CHLD=<> command features
} btif_hf_client_cb_t;

/* Max devices supported by BTIF (useful to match the value in BTA) */
#define HF_CLIENT_MAX_DEVICES 1
typedef struct {
  btif_hf_client_cb_t cb[HF_CLIENT_MAX_DEVICES];
} btif_hf_client_cb_arr_t;

/******************************************************************************
 * Local function declarations
 ******************************************************************************/
btif_hf_client_cb_t* btif_hf_client_get_cb_by_handle(uint16_t handle);
btif_hf_client_cb_t* btif_hf_client_get_cb_by_bda(const RawAddress& addr);
bool is_connected(const btif_hf_client_cb_t* cb);

/*******************************************************************************
 *  Static variables
 ******************************************************************************/
static bthf_client_callbacks_t* bt_hf_client_callbacks = NULL;

char btif_hf_client_version[PROPERTY_VALUE_MAX];

static const char* dump_hf_client_conn_state(uint16_t event) {
  switch (event) {
    CASE_RETURN_STR(BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED)
    CASE_RETURN_STR(BTHF_CLIENT_CONNECTION_STATE_CONNECTING)
    CASE_RETURN_STR(BTHF_CLIENT_CONNECTION_STATE_CONNECTED)
    CASE_RETURN_STR(BTHF_CLIENT_CONNECTION_STATE_SLC_CONNECTED)
    CASE_RETURN_STR(BTHF_CLIENT_CONNECTION_STATE_DISCONNECTING)
    default:
      return "UNKNOWN MSG ID";
  }
}

#define CHECK_BTHF_CLIENT_INIT()                                        \
  do {                                                                  \
    if (bt_hf_client_callbacks == NULL) {                               \
      BTIF_TRACE_WARNING("BTHF CLIENT: %s: not initialized", __func__); \
      return BT_STATUS_NOT_READY;                                       \
    } else {                                                            \
      BTIF_TRACE_EVENT("BTHF CLIENT: %s", __func__);                    \
    }                                                                   \
  } while (0)

#define CHECK_BTHF_CLIENT_SLC_CONNECTED(cb)                                  \
  do {                                                                       \
    if (bt_hf_client_callbacks == NULL) {                                    \
      BTIF_TRACE_WARNING("BTHF CLIENT: %s: not initialized", __func__);      \
      return BT_STATUS_NOT_READY;                                            \
    } else if ((cb)->state != BTHF_CLIENT_CONNECTION_STATE_SLC_CONNECTED) {  \
      BTIF_TRACE_WARNING("BTHF CLIENT: %s: SLC connection not up. state=%s", \
                         __func__, dump_hf_client_conn_state((cb)->state));  \
      return BT_STATUS_NOT_READY;                                            \
    } else {                                                                 \
      BTIF_TRACE_EVENT("BTHF CLIENT: %s", __func__);                         \
    }                                                                        \
  } while (0)

static btif_hf_client_cb_arr_t btif_hf_client_cb_arr;

/*******************************************************************************
 *  Static functions
 ******************************************************************************/

/*******************************************************************************
 *
 * Function        btif_in_hf_client_generic_evt
 *
 * Description     Processes generic events to be sent to JNI that are not
 *                 triggered from the BTA.
 *                 Always runs in BTIF context
 *
 * Returns          void
 *
 ******************************************************************************/
static void btif_in_hf_client_generic_evt(uint16_t event, char* p_param) {
  BTIF_TRACE_DEBUG("%s", __func__);
  RawAddress* bd_addr = (RawAddress*)p_param;
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) {
    BTIF_TRACE_ERROR("%s: failed to find block for bda", __func__);
  }

  BTIF_TRACE_EVENT("%s: event=%d", __func__, event);
  switch (event) {
    case BTIF_HF_CLIENT_CB_AUDIO_CONNECTING: {
      HAL_CBACK(bt_hf_client_callbacks, audio_state_cb, &cb->peer_bda,
                (bthf_client_audio_state_t)BTHF_CLIENT_AUDIO_STATE_CONNECTING);
    } break;
    default: {
      BTIF_TRACE_WARNING("%s: : Unknown event 0x%x", __func__, event);
    } break;
  }
}

/*******************************************************************************
 *  Functions
 ******************************************************************************/
bool is_connected(const btif_hf_client_cb_t* cb) {
  if ((cb->state == BTHF_CLIENT_CONNECTION_STATE_CONNECTED) ||
      (cb->state == BTHF_CLIENT_CONNECTION_STATE_SLC_CONNECTED))
    return true;

  BTIF_TRACE_ERROR("%s: not connected!", __func__);
  return false;
}

/*******************************************************************************
 *
 * Function        btif_hf_client_get_cb_by_handle
 *
 * Description     Get control block by handle
 *
 * Returns         btif_hf_client_cb_t pointer if available NULL otherwise
 *
 ******************************************************************************/
btif_hf_client_cb_t* btif_hf_client_get_cb_by_handle(uint16_t handle) {
  BTIF_TRACE_DEBUG("%s: cb by handle %d", __func__, handle);
  for (int i = 0; i < HF_CLIENT_MAX_DEVICES; i++) {
    // Block is valid only if it is allocated i.e. state is not DISCONNECTED
    if (btif_hf_client_cb_arr.cb[i].state !=
            BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED &&
        btif_hf_client_cb_arr.cb[i].handle == handle) {
      return &btif_hf_client_cb_arr.cb[i];
    }
  }
  BTIF_TRACE_ERROR("%s: could not find block for handle %d", __func__, handle);
  return NULL;
}

/*******************************************************************************
 *
 * Function        btif_hf_client_get_cb_by_bda
 *
 * Description     Get control block by bda
 *
 * Returns         btif_hf_client_cb_t pointer if available NULL otherwise
 *
 ******************************************************************************/
btif_hf_client_cb_t* btif_hf_client_get_cb_by_bda(const RawAddress& bd_addr) {
  VLOG(1) << __func__ << " incoming addr " << bd_addr;

  for (int i = 0; i < HF_CLIENT_MAX_DEVICES; i++) {
    // Block is valid only if it is allocated i.e. state is not DISCONNECTED
    if (btif_hf_client_cb_arr.cb[i].state !=
            BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED &&
        btif_hf_client_cb_arr.cb[i].peer_bda == bd_addr) {
      return &btif_hf_client_cb_arr.cb[i];
    }
  }
  BTIF_TRACE_ERROR("%s: could not find block for bdaddr", __func__);
  return NULL;
}

/*******************************************************************************
 *
 * Function        btif_hf_client_allocate_cb
 *
 * Description     Get control block by bda
 *
 * Returns         btif_hf_client_cb_t pointer if available NULL otherwise
 *
 ******************************************************************************/
btif_hf_client_cb_t* btif_hf_client_allocate_cb() {
  for (int i = 0; i < HF_CLIENT_MAX_DEVICES; i++) {
    btif_hf_client_cb_t* cb = &btif_hf_client_cb_arr.cb[i];
    if (cb->state == BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED) {
      return cb;
    }
  }
  BTIF_TRACE_ERROR("%s: unable to allocate control block", __func__);
  return NULL;
}

#if defined(MTK_PTS_HF_TEST) && (MTK_PTS_HF_TEST == TRUE)

#include "osi/include/thread.h"
#include "osi/include/properties.h"
static bt_status_t handle_call_action(const RawAddress* bd_addr, bthf_client_call_action_t action, int idx);
static bt_status_t start_voice_recognition(const RawAddress* bd_addr);
static bt_status_t stop_voice_recognition(const RawAddress* bd_addr);
static bt_status_t request_last_voice_tag_number(const RawAddress* bd_addr);
static bt_status_t connect_audio(const RawAddress* bd_addr);
static bt_status_t disconnect_audio(const RawAddress* bd_addr);
static bt_status_t volume_control(const RawAddress* bd_addr, bthf_client_volume_type_t type, int volume);
static bt_status_t dial_memory(const RawAddress* bd_addr, int location);
static bt_status_t dial(UNUSED_ATTR const RawAddress* bd_addr,const char* number);
static bt_status_t send_dtmf(const RawAddress* bd_addr, char code);
static bt_status_t connect(RawAddress* bd_addr);
static bt_status_t disconnect(const RawAddress* bd_addr);

static thread_t *pts_hfclient_test_thread;

static void btif_hfclient_pts_test_thread(UNUSED_ATTR void *context) {
  char value[PROPERTY_VALUE_MAX] = {0};
  RawAddress rc_addr;

  /* properties
     [1]Use "setprop bluetooth.pts.hfclient.chld on" to trigger hfclient chld
         -- HFP/HF/TWC/BV-01-I/HFP/HF/TWC/BV-03-I/HFP/HF/TWC/BV-04-I
         -- HFP/HF/TWC/BV-05-I/HFP/HF/TWC/BV-06-I
  */

  while(1){
    if ((osi_property_get("bluetooth.pts.hfclient.bd_addr", value, "0"))
      && (strcmp(value, "0"))) {
      int addr[6], i = 0;
      BTIF_TRACE_DEBUG("[pts] get bd_addr");

      sscanf(value, "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

      for(i = 0; i < 6; i++) {
        rc_addr.address[i] = addr[i];
      }
      BTIF_TRACE_DEBUG("%s: rc_addr: %s", __func__,rc_addr.ToString().c_str());
      osi_property_set("bluetooth.pts.hfclient.bd_addr", "0");
    } else if ((osi_property_get("bluetooth.pts.hfclient.callaction", value, "0"))
            && (strcmp(value, "0"))){
      int action[2];
      sscanf(value, "%d:%d", &action[0], &action[1]);
      BTIF_TRACE_EVENT("[pts] run call action[0]:%d, action[1]:%d", action[0], action[1]);
      if (action[0] == 0) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_0, 0);
      } else if (action[0] == 1) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_1, 0);
      } else if (action[0] == 2) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_2, 0);
      } else if (action[0] == 3) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_3, 0);
      } else if (action[0] == 4) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_4, 0);
      } else if (action[0] == 5) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_1x, action[1]);
      } else if (action[0] == 6) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHLD_2x, action[1]);
      } else if (action[0] == 7) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_ATA, 0);
      } else if (action[0] == 8) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_CHUP, 0);
      } else if (action[0] == 9) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_BTRH_0, 0);
      } else if (action[0] == 10) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_BTRH_1, 0);
      } else if (action[0] == 11) {
        handle_call_action(&rc_addr, BTHF_CLIENT_CALL_ACTION_BTRH_2, 0);
      }
      osi_property_set("bluetooth.pts.hfclient.callaction", "0");
    } else if((osi_property_get("bluetooth.pts.hfclient.vr", value, "0"))
                && (strcmp(value, "0"))){
      int option;
      sscanf(value, "%d", &option);
      BTIF_TRACE_EVENT("[pts] run AT+BVRA option:%u", option);

      if (option == 1) {
        start_voice_recognition(&rc_addr);
      } else if (option == 2) {
        stop_voice_recognition(&rc_addr);
      } else if (option == 3) {
        request_last_voice_tag_number(&rc_addr);
      }
      osi_property_set("bluetooth.pts.hfclient.vr", "0");
    } else if((osi_property_get("bluetooth.pts.hfclient.dtmf", value, "0"))
                && (strcmp(value, "0"))){
      BTIF_TRACE_DEBUG("[pts] run AT+VTS code:%d", *value);
      send_dtmf(&rc_addr, *value);
      osi_property_set("bluetooth.pts.hfclient.dtmf", "0");
    } else if((osi_property_get("bluetooth.pts.hfclient.volume_spk", value, "0"))
                && (strcmp(value, "0"))){
      int volume;
      sscanf(value, "%d", &volume);
      BTIF_TRACE_EVENT("[pts] run AT+VGS volume:%d", volume);
      volume_control(&rc_addr, BTHF_CLIENT_VOLUME_TYPE_SPK, volume);
      osi_property_set("bluetooth.pts.hfclient.volume_spk", "0");
    } else if((osi_property_get("bluetooth.pts.hfclient.volume_mic", value, "0"))
                && (strcmp(value, "0"))){
      int volume;
      sscanf(value, "%d", &volume);
      BTIF_TRACE_EVENT("[pts] run AT+VGM volume:%d", volume);
      volume_control(&rc_addr, BTHF_CLIENT_VOLUME_TYPE_MIC, volume);
      osi_property_set("bluetooth.pts.hfclient.volume_mic", "0");
    } else if ((osi_property_get("bluetooth.pts.hfclient.connect", value, "0"))
        && (strcmp(value, "0"))) {
      int addr[6], i = 0;
      BTIF_TRACE_DEBUG("[pts] connect:");

      sscanf(value, "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

      for(i = 0; i < 6; i++) {
        rc_addr.address[i] = addr[i];
      }
      BTIF_TRACE_DEBUG("%s: rc_addr: %s", __func__,rc_addr.ToString().c_str());
      connect(&rc_addr);
      osi_property_set("bluetooth.pts.hfclient.connect", "0");
    } else if ((osi_property_get("bluetooth.pts.hfclient.disconnect", value, "0"))
        && (strcmp(value, "0"))) {
      int addr[6], i = 0;
      BTIF_TRACE_DEBUG("[pts] disconnect:");

      sscanf(value, "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

      for(i = 0; i < 6; i++) {
        rc_addr.address[i] = addr[i];
      }
      BTIF_TRACE_DEBUG("%s: rc_addr: %s", __func__,rc_addr.ToString().c_str());
      disconnect(&rc_addr);
      osi_property_set("bluetooth.pts.hfclient.disconnect", "0");
    } else if ((osi_property_get("bluetooth.pts.hfclient.connect_audio", value, "0"))
        && (strcmp(value, "0"))) {
      int addr[6], i = 0;
      BTIF_TRACE_DEBUG("[pts] connect audio:");

      sscanf(value, "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

      for(i = 0; i < 6; i++) {
        rc_addr.address[i] = addr[i];
      }
      BTIF_TRACE_DEBUG("%s: rc_addr: %s", __func__,rc_addr.ToString().c_str());
      connect_audio(&rc_addr);
      osi_property_set("bluetooth.pts.hfclient.connect_audio", "0");
    } else if ((osi_property_get("bluetooth.pts.hfclient.disconnect_audio", value, "0"))
        && (strcmp(value, "0"))) {
      int addr[6], i = 0;
      BTIF_TRACE_DEBUG("[pts] disconnect audio:");

      sscanf(value, "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

      for(i = 0; i < 6; i++) {
        rc_addr.address[i] = addr[i];
      }
      BTIF_TRACE_DEBUG("%s: rc_addr: %s", __func__,rc_addr.ToString().c_str());
      disconnect_audio(&rc_addr);
      osi_property_set("bluetooth.pts.hfclient.disconnect_audio", "0");
    } else if((osi_property_get("bluetooth.pts.hfclient.dial_memory", value, "0"))
                && (strcmp(value, "0"))){
      int location;
      sscanf(value, "%d", &location);
      BTIF_TRACE_DEBUG("[pts] location:%d", location);
      dial_memory(&rc_addr, location);
      osi_property_set("bluetooth.pts.hfclient.dial_memory", "0");
    } else if((osi_property_get("bluetooth.pts.hfclient.dial", value, "0"))
                && (strcmp(value, "0"))){
      BTIF_TRACE_DEBUG("[pts] dial value:%s", value);
      if (!strcmp(value, "1")) {
        dial(&rc_addr, "");
      } else {
        dial(&rc_addr, value);
      }
      osi_property_set("bluetooth.pts.hfclient.dial", "0");
    }
    sleep(1);
  }
}

static void btif_hf_client_pts_test_init()
{
  //1. create a thread to check property
  // 2. check the property and do sth.
  if (NULL != pts_hfclient_test_thread)
  {
      return;
  }
  pts_hfclient_test_thread = thread_new("pts_hfclient_test");
  BTIF_TRACE_EVENT("[pts] start %s()", __func__);

  thread_post(pts_hfclient_test_thread, btif_hfclient_pts_test_thread, NULL);
}

#endif //(end #ifdef MTK_PTS_HF_TEST

/*****************************************************************************
 *
 *   btif hf api functions (no context switch)
 *
 ****************************************************************************/

/*******************************************************************************
 *
 * Function         btif_hf_client_init
 *
 * Description     initializes the hf interface
 *
 * Returns         bt_status_t
 *
 ******************************************************************************/
static bt_status_t init(bthf_client_callbacks_t* callbacks) {
  BTIF_TRACE_EVENT("%s", __func__);

  bt_hf_client_callbacks = callbacks;

  btif_enable_service(BTA_HFP_HS_SERVICE_ID);

  memset(&btif_hf_client_cb_arr, 0, sizeof(btif_hf_client_cb_arr_t));

#if defined(MTK_PTS_HF_TEST) && (MTK_PTS_HF_TEST == TRUE)
  MTK_PTS_TEST_IS_ENABLE {
    BTIF_TRACE_DEBUG("[hf_client] bluetooth.pts.thread_enable");
    btif_hf_client_pts_test_init();
  }
#endif

  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         connect
 *
 * Description     connect to audio gateway
 *
 * Returns         bt_status_t
 *
 ******************************************************************************/
static bt_status_t connect_int(RawAddress* bd_addr, uint16_t uuid) {
  btif_hf_client_cb_t* cb = btif_hf_client_allocate_cb();
  if (cb == NULL) {
    BTIF_TRACE_ERROR("%s: could not allocate block!", __func__);
    return BT_STATUS_BUSY;
  }

  cb->peer_bda = *bd_addr;
  if (is_connected(cb)) return BT_STATUS_BUSY;

  cb->state = BTHF_CLIENT_CONNECTION_STATE_CONNECTING;
  cb->peer_bda = *bd_addr;

  /* Open HF connection to remote device and get the relevant handle.
   * The handle is valid until we have called BTA_HfClientClose or the LL
   * has notified us of channel close due to remote closing, error etc.
   */
  BTA_HfClientOpen(cb->peer_bda, BTIF_HF_CLIENT_SECURITY, &cb->handle);

  return BT_STATUS_SUCCESS;
}

static bt_status_t connect(RawAddress* bd_addr) {
  BTIF_TRACE_EVENT("HFP Client version is  %s", btif_hf_client_version);
  CHECK_BTHF_CLIENT_INIT();
  return btif_queue_connect(UUID_SERVCLASS_HF_HANDSFREE, bd_addr, connect_int);
}

/*******************************************************************************
 *
 * Function         disconnect
 *
 * Description      disconnect from audio gateway
 *
 * Returns         bt_status_t
 *
 ******************************************************************************/
static bt_status_t disconnect(const RawAddress* bd_addr) {
  CHECK_BTHF_CLIENT_INIT();

  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb != NULL) {
    BTA_HfClientClose(cb->handle);
    return BT_STATUS_SUCCESS;
  } else {
    return BT_STATUS_BUSY;
  }
}

/*******************************************************************************
 *
 * Function         connect_audio
 *
 * Description     create an audio connection
 *
 * Returns         bt_status_t
 *
 ******************************************************************************/
static bt_status_t connect_audio(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  if ((BTIF_HF_CLIENT_FEATURES & BTA_HF_CLIENT_FEAT_CODEC) &&
      (cb->peer_feat & BTA_HF_CLIENT_PEER_CODEC)) {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BCC, 0, 0, NULL);
  } else {
    BTA_HfClientAudioOpen(cb->handle);
  }

  /* Inform the application that the audio connection has been initiated
   * successfully */
  btif_transfer_context(btif_in_hf_client_generic_evt,
                        BTIF_HF_CLIENT_CB_AUDIO_CONNECTING, (char*)bd_addr,
                        sizeof(RawAddress), NULL);
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         disconnect_audio
 *
 * Description      close the audio connection
 *
 * Returns         bt_status_t
 *
 ******************************************************************************/
static bt_status_t disconnect_audio(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  BTA_HfClientAudioClose(cb->handle);
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         start_voice_recognition
 *
 * Description      start voice recognition
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t start_voice_recognition(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  if (cb->peer_feat & BTA_HF_CLIENT_PEER_FEAT_VREC) {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BVRA, 1, 0, NULL);
    return BT_STATUS_SUCCESS;
  }
  return BT_STATUS_UNSUPPORTED;
}

/*******************************************************************************
 *
 * Function         stop_voice_recognition
 *
 * Description      stop voice recognition
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t stop_voice_recognition(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  if (cb->peer_feat & BTA_HF_CLIENT_PEER_FEAT_VREC) {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BVRA, 0, 0, NULL);
    return BT_STATUS_SUCCESS;
  }
  return BT_STATUS_UNSUPPORTED;
}

/*******************************************************************************
 *
 * Function         volume_control
 *
 * Description      volume control
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t volume_control(const RawAddress* bd_addr,
                                  bthf_client_volume_type_t type, int volume) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  switch (type) {
    case BTHF_CLIENT_VOLUME_TYPE_SPK:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_VGS, volume, 0, NULL);
      break;
    case BTHF_CLIENT_VOLUME_TYPE_MIC:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_VGM, volume, 0, NULL);
      break;
    default:
      return BT_STATUS_UNSUPPORTED;
  }

  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         dial
 *
 * Description      place a call
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t dial(UNUSED_ATTR const RawAddress* bd_addr,
                        const char* number) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  if ((number != NULL) && (strcmp(number, "") != 0)) {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_ATD, 0, 0, number);
  } else {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BLDN, 0, 0, NULL);
  }
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         dial_memory
 *
 * Description      place a call with number specified by location (speed dial)
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t dial_memory(const RawAddress* bd_addr, int location) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_ATD, location, 0, NULL);
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         handle_call_action
 *
 * Description      handle specified call related action
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t handle_call_action(const RawAddress* bd_addr,
                                      bthf_client_call_action_t action,
                                      int idx) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  switch (action) {
    case BTHF_CLIENT_CALL_ACTION_CHLD_0:
      if (cb->chld_feat & BTA_HF_CLIENT_CHLD_REL) {
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 0, 0, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_CHLD_1:
      // CHLD 1 is mandatory for 3 way calling
      if (cb->peer_feat & BTA_HF_CLIENT_PEER_FEAT_3WAY) {
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 1, 0, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_CHLD_2:
      // CHLD 2 is mandatory for 3 way calling
      if (cb->peer_feat & BTA_HF_CLIENT_PEER_FEAT_3WAY) {
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 2, 0, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_CHLD_3:
      if (cb->chld_feat & BTA_HF_CLIENT_CHLD_MERGE) {
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 3, 0, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_CHLD_4:
      if (cb->chld_feat & BTA_HF_CLIENT_CHLD_MERGE_DETACH) {
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 4, 0, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_CHLD_1x:
      if (cb->peer_feat & BTA_HF_CLIENT_PEER_ECC) {
        if (idx < 1) {
          return BT_STATUS_FAIL;
        }
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 1, idx, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_CHLD_2x:
      if (cb->peer_feat & BTA_HF_CLIENT_PEER_ECC) {
        if (idx < 1) {
          return BT_STATUS_FAIL;
        }
        BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHLD, 2, idx, NULL);
        break;
      }
      return BT_STATUS_UNSUPPORTED;
    case BTHF_CLIENT_CALL_ACTION_ATA:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_ATA, 0, 0, NULL);
      break;
    case BTHF_CLIENT_CALL_ACTION_CHUP:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CHUP, 0, 0, NULL);
      break;
    case BTHF_CLIENT_CALL_ACTION_BTRH_0:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BTRH, 0, 0, NULL);
      break;
    case BTHF_CLIENT_CALL_ACTION_BTRH_1:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BTRH, 1, 0, NULL);
      break;
    case BTHF_CLIENT_CALL_ACTION_BTRH_2:
      BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BTRH, 2, 0, NULL);
      break;
    default:
      return BT_STATUS_FAIL;
  }

  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         query_current_calls
 *
 * Description      query list of current calls
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t query_current_calls(UNUSED_ATTR const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  if (cb->peer_feat & BTA_HF_CLIENT_PEER_ECS) {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CLCC, 0, 0, NULL);
    return BT_STATUS_SUCCESS;
  }

  return BT_STATUS_UNSUPPORTED;
}

/*******************************************************************************
 *
 * Function         query_current_operator_name
 *
 * Description      query current selected operator name
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t query_current_operator_name(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_COPS, 0, 0, NULL);
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         retieve_subscriber_info
 *
 * Description      retrieve subscriber number information
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t retrieve_subscriber_info(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_CNUM, 0, 0, NULL);
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         send_dtmf
 *
 * Description      send dtmf
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t send_dtmf(const RawAddress* bd_addr, char code) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_VTS, code, 0, NULL);
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         request_last_voice_tag_number
 *
 * Description      Request number from AG for VR purposes
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t request_last_voice_tag_number(const RawAddress* bd_addr) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  if (cb->peer_feat & BTA_HF_CLIENT_PEER_VTAG) {
    BTA_HfClientSendAT(cb->handle, BTA_HF_CLIENT_AT_CMD_BINP, 1, 0, NULL);
    return BT_STATUS_SUCCESS;
  }
  return BT_STATUS_UNSUPPORTED;
}

/*******************************************************************************
 *
 * Function         cleanup
 *
 * Description      Closes the HF interface
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static void cleanup(void) {
  BTIF_TRACE_EVENT("%s", __func__);

  btif_queue_cleanup(UUID_SERVCLASS_HF_HANDSFREE);
  if (bt_hf_client_callbacks) {
    btif_disable_service(BTA_HFP_HS_SERVICE_ID);
    bt_hf_client_callbacks = NULL;
  }
}

/*******************************************************************************
 *
 * Function         send_at_cmd
 *
 * Description      Send requested AT command to rempte device.
 *
 * Returns          bt_status_t
 *
 ******************************************************************************/
static bt_status_t send_at_cmd(const RawAddress* bd_addr, int cmd, int val1,
                               int val2, const char* arg) {
  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(*bd_addr);
  if (cb == NULL || !is_connected(cb)) return BT_STATUS_FAIL;

  CHECK_BTHF_CLIENT_SLC_CONNECTED(cb);

  BTIF_TRACE_EVENT("%s: Cmd %d val1 %d val2 %d arg %s", __func__, cmd, val1,
                   val2, (arg != NULL) ? arg : "<null>");
  BTA_HfClientSendAT(cb->handle, cmd, val1, val2, arg);

  return BT_STATUS_SUCCESS;
}

static const bthf_client_interface_t bthfClientInterface = {
    sizeof(bthf_client_interface_t),
    .init = init,
    .connect = connect,
    .disconnect = disconnect,
    .connect_audio = connect_audio,
    .disconnect_audio = disconnect_audio,
    .start_voice_recognition = start_voice_recognition,
    .stop_voice_recognition = stop_voice_recognition,
    .volume_control = volume_control,
    .dial = dial,
    .dial_memory = dial_memory,
    .handle_call_action = handle_call_action,
    .query_current_calls = query_current_calls,
    .query_current_operator_name = query_current_operator_name,
    .retrieve_subscriber_info = retrieve_subscriber_info,
    .send_dtmf = send_dtmf,
    .request_last_voice_tag_number = request_last_voice_tag_number,
    .cleanup = cleanup,
    .send_at_cmd = send_at_cmd,
};

static void process_ind_evt(tBTA_HF_CLIENT_IND* ind) {
  BTIF_TRACE_DEBUG("%s", __func__);

  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(ind->bd_addr);
  if (cb == NULL || !is_connected(cb)) return;

  switch (ind->type) {
    case BTA_HF_CLIENT_IND_CALL:
      HAL_CBACK(bt_hf_client_callbacks, call_cb, &cb->peer_bda,
                (bthf_client_call_t)ind->value);
      break;

    case BTA_HF_CLIENT_IND_CALLSETUP:
      HAL_CBACK(bt_hf_client_callbacks, callsetup_cb, &cb->peer_bda,
                (bthf_client_callsetup_t)ind->value);
      break;
    case BTA_HF_CLIENT_IND_CALLHELD:
      HAL_CBACK(bt_hf_client_callbacks, callheld_cb, &cb->peer_bda,
                (bthf_client_callheld_t)ind->value);
      break;

    case BTA_HF_CLIENT_IND_SERVICE:
      HAL_CBACK(bt_hf_client_callbacks, network_state_cb, &cb->peer_bda,
                (bthf_client_network_state_t)ind->value);
      break;

    case BTA_HF_CLIENT_IND_SIGNAL:
      HAL_CBACK(bt_hf_client_callbacks, network_signal_cb, &cb->peer_bda,
                ind->value);
      break;

    case BTA_HF_CLIENT_IND_ROAM:
      HAL_CBACK(bt_hf_client_callbacks, network_roaming_cb, &cb->peer_bda,
                (bthf_client_service_type_t)ind->value);
      break;

    case BTA_HF_CLIENT_IND_BATTCH:
      HAL_CBACK(bt_hf_client_callbacks, battery_level_cb, &cb->peer_bda,
                ind->value);
      break;

    default:
      break;
  }
}

/*******************************************************************************
 *
 * Function         btif_hf_client_upstreams_evt
 *
 * Description      Executes HF CLIENT UPSTREAMS events in btif context
 *
 * Returns          void
 *
 ******************************************************************************/
static void btif_hf_client_upstreams_evt(uint16_t event, char* p_param) {
  tBTA_HF_CLIENT* p_data = (tBTA_HF_CLIENT*)p_param;

  btif_hf_client_cb_t* cb = btif_hf_client_get_cb_by_bda(p_data->bd_addr);
  if (cb == NULL && event == BTA_HF_CLIENT_OPEN_EVT) {
    BTIF_TRACE_DEBUG("%s: event BTA_HF_CLIENT_OPEN_EVT allocating block",
                     __func__);
    cb = btif_hf_client_allocate_cb();
    cb->handle = p_data->open.handle;
    cb->peer_bda = p_data->open.bd_addr;
  } else if (cb == NULL) {
    BTIF_TRACE_ERROR("%s: event %d but not allocating block: cb not found",
                     __func__, event);
    return;
  }

  BTIF_TRACE_DEBUG("%s: event=%s (%u)", __func__, dump_hf_client_event(event),
                   event);

  switch (event) {
    case BTA_HF_CLIENT_OPEN_EVT:
      if (p_data->open.status == BTA_HF_CLIENT_SUCCESS) {
        cb->state = BTHF_CLIENT_CONNECTION_STATE_CONNECTED;
        cb->peer_feat = 0;
        cb->chld_feat = 0;
      } else if (cb->state == BTHF_CLIENT_CONNECTION_STATE_CONNECTING) {
        cb->state = BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED;
      } else {
        BTIF_TRACE_WARNING(
            "%s: HF CLient open failed, but another device connected. "
            "status=%d state=%d connected device=%s",
            __func__, p_data->open.status, cb->state,
            cb->peer_bda.ToString().c_str());
        break;
      }

      HAL_CBACK(bt_hf_client_callbacks, connection_state_cb, &cb->peer_bda,
                cb->state, 0, /* peer feat */
                0 /* AT+CHLD feat */);

      if (cb->state == BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED)
        cb->peer_bda = RawAddress::kAny;

      if (p_data->open.status != BTA_HF_CLIENT_SUCCESS) btif_queue_advance();
      break;

    case BTA_HF_CLIENT_CONN_EVT:
      cb->peer_feat = p_data->conn.peer_feat;
      cb->chld_feat = p_data->conn.chld_feat;
      cb->state = BTHF_CLIENT_CONNECTION_STATE_SLC_CONNECTED;

      HAL_CBACK(bt_hf_client_callbacks, connection_state_cb, &cb->peer_bda,
                cb->state, cb->peer_feat, cb->chld_feat);

      /* Inform the application about in-band ringtone */
      if (cb->peer_feat & BTA_HF_CLIENT_PEER_INBAND) {
        HAL_CBACK(bt_hf_client_callbacks, in_band_ring_tone_cb, &cb->peer_bda,
                  BTHF_CLIENT_IN_BAND_RINGTONE_PROVIDED);
      }

      btif_queue_advance();
      break;

    case BTA_HF_CLIENT_CLOSE_EVT:
      cb->state = BTHF_CLIENT_CONNECTION_STATE_DISCONNECTED;
      HAL_CBACK(bt_hf_client_callbacks, connection_state_cb, &cb->peer_bda,
                cb->state, 0, 0);
      cb->peer_bda = RawAddress::kAny;
      cb->peer_feat = 0;
      cb->chld_feat = 0;
      btif_queue_advance();
      break;

    case BTA_HF_CLIENT_IND_EVT:
      process_ind_evt(&p_data->ind);
      break;

    case BTA_HF_CLIENT_MIC_EVT:
      HAL_CBACK(bt_hf_client_callbacks, volume_change_cb, &cb->peer_bda,
                BTHF_CLIENT_VOLUME_TYPE_MIC, p_data->val.value);
      break;

    case BTA_HF_CLIENT_SPK_EVT:
      HAL_CBACK(bt_hf_client_callbacks, volume_change_cb, &cb->peer_bda,
                BTHF_CLIENT_VOLUME_TYPE_SPK, p_data->val.value);
      break;

    case BTA_HF_CLIENT_VOICE_REC_EVT:
      HAL_CBACK(bt_hf_client_callbacks, vr_cmd_cb, &cb->peer_bda,
                (bthf_client_vr_state_t)p_data->val.value);
      break;

    case BTA_HF_CLIENT_OPERATOR_NAME_EVT:
      HAL_CBACK(bt_hf_client_callbacks, current_operator_cb, &cb->peer_bda,
                p_data->operator_name.name);
      break;

    case BTA_HF_CLIENT_CLIP_EVT:
      HAL_CBACK(bt_hf_client_callbacks, clip_cb, &cb->peer_bda,
                p_data->number.number);
      break;

    case BTA_HF_CLIENT_BINP_EVT:
      HAL_CBACK(bt_hf_client_callbacks, last_voice_tag_number_callback,
                &cb->peer_bda, p_data->number.number);
      break;

    case BTA_HF_CLIENT_CCWA_EVT:
      HAL_CBACK(bt_hf_client_callbacks, call_waiting_cb, &cb->peer_bda,
                p_data->number.number);
      break;

    case BTA_HF_CLIENT_AT_RESULT_EVT:
      HAL_CBACK(bt_hf_client_callbacks, cmd_complete_cb, &cb->peer_bda,
                (bthf_client_cmd_complete_t)p_data->result.type,
                p_data->result.cme);
      break;

    case BTA_HF_CLIENT_CLCC_EVT:
      HAL_CBACK(bt_hf_client_callbacks, current_calls_cb, &cb->peer_bda,
                p_data->clcc.idx,
                p_data->clcc.inc ? BTHF_CLIENT_CALL_DIRECTION_INCOMING
                                 : BTHF_CLIENT_CALL_DIRECTION_OUTGOING,
                (bthf_client_call_state_t)p_data->clcc.status,
                p_data->clcc.mpty ? BTHF_CLIENT_CALL_MPTY_TYPE_MULTI
                                  : BTHF_CLIENT_CALL_MPTY_TYPE_SINGLE,
                p_data->clcc.number_present ? p_data->clcc.number : "");
      break;

    case BTA_HF_CLIENT_CNUM_EVT:
      if (p_data->cnum.service == 4) {
        HAL_CBACK(bt_hf_client_callbacks, subscriber_info_cb, &cb->peer_bda,
                  p_data->cnum.number, BTHF_CLIENT_SERVICE_VOICE);
      } else if (p_data->cnum.service == 5) {
        HAL_CBACK(bt_hf_client_callbacks, subscriber_info_cb, &cb->peer_bda,
                  p_data->cnum.number, BTHF_CLIENT_SERVICE_FAX);
      } else {
        HAL_CBACK(bt_hf_client_callbacks, subscriber_info_cb, &cb->peer_bda,
                  p_data->cnum.number, BTHF_CLIENT_SERVICE_UNKNOWN);
      }
      break;

    case BTA_HF_CLIENT_BTRH_EVT:
      if (p_data->val.value <= BTRH_CLIENT_RESP_AND_HOLD_REJECT) {
        HAL_CBACK(bt_hf_client_callbacks, resp_and_hold_cb, &cb->peer_bda,
                  (bthf_client_resp_and_hold_t)p_data->val.value);
      }
      break;

    case BTA_HF_CLIENT_BSIR_EVT:
      if (p_data->val.value != 0) {
        HAL_CBACK(bt_hf_client_callbacks, in_band_ring_tone_cb, &cb->peer_bda,
                  BTHF_CLIENT_IN_BAND_RINGTONE_PROVIDED);
      } else {
        HAL_CBACK(bt_hf_client_callbacks, in_band_ring_tone_cb, &cb->peer_bda,
                  BTHF_CLIENT_IN_BAND_RINGTONE_NOT_PROVIDED);
      }
      break;

    case BTA_HF_CLIENT_AUDIO_OPEN_EVT:
      HAL_CBACK(bt_hf_client_callbacks, audio_state_cb, &cb->peer_bda,
                BTHF_CLIENT_AUDIO_STATE_CONNECTED);
      break;

    case BTA_HF_CLIENT_AUDIO_MSBC_OPEN_EVT:
      HAL_CBACK(bt_hf_client_callbacks, audio_state_cb, &cb->peer_bda,
                BTHF_CLIENT_AUDIO_STATE_CONNECTED_MSBC);
      break;

    case BTA_HF_CLIENT_AUDIO_CLOSE_EVT:
      HAL_CBACK(bt_hf_client_callbacks, audio_state_cb, &cb->peer_bda,
                BTHF_CLIENT_AUDIO_STATE_DISCONNECTED);
      break;
    case BTA_HF_CLIENT_RING_INDICATION:
      HAL_CBACK(bt_hf_client_callbacks, ring_indication_cb, &cb->peer_bda);
      break;
    default:
      BTIF_TRACE_WARNING("%s: Unhandled event: %d", __func__, event);
      break;
  }
}

/*******************************************************************************
 *
 * Function         bta_hf_client_evt
 *
 * Description      Switches context from BTA to BTIF for all HF Client events
 *
 * Returns          void
 *
 ******************************************************************************/

static void bta_hf_client_evt(tBTA_HF_CLIENT_EVT event,
                              tBTA_HF_CLIENT* p_data) {
  bt_status_t status;

  /* switch context to btif task context (copy full union size for convenience)
   */
  status = btif_transfer_context(btif_hf_client_upstreams_evt, (uint16_t)event,
                                 (char*)p_data, sizeof(*p_data), NULL);

  /* catch any failed context transfers */
  ASSERTC(status == BT_STATUS_SUCCESS, "context transfer failed", status);
}

/*******************************************************************************
 *
 * Function         btif_hf_client_execute_service
 *
 * Description      Initializes/Shuts down the service
 *
 * Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
 *
 ******************************************************************************/
bt_status_t btif_hf_client_execute_service(bool b_enable) {
  BTIF_TRACE_EVENT("%s: enable: %d", __func__, b_enable);

  if (b_enable) {
    /* Enable and register with BTA-HFClient */
    BTIF_TRACE_EVENT("%s: support codec negotiation %d ", __func__,
                     BTIF_HF_CLIENT_FEATURES);
    BTA_HfClientEnable(bta_hf_client_evt, BTIF_HF_CLIENT_SECURITY,
                       BTIF_HF_CLIENT_FEATURES, BTIF_HF_CLIENT_SERVICE_NAME);
  } else {
    BTA_HfClientDisable();
  }
  return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         btif_hf_get_interface
 *
 * Description      Get the hf callback interface
 *
 * Returns          bthf_interface_t
 *
 ******************************************************************************/
const bthf_client_interface_t* btif_hf_client_get_interface(void) {
  BTIF_TRACE_EVENT("%s", __func__);
  return &bthfClientInterface;
}
