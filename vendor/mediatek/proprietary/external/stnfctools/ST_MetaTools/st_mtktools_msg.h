/******************************************************************************
 *
 *  Copyright (C) 2016 ST Microelectronics S.A.
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

#ifndef _ST_MTKTOOLS_MSG_H
#define _ST_MTKTOOLS_MSG_H

// This file contains definitions of the protocol specific to Mediatek tools
// (EM/FM/Meta/...) Not to be shared outside of ST and Mediatek.

#define MTK_SOCKET_NAME "/data/mtknfc_server"
#define MTK_SOCKET_NAME_EM "/data/nfc_socket/mtknfc_server"
#define MTK_SOCKET_NAME_FM "/data/vendor/nfc_socket/mtknfc_server"

// generic type to be cast afterwards
struct mtk_msg_hdr {
  uint32_t cmd_le;
  uint32_t len_le;
  // followed by (len) bytes of payload depending on message type.
};

// function that converts little-endian uint32_t to host integers and the other
// direction.
static __inline__ uint32_t le_to_h_32(uint32_t le) {
  // we assume the host is LE at the moment. we can revise later.
  return le;
}
// function that converts little-endian uint16_t to host integers and the other
// direction.
static __inline__ uint16_t le_to_h_16(uint16_t le) {
  // we assume the host is LE at the moment. we can revise later.
  return le;
}

// any message showing a length bigger than this triggers a disconnect:
#define MAX_MSG_SIZE 2048

// some of the commands have a timeout (e.g. read-dep in FM mode). Value in
// seconds.
#define W84_TIMEOUT 60

// Some definitions imported from MTK source code:
enum {
  // meta and engineering modes
  MTK_NFC_EM_START_CMD = 100,
  MTK_NFC_EM_ALS_READER_MODE_REQ,
  MTK_NFC_EM_ALS_READER_MODE_RSP,
  MTK_NFC_EM_ALS_READER_MODE_OPT_REQ,
  MTK_NFC_EM_ALS_READER_MODE_OPT_RSP,
  MTK_NFC_EM_ALS_P2P_MODE_REQ,
  MTK_NFC_EM_ALS_P2P_MODE_RSP,
  MTK_NFC_EM_ALS_CARD_MODE_REQ,
  MTK_NFC_EM_ALS_CARD_MODE_RSP,
  MTK_NFC_EM_POLLING_MODE_REQ,
  MTK_NFC_EM_POLLING_MODE_RSP,
  MTK_NFC_EM_TX_CARRIER_ALS_ON_REQ,
  MTK_NFC_EM_TX_CARRIER_ALS_ON_RSP,
  MTK_NFC_EM_VIRTUAL_CARD_REQ,
  MTK_NFC_EM_VIRTUAL_CARD_RSP,
  MTK_NFC_EM_PNFC_CMD_REQ,  // NOT IMPLEMENTED
  MTK_NFC_EM_PNFC_CMD_RSP,  // NOT IMPLEMENTED
  MTK_NFC_EM_POLLING_MODE_NTF,
  MTK_NFC_EM_ALS_READER_MODE_NTF,
  MTK_NFC_EM_ALS_P2P_MODE_NTF,
  MTK_NFC_EM_STOP_CMD,  // 120

  MTK_NFC_TESTMODE_SETTING_REQ = 127,
  MTK_NFC_TESTMODE_SETTING_RSP,
  MTK_EM_LOOPBACK_TEST_REQ,
#define MTK_NFC_EM_LOOPBACK_TEST_REQ MTK_EM_LOOPBACK_TEST_REQ
  MTK_EM_LOOPBACK_TEST_RSP,
#define MTK_NFC_EM_LOOPBACK_TEST_RSP MTK_EM_LOOPBACK_TEST_RSP
  MTK_NFC_SW_VERSION_QUERY,
  MTK_NFC_SW_VERSION_RESPONSE,

  MTK_NFC_EM_DEACTIVATE_CMD = 135,

  // factory mode
  MTK_NFC_FM_SWP_TEST_REQ = 201,
  MTK_NFC_FM_SWP_TEST_NTF,
  MTK_NFC_FM_SWP_TEST_RSP,
  MTK_NFC_FM_READ_UID_TEST_REQ,  // NOT IMPLEMENTED
  MTK_NFC_FM_READ_UID_TEST_RSP,  // NOT IMPLEMENTED
  MTK_NFC_FM_READ_DEP_TEST_REQ,
  MTK_NFC_FM_READ_DEP_TEST_RSP,
  MTK_NFC_FM_CARD_MODE_TEST_REQ,
  MTK_NFC_FM_CARD_MODE_TEST_RSP,

  MTK_NFC_FM_VIRTUAL_CARD_REQ = 212,
  MTK_NFC_FM_VIRTUAL_CARD_RSP,

  MTK_NFC_FM_ANTENNA_TEST_REQ = 220,
  MTK_NFC_FM_ANTENNA_TEST_RSP,

  MTK_NFC_META_GET_SELIST_REQ = 1017,
  MTK_NFC_META_GET_SELIST_RSP,

};

/* BITMAP OF EM_ALS_READER_M_TYPE*/
#define EM_ALS_READER_M_TYPE_A (1 << 0)
#define EM_ALS_READER_M_TYPE_B (1 << 1)
#define EM_ALS_READER_M_TYPE_F (1 << 2)
#define EM_ALS_READER_M_TYPE_V (1 << 3)
#define EM_ALS_READER_M_TYPE_Bprime (1 << 4)  // unsupported
#define EM_ALS_READER_M_TYPE_KOVIO (1 << 5)

