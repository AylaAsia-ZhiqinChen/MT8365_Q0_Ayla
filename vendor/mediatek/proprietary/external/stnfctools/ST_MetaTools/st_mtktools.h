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

#ifndef _ST_MTKTOOLS_H
#define _ST_MTKTOOLS_H

// Standard includes
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Android includes
#define LOG_TAG "ST_MTKTools"

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <nativehelper/scoped_local_ref.h>
#include <nativehelper/scoped_primitive_array.h>
#include <nativehelper/scoped_utf_chars.h>
#include <semaphore.h>

// libnfc-nci includes
#include "nfa_p2p_api.h"
#include "nfa_api.h"
#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_hci_defs.h"
#include "nfa_ce_api.h"
#include "nfa_rw_api.h"
#include "nfc_config.h"

#include "ndef_utils.h"

using android::base::StringPrintf;
extern bool nfc_debug_enabled;

// ****************************************************************************

#include "st_mtktools_msg.h"

#ifndef ST_MTKTOOLS_VERSION
#define ST_MTKTOOLS_VERSION "ST 9.0-20180619"
#endif

// Some definitions used with the function prototypes following
enum mt_state {
  ST_IDLE = 0,  // after a deactivated
  ST_ALS_READER,
  ST_ALS_READER_TAG_CONNECTED,
  ST_ALS_P2P,
  ST_ALS_P2P_LLCP_UP,
  ST_ALS_CE,
  ST_ALS_HCE,
  ST_POLLING,
  ST_POLLING_TAG_CONNECTED,
  ST_POLLING_LLCP_UP,
  ST_TXON,
  ST_TM_OPTIONCHECK_W84TAG,
  ST_FM_READDEP_W84TAG,
  ST_FM_CM_CE_W84READER,
  ST_FM_CM_HCE_W84READER
};

enum rdr_ntf_type { RDR_NTF_CONNECT = 0, RDR_NTF_FAIL, RDR_NTF_DISCONNECT };

enum rdr_ntf_data {
  RDR_NTF_NonNDEF = 0,
  RDR_NTF_NDEF,
  RDR_NTF_READ_ONLY,
  RDR_NTF_dataFAIL,
  RDR_NTF_dataDISCONNECT,
};

enum p2p_ntf_type { P2P_NTF_LLCP_DOWN = 0, P2P_NTF_LLCP_UP };

// The daemon main loop works on the following state.
extern struct st_mt_state {
  // sync across threads
  pthread_mutex_t mtx;
  pthread_cond_t cond;  // signaled when a new message is received from client
                        // or on other events.

  // signal when the daemon is quitting
  int exiting;

  // communication with client.
  int servsock, clisock;  // -1 if socket is not opened or in error.
  pthread_t server_thread;
  struct mtk_msg_hdr *recv_msg;  // non-null: we received a message from client.

  // is the client EM or FM/Meta ?
  int is_em;

  // Configuration for the Reader mode. set before calling st_mt_glue_startmode.
  struct {
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t f : 1;
    uint8_t v : 1;
    uint8_t kovio : 1;
  } rdr_techs;

  struct {
    uint8_t bps106 : 1;
    uint8_t bps212 : 1;
    uint8_t bps424 : 1;
    uint8_t bps848 : 1;
  } rdr_a_datarate;

  struct {
    uint8_t bps106 : 1;
    uint8_t bps212 : 1;
    uint8_t bps424 : 1;
    uint8_t bps848 : 1;
  } rdr_b_datarate;

  struct {
    uint8_t bps662 : 1;
    uint8_t bps2648 : 1;
  } rdr_v_datarate;

  struct {
    uint8_t dual_subcarrier : 1;
    uint8_t coding_1of256 : 1;
  } rdr_v_opts;

  struct {
    uint8_t bps212 : 1;
    uint8_t bps424 : 1;
  } rdr_f_datarate;

  // Configuration for the P2P mode. set before calling st_mt_glue_startmode.
  struct {
    uint8_t a : 1;
    uint8_t f : 1;
  } p2p_techs;

  struct {
    uint8_t bps106 : 1;
    uint8_t bps212 : 1;
    uint8_t bps424 : 1;
    uint8_t bps848 : 1;
  } p2p_a_datarate;

  struct {
    uint8_t bps212 : 1;
    uint8_t bps424 : 1;
  } p2p_f_datarate;

  struct {
    uint8_t active : 1;
    uint8_t passive : 1;
    uint8_t initiator : 1;
    uint8_t target : 1;
    uint8_t disable_CE : 1;
  } p2p_opts;

  // Configuration for the CARD EMULATION and HCE mode. set before calling
  // st_mt_glue_startmode.
  enum e_ce_sel { HCE = 0, CE_UICC1 = 1, CE_UICC2 = 2, CE_eSE = 4 } ce_sel;

  struct {
    uint8_t a : 1;
    uint8_t b : 1;
    uint8_t f : 1;
    uint8_t f212 : 1;    // only used for HCE
    uint8_t f424 : 1;    // only used for HCE
    uint8_t bprime : 1;  // not supported
  } ce_techs;

  // State machine for the daemon.
  enum mt_state state;

  struct timespec since;  // when was the state changed ?

  // -------
  // following data is managed by the st_mt_glue.c file.
  // save the callbacks pointers registered by startmode.
  void (*reader_cb)(enum rdr_ntf_type t, enum rdr_ntf_data d, int uidlen,
                    uint8_t *uid, uint8_t discotype, uint8_t datarate);
  void (*p2p_cb)(enum p2p_ntf_type t, uint8_t discotype, uint8_t datarate,
                 uint8_t role);
  void (*ce_cb)();

  // upon reception of an event, store the data here to be processed by
  // different thread. A new event overwrites the previous one. only events that
  // are not handled "synchronously" (i.e. directly using a SyncEventGuard in
  // st_mt_glue) are posted here.
  int has_CONN_event;
  uint8_t CONN_event_code;
  tNFA_CONN_EVT_DATA CONN_event_data;

  // in case of multi detection, we store here the first interesting candidate.
  int disc_has_candidate;
  tNFC_RESULT_DEVT disc_candidate;

} * st_mt_g_state;

