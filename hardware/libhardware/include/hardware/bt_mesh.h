/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*
* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
* applicable license agreements with MediaTek Inc.
*/


#ifndef ANDROID_INCLUDE_BT_MESH_H
#define ANDROID_INCLUDE_BT_MESH_H

#include <stdint.h>

__BEGIN_DECLS
; /* don't remove it, it's for source insight can parse below symbols */
#define BT_PROFILE_MESH_ID "btmesh"

#define MESHIF_VERSION_LEN  (30)
#define MESHIF_UUID_SIZE    (16)
#define MESHIF_TTX_MAX  (0x7F)
#define MESHIF_KEY_SIZE (16)
#define MESHIF_AUTHENTICATION_SIZE  (16)
#define MESHIF_DEVKEY_SIZE  MESHIF_KEY_SIZE
#define MESHIF_PUBLIC_KEY_SIZE  (64)
#define MESHIF_BLE_ADDR_LEN     (6)
#define MESHIF_MAX_FACTOR_LEN   (128)
#define MESHIF_MAX_OTA_NODE     (20)
#define MESHIF_URI_HASH_LEN     (4)

#define MESHIF_EVENT_MESH   (200)
#define MESHIF_EVENT_ADV_REPORT (0x1003)
#define MESHIF_EVENT_GATT       (0x3000)
#define MESHIF_ATT_MTU          (23)

#define MESHIF_FEATURE_NONE     0x00    /**< A bit field indicating no feature. */
#define MESHIF_FEATURE_RELAY    0x01    /**< A bit field indicating feature relay. */
#define MESHIF_FEATURE_PROXY    0x02    /**< A bit field indicating feature proxy. */
#define MESHIF_FEATURE_FRIEND   0x04    /**< A bit field indicating feature friend. */
#define MESHIF_FEATURE_LPN      0x08    /**< A bit field indicating feature low power node. */

#define MESHIF_FLASH_INVALID_DATA  0xFF
#define MESHIF_FLASH_VALID_DATA    0x0F

#define MESHIF_NET_KEY_RECORD_NUMBER              20   /**< The maximum number of network keys stored in flash.*/
#define MESHIF_APP_KEY_RECORD_NUMBER              50   /**< The maximum number of application keys stored in flash.*/
#define MESHIF_MODEL_RECORD_NUMBER                50   /**< The maximum number of model records stored in flash.*/
#define MESHIF_MODEL_PUBLICATION_RECORD_NUMBER    50   /**< The maximum number of model publication records stored in flash.*/
#define MESHIF_MODEL_SUBSCRIPTION_RECORD_NUMBER   160  /**< The maximum number of model subscription records stored in flash.*/
#define MESHIF_HEALTH_SERVER_RECORD_NUMBER        10   /**< The maximum number of health server records stored in flash.*/
#define MESHIF_FRIEND_RECORD_NUMBER               15   /**< The max number of friend stored in flash.*/
#define MESHIF_LOCAL_DEVICE_INFO_RECORD_NUMBER    1    /**< The max number of local device information stored in flash.*/
#define MESHIF_REMOTE_DEVICE_INFO_RECORD_NUMBER   50   /**< The max number of remote device information stored in flash.*/
#define MESHIF_CONFIGURATION_SERVER_RECORD_NUMBER 1    /**< The max number of configuration server record stored in flash.*/
#define MESHIF_SEQUENCE_NUMBER_RECORD_NUMBER      1    /**< The max number of sequence number stored in memory.*/

//iv index phase
#define MESHIF_IV_INDEX_UPDATE_NORMAL_STATE_STAGE_1          0x01
#define MESHIF_IV_INDEX_UPDATE_NORMAL_STATE_STAGE_2          0x02
#define MESHIF_IV_INDEX_UPDATE_IN_PROGRESS_STATE_STAGE_1     0x03
#define MESHIF_IV_INDEX_UPDATE_IN_PROGRESS_STATE_STAGE_2     0x04

//Configuration model messages
#define MESHIF_ACCESS_MSG_CONFIG_BEACON_GET 0x8009
#define MESHIF_ACCESS_MSG_CONFIG_BEACON_SET 0x800A
#define MESHIF_ACCESS_MSG_CONFIG_BEACON_STATUS  0x800B
#define MESHIF_ACCESS_MSG_CONFIG_COMPOSITION_DATA_GET   0x8008
#define MESHIF_ACCESS_MSG_CONFIG_COMPOSITION_DATA_STATUS    0x02
#define MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_GET    0x800C
#define MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_SET    0x800D
#define MESHIF_ACCESS_MSG_CONFIG_DEFAULT_TTL_STATUS 0x800E
#define MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_GET 0x8012
#define MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_SET 0x8013
#define MESHIF_ACCESS_MSG_CONFIG_GATT_PROXY_STATUS  0x8014
#define MESHIF_ACCESS_MSG_CONFIG_FRIEND_GET 0x800F
#define MESHIF_ACCESS_MSG_CONFIG_FRIEND_SET 0x8010
#define MESHIF_ACCESS_MSG_CONFIG_FRIEND_STATUS  0x8011
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_GET  0x8018
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_SET  0x03
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_STATUS   0x8019
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET  0x801A
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD 0x801B
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE  0x801C
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL  0x801D
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE   0x801E
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_STATUS  0x801F
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD 0x8020
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE  0x8021
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE   0x8022
#define MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_GET   0x8023
#define MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_SET   0x8024
#define MESHIF_ACCESS_MSG_CONFIG_NETWORK_TRANSMIT_STATUS    0x8025
#define MESHIF_ACCESS_MSG_CONFIG_RELAY_GET  0x8026
#define MESHIF_ACCESS_MSG_CONFIG_RELAY_SET  0x8027
#define MESHIF_ACCESS_MSG_CONFIG_RELAY_STATUS   0x8028
#define MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET 0x8029
#define MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST    0x802A
#define MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET  0x802B
#define MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST 0x802C
#define MESHIF_ACCESS_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_GET    0x802D
#define MESHIF_ACCESS_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_STATUS 0x802E
#define MESHIF_ACCESS_MSG_CONFIG_NETKEY_ADD 0x8040
#define MESHIF_ACCESS_MSG_CONFIG_NETKEY_DELETE  0x8041
#define MESHIF_ACCESS_MSG_CONFIG_NETKEY_GET 0x8042
#define MESHIF_ACCESS_MSG_CONFIG_NETKEY_LIST    0x8043
#define MESHIF_ACCESS_MSG_CONFIG_NETKEY_STATUS  0x8044
#define MESHIF_ACCESS_MSG_CONFIG_NETKEY_UPDATE  0x8045
#define MESHIF_ACCESS_MSG_CONFIG_APPKEY_ADD 0x00
#define MESHIF_ACCESS_MSG_CONFIG_APPKEY_UPDATE  0x01
#define MESHIF_ACCESS_MSG_CONFIG_APPKEY_DELETE  0x8000
#define MESHIF_ACCESS_MSG_CONFIG_APPKEY_GET 0x8001
#define MESHIF_ACCESS_MSG_CONFIG_APPKEY_LIST    0x8002
#define MESHIF_ACCESS_MSG_CONFIG_APPKEY_STATUS  0x8003
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_BIND 0x803D
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_STATUS   0x803E
#define MESHIF_ACCESS_MSG_CONFIG_MODEL_APP_UNBIND   0x803F
#define MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_APP_GET  0x804B
#define MESHIF_ACCESS_MSG_CONFIG_SIG_MODEL_APP_LIST 0x804C
#define MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_APP_GET   0x804D
#define MESHIF_ACCESS_MSG_CONFIG_VENDOR_MODEL_APP_LIST  0x804E
#define MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_GET  0x8046
#define MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_SET  0x8047
#define MESHIF_ACCESS_MSG_CONFIG_NODE_IDENTITY_STATUS   0x8048
#define MESHIF_ACCESS_MSG_CONFIG_NODE_RESET 0x8049
#define MESHIF_ACCESS_MSG_CONFIG_NODE_RESET_STATUS  0x804A
#define MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_GET  0x8015
#define MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_SET  0x8016
#define MESHIF_ACCESS_MSG_CONFIG_KEY_REFRESH_PHASE_STATUS   0x8017
#define MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET  0x8038
#define MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET  0x8039
#define MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_PUBLICATION_STATUS   0x06
#define MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET 0x803A
#define MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET 0x803B
#define MESHIF_ACCESS_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS  0x803C

#define MESHIF_ACCESS_MSG_HEALTH_CURRENT_STATUS 0x04
#define MESHIF_ACCESS_MSG_HEALTH_FAULT_STATUS   0x05
#define MESHIF_ACCESS_MSG_HEALTH_FAULT_CLEAR    0x802F
#define MESHIF_ACCESS_MSG_HEALTH_FAULT_CLEAR_UNACKNOWLEDGED 0x8030
#define MESHIF_ACCESS_MSG_HEALTH_FAULT_GET  0x8031
#define MESHIF_ACCESS_MSG_HEALTH_FAULT_TEST 0x8032
#define MESHIF_ACCESS_MSG_HEALTH_FAULT_TEST_UNACKNOWLEDGED  0x8033
#define MESHIF_ACCESS_MSG_HEALTH_PERIOD_GET 0x8034
#define MESHIF_ACCESS_MSG_HEALTH_PERIOD_SET 0x8035
#define MESHIF_ACCESS_MSG_HEALTH_PERIOD_SET_UNACKNOWLEDGED  0x8036
#define MESHIF_ACCESS_MSG_HEALTH_PERIOD_STATUS  0x8037
#define MESHIF_ACCESS_MSG_HEALTH_ATTENTION_GET  0x8004
#define MESHIF_ACCESS_MSG_HEALTH_ATTENTION_SET  0x8005
#define MESHIF_ACCESS_MSG_HEALTH_ATTENTION_SET_UNACKNOWLEDGED   0x8006
#define MESHIF_ACCESS_MSG_HEALTH_ATTENTION_STATUS   0x8007

//friend
#define MESHIF_FRIEND_DEFAULT_LPN_NUMBER                       10
#define MESHIF_FRIEND_DEFAULT_QUEUE_SIZE                       10
#define MESHIF_FRIEND_DEFAULT_SUBSCRIPTION_LIST_SIZE           5