///* BITMAP OF EM_ALS_CARD_M_TYPE*/
//#define EM_ALS_CARD_M_TYPE_A        (1 << 0)
//#define EM_ALS_CARD_M_TYPE_B        (1 << 1)
//#define EM_ALS_CARD_M_TYPE_BPrime   (1 << 2)
//#define EM_ALS_CARD_M_TYPE_F212     (1 << 3)
//#define EM_ALS_CARD_M_TYPE_F424     (1 << 4)

/* BITMAP OF EM_ALS_READER_M_SPDRATE*/
#define EM_ALS_READER_M_SPDRATE_106 (1 << 0)
#define EM_ALS_READER_M_SPDRATE_212 (1 << 1)
#define EM_ALS_READER_M_SPDRATE_424 (1 << 2)
#define EM_ALS_READER_M_SPDRATE_848 (1 << 3)
#define EM_ALS_READER_M_SPDRATE_662 (1 << 4)
#define EM_ALS_READER_M_SPDRATE_2648 (1 << 5)

/* BITMAP OF EM_ALS_CARD_M_SW_NUM */
#define EM_ALS_CARD_M_SW_NUM_SWIO1 (1 << 0)
#define EM_ALS_CARD_M_SW_NUM_SWIO2 (1 << 1)
#define EM_ALS_CARD_M_SW_NUM_SWIOSE (1 << 2)

/* BITMAP OF EM_ENABLE_FUNC */
#define EM_ENABLE_FUNC_READER_MODE (1 << 0)
#define EM_ENABLE_FUNC_CARD_MODE (1 << 1)
#define EM_ENABLE_FUNC_P2P_MODE (1 << 2)

/* BITMAP OF EM_P2P_MODE */
#define EM_P2P_MODE_PASSIVE_MODE (1 << 0)
#define EM_P2P_MODE_ACTIVE_MODE (1 << 1)

/* BITMAP OF EM_P2P_ROLE */
#define EM_P2P_ROLE_INITIATOR_MODE (1 << 0)
#define EM_P2P_ROLE_TARGET_MODE (1 << 1)

enum {
  NFC_EM_ACT_START = 0,
  NFC_EM_ACT_STOP,
};

enum {
  NFC_EM_OPT_ACT_READ = 0,
  NFC_EM_OPT_ACT_WRITE,
  NFC_EM_OPT_ACT_FORMAT,
  //   NFC_EM_OPT_ACT_WRITE_RAW
};

enum {
  nfc_ndef_type_uri = 0,
  nfc_ndef_type_text,
  nfc_ndef_type_sp,
  nfc_ndef_type_others
};

enum {
  nfc_ndef_lang_DEFAULT = 0,
  nfc_ndef_lang_DE = 1,
  nfc_ndef_lang_EN = 2,
  nfc_ndef_lang_FR = 3
};

enum { MTK_RESULT_SUCCESS = 0, MTK_RESULT_FAIL };

enum SE_STATUS { DISABLE = 0, ENABLE, SE_STATUS_ENUM_END };

enum SE_TYPE { UICC = 0, EMBEDDED_SE, uSD_CARD, SE_TYPE_ENUM_END };

enum SE_CONNECT_TYPE {
  SE_OFF = 0,
  SE_CONTACTLESS,
  SE_HOST_ACCESS,
  SE_ALL,
  SE_CONNECT_ENUM_END
};