// store one NDEF message as used by the tools
struct ndef_msg {
  // NDEF type
  enum e_type {
    NDEF_URI = 0,
    NDEF_TEXT,
    NDEF_SP,
    NDEF_OTHERS
  } type;  // aligned with EM_ALS_READER_MODE_OPT_RSP

  enum e_lang {
    NDEF_LN_DEFAULT = 0,
    NDEF_LN_DE,
    NDEF_LN_EN,
    NDEF_LN_FR
  } lang;  // aligned with EM_ALS_READER_MODE_OPT_RSP

  // Record information only present when reading NDEF
  uint8_t record_flag;
  uint8_t record_id[32];
  uint8_t record_tnf;

  // NDEF payload
  uint32_t len;
  uint8_t *data;  // malloc'd by ndef_read, caller needs to free it.
};

// ------------------------------------------------------------------

// Server (socket) functions for comm with MTK tools clients.
int st_mt_serv_start();
void st_mt_serv_stop();
int st_mt_server_send(struct mtk_msg_hdr *msg);

// Main loop in this server daemon.
void st_mt_event_loop();

void st_mt_event_chstate(enum mt_state newstate);

// glue code with NFC stack
int st_mt_glue_startNFC();  // init the NFC stack and open the device
int st_mt_glue_stopNFC();   // deinit the NFC stack and close the device.

int st_mt_glue_setRFMergeMode(
    int rfmerge);  // set the RF Merge mode config and reset the stack.

int st_mt_glue_handle_events(
    int prescheck);  // called from main thread when condition is signaled.
                     // return 0 on success, 1 on warning, 2 on fatal error.

int st_mt_glue_getversions(uint8_t *mwver /* 19 bytes */,
                           uint8_t *fw_ver /* 2 bytes */,
                           uint8_t *hw_ver /* 2 bytes */,
                           uint8_t *chip_ver /* 2 bytes */);

int st_mt_glue_resetmode();  // go to idle, whatever mode was started
                             // previously. clean the state. returns !0 on fatal
                             // error.

int st_mt_glue_startmode(  // ret value: 0 = success, 1: fail.
    int polling_period,    // use 0 for default.
    void (*reader_cb)(enum rdr_ntf_type t, enum rdr_ntf_data d, int uidlen,
                      uint8_t *uid, uint8_t discotype,
                      uint8_t datarate),  // if ! NULL, starts reader mode and
                                          // calls this cb when notification of
                                          // tag (connect /disconnect /failure)
    void (*p2p_cb)(enum p2p_ntf_type t, uint8_t discotype, uint8_t datarate,
                   uint8_t role),  // if ! NULL, starts p2p mode and calls this
                                   // callback when llcp status changes
    int start_CE,    // if !0, start CE or HCE depending on ce_sel in config.
    void (*ce_cb)()  // if !NULL, will be called when a reader has read the
                     // emulated card (UID was read).
);

// read / write NDEF (when connected)
int st_mt_glue_ndef_format();
int st_mt_glue_ndef_write(
    struct ndef_msg *msg);  // will remove any previous NDEF content.
int st_mt_glue_ndef_read(
    struct ndef_msg *msg);  // returns the 1st record found.

// the following commands can only be run if no mode (reader, CE, etc) was
// started.
int st_mt_glue_test_swp(int swpnr);  // returns 0 on SWP success, 1 on SWP
                                     // failure, -1 on other failure
int st_mt_glue_test_loopback();  // returns 0 on HCI loopback success, 1 on test
                                 // failure, -1 on other failure
int st_mt_glue_test_antenna();   // returns 0 on antenna loopback success, 1 on
                                 // test failure, -1 on other failure
int st_mt_glue_test_TXON(int on);  // returns 0 on success, 1 in case of failure
int st_mt_glue_test_antenna_DIAG_MEAS(
    uint16_t *diff_ampl, uint16_t *diff_phase, uint16_t *rfo1_ampl,
    uint16_t *rfo1_phase, uint16_t *rfo2_ampl, uint16_t *rfo2_phase,
    uint8_t *vddrf, uint8_t rege,
    int nrloops);  // measures responses in the antenna in different modes

#endif  // _ST_MTKTOOLS_H