#define HCI_EVT_LE_CONN_COMPLETE      0x01
struct hci_evt_le_conn_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  role;
    uint8_t  peer_addr_type;
    uint8_t  peer_addr[6];
    uint16_t interval;
    uint16_t latency;
    uint16_t supv_timeout;
    uint8_t  clock_accuracy;
} __attribute__ ((packed));

#define HCI_EVT_LE_ADV_REPORT        0x02
struct hci_evt_le_adv_report {
    uint8_t  num_reports;
    uint8_t  event_type;
    uint8_t  addr_type;
    uint8_t  addr[6];
    uint8_t  data_len;
    uint8_t  data[0];
} __attribute__ ((packed));

#define HCI_EVT_DISCONNECT_COMPLETE      0x05
struct hci_evt_disconnect_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  reason;
} __attribute__ ((packed));

#define HCI_EVT_LE_ENHANCED_CONN_COMPLETE      0x0A
struct hci_evt_le_enhanced_conn_complete {
    uint8_t  status;
    uint16_t handle;
    uint8_t  role;
    uint8_t  peer_addr_type;
    uint8_t  peer_addr[6];
    uint8_t  local_rpa[6];
    uint8_t  peer_rpa[6];
    uint16_t interval;
    uint16_t latency;
    uint16_t supv_timeout;
    uint8_t  clock_accuracy;
} __attribute__ ((packed));


////////////////////////////////////////////////////////////

typedef struct {
    uint8_t number_of_elements;
    uint16_t algorithms;
    uint8_t public_key_type;
    uint8_t static_oob_type;
    uint8_t output_oob_size;
    uint16_t output_oob_action;
    uint8_t input_oob_size;
    uint16_t input_oob_action;
} meshif_prov_capabilities_t;

typedef struct {
    meshif_prov_capabilities_t cap;
} meshif_evt_prov_capabilities_t;

typedef struct {
    uint8_t method;
    uint8_t action;
    uint8_t size;
} meshif_evt_prov_request_auth_t;

typedef struct {
    uint8_t pk[MESHIF_PUBLIC_KEY_SIZE];
} meshif_evt_prov_show_pk_t;

typedef struct {
    uint8_t auth[MESHIF_AUTHENTICATION_SIZE];
} meshif_evt_prov_show_auth_t;

/* Provisioning error code used for #meshif_prov_error_t */
typedef enum {
    MESHIF_PROV_SUCCESS = 0,         /**< Provisioning success */
    MESHIF_PROV_FAILED_ERROR_CODE_INVALID_PDU = 1,        /**< The provisioning protocol PDU is not recognized by the device */
    MESHIF_PROV_FAILED_ERROR_CODE_INVALID_FORMAT = 2,     /**< The arguments of the protocol PDUs are outside expected values or the length of the PDU is different than expected */
    MESHIF_PROV_FAILED_ERROR_CODE_UNEXPECTED_PDU = 3,     /**< The PDU received was not expected at this moment of the procedure */
    MESHIF_PROV_FAILED_ERROR_CODE_CONFIRMATION_FAILED = 4, /**< The computed confirmation value was not successfully verified */
    MESHIF_PROV_FAILED_ERROR_CODE_OUT_OF_RESOURCES = 5,   /**< The provisioning protocol cannot be continued due to insufficient resources in the device */
    MESHIF_PROV_FAILED_ERROR_CODE_DECRYPTION_FAILED = 6,  /**< The Data block was not successfully decrypted */
    MESHIF_PROV_FAILED_ERROR_CODE_UNEXPECTED_ERROR = 7,   /**< An unexpected error occurred that may not be recoverable */
    MESHIF_PROV_FAILED_ERROR_CODE_CANNOT_ASSIGN_ADDR = 8, /**< The device cannot assign consecutive unicast addresses to all elements */
    MESHIF_PROV_FAILED_ERROR_CODE_TRANSACTION_TIMEOUT = 200, /**MTK private enum field, reserve 200 elements for original SDK */
    MESHIF_PROV_FAILED_ERROR_CODE_PROVISION_TIMEOUT = 201,
    MESHIF_PROV_FAILED_ERROR_CODE_AUTHENTICATION_FAILED = 202,
} meshif_prov_error_t;


typedef struct {
    meshif_prov_error_t reason;
    uint16_t address;
    uint8_t device_key[MESHIF_DEVKEY_SIZE];
    bool success;
    bool gatt_bearer;
} meshif_evt_prov_done_t;

typedef struct {
    uint8_t uuid[MESHIF_UUID_SIZE];
    uint16_t oob_info;
    uint8_t uri_hash_avaliable;
    uint8_t uri_hash[MESHIF_URI_HASH_LEN];
} meshif_prov_unprovisioned_device_t;

typedef struct {
    uint32_t total_count;   //total number of the scanned ud
    uint32_t current_index; //the index of current notified ud, start from 1. (<= total_count)
    meshif_prov_unprovisioned_device_t ud;  //current ud info
} meshif_evt_prov_scan_ud_t;

typedef enum {
    MESHIF_PROV_FACTOR_CONFIRMATION_KEY,
    MESHIF_PROV_FACTOR_RANDOM_PROVISIONER,
    MESHIF_PROV_FACTOR_RANDOM_DEVICE,
    MESHIF_PROV_FACTOR_CONFIRMATION_PROVISIONER,
    MESHIF_PROV_FACTOR_CONFIRMATION_DEVICE,
    MESHIF_PROV_FACTOR_PUB_KEY,
    MESHIF_PROV_FACTOR_AUTHEN_VALUE,
    MESHIF_PROV_FACTOR_AUTHEN_RESULT,
    MESHIF_PROV_FACTOR_PROV_INVITE,
} meshif_prov_factor_type_t;

//This is a common structure for provisioning factor data, which may be set from APP or notified to APP. It is implementation dependent.
typedef struct {
    meshif_prov_factor_type_t type;
    uint8_t buf[MESHIF_MAX_FACTOR_LEN];
    uint16_t buf_len;
} meshif_prov_factor_t;

typedef enum
{
    MESHIF_ADDR_TYPE_PUBLIC = 0,                   ///< public address
    MESHIF_ADDR_TYPE_RANDOM_STATIC = 1,            ///< random static address
    MESHIF_ADDR_TYPE_RANDOM_RESOLVABLE = 2,        ///< random resolvable addresss
    MESHIF_ADDR_TYPE_RANDOM_NON_RESOLVABLE = 3,    ///< random non resolvable address
} meshif_addr_type_t;

typedef struct
{
    meshif_addr_type_t  addr_type;                    ///< address type
    uint8_t             addr[MESHIF_BLE_ADDR_LEN];    ///< address byte array
} meshif_addr_t;

typedef enum
{
    MESHIF_REPORT_TYPE_IND = 0x00,                 ///< Type for ADV_IND found (passive)
    MESHIF_REPORT_TYPE_DIRECT_IND = 0x01,          ///< Type for ADV_DIRECT_IND found (passive)
    MESHIF_REPORT_TYPE_SCAN_IND    = 0x02,         ///< Type for ADV_SCAN_IND found (passive)
    MESHIF_REPORT_TYPE_NONCONN_IND  = 0x03,        ///< Type for ADV_NONCONN_IND found (passive)
    MESHIF_REPORT_TYPE_SCAN_RSP = 0x04             ///< Type for SCAN_RSP found (active)
} meshif_report_type_t;

typedef struct
{
  meshif_addr_t           peer_addr;
  int8_t                  rssi;
  meshif_report_type_t    type;
  uint8_t                 dlen;
  uint8_t                 data[62];
}  mehsif_evt_adv_report_t;

typedef enum {
    MESHIF_KEY_REFRESH_STATE_NONE = 0,    /**< Key refresh phase 0. Indicates normal device operation. */
    MESHIF_KEY_REFRESH_STATE_1 = 1,       /**< Key refresh phase 1. Old keys are used for packet transmission, but new keys can be used to receive messages. */
    MESHIF_KEY_REFRESH_STATE_2 = 2,       /**< Key refresh phase 2. New keys are used for packet transmission, but old keys can be used to receive messages. */
    MESHIF_KEY_REFRESH_STATE_3 = 3,       /**< Key refresh phase 3. Used to complete a key refresh procedure and transition back to phase 0. */
} meshif_key_refresh_state_t;

typedef enum {
    MESHIF_FEATURE_STATE_DISABLED = 0,       /**< Feature is supported, but disabled. */
    MESHIF_FEATURE_STATE_ENABLED = 1,        /**< Feature is supported and enabled. */
    MESHIF_FEATURE_STATE_NOT_SUPPORTED = 2   /**< Feature is not supported. */
} mehsif_feature_state_t;

typedef enum {
    MESHIF_IV_UPDATE_STATE_NORMAL = 0,        /**< Indicates IV update is in normal operation. */
    MESHIF_IV_UPDATE_STATE_IN_PROGRESS = 1,   /**< Indicates IV update is in progress. */
} mehsif_iv_update_state_t;

typedef struct {
    uint32_t iv_index;  /**< The IV index currently used for sending messages. */
    mehsif_iv_update_state_t state;      /**< Current IV update state.*/
    uint8_t iv_phase;
}mehsif_evt_iv_update_t;

typedef struct {
    uint16_t netkey_index;  /**< The network key index. */
    meshif_key_refresh_state_t phase;          /**< Current key refresh phase. */
} meshif_evt_key_refresh_t;

typedef struct {
    uint32_t seq_num;
}mehsif_evt_seq_change_t;

typedef struct {
    uint16_t address;
    uint8_t active;
}mehsif_evt_heartbeat_t;

typedef enum
{
    MESHIF_OTA_ERROR_CODE_SUCCESS,                  /**< Error code of indicating success.*/
    MESHIF_OTA_ERROR_CODE_WRONG_FIRMWARE_ID,        /**< Error code of inidcating wrong firmware id.*/
    MESHIF_OTA_ERROR_CODE_BUSY,                     /**< Error code of inidcating busy of distributor*/
    MESHIF_OTA_ERROR_CODE_NO_RESPONSE,              /**< Error code of inidcating no response of distributor*/
    MESHIF_OTA_ERROR_CODE_USER_STOP,                /**< Error code of inidcating user interuption*/
} meshif_ota_error_code_t;