// All structures below need to be packed
#pragma pack(push, 1)

// Additional definitions of the messages
struct mtk_msg_EM_ALS_READER_MODE_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;  // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, bit 3: NFC V,
                     // bit 5: KOVIO -> EM_ALS_READER_M_TYPE
  uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t type_b_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t
      type_v_rate_le;  // bit 4: 662, bit 5: 2648 -> EM_ALS_READER_M_SPDRATE_
  uint32_t
      type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
  uint32_t type_v_subcar_le;  // SubCarrier = 0, DualSubCarrier = 1
  uint32_t type_v_coding_le;  // 1of4 = 0, 1of256 = 1
};

struct mtk_msg_EM_ALS_READER_MODE_REQ_legacy {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;  // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, bit 3: NFC V,
                     // bit 5: KOVIO -> EM_ALS_READER_M_TYPE
  uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t type_b_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t
      type_v_rate_le;  // bit 4: 662, bit 5: 2648 -> EM_ALS_READER_M_SPDRATE_
  uint32_t
      type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
  uint32_t type_v_subcar_le;  // SubCarrier = 0, DualSubCarrier = 1
};

struct mtk_msg_EM_ALS_READER_MODE_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_EM_ALS_READER_MODE_NTF {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;      // CONNECT=0, FAIL=1, DISCONNECT = 2
  uint32_t is_ndef_le;     // non NDEF = 0, NDEF = 1, READ_ONLY = 2, FAIL = 3,
                           // DISCONNECT=4
  uint32_t uidlen_le;      // length of the UID
  uint8_t uid[32];         // UID value ...
  uint8_t discovery_type;  // one of the  NFC_DISCOVERY_TYPE_* defined in
                           // external/libnfc-nci/src/nfc/include/nfc_api.h
  uint8_t bitrate;         // one of the NFC_BIT_RATE_* defined in
                           // external/libnfc-nci/src/nfc/include/nfc_api.h
};

struct mtk_msg_EM_ALS_READER_MODE_OPT_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // READ=0, WRITE=1, FORMAT =2 -> NFC_EM_OPT_ACT_
  union {
    struct {
      uint32_t type_le;     // URI:0, TEXT:1, SP:2, Other:3 -> nfc_ndef_type_*
      uint32_t lang_le;     // default:0, DE:1, EN:2, FR:3  -> nfc_ndef_lang_*
      uint32_t datalen_le;  // this header is followed by this amount of bytes
      uint8_t data[512 + 66];  // it is actually always this size apparently...
                               // can be changed if needed later
    } write;
    // other operations don t have a payload.
  } payload;
  // nothing can come after this definition because the payload.write.data will
  // be actually larger.
};

#define NDEF_DATA_LEN (512)

struct mtk_ndef_read_payload {
  uint32_t type_le;  // URI:0, TEXT:1, SP:2, Other:3 -> nfc_ndef_type_*
  uint8_t lang[3];   // string of the lang "DE", etc.
  uint8_t recflag;
  uint8_t recid[32];
  uint8_t rectnf;
  uint32_t datalen_le;          // the actual len of the data below
  uint8_t data[NDEF_DATA_LEN];  // this seems to be constant len in MTK client
                                // tools.
};

struct mtk_msg_EM_ALS_READER_MODE_OPT_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
  union {
    struct mtk_ndef_read_payload read;
    // other operations don t have a payload.
  } payload;
  // nothing can come after this definition because the payload.read.data will
  // be actually larger.
};

struct mtk_msg_EM_ALS_P2P_MODE_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;    // bit 0: NFC A, bit 2: NFC F -> EM_ALS_READER_M_TYPE
  uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t
      type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
  uint32_t mode_le;  // bit 1: Passive mode, bit 2: Active mode -> EM_P2P_MODE_
  uint32_t role_le;  // bit 0: initiator, bit 1: target -> EM_P2P_ROLE_
  uint32_t disable_ce_le;  // Not_disable = 0, disable = 1
};

struct mtk_msg_EM_ALS_P2P_MODE_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_EM_ALS_P2P_MODE_NTF {
  struct mtk_msg_hdr hdr;
  uint32_t status_le;      // LLCP up = 0; LLCP down = 1
  uint8_t discovery_type;  // one of the  NFC_DISCOVERY_TYPE_* defined in
                           // external/libnfc-nci/src/nfc/include/nfc_api.h
  uint8_t bitrate;         // one of the NFC_BIT_RATE_* defined in
                           // external/libnfc-nci/src/nfc/include/nfc_api.h
  uint8_t role;            // 0: is target; 1: is initiator
};

struct mtk_msg_EM_ALS_CARD_MODE_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t swionr_le;  // UICC1 = 1, UICC2 = 2, eSE = 3
  uint32_t type_le;    // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, (bit 4 : B')
                       // -> EM_ALS_READER_M_TYPE
  uint32_t hce_le;     // 0: not virtual. 1: virtual (not used).
};

struct mtk_msg_EM_ALS_CARD_MODE_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_EM_POLLING_MODE_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;    // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t phase_le;     // RFU
  uint32_t period_le;    // polling loop duration.
  uint32_t function_le;  // bit 0: reader mode; bit 1: card emul, bit 2: p2p ->
                         // EM_ENABLE_FUNC_
  struct {
    uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
    uint32_t type_le;    // bit 0: NFC A, bit 2: NFC F -> EM_ALS_READER_M_TYPE
    uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848
                              // -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        mode_le;  // bit 1: Passive mode, bit 2: Active mode -> EM_P2P_MODE_
    uint32_t role_le;        // bit 0: initiator, bit 1: target -> EM_P2P_ROLE_
    uint32_t disable_ce_le;  // Not_disable = 0, disable = 1
  } p2p;
  struct {
    uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
    uint32_t swionr_le;  // UICC1 = 1, UICC2 = 2, eSE = 3
    uint32_t type_le;  // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, (bit 4 : B')
                       // -> EM_ALS_READER_M_TYPE
    uint32_t hce_le;   // 0: not virtual. 1: virtual (not used).
  } ce;
  struct {
    uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
    uint32_t type_le;    // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, bit 3: NFC
                         // V, bit 5: KOVIO -> EM_ALS_READER_M_TYPE
    uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848
                              // -> EM_ALS_READER_M_SPDRATE_
    uint32_t type_b_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848
                              // -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        type_v_rate_le;  // bit 4: 662, bit 5: 2648 -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
    uint32_t type_v_subcar_le;  // SubCarrier = 0, DualSubCarrier = 1
    uint32_t type_v_coding_le;  // 1of4 = 0, 1of256 = 1
  } reader;
};

struct mtk_msg_EM_POLLING_MODE_REQ_legacy {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;    // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t phase_le;     // RFU
  uint32_t period_le;    // polling loop duration.
  uint32_t function_le;  // bit 0: reader mode; bit 1: card emul, bit 2: p2p ->
                         // EM_ENABLE_FUNC_
  struct {
    uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
    uint32_t type_le;    // bit 0: NFC A, bit 2: NFC F -> EM_ALS_READER_M_TYPE
    uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848
                              // -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        mode_le;  // bit 1: Passive mode, bit 2: Active mode -> EM_P2P_MODE_
    uint32_t role_le;        // bit 0: initiator, bit 1: target -> EM_P2P_ROLE_
    uint32_t disable_ce_le;  // Not_disable = 0, disable = 1
  } p2p;
  struct {
    uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
    uint32_t swionr_le;  // UICC1 = 1, UICC2 = 2, eSE = 3
    uint32_t type_le;  // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, (bit 4 : B')
                       // -> EM_ALS_READER_M_TYPE
    uint32_t hce_le;   // 0: not virtual. 1: virtual (not used).
  } ce;
  struct {
    uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
    uint32_t type_le;    // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, bit 3: NFC
                         // V, bit 5: KOVIO -> EM_ALS_READER_M_TYPE
    uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848
                              // -> EM_ALS_READER_M_SPDRATE_
    uint32_t type_b_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848
                              // -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        type_v_rate_le;  // bit 4: 662, bit 5: 2648 -> EM_ALS_READER_M_SPDRATE_
    uint32_t
        type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
    uint32_t type_v_subcar_le;  // SubCarrier = 0, DualSubCarrier = 1
  } reader;
};