typedef enum
{
    MESHIF_OTA_EVENT_DISTRIBUTION_STARTING, /**< Event id for informing status of a new distribution was starting.*/
    MESHIF_OTA_EVENT_DISTRIBUTION_STARTED,  /**< Event id for informing status of a new distribution was started.*/
    MESHIF_OTA_EVENT_DISTRIBUTION_ONGOING,  /**< Event id for informing status of the distribution was ongoing.*/
    MESHIF_OTA_EVENT_DISTRIBUTION_STOP,     /**< Event id for informing status of the distirbution was stopped.*/
    MESHIF_OTA_EVENT_DISTRIBUTION_QUEUED,   /**< Event id for informing status of a new distribution was queued.*/
    MESHIF_OTA_EVENT_DISTRIBUTION_DFU_READY,  /**< Event id for informing status of the distribution was dfu ready.*/
} meshif_ota_event_t;

typedef enum
{
    MESHIF_OTA_NODE_UPDATE_STATUS_SUCCESS = 0,
    MESHIF_OTA_NODE_UPDATE_STATUS_IN_PROGRESS,
    MESHIF_OTA_NODE_UPDATE_STATUS_CANCELED,    //failed with some problem
    MESHIF_OTA_NODE_UPDATE_STATUS_DFU_READY,
} meshif_ota_node_update_status_t;

typedef struct
{
    uint16_t addr;
    meshif_ota_node_update_status_t status;
} meshif_ota_node_update_status_info_t;

typedef struct {
    meshif_ota_event_t event_id; /** Event id*/
    meshif_ota_error_code_t error_code; /**< Status code*/
    uint32_t serial_number; /**< Serial number*/
    uint32_t firmware_id;   /**< Firmware id*/
    uint32_t time_escaped; /**< time escaped from started*/
    uint16_t nodes_num;
    uint8_t curr_block;
    uint8_t total_block;
    uint16_t curr_chunk;
    uint16_t chunk_mask;
    meshif_ota_node_update_status_info_t nodes_status[MESHIF_MAX_OTA_NODE]; //this is an array with element number of nodes_num
} meshif_evt_ota_t;

typedef enum {
    MESHIF_FRIENDSHIP_TERMINATED = 0,         /**< The friendship is terminated. */
    MESHIF_FRIENDSHIP_ESTABLISHED = 1,        /**< The friendship is successfully established. */
    MESHIF_FRIENDSHIP_ESTABLISH_FAILED = 2,       /**< The friendship is not established. */
    MESHIF_FRIENDSHIP_REQUEST_FRIEND_TIMEOUT = 3, /**< Request friend procedure timeout. The status is only received when low power feature in use. */
    MESHIF_FRIENDSHIP_SELECT_FRIEND_TIMEOUT = 4,  /**< Select friend procedure timeout. The status is only received when low power feature in use. */
} meshif_friendship_status_t;

typedef struct {
    uint16_t address;                                   /**< Indicates the friend or the low-power node unicast address. */
    meshif_friendship_status_t status;                /**< Indicates the friendship status between the nodes. */
} meshif_evt_friendship_status_t;

typedef enum {
    MESHIF_BEARER_GATT_STATUS_CONNECTED = 0,      /**< Bearer GATT is connected. */
    MESHIF_BEARER_GATT_STATUS_DISCONNECTED = 1,   /**< Bearer GATT is disconnected. */
    MESHIF_BEARER_GATT_STATUS_NO_SERVICE = 2,     /**< Bearer GATT failed to be established because the specified service was not found. */
    MESHIF_BEARER_GATT_STATUS_NO_CHARACTERISTIC = 3,  /**< Bearer GATT failed to be established because the specified characteristics were not found. */
    MESHIF_BEARER_GATT_STATUS_WRITE_CCCD_FAILED = 4,  /**< Bearer GATT failed to be established because writing the CCCD failed. */
} meshif_bearer_gatt_status_t;

typedef struct {
    uint32_t handle;  /**< The handle of this connection. */
    meshif_bearer_gatt_status_t status;      /**< The status of bearer GATT. */
} meshif_evt_bearer_gatt_status_t;

typedef enum {
    MESHIF_ERROR_NO_RESOURCE_TO_ADD_REPLAYPROTECTION,
} meshoif_error_code_type_t;

typedef struct {
    meshoif_error_code_type_t type;
} meshif_evt_error_code_t;

typedef struct {
    uint16_t src;
    uint16_t dst;
    uint8_t mic[8];
    uint8_t mic_len;
}meshif_evt_trans_pdu_info_t;

typedef struct {
    union {
        meshif_evt_prov_capabilities_t      prov_cap;
        meshif_evt_prov_request_auth_t      prov_request_auth;
        meshif_evt_prov_show_pk_t           prov_show_pk;
        meshif_evt_prov_show_auth_t         prov_show_auth;
        meshif_evt_prov_done_t              prov_done;
        meshif_evt_prov_scan_ud_t           prov_scan_ud;
        meshif_prov_factor_t                prov_factor;
        mehsif_evt_adv_report_t             adv_report;
        mehsif_evt_iv_update_t              iv_update;          /**<  parameter of mesh event @ref BLE_MESH_EVT_IV_UPDATE */
        meshif_evt_key_refresh_t            key_refresh;        /**<  parameter of mesh event @ref BLE_MESH_EVT_KEY_REFRESH */
        mehsif_evt_seq_change_t             seq_change;
        mehsif_evt_heartbeat_t              heartbeat;
        meshif_evt_ota_t                    ota_evt;
        meshif_evt_friendship_status_t      friendship_status;
        meshif_evt_bearer_gatt_status_t     bearer_gatt_status;
        meshif_evt_error_code_t             error_code;
        meshif_evt_trans_pdu_info_t         trans_pdu_info;
    } mesh;
} meshif_evt_t;

typedef enum {
    MESHIF_EVT_INIT_DONE = MESHIF_EVENT_MESH,
    MESHIF_EVT_PROV_CAPABILITIES,
    MESHIF_EVT_PROV_REQUEST_OOB_PUBLIC_KEY,
    MESHIF_EVT_PROV_REQUEST_OOB_AUTH_VALUE,
    MESHIF_EVT_PROV_SHOW_OOB_PUBLIC_KEY,
    MESHIF_EVT_PROV_SHOW_OOB_AUTH_VALUE,
    MESHIF_EVT_PROV_DONE,
    MESHIF_EVT_PROV_SCAN_UD_RESULT,
    MESHIF_EVT_CONFIG_RESET,
    MESHIF_EVT_FRIENDSHIP_STATUS,                     /**< Event for mesh friendship status change. */
    MESHIF_EVT_LPN_FRIEND_OFFER,                      /**< Event for mesh LPN receiving friend offer. */
    MESHIF_EVT_LPN_FRIEND_SUBSCRIPTION_LIST_CONFRIM,  /**< Event for mesh LPN receiving friend subscription list confirm. */
    MESHIF_EVT_HEARTBEAT,                             /**< Event for mesh heartbeat. */
    MESHIF_EVT_IV_UPDATE,                             /**< Event for mesh IV index update. */
    MESHIF_EVT_KEY_REFRESH,                           /**< Event for mesh key refresh. */
    MESHIF_EVT_BEARER_GATT_STATUS,                    /**< Event for the mesh bearer GATT status. */

    //The above enum value match with SDK
    MESHIF_EVT_PROV_UD_RESULT_COMPLETE = MESHIF_EVENT_MESH+200, //Indicates that all the unprovisioned device are notified to APP.
    MESHIF_EVT_PROV_FACTOR,
    MESHIF_EVT_SEQ_CHANGE,
    MESHIF_EVT_OTA_EVENT,
    MESHIF_EVT_ERROR_CODE,
    MESHIF_EVT_TRANS_PDU_DECRYPTION_FAIL,

    MESHIF_EVT_ADV_REPORT = MESHIF_EVENT_ADV_REPORT,
} meshif_event_id;

typedef struct {
    uint32_t evt_id; ///< Event ID
    union {
      meshif_evt_t mesh_evt; ///< MESH event structure
    }evt;
} meshif_bt_evt_t;

typedef enum {
    MESHIF_ADDRESS_TYPE_UNASSIGNED = 0,   /**< unassigned address */
    MESHIF_ADDRESS_TYPE_UNICAST,          /**< unicast address */
    MESHIF_ADDRESS_TYPE_VIRTUAL,          /**< virtual address */
    MESHIF_ADDRESS_TYPE_GROUP,            /**< group address */
} meshif_address_type_t;

typedef struct {
    meshif_address_type_t type;   /**< the address type of this address */
    uint16_t value;                 /**< address value */
    uint8_t *virtual_uuid;    /**< virtual uuid value, must be NULL unless address type is #MESHIF_ADDRESS_TYPE_VIRTUAL */
} meshif_address_t;

typedef struct {
    uint16_t netidx;
    uint16_t appidx;
    uint8_t *device_key;
} meshif_security_t;

typedef struct {
    meshif_address_t dst;
    uint16_t src;
    uint8_t ttl;
    uint8_t *data;
    uint16_t data_len;
    meshif_security_t security;
} meshif_tx_params_t;

typedef struct {
    uint16_t opcode;        /**< Operation code. */
    uint16_t company_id;    /**< Company id, use #MESH_MODEL_COMPANY_ID_NONE if this is a SIG access message */
} meshif_access_opcode_t;

/** @brief This structure defines the meatdata of a received access message */
typedef struct {
    uint16_t src_addr;      /**< The source address of this message. */
    uint16_t dst_addr;      /**< The destination address of this message */
    uint16_t appkey_index;        /**< The application key index used for this message. */
    uint16_t netkey_index;     /**< The network key index used for this message. */
    uint8_t rssi;           /**< The RSSI value . */
    uint8_t ttl;            /**< The received TTL value . */
    uint16_t sub_addr;      /**< The subscription address of this message */
} meshif_access_message_rx_meta_t;

/** @brief This structure defines the received access message */
typedef struct {
    meshif_access_opcode_t opcode;    /**< The operation code information. */
    uint8_t *buf;                       /**< The received message buffer. */
    uint16_t buf_len;                   /**< The length of received message. */
    meshif_access_message_rx_meta_t meta_data;    /**< The metadata of this message. */
} meshif_access_message_rx_t;

typedef void (*meshif_access_msg_handler) (uint16_t model_handle, const meshif_access_message_rx_t *msg, const void *arg);
typedef void (*meshif_access_publish_timeout_cb_t) (uint16_t model_handle, void * arg);