struct mtk_msg_EM_POLLING_MODE_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_EM_POLLING_MODE_NTF {
  struct mtk_msg_hdr hdr;
  uint32_t type_le;  // bit 0: reader mode; (bit 1: card emul), bit 2: p2p ->
                     // EM_ENABLE_FUNC_. Only 1 bit can be set.
  union {
    struct {
      uint32_t result_le;   // CONNECT=0, FAIL=1, DISCONNECT = 2
      uint32_t is_ndef_le;  // non NDEF = 0, NDEF = 1, READ_ONLY = 2, FAIL = 3,
                            // DISCONNECT=4
      uint32_t uidlen_le;   // length of the UID
      uint8_t uid[32];      // UID value ...
      uint8_t discovery_type;  // one of the  NFC_DISCOVERY_TYPE_* defined in
                               // external/libnfc-nci/src/nfc/include/nfc_api.h
      uint8_t bitrate;         // one of the NFC_BIT_RATE_* defined in
                               // external/libnfc-nci/src/nfc/include/nfc_api.h
    } reader;
    struct {
      uint32_t status_le;      // LLCP up = 0; LLCP down = 1
      uint8_t discovery_type;  // one of the  NFC_DISCOVERY_TYPE_* defined in
                               // external/libnfc-nci/src/nfc/include/nfc_api.h
      uint8_t bitrate;         // one of the NFC_BIT_RATE_* defined in
                               // external/libnfc-nci/src/nfc/include/nfc_api.h
      uint8_t role;            // 0: is target; 1: is initiator
    } p2p;
    // no notification for card... MTK source code uses the RSP message here,
    // not sure why.
  } u;
};

struct mtk_msg_EM_VIRTUAL_CARD_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;    // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, (bit 4 : B')
                       // -> EM_ALS_READER_M_TYPE
  uint32_t
      type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
};

struct mtk_msg_EM_VIRTUAL_CARD_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_FM_SWP_TEST_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0-> NFC_EM_ACT_
  struct {
    uint32_t swionr_le;  // UICC1 = 1, UICC2 = 2, eSE = 4
  } opt;  // ==> this is missing in the PDF. Code needs to be able to handle REQ
          // without it.
};

struct mtk_msg_FM_SWP_TEST_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_SW_VERSION_QUERY {
  struct mtk_msg_hdr hdr;
};

struct mtk_msg_SW_VERSION_RESPONSE {
  struct mtk_msg_hdr hdr;
  uint8_t mw_ver[19];
  uint8_t fw_ver[2];
  uint8_t hw_ver[2];
  uint8_t chip_ver[2];
};

struct mtk_msg_EM_TX_CARRIER_ALS_ON_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
};

struct mtk_msg_EM_TX_CARRIER_ALS_ON_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
};

struct mtk_msg_TESTMODE_SETTING_REQ {
  struct mtk_msg_hdr hdr;
  uint16_t rfu;
  uint16_t autocheck_le;  // 0: disable; 1: enable.
};

struct mtk_msg_TESTMODE_SETTING_RSP {
  struct mtk_msg_hdr hdr;
  uint8_t result;  // -> MTK_RESULT_*
};

struct mtk_msg_EM_LOOPBACK_TEST_REQ {
  struct mtk_msg_hdr hdr;
  uint8_t action;  // START = 0, STOP = 1 -> NFC_EM_ACT_
};

struct mtk_msg_EM_LOOPBACK_TEST_RSP {
  struct mtk_msg_hdr hdr;
  uint8_t result;  // -> MTK_RESULT_*
};

struct mtk_msg_FM_READ_DEP_TEST_REQ {
  struct mtk_msg_hdr hdr;
  // same content as mtk_msg_EM_ALS_READER_MODE_REQ, but using only NFC A with
  // all data rates.
  uint32_t action_le;       // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;         // bit 0: NFC A -> EM_ALS_READER_M_TYPE
  uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t type_b_rate_le;  // 0
  uint32_t type_v_rate_le;  // 0
  uint32_t type_f_rate_le;  // 0
  uint32_t type_v_subcar_le;  // 0
  uint32_t type_v_coding_le;  // 0
};

struct mtk_msg_FM_READ_DEP_TEST_REQ_legacy {
  struct mtk_msg_hdr hdr;
  // same content as mtk_msg_EM_ALS_READER_MODE_REQ, but using only NFC A with
  // all data rates.
  uint32_t action_le;       // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;         // bit 0: NFC A -> EM_ALS_READER_M_TYPE
  uint32_t type_a_rate_le;  // bit 0: 106, bit 1: 212, bit 2: 424, bit 3: 848 ->
                            // EM_ALS_READER_M_SPDRATE_
  uint32_t type_b_rate_le;  // 0
  uint32_t type_v_rate_le;  // 0
  uint32_t type_f_rate_le;  // 0
  uint32_t type_v_subcar_le;  // 0
};