/** @brief This structure defines the mapping of operation code and handler */
typedef struct {
    meshif_access_opcode_t opcode;            /**< The operation code information. */
    meshif_access_msg_handler   handler;    /**< The message handler for this opcode. */
} meshif_access_opcode_handler_t;

typedef struct {
    uint32_t model_id;                      /**< The model id, could be SIG defined value or Vendor defined value. */
    uint16_t element_index;                 /**< The target element index to add model. */
    meshif_access_opcode_handler_t *opcode_handlers;    /**< The access message handler for this model. */
    uint8_t opcode_count;                   /**< Indicate how many opcode need to handle in this model. */
} meshif_model_add_params_t;

typedef enum {
    MESHIF_ROLE_PROVISIONEE,
    MESHIF_ROLE_PROVISIONER,
} meshif_role_t;

typedef struct {
    meshif_prov_capabilities_t cap;
} meshif_prov_provisionee_params_t;

typedef struct {
    uint8_t device_uuid[MESHIF_UUID_SIZE];    /**< The device UUID. */
    uint16_t oob_info;                      /**< The OOB information of this device. Please reference to Provision OOB Info*/
    uint8_t default_ttl;                    /**< The default TTL value. */
    char *uri;                          /**< The uri infomation of this device, can be NULL */
} meshif_config_init_params_t;

typedef enum {
    MESHIF_FEATURE_HEARTBEAT = 1 << 0,   /**heartbeat feature */
    MESHIF_FEATURE_OTA = 1 << 1,          /**OTA feature */
} meshif_feature_mask_t;

typedef struct {
    uint8_t lpn_number;                     /**< The max number of low power nodes to be friends with. The default and maximum value is 2. */
    uint8_t queue_size;                     /**< The size of messages that can store for a Low Power node. The default and maximum value is 2. */
    uint8_t subscription_list_size;         /**< The size of subscription list that supported for a Low Power node. The default and maximum value is 5. */
} meshif_friend_init_params_t;

/** @brief The initialization parameters for mesh debug module. This parameter is used in #ble_mesh_init. */
typedef struct {
    uint16_t verbose_level;           /**< default debug level, verbose output */
    uint16_t info_level;            /**< default info level, rich output */
    uint16_t notify_level;          /**< default notify level, few output */
    uint16_t warning_level;         /**< default warning level, rare output */
} meshif_debug_init_params_t;

typedef struct {
    uint16_t max_remote_node_cnt;       /**< the max number of remote node*/
    uint8_t save2flash;             /**< automatically save mesh data to flash or not*/
} meshif_customize_para_t;

typedef struct {
    meshif_role_t role;
    meshif_prov_provisionee_params_t *provisionee;
    meshif_config_init_params_t *config;
    uint32_t feature_mask;
    meshif_friend_init_params_t *friend_params;              /**< initialization parameters of friend. The parameters is available only when friend feature is supported and enabled. Default value will be used when init_params is NULL. */
    meshif_debug_init_params_t *debug;                /**< initialization parameters of debug */
    meshif_customize_para_t *customize_params;
} meshif_init_params_t;

typedef struct {
    uint8_t attention_duration;
} meshif_prov_invite_t;


typedef struct {
    uint8_t algorithm;
    uint8_t public_key;
    uint8_t authentication_method;
    uint8_t authentication_action;
    uint8_t authentication_size;
} meshif_prov_start_t;

typedef struct {
    uint8_t netkey[MESHIF_KEY_SIZE];
    uint16_t netkey_index;
    uint32_t iv_index;
    uint16_t address;
    uint8_t flags;
} meshif_prov_data_t;

typedef struct {
    meshif_prov_start_t start;
    meshif_prov_data_t data;
} meshif_prov_provisioner_params_t;

typedef struct {
    uint16_t element_address;               /**< Address of the element. */
    meshif_address_t publish_address;     /**< Value of the publish address. */
    uint16_t appkey_index;                  /**< Index of the application key. */
    bool friendship_credential_flag;         /**< Value of the Friendship Credential Flag. */
    uint8_t publish_ttl;                    /**< Default TTL value for the outgoing messages. */
    uint8_t publish_period;                 /**< Period for periodic status publishing. */
    uint8_t retransmit_count;               /**< Number of retransmissions for each published message. */
    uint8_t retransmit_interval_steps;      /**< Number of 50-millisecond steps between retransmissions. */
    uint32_t model_id;                      /**< SIG Model ID or Vendor Model ID. */
} meshif_model_publication_state_t;

typedef struct {
    uint16_t destination;                   /**< Destination address for Heartbeat messages. */
    uint8_t count_log;                      /**< Destination address for Heartbeat messages. */
    uint8_t period_log;                     /**< Period for sending Heartbeat messages. */
    uint8_t ttl;                            /**< TTL to be used when sending Heartbeat messages. */
    uint16_t features;                      /**< Bit field indicating features that trigger Heartbeat messages when changed. */
    uint16_t netkey_index;                  /**< Network key index. */
} meshif_heartbeat_publication_t;

/** @brief The heartbeat subscription parameters structure */
typedef struct {
    uint16_t source;                        /**< Source address for Heartbeat messages. */
    uint16_t destination;                   /**< Destination address for Heartbeat messages. */
    uint8_t period_log;                     /**< Period for receiving Heartbeat messages. */
} meshif_heartbeat_subscription_t;






/////////////////////////////////////////////////////////////////

typedef void (*meshif_bt_event_handler)(meshif_bt_evt_t *evt);

typedef struct {
    uint16_t src_addr;
    uint16_t dst_addr;
    uint8_t ttl;
    uint16_t msg_netkey_index;
} config_meta_t;

typedef struct {
    config_meta_t meta;
} config_beacon_get_t,  config_default_ttl_get_t,
config_gatt_proxy_get_t, config_friend_get_t,
config_relay_get_t, config_network_transmit_get_t,
config_netkey_get_t, config_node_reset_t,
config_hb_pub_get_t, config_hb_sub_get_t;

typedef struct {
    config_meta_t meta;
    uint8_t beacon;
} config_beacon_set_t;

typedef struct {
    config_meta_t meta;
    uint8_t page;
} config_composition_data_get_t;

typedef struct {
    config_meta_t meta;
    uint8_t TTL;
} config_default_ttl_set_t;

typedef struct {
    config_meta_t meta;
    uint8_t gatt_proxy;
} config_gatt_proxy_set_t;

typedef struct {
    config_meta_t meta;
    uint8_t mesh_friend;
} config_friend_set_t;

typedef struct {
    config_meta_t meta;
    uint8_t relay;
    uint8_t retransmit_count;
    uint8_t retransmit_interval_steps;
} config_relay_set_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint32_t model_id;
} config_model_pub_get_t;

typedef struct {
    config_meta_t meta;
    meshif_model_publication_state_t *state;
} config_model_pub_set_t;

typedef struct {
    config_meta_t   meta;
    uint16_t element_addr;
    meshif_address_t address;
    uint32_t model_id;
} config_model_sub_add_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    meshif_address_t address;
    uint32_t model_id;
} config_model_sub_del_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    meshif_address_t address;
    uint32_t model_id;
} config_model_sub_ow_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint32_t model_id;
} config_model_sub_del_all_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint16_t model_id;
} config_sig_model_sub_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint32_t model_id;
} config_vendor_model_sub_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
    uint8_t *netkey;
} config_netkey_add_t, config_netkey_update_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
} config_netkey_del_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
    uint16_t appkey_index;
    uint8_t *appkey;
} config_appkey_add_t, config_appkey_update_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
    uint16_t appkey_index;
} config_appkey_del_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
} config_appkey_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint16_t appkey_index;
    uint32_t model_id;
} config_model_app_bind_t, config_model_app_unbind_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint16_t model_id;
} config_sig_model_app_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t element_addr;
    uint32_t model_id;
} config_vendor_model_app_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
} config_node_identity_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
    uint8_t identity;
} config_node_identity_set_t;

typedef struct {
    config_meta_t   meta;
    uint16_t        netkey_index;
} config_key_refresh_phase_get_t;

typedef struct {
    config_meta_t meta;
    uint16_t netkey_index;
    uint8_t transition;
} config_key_refresh_phase_set_t;

typedef struct {
    config_meta_t meta;
    meshif_heartbeat_publication_t *publication;
} config_hb_pub_set_t;

typedef struct {
    config_meta_t meta;
    meshif_heartbeat_subscription_t *subscription;
} config_hb_sub_set_t;

typedef struct {
    config_meta_t meta;
    uint8_t count;
    uint8_t interval_steps;
} config_network_transmit_set_t;

/** @brief configuration msg structure */
typedef struct {
    uint16_t                            opcode;                                /**<The operation code information */
    union {
        config_beacon_get_t             beacon_get;
        config_beacon_set_t             beacon_set;
        config_composition_data_get_t   composition_data_get;
        config_default_ttl_get_t        default_ttl_get;
        config_default_ttl_set_t        default_ttl_set;
        config_gatt_proxy_get_t         gatt_proxy_get;
        config_gatt_proxy_set_t         gatt_proxy_set;
        config_friend_get_t             friend_get;
        config_friend_set_t             friend_set;
        config_relay_get_t              relay_get;
        config_relay_set_t              relay_set;
        config_model_pub_get_t          model_pub_get;
        config_model_pub_set_t          model_pub_set;
        config_model_sub_add_t          model_sub_add;
        config_model_sub_del_t          model_sub_del;
        config_model_sub_ow_t           model_sub_ow;
        config_model_sub_del_all_t      model_sub_del_all;
        config_sig_model_sub_get_t      sig_model_sub_get;
        config_vendor_model_sub_get_t   vendor_model_sub_get;
        config_netkey_add_t             netkey_add;
        config_netkey_update_t          netkey_update;
        config_netkey_del_t             netkey_del;
        config_netkey_get_t             netkey_get;
        config_appkey_add_t             appkey_add;
        config_appkey_update_t          appkey_update;
        config_appkey_del_t             appkey_del;
        config_appkey_get_t             appkey_get;
        config_model_app_bind_t         model_app_bind;
        config_model_app_unbind_t       model_app_unbind;
        config_sig_model_app_get_t      sig_model_app_get;
        config_vendor_model_app_get_t   vendor_model_app_get;
        config_node_identity_get_t      node_identity_get;
        config_node_identity_set_t      node_identity_set;
        config_node_reset_t             node_reset;
        config_key_refresh_phase_get_t  key_ref_pha_get;
        config_key_refresh_phase_set_t  key_ref_pha_set;
        config_hb_pub_get_t             hb_pub_get;
        config_hb_pub_set_t             hb_pub_set;
        config_hb_sub_get_t             hb_sub_get;
        config_hb_sub_set_t             hb_sub_set;
        config_network_transmit_get_t   net_trans_get;
        config_network_transmit_set_t   net_trans_set;
    } data;
} meshif_configuration_msg_tx_t;

/** @brief Mesh opcode of model data */
typedef enum {
    MESHIF_MODEL_DATA_OP_SET_COMPOSITION_DATA_HEADER,     /**< opcode for mesh set composition data header with parameter composition_data_t. */
    MESHIF_MODEL_DATA_OP_ADD_ELEMENT,                     /**< opcode for mesh add element with parameter element_data_t */
    MESHIF_MODEL_DATA_OP_SET_ELEMENT_ADDR,                /**< opcode for mesh set element addr start unicast addr with parameter element_addr_t: provisioner */
    MESHIF_MODEL_DATA_OP_ADD_CONFIGURATION_SERVER,        /**< opcode for mesh add configuration server without parameter */
    MESHIF_MODEL_DATA_OP_ADD_CONFIGURATION_CLIENT,        /**< opcode for mesh add configuration client with parameter configuration_client_data_t */
    MESHIF_MODEL_DATA_OP_ADD_HEALTH_SERVER,               /**< opcode for mesh add health server with parameter health_server_data_t */
    MESHIF_MODEL_DATA_OP_ADD_HEALTH_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_GENERIC_ONOFF_SERVER,
    MESHIF_MODEL_DATA_OP_ADD_CTL_SETUP_SERVER,
    MESHIF_MODEL_DATA_OP_ADD_GENERIC_POWER_ONOFF_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_LIGHTNESS_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_CTL_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_HSL_SETUP_SERVER,
    MESHIF_MODEL_DATA_OP_ADD_HSL_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_GENERIC_LEVEL_SERVER,
    MESHIF_MODEL_DATA_OP_ADD_GENERIC_LEVEL_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_GENERIC_ONOFF_CLIENT,
    MESHIF_MODEL_DATA_OP_ADD_MODEL = 500,                       /**< opcode for mesh add model with parameter generic_model_data_t*/
} meshif_model_data_opcode_t;

typedef struct {
    uint8_t buf_len;
    uint8_t *buf;
} composition_data_t;

typedef struct {
    uint16_t location;
    uint16_t *element_index;
} element_data_t;

typedef struct {
    uint16_t unicast_addr;
} element_addr_t;

typedef struct {
    uint16_t *model_handle;
    uint16_t element_index;
    meshif_access_msg_handler callback;
    meshif_access_publish_timeout_cb_t publish_timeout_cb;
} health_server_data_t,
configuration_client_data_t, generic_onoff_server_data_t,
ctl_setup_server_data_t, generic_power_onoff_client_data_t,
lightness_client_data_t, ctl_client_data_t,
hsl_setup_server_data_t, hsl_client_data_t,
generic_level_server_data_t, generic_level_client_data_t,
generic_onoff_client_data_t;

typedef struct {
    uint8_t test_id;            /**< Identifier of a most recently performed test. */
    uint16_t company_id;        /**< 16-bit Bluetooth assigned Company Identifier. */
    uint8_t *fault_array;       /**< An array contains a sequence of 1-octet fault values. */
    uint8_t fault_array_length; /**< Length of the fault array. */
} meshif_health_client_evt_current_status_t;

typedef struct {
    uint8_t test_id;            /**< Identifier of a most recently performed test. */
    uint16_t company_id;        /**< 16-bit Bluetooth assigned Company Identifier. */
    uint8_t *fault_array;       /**< An array contains a sequence of 1-octet fault values. */
    uint8_t fault_array_length; /**< Length of the fault array. */
} meshif_health_client_evt_fault_status_t;

typedef struct {
    uint8_t fast_period_divisor;    /**< Divider for the Publish Period. Modified Publish Period is used for sending Current Health Status messages when there are active faults to communicate.*/
} meshif_health_client_evt_period_status_t;

typedef struct {
    uint8_t attention; /**< Value of the Attention Timer state, which represents the remaining duration of the attention state of a server in seconds. */
} meshif_health_client_evt_attention_status_t;

typedef struct {
    union {
        meshif_health_client_evt_current_status_t current_status;      /**<  parameter of health client model event @ref MESHIF_ACCESS_MSG_HEALTH_CURRENT_STATUS */
        meshif_health_client_evt_fault_status_t fault_status;          /**<  parameter of health client model event @ref MESHIF_ACCESS_MSG_HEALTH_FAULT_STATUS */
        meshif_health_client_evt_period_status_t period_status;        /**<  parameter of health client model event @ref MESHIF_ACCESS_MSG_HEALTH_PERIOD_STATUS */
        meshif_health_client_evt_attention_status_t attention_status;  /**<  parameter of health client model event @ref MESHIF_ACCESS_MSG_HEALTH_ATTENTION_STATUS */
    } data;
} meshif_health_client_evt_t;

typedef void (*meshif_health_client_evt_handler)(uint16_t model_handle, const meshif_access_message_rx_t *msg, const meshif_health_client_evt_t *event);

typedef struct {
    uint16_t *model_handle;
    uint16_t element_index;
    meshif_health_client_evt_handler callback;
} health_client_data_t;

typedef struct {
    uint16_t *model_handle;
    meshif_access_msg_handler callback;
} configuration_server_data_t;

typedef struct {
    uint16_t                          *model_handle;
    meshif_model_add_params_t *model_params;
} generic_model_data_t;


/** @brief model data structure */
typedef struct {
    meshif_model_data_opcode_t  opcode;                                    /**<  The operation code information @ref model_data_opcode_t */
    union {
        composition_data_t               composition_data;                          /**<  parameter of model data @ref MODEL_DATA_OP_SET_COMPOSITION_DATA_HEADER*/
        element_data_t                   element;                              /**<  parameter of model data @ref MODEL_DATA_OP_ADD_ELEMENT */
        element_addr_t                   element_addr;                              /**<  parameter of model data @ref MODEL_DATA_OP_SET_ELEMENT_ADDR */
        configuration_server_data_t      configuration_server;
        configuration_client_data_t      configuration_client;                                  /**<  parameter of model data @ref MODEL_DATA_OP_ADD_CONFIGURATION_CLIENT */
        health_server_data_t             health_server;                        /**<  parameter of model data @ref MODEL_DATA_OP_ADD_HEALTH_SERVER */
        health_client_data_t             health_client;
        generic_onoff_server_data_t      generic_onoff_server;
        ctl_setup_server_data_t          ctl_setup_server;
        generic_power_onoff_client_data_t generic_power_onoff_client;
        lightness_client_data_t           lightness_client;
        ctl_client_data_t                 ctl_client;
        hsl_setup_server_data_t          hsl_setup_server;
        hsl_client_data_t                hsl_client;
        generic_level_server_data_t      generic_level_server;
        generic_level_client_data_t      generic_level_client;
        generic_onoff_client_data_t      generic_onoff_client;
        generic_model_data_t             generic_model_data;                                /**<  parameter of model data @ref MODEL_DATA_OP_ADD_MODEL */
    } data;
} meshif_model_data_t;

/** @brief key opcode */
typedef enum {
    MESHIF_KEY_ADD = 0,         /**< add a netkey or appkey */
    MESHIF_KEY_UPDATE,      /**< update a netkey or appkey */
    MESHIF_KEY_USE_NEW_NETKEY,  //Key refreh phase2 - swithing to the new keys
    MESHIF_KEY_REVOKE_OLD_NETKEY,   //Key refresh phase3 - revoking old keys
} meshif_key_opcode_t;

/** @brief MESH netkey structure.*/
typedef struct {
    meshif_key_opcode_t opcode;    /**< The operation code information */
    uint8_t *network_key;            /**< network key */
    uint16_t key_index;              /**< index of network key */
} meshif_netkey_t;

/** @brief MESH appkey structure.*/
typedef struct {
    meshif_key_opcode_t opcode;    /**< The operation code information */
    uint8_t *application_key;        /**< applicatin key */
    uint16_t appkey_index;           /**< index of applicatin key */
    uint16_t netkey_index;           /**< index of network key */
} meshif_appkey_t;

/** @brief MESH device key info structure.*/
typedef struct {
    uint16_t unicast_addr;
    uint8_t  deviceKey[MESHIF_KEY_SIZE];
    uint8_t  uuid[MESHIF_UUID_SIZE];
} meshif_devkey_info_t;

typedef enum {
    MESHIF_DEV_INFO_OP_ADD_DEVKEY = 0,
    MESHIF_DEV_INFO_OP_GET_DEVKEY,
    MESHIF_DEV_INFO_OP_DELETE_DEVKEY,
    //...
} meshif_device_info_opcode_t;

typedef struct {
    meshif_device_info_opcode_t  opcode;
    union {
        meshif_devkey_info_t             devkey;    //For ADD_DEVKEY, all the fields shall be valid. For GET_DEVKEY, deviceKey[] and uuid[] are output. For DELETE_DEVKEY, only unicast_addr is used.
    } data;
} meshif_device_info_t;


/** @brief MESH IV info structure.*/
typedef struct {
    uint32_t ivIndex;
    uint8_t  ivPhase;
} meshif_iv_t;

/** @brief This structure defines a access message for sending. */
typedef struct {
    meshif_access_opcode_t opcode;    /**< The operation code information. */
    uint8_t *p_buffer;                  /**< The message buffer for sending. */
    uint16_t length;                    /**< The length of this message. */
} meshif_access_message_tx_t;

/** @brief This struct defines the advertising parameters*/
typedef struct {
    uint32_t adv_period;            /**< adv period in milliseconds*/
    uint16_t min_interval;          /**< adv minimum interval in 625 us units*/
    uint16_t max_interval;          /**< adv maximum interval in 625 us units*/
    uint8_t resend;                 /**< resend number of times, 0xFF means resending forever until user discard it*/
} meshif_bearer_adv_params_t;