struct mtk_msg_FM_READ_DEP_TEST_RSP {
  struct mtk_msg_hdr hdr;
  // same content as mtk_msg_EM_ALS_READER_MODE_OPT_RSP.
  // the tool actually only look at the result code.
  uint32_t result_le;  // -> MTK_RESULT_*
  union {
    struct mtk_ndef_read_payload read;
  } payload;
};

struct mtk_msg_FM_CARD_MODE_TEST_REQ {
  struct mtk_msg_hdr hdr;
  // same content as mtk_msg_EM_ALS_CARD_MODE_REQ, but using only UICC1 with all
  // techs, and hce is off.
  uint32_t action_le;  // START = 0
  uint32_t swionr_le;  // UICC1 = 1
  uint32_t type_le;    // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, (bit 4 : B')
                       // -> EM_ALS_READER_M_TYPE
  uint32_t hce_le;     // 0: not virtual
};

struct mtk_msg_FM_CARD_MODE_TEST_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // 0: success (has been read by reader); 1: fail /
                       // timeout. 0xE1: no UICC1.
};

struct mtk_msg_FM_VIRTUAL_CARD_REQ {
  struct mtk_msg_hdr hdr;
  // same content as mtk_msg_EM_VIRTUAL_CARD_REQ.
  uint32_t action_le;  // START = 0, STOP = 1 -> NFC_EM_ACT_
  uint32_t type_le;    // bit 0: NFC A, bit 1: NFC B, bit 2: NFC F, (bit 4 : B')
                       // -> EM_ALS_READER_M_TYPE
  uint32_t
      type_f_rate_le;  // bit 1: 212, bit 2: 424 -> EM_ALS_READER_M_SPDRATE_
};

struct mtk_msg_FM_VIRTUAL_CARD_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t
      result_le;  // 0: success (has been read by reader); 1: fail / timeout.
};

struct mtk_msg_FM_ANTENNA_TEST_REQ {
  struct mtk_msg_hdr hdr;
  uint32_t action_le;  // START = 0 -> NFC_EM_ACT_
};

struct mtk_msg_FM_ANTENNA_TEST_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
  /* ST21NFC extensions */
  // Payload of the PROP_TEST_RFI_DIAG_MEAS_GET_RSP data collected (13 bytes).

  // B0:B3 are referred to Diff configuration:
  // Amplitude (2 bytes) and phase (2 bytes) values for a
  // differential driving of antenna
  uint16_t Diff_Amplitude;  // B0:B1
  uint16_t Diff_Phase;      // B2:B3

  // B4:B7 are referred to RFO1,RFI1 configuration
  // Amplitude (2 bytes) and phase (2 bytes) values for a
  // single driving of antenna
  uint16_t RFO1_Amplitude;  // B4:B5
  uint16_t RFO1_Phase;      // B6:B7

  // B8:B11 are referred to RFO2,RFI2 configuration
  // Amplitude (2 bytes) and phase (2 bytes) values for a
  // single driving of antenna
  uint16_t RFO2_Amplitude;  // B8:B9
  uint16_t RFO2_Phase;      // B10:B11

  // B12 is the VDD_RF used for measurements
  uint8_t VDDRF;
};

struct mtk_msg_META_GET_SELIST_REQ {
  struct mtk_msg_hdr hdr;
  //  uint32_t    status_le; // 1 => enable SE function. 0 : disable.
};

struct mtk_nfc_tool_se_info {
  uint32_t seid_le;
  uint32_t /*enum SE_STATUS*/ status_le; /* 1: enable, 0 : disable*/
  uint32_t /*enum SE_TYPE*/ type_le;
  uint32_t /*enum SE_CONNECT_TYPE*/ connecttype_le;
  uint32_t             /*enum SE_STATUS*/
      lowpowermode_le; /* 1: enable low power mode, 0 : disable*/
  uint32_t             /*enum SE_STATUS*/
      pbf_le;          /* Each SE current status of Power by field*/
};

#define MTK_DEMO_TOOL_SE_NUM (3)

struct mtk_msg_META_GET_SELIST_RSP {
  struct mtk_msg_hdr hdr;
  uint32_t result_le;  // -> MTK_RESULT_*
  uint8_t SeCount;     /* total number of se */
  struct mtk_nfc_tool_se_info SE[MTK_DEMO_TOOL_SE_NUM];
};

#pragma pack(pop)

#endif  // _ST_MTKTOOLS_MSG_H