/** @brief This struct defines the scanner parameters*/
typedef struct {
    uint32_t scan_period;           /**< scan period in milliseconds*/
    uint16_t scan_interval;         /**< scan interval in 625 us units*/
    uint16_t scan_window;           /**< scan window in 625 us units*/
} meshif_bearer_scan_params_t;

//the bit31 indicates role, 0 for provisionee, 1 for provisioner
typedef enum {
    MESHIF_FLASH_RECORD_ROLE_PROVISIONEE       = 0x00000000,
    MESHIF_FLASH_RECORD_ROLE_PROVISIONER       = 0x80000000,

    MESHIF_FLASH_RECORD_DATA          = 0x00000001,
    MESHIF_FLASH_RECORD_SEQ_NUM       = 0x00000002,
    MESHIF_FLASH_RECORD_REMOTE_NODE   = 0x00000004,
    //TODO Add more...
    MESHIF_FLASH_RECORD_ALL           = 0x7FFFFFFF,
} meshif_flash_record_t;

typedef enum {
    MESHIF_SUCCESS = 0,           /**< success */
    MESHIF_ERROR_OOM,             /**< no memory */
    MESHIF_ERROR_NULL,            /**< parameter is null pointer */
    MESHIF_ERROR_INVALID_ADDR,    /**< invalid address */
    MESHIF_ERROR_INVALID_TTL,     /**< invalid TTL value */
    MESHIF_ERROR_INVALID_KEY,     /**< invalid key index */
    MESHIF_ERROR_NOT_INIT,        /**< mesh core is not initialized */
    MESHIF_ERROR_INVALID_STATE,   /**< invalid state */
    MESHIF_ERROR_INVALID_ROLE,    /**< invalid role */
    MESHIF_ERROR_FAIL,            /**< operation failed */
} meshif_status_code_t;

typedef enum {
    MESHIF_ACCESS_MSG_STATUS_SUCCESS = 0,                        /**< Success */
    MESHIF_ACCESS_MSG_STATUS_INVALID_ADDRESS = 1,                /**< Invalid Address */
    MESHIF_ACCESS_MSG_STATUS_INVALID_MODEL = 2,                  /**< Invalid Model */
    MESHIF_ACCESS_MSG_STATUS_INVALID_APPKEY = 3,                 /**< Invalid AppKey Index */
    MESHIF_ACCESS_MSG_STATUS_INVALID_NETKEY = 4,                 /**< Invalid NetKey Index */
    MESHIF_ACCESS_MSG_STATUS_INSUFFICIENT_RESOURCES = 5,         /**< Insufficient Resources */
    MESHIF_ACCESS_MSG_STATUS_KEY_INDEX_ALREADY_STORED = 6,       /**< Key Index Already Stored */
    MESHIF_ACCESS_MSG_STATUS_INVALID_PUBLISH_PARAMS = 7,         /**< Invalid Publish Parameters */
    MESHIF_ACCESS_MSG_STATUS_NOT_A_SUBSCRIBE_MODEL = 8,          /**< Not a Subscribe Model */
    MESHIF_ACCESS_MSG_STATUS_STORAGE_FAILURE = 9,                /**< Storage Failure */
    MESHIF_ACCESS_MSG_STATUS_FEATURE_NOT_SUPPORTED = 10,         /**< Feature Not Supported */
    MESHIF_ACCESS_MSG_STATUS_CANNOT_UPDATE = 11,                 /**< Cannot Update */
    MESHIF_ACCESS_MSG_STATUS_CANNOT_REMOVE = 12,                 /**< Cannot Remove */
    MESHIF_ACCESS_MSG_STATUS_CANNOT_BIND = 13,                   /**< Cannot Bind */
    MESHIF_ACCESS_MSG_STATUS_TEMPORARILY_UNABLE_TO_CHANGE_STATE = 14,    /**< Temporarily Unable to Change State */
    MESHIF_ACCESS_MSG_STATUS_CANNOT_SET = 15,                    /**< Cannot Set */
    MESHIF_ACCESS_MSG_STATUS_UNSPECIFIED_ERROR = 16,             /**< Unspecified Error */
    MESHIF_ACCESS_MSG_STATUS_INVALID_BINDING = 17,               /**< Invalid Binding */
} meshif_access_msg_status_code_t;

typedef enum {
    MESHIF_DUMP_TYPE_UUID = 0,
    MESHIF_DUMP_TYPE_NETWORK,
    MESHIF_DUMP_TYPE_TRANSPORT,
    MESHIF_DUMP_TYPE_CONFIG,
    MESHIF_DUMP_TYPE_MODEL,
    MESHIF_DUMP_TYPE_LPN,
    MESHIF_DUMP_TYPE_PROXY,
    MESHIF_DUMP_TYPE_ADV,
    MESHIF_DUMP_TYPE_ALL,
} meshif_dump_type_t;

typedef enum {
    MESHIF_BLE_ADDR_TYPE_PUBLIC = 0,                /**< public address */
    MESHIF_BLE_ADDR_TYPE_RANDOM_STATIC = 1,         /**< random static address */
    MESHIF_BLE_ADDR_TYPE_RANDOM_RESOLVABLE = 2,     /**< random resolvable addresss */
    MESHIF_BLE_ADDR_TYPE_RANDOM_NON_RESOLVABLE = 3, /**< random non resolvable address */
} meshif_ble_addr_type_t;

typedef struct {
    meshif_ble_addr_type_t addr_type;               /**< address type */
    uint8_t addr[MESHIF_BLE_ADDR_LEN];              /**< address byte array */
} meshif_ble_addr_t;

typedef enum {
    MESHIF_GATT_SERVICE_PROXY,                      /**< Mesh proxy service */
    MESHIF_GATT_SERVICE_PROVISION,                  /**< Mesh provisioning service */
} meshif_gatt_service_t;

typedef struct {
    uint8_t isValidData;
    uint16_t keyidx;
    uint8_t key[MESHIF_KEY_SIZE];
    uint8_t ivphase;
    uint32_t ivIndex;
    uint8_t phase : 4;
    uint8_t node_identity : 4;
    uint8_t tmpkey[MESHIF_KEY_SIZE];
} meshif_netkey_flash_data_t;

typedef struct {
    uint8_t isValidData;
    uint16_t netkeyIdx;
    uint8_t lpn_addr[2];
    uint8_t friend_addr[2];
    uint8_t lpn_counter[2];
    uint8_t friend_counter[2];
} meshif_friend_flash_data_t;

typedef struct {
    uint8_t isValidData;
    uint16_t appkeyIdx;
    uint8_t key[MESHIF_KEY_SIZE];
    uint16_t netkeyIdx;
    uint8_t phase;
    uint8_t tmpkey[MESHIF_KEY_SIZE];
} meshif_appkey_flash_data_t;

typedef struct {
    uint8_t isValidData;
    uint16_t appkeyIdx;
    uint8_t idLength;
    uint32_t model_id;
    uint16_t unicast_addr;
} meshif_model_flash_data_t;

typedef struct {
    uint16_t addr;                           /**< publish address */
    uint16_t appkey_index : 12;              /**< AppKey index */
    uint16_t flag : 1;                       /**< friendship credentials flag */
    uint16_t rfu : 3;                        /**< reserve for future use. */
    uint8_t ttl;                             /**< publish TTL */
    uint8_t period;                          /**< publish period */
    uint8_t retransmit_count : 3;            /**< publish retransmit count */
    uint8_t retransmit_interval_steps : 5;   /**< Publish retransmit interval steps */
} meshif_ble_mesh_model_publication_t;

typedef struct {
    uint8_t isValidData;
    uint32_t model_id;
    uint16_t unicast_addr;
    meshif_ble_mesh_model_publication_t model_publication;
} meshif_model_publication_flash_data_t;

typedef struct {
    uint8_t isValidData;
    uint32_t model_id;
    uint16_t unicast_addr;
    uint16_t subscriptionAddr;
} meshif_model_subscription_flash_data_t;

typedef struct {
    uint8_t isValidData;
    uint8_t uuid[MESHIF_UUID_SIZE];
    uint8_t deviceKey[MESHIF_KEY_SIZE];
    uint16_t unicast_addr;
} meshif_device_flash_data_t;

typedef struct {
    uint8_t network_transmit_count : 3;             /**< Network transmit count */
    uint8_t network_transmit_interval_steps : 5;    /**< Network transmit interval steps */
} meshif_network_transmit_t;

typedef struct {
    uint8_t relay_retransmit_count : 3;             /**< Relay retransmit count */
    uint8_t relay_retransmit_interval_steps : 5;    /**< Relay retransmit interval steps */
} meshif_relay_retransmit_t;

typedef struct {
    uint8_t isValidData;
    uint8_t secureNetworkBeacon;
    uint8_t defaultTTL;
    uint8_t gattProxy;
    uint8_t mesh_friend;
    uint8_t relay;
    uint8_t nodeIdentity;
    uint8_t keyRefreshPhase;
    uint16_t unicastAddr;
    meshif_heartbeat_publication_t heartbeatPublication;
    meshif_heartbeat_subscription_t heartbeatSubscription;
    meshif_network_transmit_t networkTransmit;
    meshif_relay_retransmit_t relayRetransmit;
} meshif_configuration_server_flash_data_t;

typedef struct {
    uint8_t fastPeriod;
} meshif_health_period_t;

typedef struct {
    uint8_t isValidData;
    meshif_health_period_t healthPeriod;
    uint8_t attention;
    uint16_t appkeyIdx;
    uint16_t unicastAddr;
} meshif_health_server_flash_data_t;

typedef struct {
    uint8_t isValidData;
    uint32_t seq_num;
} meshif_seqnum_flash_data_t;

typedef struct {
    meshif_netkey_flash_data_t               netkey[MESHIF_NET_KEY_RECORD_NUMBER];
    meshif_friend_flash_data_t               mesh_friend[MESHIF_FRIEND_RECORD_NUMBER];
    meshif_appkey_flash_data_t               appkey[MESHIF_APP_KEY_RECORD_NUMBER];
    meshif_model_flash_data_t                model[MESHIF_MODEL_RECORD_NUMBER];
    meshif_model_publication_flash_data_t    publication[MESHIF_MODEL_PUBLICATION_RECORD_NUMBER];
    meshif_model_subscription_flash_data_t   subscription[MESHIF_MODEL_SUBSCRIPTION_RECORD_NUMBER];
    meshif_device_flash_data_t               local_deviceInfo[MESHIF_LOCAL_DEVICE_INFO_RECORD_NUMBER];
    meshif_seqnum_flash_data_t               seq_info[MESHIF_SEQUENCE_NUMBER_RECORD_NUMBER];
} meshif_record;

//////////////////meshif internal use

typedef struct model_handler_entry_t{
    uint16_t model_handle;                  //the model handle for this entry
    uint8_t opcode_cnt;                     //The opcode count
    uint16_t opcode;                        //opcode for this entry
    uint16_t company_id;                    //company id for this operation
    meshif_access_msg_handler   handler;    //handler for this model or for specific opcode
    meshif_access_publish_timeout_cb_t publish_timeout_cb;  //callback for publish timeout
    void *handlers_mem;                     //handlers_mem shall be freed in deinit(), all the handler entry for the same model handle share the same handlers_mem, it shall NOT be double freed!!!
} meshif_model_handler_entry_t;

typedef enum
{
    MESHIF_MODEL_GENERIC_ONOFF_SET,
    MESHIF_MODEL_GENERIC_ONOFF_GET,
    MESHIF_MODEL_LIGHT_CTL_SET,
    MESHIF_MODEL_LIGHT_CTL_GET,
    MESHIF_MODEL_LIGHT_CTL_TEMPERATURE_SET,
    MESHIF_MODEL_LIGHT_CTL_TEMPERATURE_GET,
    MESHIF_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_SET,
    MESHIF_MODEL_LIGHT_CTL_TEMPERATURE_RANGE_GET,
    MESHIF_MODEL_LIGHT_CTL_DEFAULT_SET,
    MESHIF_MODEL_LIGHT_CTL_DEFAULT_GET,
} meshif_model_operation_type_t;

typedef struct
{
    meshif_model_operation_type_t op_type;
    uint16_t model_handle;
    bool reliable;
} meshif_model_operation_t;

typedef struct
{
    uint16_t element_idx;
    uint32_t model_id;
}ble_mesh_extend_model_element_info_t;

typedef enum
{
    MESHIF_OTA_INITIATOR_OP_REG_MSG_HANDLER = 0,
    MESHIF_OTA_INITIATOR_OP_FW_INFO_GET,
    MESHIF_OTA_INITIATOR_OP_START_DISTRIBUTION,
    MESHIF_OTA_INITIATOR_OP_STOP_DISTRIBUTION,
    MESHIF_OTA_INITIATOR_OP_APPLY_DISTRIBUTION,
} meshif_ota_initiator_opcode_t;

typedef struct
{
    uint16_t appkey_index;            /**< appkey index for ota msg. */
    meshif_access_msg_handler   ota_msg_handler;    /**< The message handler for ota msg. */
}meshif_ota_initiator_msg_handler_t;

typedef struct
{
    uint16_t node_addr;
}meshif_ota_initiator_fw_info_get_t;

typedef struct
{
    char obj_file[128];      //the path of the new firmware image object. e.g. /data/new_firmware.bin
    uint32_t obj_size;       //size of the image data which should be transferred
    uint8_t obj_id[8];       //unique object id
    uint32_t new_fw_id;         //fw version
    uint16_t appkey_index;      //the appkey index used for ota related msg, this appkey shall have been added to updaters.
    uint16_t distributor_addr;  //The address of the distributor node, currently, it is the same with initiator(self)
    uint16_t group_addr;        //the group address for the updaters
    uint16_t updaters_num;       //Number of updaters
    uint16_t *updaters;         //the address list of all the updaters in the group
    bool manual_apply;  //manual_apply or auto apply immediately when dfu ready
} meshif_ota_initiator_start_params_t;

typedef struct
{
    uint32_t new_fw_id;
    uint16_t distributor_addr;  //The address of the distributor node, currently, it is the same with initiator
} meshif_ota_initiator_stop_params_t;

typedef struct
{
    uint16_t opcode;    /**< The operation code information */
    union
    {
        meshif_ota_initiator_msg_handler_t msg_handler;
        meshif_ota_initiator_fw_info_get_t fw_info_get;
        meshif_ota_initiator_start_params_t start_params;
        meshif_ota_initiator_stop_params_t stop_params;
    }params;
} meshif_ota_operation_params_t;

typedef struct
{
    uint8_t sn_increase_flag;            /**< 0: sn not increase,  1: sn increase*/
    uint32_t sn_increase_interval;          /**< seq number increase interval, unit: ms*/
    uint16_t adv_interval;          /**< adv  interval in 625 us units*/
    uint32_t adv_period;            /**< adv period in milliseconds*/
}meshif_special_pkt_params_t;

typedef enum
{
    MESHIF_MESH_MODE_OFF = 0,  //normal mode
    MESHIF_MESH_MODE_ON = 1,   //mesh working mode
    MESHIF_MESH_MODE_STANDBY = 2,  //standby mode
} meshif_mesh_mode_t;

typedef enum
{
    MESHIF_GATT_EVT_UNEXPECTED_RESPONSE_FROM_SERVER = MESHIF_EVENT_GATT,
    MESHIF_GATT_EVT_GET_ERROR_RESPONSE_FROM_SERVER,
    MESHIF_GATT_EVT_MTU_EXCHANGED,
    MESHIF_GATT_EVT_PRIM_SERVICE_DISCOVERED,
    MESHIF_GATT_EVT_UUID_SERVICE_DISCOVERED,
    MESHIF_GATT_EVT_INCLUDED_SERVICE_FOUND,
    MESHIF_GATT_EVT_SERVICE_CHAR_DISCOVERED,
    MESHIF_GATT_EVT_UUID_CHAR_DISCOVERED,
    MESHIF_GATT_EVT_CHAR_DESCRIP_DISCOVERED,
    MESHIF_GATT_EVT_CHAR_VALUE_READ,
    MESHIF_GATT_EVT_UUID_CHAR_READ,
    MESHIF_GATT_EVT_LONG_CHAR_READ,
    MESHIF_GATT_EVT_MULTI_CHAR_READ,
    MESHIF_GATT_EVT_CHAR_VALUE_WRITTEN,
    MESHIF_GATT_EVT_LONG_CHAR_WRITTEN,
    MESHIF_GATT_EVT_RELIABLE_WRITE_FINISHED,
    MESHIF_GATT_EVT_CHAR_DESCRIP_READ,
    MESHIF_GATT_EVT_LONG_DESCRIP_READ,
    MESHIF_GATT_EVT_CHAR_DESCRIP_WRITTEN,
    MESHIF_GATT_EVT_LONG_DESCRIP_WRITTEN,
    MESHIF_GATT_EVT_HANDLE_VALUE_REPORTED,

}meshif_gatt_event_id;

typedef struct
{
    uint8_t err_code;       /**  standard attribute protocol error code returned by gatt server*/
} __attribute__((packed)) meshif_gatt_evt_err_rsponse_t;

typedef struct
{
    uint16_t new_mtu;       /**  new mtu size negotiated with gatt server*/
    uint16_t conn_handle;   /**  connection handle of this exchanged MTU*/
} __attribute__((packed)) meshif_gatt_evt_mtu_exchange_t;

typedef struct
{
    uint8_t num;                    /**   number of services in this event*/
    struct
    {
        uint16_t handle_start;      /**   handle of the first attribute in this service*/
        uint16_t handle_end;        /**   handle of the last attribute in this service*/
        uint8_t  uuid_len;          /**   uuid length of this service */
        uint8_t  uuid[16];          /**   uuid of this service */
    }__attribute__((packed))service[1];
} __attribute__((packed)) meshif_gatt_evt_prim_service_discovered_t;

typedef struct
{
    uint8_t num;                    /**   number of services in this event*/
    uint8_t uuid_len;               /**   uuid length of these services */
    uint8_t uuid[16];               /**   uuid of these services */
    struct
    {
        uint16_t handle_start;      /**   handle of the first attribute in this service*/
        uint16_t handle_end;        /**   handle of the last attribute in this service*/
    }__attribute__((packed))service[1];
} __attribute__((packed))meshif_gatt_evt_uuid_service_discovered_t;

typedef struct
{
    uint8_t num;                    /** number of included services in this event  */
    struct
    {
        uint16_t handle;            /**   handle of the include declaration attribute */
        uint16_t service_start;     /**   handle of the first attribute of the included service*/
        uint16_t service_end;       /**   handle of the last attribute of the included service*/
        uint8_t  service_uuid_len;  /**   uuid length of the included service */
        uint8_t  service_uuid[16];  /**   uuid of the included service */
    }__attribute__((packed))include[1];
} __attribute__((packed))meshif_gatt_evt_included_service_found_t;

typedef struct
{
    uint8_t num;                    /** number of characteristics in this event  */
    struct
    {
        uint16_t declaration_handle;    /** handle of the characteristic declaration*/
        uint16_t value_handle;          /** handle of the characteristic value */
        uint8_t  property;              /** characteristic property */
        uint8_t  uuid_len;              /** uuid length of the characteristic */
        uint8_t  uuid[16];              /** uuid of the characteristic */
    }__attribute__((packed))charact[1];
} __attribute__((packed))meshif_gatt_evt_service_char_discovered_t;

typedef struct
{
    uint8_t num;                /** number of characteristics in this event  */
    uint8_t uuid_len;           /** uuid length of these characteristics */
    uint8_t uuid[16];           /** uuid of these characteristics */
    struct
    {
        uint16_t declaration_handle;    /** handle of the characteristic declaration*/
        uint16_t value_handle;          /** handle of the characteristic value */
        uint8_t  property;              /** characteristic property */
    }__attribute__((packed))charact[1];
} __attribute__((packed))meshif_gatt_evt_uuid_char_discovered_t;

typedef struct
{
    uint8_t num;                /** number of descriptors in this event  */
    struct
    {
        uint16_t handle;        /** handle of the descriptor */
        uint8_t  uuid_len;      /** uuid length of this descriptor */
        uint8_t  uuid[16];      /** uuid of this descriptor */
    }__attribute__((packed))descrip[1];
} __attribute__((packed))meshif_gatt_evt_char_descrip_discovered_t;

typedef struct
{
    uint16_t handle;            /** handle of the characteristic */
    uint16_t len;               /** attribute value length */
    uint8_t  value[1];          /** attribute value */
} __attribute__((packed))meshif_gatt_evt_char_value_read_t;

typedef struct
{
    uint8_t num;                /** number of characteristics read in this event  */
    uint8_t uuid_len;           /** designated uuid length  */
    uint8_t uuid[16];           /** designated uuid */
    struct
    {
        uint16_t handle;        /** handle of the characteristic */
        uint8_t  len;           /** attribute value length */
        uint8_t  value[19];     /** attribute value (19 bytes at most)*/
    }__attribute__((packed))charact[1];
} __attribute__((packed))meshif_gatt_evt_uuid_char_read_t;

typedef struct
{
    uint16_t handle;            /** handle of the characteristic */
    uint16_t len;               /** attribute value length */
    uint8_t  value[1];          /** attribute value */
} __attribute__((packed))meshif_gatt_evt_long_char_read_t;

typedef struct
{
    uint8_t  num;               /** number of characteristics read in this event  */
    uint16_t handle[5];         /** array of characteristic handle */
    uint16_t len;               /** length of packed characteristic values */
    uint8_t  value[1];          /** packed characteristic values */
} __attribute__((packed))meshif_gatt_evt_multi_char_read_t;

typedef struct
{
    uint16_t handle;            /** handle of the characteristic which was written successfully*/
} __attribute__((packed))meshif_gatt_evt_char_value_written_t;

typedef struct
{
    uint16_t handle;            /** handle of the characteristic which was written successfully*/
    uint16_t len;               /** length of data */
} __attribute__((packed))meshif_gatt_evt_long_char_written_t;

typedef struct
{
    uint8_t is_cancelled;       /** 0: data was written successfully. \n  1: write procedure was canceled since returned data is different from original request*/
    uint8_t num;                /** number of successful writes. */
} __attribute__((packed))meshif_gatt_evt_reliable_write_finished_t;

typedef struct
{
    uint16_t handle;            /** handle of the descriptor */
    uint16_t len;               /** attribute value length */
    uint8_t  value[1];          /** attribute value */
} __attribute__((packed))meshif_gatt_evt_char_descrip_read_t;

typedef struct
{
    uint16_t handle;            /** handle of the characteristic */
    uint16_t len;               /** attribute value length */
    uint8_t  value[1];          /** attribute value */
} __attribute__((packed))meshif_gatt_evt_long_descrip_read_t;

typedef struct
{
    uint16_t handle;            /** handle of the descriptor which was written successfully*/
} __attribute__((packed))meshif_gatt_evt_char_descrip_written_t;

typedef struct
{
    uint16_t handle;            /** handle of the descriptor which was written successfully*/
    uint16_t len;               /** length of data */
} __attribute__((packed))meshif_gatt_evt_long_descrip_written_t;

typedef struct
{
    uint16_t conn_handle;       /** connection handle of this report*/
    uint16_t handle;            /** attribute handle of this report */
    uint16_t len;               /** attribute value length */
    uint8_t  value[1];          /** attribute value */
} __attribute__((packed))meshif_gatt_evt_handle_value_reported_t;

typedef struct
{
    union                                                     ///< union alternative identified by evt_id in enclosing struct.
    {
        meshif_gatt_evt_err_rsponse_t                err_response;                 ///<  parameter of gatt event @ref BLE_GATT_EVT_GET_ERROR_RESPONSE_FROM_SERVER
        meshif_gatt_evt_mtu_exchange_t               mtu_exchanged;                ///<  parameter of gatt event @ref BLE_GATT_EVT_MTU_EXCHANGED
        meshif_gatt_evt_prim_service_discovered_t    prim_service_discovered;      ///<  parameter of gatt event @ref BLE_GATT_EVT_PRIM_SERVICE_DISCOVERED
        meshif_gatt_evt_uuid_service_discovered_t    uuid_service_discovered;      ///<  parameter of gatt event @ref BLE_GATT_EVT_UUID_SERVICE_DISCOVERED
        meshif_gatt_evt_included_service_found_t     included_service_found;       ///<  parameter of gatt event @ref BLE_GATT_EVT_INCLUDED_SERVICE_FOUND
        meshif_gatt_evt_service_char_discovered_t    service_char_discovered;      ///<  parameter of gatt event @ref BLE_GATT_EVT_SERVICE_CHAR_DISCOVERED
        meshif_gatt_evt_uuid_char_discovered_t       uuid_char_discovered;         ///<  parameter of gatt event @ref BLE_GATT_EVT_UUID_CHAR_DISCOVERED
        meshif_gatt_evt_char_descrip_discovered_t    char_descrip_discovered;      ///<  parameter of gatt event @ref BLE_GATT_EVT_CHAR_DESCRIP_DISCOVERED
        meshif_gatt_evt_char_value_read_t            char_value_read;              ///<  parameter of gatt event @ref BLE_GATT_EVT_CHAR_VALUE_READ
        meshif_gatt_evt_uuid_char_read_t             uuid_char_read;               ///<  parameter of gatt event @ref BLE_GATT_EVT_UUID_CHAR_READ
        meshif_gatt_evt_long_char_read_t             long_char_read;               ///<  parameter of gatt event @ref BLE_GATT_EVT_LONG_CHAR_READ
        meshif_gatt_evt_multi_char_read_t            multi_char_read;              ///<  parameter of gatt event @ref BLE_GATT_EVT_MULTI_CHAR_READ
        meshif_gatt_evt_char_value_written_t         char_value_written;           ///<  parameter of gatt event @ref BLE_GATT_EVT_CHAR_VALUE_WRITTEN
        meshif_gatt_evt_long_char_written_t          long_char_written;            ///<  parameter of gatt event @ref BLE_GATT_EVT_LONG_CHAR_WRITTEN
        meshif_gatt_evt_reliable_write_finished_t    reliable_write_finished;      ///<  parameter of gatt event @ref BLE_GATT_EVT_RELIABLE_WRITE_FINISHED
        meshif_gatt_evt_char_descrip_read_t          char_descrip_read;            ///<  parameter of gatt event @ref BLE_GATT_EVT_CHAR_DESCRIP_READ
        meshif_gatt_evt_long_descrip_read_t          long_descrip_read;            ///<  parameter of gatt event @ref BLE_GATT_EVT_LONG_DESCRIP_READ
        meshif_gatt_evt_char_descrip_written_t       char_descrip_written;         ///<  parameter of gatt event @ref BLE_GATT_EVT_CHAR_DESCRIP_WRITTEN
        meshif_gatt_evt_long_descrip_written_t       long_descrip_written;         ///<  parameter of gatt event @ref BLE_GATT_EVT_LONG_DESCRIP_WRITTEN
        meshif_gatt_evt_handle_value_reported_t      handle_value_reported;        ///<  parameter of gatt event @ref BLE_GATT_EVT_HANDLE_VALUE_REPORTED
    } __attribute__((packed)) gatt;                                                                 ///< Event Parameters.
}  __attribute__((packed)) meshif_gatt_evt_t;

/** Represents the standard Bluetooth MESH interface. */
typedef struct {
    /** Set to sizeof(btmesh_interface_t) */
    size_t          size;

    /**
     * Initializes the interface and provides callback routines
     */
    meshif_status_code_t (*init)(meshif_bt_event_handler callbacks);

    meshif_status_code_t (*deinit)(void);

    bool (*set_model_data)(meshif_model_data_t *md);

    meshif_status_code_t (*enable) (const meshif_init_params_t *init_params);

    meshif_status_code_t (*disable) (void);

    meshif_access_msg_status_code_t (*set_netkey) (meshif_netkey_t *netkey);

    meshif_access_msg_status_code_t (*set_appkey) (meshif_appkey_t *appkey);

    void (*unprov_dev_scan) (bool start, uint32_t duration);

    meshif_status_code_t (*invite_provisioning) (uint8_t *target_uuid, meshif_prov_invite_t *invite);

    meshif_status_code_t (*start_provisioning) (meshif_prov_provisioner_params_t *params, uint8_t mode);

    meshif_status_code_t (*set_prov_factor) (meshif_prov_factor_t *prov_factor);

    meshif_status_code_t (*model_cc_msg_tx) (meshif_configuration_msg_tx_t *msg);

    meshif_status_code_t (*send_packet) (const meshif_tx_params_t *tx_params);

    meshif_status_code_t (*model_publish) (uint16_t model_handle, const meshif_access_message_tx_t *msg);

    meshif_status_code_t (*set_mesh_mode) (meshif_mesh_mode_t mode);

    bool (*data_reset) (uint32_t record);

    bool (*data_save) (void);

    bool (*data_set) (meshif_record *data);

    void (*version) (char *buf);

    void (*dump) (meshif_dump_type_t type);

    uint16_t (*get_element_address) (uint16_t element_index);

    void (*set_default_ttl) (uint8_t ttl);

    uint8_t (*get_default_ttl) (void);

    meshif_access_msg_status_code_t (*set_iv) (meshif_iv_t *iv);

    meshif_access_msg_status_code_t (*set_device_info) (meshif_device_info_t *deviceInfo);

    meshif_access_msg_status_code_t (*model_app_bind) (uint16_t model_handle, uint16_t appkey_index);

    meshif_status_code_t (*access_model_reply) (uint16_t model_handle, meshif_access_message_rx_t *msg, meshif_access_message_tx_t *reply);

    void (*log_setlevel) (uint32_t level);

    meshif_status_code_t (*set_heartbeat_period) (uint8_t num, uint32_t hb_timeout);

    meshif_access_msg_status_code_t (*ota_initiator_operation) (meshif_ota_operation_params_t *params);

    void (*ota_get_client_model_handle) (uint16_t *dist_client, uint16_t *update_client);

    uint16_t (*get_model_handle_by_elementIdx_and_modeId) (uint32_t model_id, uint16_t element_idx);

    meshif_status_code_t (*meshif_bearer_adv_set_params) (meshif_bearer_adv_params_t *adv_params, meshif_bearer_scan_params_t *scan_params);

    meshif_status_code_t (*meshif_set_special_pkt_params) (meshif_special_pkt_params_t *pkt_params);

    /* MTK GATT bearer */
    meshif_status_code_t (*gatt_connect) (meshif_ble_addr_t *bd_addr, meshif_gatt_service_t type);

    meshif_status_code_t (*gatt_disconnect) (void);

    /** Closes the interface */
    void (*cleanup)( void );

} btmesh_interface_t;

__END_DECLS

#endif /* ANDROID_INCLUDE_BT_MESH_H */
